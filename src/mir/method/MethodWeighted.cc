/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Peter Bispham
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date May 2015


#include "mir/method/MethodWeighted.h"

#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include "eckit/config/Resource.h"
#include "eckit/log/Plural.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "atlas/grid/Grid.h"
#include "atlas/mesh/Mesh.h"
#include "mir/action/context/Context.h"
#include "mir/caching/InMemoryCache.h"
#include "mir/caching/MeshCache.h"
#include "mir/caching/WeightCache.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/data/MIRFieldStats.h"
#include "mir/lsm/LandSeaMasks.h"
#include "mir/method/GridSpace.h"
#include "mir/method/decompose/Decompose.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Compare.h"
#include "mir/util/MIRStatistics.h"


using mir::util::compare::is_approx_zero;
using mir::util::compare::is_approx_one;


namespace mir {
namespace method {

//----------------------------------------------------------------------------------------------------------------------

namespace {
static eckit::Mutex local_mutex;
static InMemoryCache<WeightMatrix> matrix_cache("mirMatrix", 512 * 1024 * 1024, "$MIR_MATRIX_CACHE_MEMORY_FOOTPRINT");
static InMemoryCache<atlas::mesh::Mesh> mesh_cache("mirMesh",  512 * 1024 * 1024, "$MIR_MESH_CACHE_MEMORY_FOOTPRINT");
}

MethodWeighted::MethodWeighted(const param::MIRParametrisation &parametrisation) :
    Method(parametrisation) {
    ASSERT(parametrisation.get("lsm.weight.adjustment", lsmWeightAdjustement_));
}


MethodWeighted::~MethodWeighted() {
}

atlas::mesh::Mesh& MethodWeighted::generateMeshAndCache(const atlas::grid::Grid& grid) const {


    eckit::MD5 md5;
    grid.hash(md5);

    hash(md5); // this adds mesh generator settings to make it trully unique key

//    if((mesh = mir::caching::MeshCache::get(md5.digest()))) { return mesh; }

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    InMemoryCache<atlas::mesh::Mesh>::iterator j = mesh_cache.find(md5);
    if (j != mesh_cache.end()) {
        return *j;
    }

    atlas::mesh::Mesh& mesh = mesh_cache[md5];

    try {
        generateMesh(grid, mesh);
    }
    catch (...) {
        // Make sure we don't leave an incomplete entry in the cache
        mesh_cache.erase(md5);
        throw;
    }

    return mesh;
}

void MethodWeighted::generateMesh(const atlas::grid::Grid& g, atlas::mesh::Mesh& mesh) const {
    std::ostringstream oss;
    oss << "Method " << name()
        << " needs a mesh() but does not implement generateMesh()"
        << std::endl;
    throw eckit::SeriousBug(oss.str(), Here());
}

// This returns a 'const' matrix so we ensure that we don't change it and break the in-memory cache
const WeightMatrix &MethodWeighted::getMatrix(context::Context& ctx, const atlas::grid::Grid &in, const atlas::grid::Grid &out) const {

    eckit::Log::debug<LibMir>() << "MethodWeighted::getMatrix " << *this << std::endl;

    eckit::TraceTimer<LibMir> timer("MethodWeighted::getMatrix");

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    double here = timer.elapsed();
    const lsm::LandSeaMasks masks = getMasks(ctx, in, out);
    eckit::Log::debug<LibMir>() << "Compute LandSeaMasks " << timer.elapsed() - here << std::endl;

    eckit::Log::debug<LibMir>() << "++++ LSM masks " << masks << std::endl;
    here = timer.elapsed();
    eckit::MD5 md5;
    md5 << *this;
    md5 << in;
    md5 << out;

    const eckit::MD5::digest_t md5_no_masks(md5.digest());
    md5 << masks;
    const eckit::MD5::digest_t md5_with_masks(md5.digest());
    eckit::Log::debug<LibMir>() << "Compute md5 " << timer.elapsed() - here << std::endl;


    const std::string shortName_in  = in.shortName();
    const std::string shortName_out = out.shortName();
    ASSERT(!shortName_in.empty());
    ASSERT(!shortName_out.empty());


    const std::string base_name      = std::string(name()) + "-" + shortName_in + "-" + shortName_out;
    const std::string key_no_masks   = base_name + "-"      + md5_no_masks;
    const std::string key_with_masks = base_name +  "-LSM-" + md5_with_masks;

    InMemoryCache<WeightMatrix>::iterator j = matrix_cache.find(key_with_masks);
    if (j != matrix_cache.end()) {
        return *j;
    }

    const std::string cache_key = (masks.active() && masks.cacheable()) ?
                                  key_with_masks
                                  : key_no_masks;

    // Shorten the key, to avoid "file name to long" errors


    // calculate weights matrix, apply mask if necessary

    eckit::Log::debug<LibMir>() << "Elapsed 1 " << timer.elapsed()  << std::endl;

    here = timer.elapsed();
    WeightMatrix W(out.npts(), in.npts());
    eckit::Log::debug<LibMir>() << "Create matrix " << timer.elapsed() - here << std::endl;

    bool caching = true;
    parametrisation_.get("caching", caching);

    // The WeightCache is parametrised by 'caching', as caching may be disabled on a field by field basis (unstructured grids)
    static caching::WeightCache cache;

    if (!caching || !cache.retrieve(cache_key, W)) {

        computeMatrixWeights(ctx, in, out, W);
        W.validate("computeMatrixWeights");

        if (masks.active() && masks.cacheable()) {
            applyMasks(W, masks, ctx.statistics());
            W.validate("applyMasks");
        }
        if (caching) {
            cache.insert(cache_key, W);
        }
    } else {
        W.validate("fromCache");
    }

    // If LSM not cacheabe, e.g. user provided, we apply the mask after
    if (masks.active() && !masks.cacheable())  {
        applyMasks(W, masks, ctx.statistics());
        W.validate("applyMasks");
    }

    WeightMatrix& w = matrix_cache[key_with_masks];
    std::swap(w, W);

    matrix_cache.footprint(key_with_masks, w.footprint());

    return w;
}

lsm::LandSeaMasks MethodWeighted::getMasks(context::Context& ctx, const atlas::grid::Grid &in, const atlas::grid::Grid &out) const {
    return lsm::LandSeaMasks::lookup(parametrisation_, in, out);
}

void MethodWeighted::execute(context::Context &ctx, const atlas::grid::Grid &in, const atlas::grid::Grid &out) const {
    using util::compare::IsMissingFn;

    // Make sure another thread to no evict anything from the cache while we are using it
    InMemoryCacheUser<WeightMatrix>      matrix_use(matrix_cache, ctx.statistics().matrixCache_);
    InMemoryCacheUser<atlas::mesh::Mesh> cache_use(mesh_cache, ctx.statistics().meshCache_);


    static bool check_stats = eckit::Resource<bool>("mirCheckStats", false);

    eckit::TraceTimer<LibMir> timer("MethodWeighted::execute");
    eckit::Log::debug<LibMir>() << "MethodWeighted::execute" << std::endl;

    // setup sizes & checks
    const size_t npts_inp = in.npts();
    const size_t npts_out = out.npts();

    const WeightMatrix &W = getMatrix(ctx, in, out);

    ASSERT( W.rows() == npts_out );
    ASSERT( W.cols() == npts_inp );

    data::MIRField& field = ctx.field();

    for (size_t i = 0; i < field.dimensions(); i++) {

        std::ostringstream os;
        os << "Interpolating field ("  << eckit::BigNum(npts_inp) << " -> " << eckit::BigNum(npts_out) << ")";
        eckit::TraceTimer<LibMir> t(os.str());

        // compute some statistics on the result
        // This is expensive so we might want to skip it in production code
        data::MIRFieldStats istats;

        if (check_stats) {
            istats = field.statistics(i);
        }


        const std::vector<double> &values = field.values(i);
        ASSERT(values.size() == npts_inp);

        {
            eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().matrixTiming_);

            // FIXME: remove this const cast once Matrix provides read-only view
            WeightMatrix::Matrix mi(const_cast<double *>(values.data()), values.size(), 1);

            eckit::ScopedPtr<decompose::Decompose> decomp(decompose::DecomposeFactory::build("DecomposeSkip"));
            if (true) {
                decomp.reset(decompose::DecomposeFactory::build("DecomposePolarAngleDegreesAssymmetric"));
            }

            WeightMatrix::Matrix mi_decomposed;
            decomp->decompose(mi, mi_decomposed);

            // This should be local to the loop as field.value() will take ownership of result with std::swap()
            // For optimisation, one can also create result outside the loop, and resize() it here
            WeightMatrix::Matrix mo_decomposed(npts_out, mi_decomposed.cols());

            if ( field.hasMissing() ) {

                std::vector<bool> fieldMissingValues(npts_inp, false);
                std::transform(values.begin(), values.end(), fieldMissingValues.begin(), IsMissingFn(field.missingValue()));

                // Assumes compiler does return value optimization
                // otherwise we need to pass result matrix as parameter
                WeightMatrix MW = applyMissingValues(W, fieldMissingValues);

                MW.multiply(mi_decomposed, mo_decomposed);

            } else {

                W.multiply(mi_decomposed, mo_decomposed);

            }

            std::vector<double> result(npts_out);
            WeightMatrix::Matrix mo(result.data(), result.size(), 1);
            decomp->recompose(mo_decomposed, mo);

            field.update(result, i);  // Update field with result
        }


        if (check_stats) {
            // compute some statistics on the result
            // This is expensive so we might want to skip it in production code
            eckit::Log::debug<LibMir>() << "Input  Field statistics : " << istats << std::endl;

            data::MIRFieldStats ostats = field.statistics(i);
            eckit::Log::debug<LibMir>() << "Output Field statistics : " << ostats << std::endl;

            /// FIXME: This assertion is to early in the case of LocalGrid input
            ///        because there will be output points which won't be updated (where skipped)
            ///        but later should be cropped out
            ///        UNLESS, we compute the statistics based on only points contained in the Domain

            if ( in.domain().isGlobal() ) {
                ASSERT(eckit::FloatCompare<double>::isApproximatelyGreaterOrEqual(ostats.minimum(), istats.minimum()));
                ASSERT(eckit::FloatCompare<double>::isApproximatelyGreaterOrEqual(istats.maximum(), ostats.maximum()));
            }
        }

    }

    // TODO: move logic to MIRField
    // update if missing values are present
    if (field.hasMissing()) {
        const IsMissingFn isMissing(field.missingValue());
        bool still_has_missing = false;
        for (size_t i = 0; i < field.dimensions() && !still_has_missing; ++i) {
            const std::vector< double > &values = field.values(i);
            still_has_missing = (std::find_if(values.begin(), values.end(), isMissing) != values.end());
        }
        field.hasMissing(still_has_missing);
    }
}


void MethodWeighted::computeMatrixWeights(context::Context& ctx, const atlas::grid::Grid &in, const atlas::grid::Grid &out, WeightMatrix &W) const {

    eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().computeMatrixTiming_);

    if (in.same(out)) {
        eckit::Log::debug<LibMir>() << "Matrix is indentity" << std::endl;
        W.setIdentity();        // grids are the same, use identity matrix
    } else {
        eckit::TraceTimer<LibMir> timer("Assemble matrix");
        GridSpace iSpace(in, *this);
        GridSpace oSpace(out, *this);
        assemble(ctx, W, iSpace, oSpace);   // assemble matrix of coefficients
        W.cleanup();
    }
}

WeightMatrix MethodWeighted::applyMissingValues(const WeightMatrix &W, const std::vector<bool>& fieldMissingValues) const {

    // correct matrix weigths for the missing values (matrix copy happens here)
    ASSERT( W.cols() == fieldMissingValues.size() );
    WeightMatrix X(W);

    for (size_t i = 0; i < X.rows(); i++) {

        // count missing values and accumulate weights
        double sum = 0.; // accumulated row weight, disregarding field missing values
        size_t Nmiss = 0;
        size_t Ncol  = 0;
        for (WeightMatrix::inner_const_iterator j(X, i); j; ++j, ++Ncol) {
            if (fieldMissingValues[static_cast<size_t>(j.col())])
                ++Nmiss;
            else
                sum += *j;
        }
        const bool missingSome = (Nmiss > 0);
        const bool missingAll  = (Ncol == Nmiss);

        // redistribution; either:
        // - all values are missing (or weights wrongly computed), erase row & force missing value, or
        // - some values are missing, so apply linear redistribution
        if ((missingAll || is_approx_zero(sum)) && (Ncol > 0)) {

            bool found = false;
            for (WeightMatrix::inner_iterator j(X, i); j; ++j) {
                *j = 0.;
                if (!found && fieldMissingValues[static_cast<size_t>(j.col())]) {
                    *j = 1.;
                    found = true;
                }
            }
            ASSERT(found);

        } else if (missingSome) {

            ASSERT(!is_approx_zero(sum));
            for (WeightMatrix::inner_iterator j(X, i); j; ++j) {
                if (fieldMissingValues[static_cast<size_t>(j.col())]) {
                    *j = 0.;
                } else {
                    *j /= sum;
                }
            }

        }
    }

    X.validate("MethodWeighted::applyMissingValues");
    return X;
}


void MethodWeighted::applyMasks(WeightMatrix &W,
                                const lsm::LandSeaMasks &masks,
                                util::MIRStatistics&) const {

    eckit::TraceTimer<LibMir> timer("MethodWeighted::applyMasks");

    eckit::Log::debug<LibMir>() << "======== MethodWeighted::applyMasks(" << masks << ")" << std::endl;
    ASSERT(masks.active());

    const std::vector< bool > &imask = masks.inputMask();
    const std::vector< bool > &omask = masks.outputMask();

    eckit::Log::debug<LibMir>() << "imask size " << imask.size() << std::endl;
    eckit::Log::debug<LibMir>() << "omask size " << omask.size() << std::endl;

    eckit::Log::debug<LibMir>() << "cols " << W.cols() << std::endl;
    eckit::Log::debug<LibMir>() << "rows " << W.rows() << std::endl;

    ASSERT(imask.size() == W.cols());
    ASSERT(omask.size() == W.rows());


    // apply corrections on inequality != (XOR) of logical masks,
    // then redistribute weights
    // - output mask (omask) operates on matrix row index, here i
    // - input mask (imask) operates on matrix column index, here j.col()

    size_t fix = 0;
    for (size_t i = 0; i < W.rows(); i++) {

        ASSERT(i < omask.size());

        // correct weight of non-matching input point weight contribution
        double sum = 0.;
        bool row_changed = false;
        for (WeightMatrix::inner_iterator j(W, i); j; ++j) {

            ASSERT(j.col() < int(imask.size()));

            if (omask[i] != imask[j.col()]) {
                *j *= lsmWeightAdjustement_;
                row_changed = true;
            }
            sum += *j;
        }

        // apply linear redistribution if necessary
        if (row_changed && !is_approx_zero(sum)) {
            ++fix;
            for (WeightMatrix::inner_iterator j(W, i); j; ++j) {
                *j /= sum;
            }
        }

    }


    // log corrections
    eckit::Log::debug<LibMir>() << "MethodWeighted: applyMasks corrected " << eckit::BigNum(fix) << " out of " << eckit::Plural(W.rows() , "row") << std::endl;
}


void MethodWeighted::hash(eckit::MD5 &md5) const {
    md5.add(name());
}


}  // namespace method
}  // namespace mir

