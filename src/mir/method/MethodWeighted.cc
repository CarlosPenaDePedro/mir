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
#include "mir/caching/WeightCache.h"

#include <algorithm>
#include <map>
#include <string>

#include "eckit/log/Plural.h"
#include "eckit/log/Timer.h"
#include "eckit/log/Seconds.h"

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/types/FloatCompare.h"

#include "atlas/Grid.h"

#include "mir/data/MIRField.h"
#include "mir/lsm/LandSeaMasks.h"
// #include "mir/param/MIRParametrisation.h"


namespace mir {
namespace method {


namespace {


static eckit::Mutex local_mutex;


/// Convert values vector to booleans, with equality
template< typename T >
struct check_equality {
    check_equality(const T& ref_) : ref(ref_) {}
    bool operator()(const T& v) const { return v==ref; }
    const T ref;
};


/// Convert values vector to booleans, with "greater than or equal to"
template< typename T >
struct check_inequality_ge {
    check_inequality_ge(const T& ref_) : ref(ref_) {}
    bool operator()(const T& v) const { return v>=ref; }
    const T ref;
};


}  // (anonymous namespace)


static std::map<std::string, WeightMatrix> matrix_cache;


MethodWeighted::MethodWeighted(const param::MIRParametrisation &param) :
    Method(param) {
}


MethodWeighted::~MethodWeighted() {
}


// This returns a 'const' matrix so we ensure that we don't change it and break the in-memory cache
const WeightMatrix &MethodWeighted::getMatrix(const atlas::Grid &in, const atlas::Grid &out) const {

    eckit::Timer timer("MethodWeighted::getMatrix");

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    double here = timer.elapsed();
    const lsm::LandSeaMasks masks = lsm::LandSeaMasks::lookup(parametrisation_, in, out);
    eckit::Log::info() << "Compute LandSeaMasks " << timer.elapsed() - here << std::endl;

    eckit::Log::info() << "++++ LSM masks " << masks << std::endl;
    here = timer.elapsed();
    eckit::MD5 md5;
    md5 << *this;
    md5 << in;
    md5 << out;

    const eckit::MD5::digest_t md5_no_masks(md5.digest());
    md5 << masks;
    const eckit::MD5::digest_t md5_with_masks(md5.digest());
    eckit::Log::info() << "Compute md5 " << timer.elapsed() - here << std::endl;


    const std::string
    basen = std::string(name()) + "-" + in.shortName() + "-" + out.shortName(),
    key_no_masks   = basen + "-"      + md5_no_masks,
    key_with_masks = basen +  "-LSM-" + md5_with_masks;

    std::map<std::string, WeightMatrix>::iterator j = matrix_cache.find(key_with_masks);
    if (j != matrix_cache.end()) {
        return (*j).second;
    }

    const std::string cache_key = (masks.active() && masks.cacheable())?
                key_with_masks
              : key_no_masks;

    // Shorten the key, to avoid "file name to long" errors


    // calculate weights matrix, apply mask if necessary

    eckit::Log::info() << "Elapsed 1 " << timer.elapsed()  << std::endl;

    here = timer.elapsed();
    WeightMatrix W(out.npts(), in.npts());
    eckit::Log::info() << "Create matrix " << timer.elapsed() - here << std::endl;


    static caching::WeightCache cache;

    if (!cache.retrieve(cache_key, W)) {
        computeWeights(in, out, W);
        if (masks.active() && masks.cacheable()) {
            applyMasks(W, masks);
        }
        cache.insert(cache_key, W);
    }

    // If LSM not cacheabe, e.g. user provided, we apply the mask after
    if (masks.active() && !masks.cacheable())  {
        applyMasks(W, masks);
    }

    here = timer.elapsed();
    std::swap(matrix_cache[key_with_masks], W);
    eckit::Log::info() << "Swap matrix " << timer.elapsed() - here << std::endl;

    return matrix_cache[key_with_masks];
}


void MethodWeighted::execute(data::MIRField &field, const atlas::Grid &in, const atlas::Grid &out) const {

    eckit::Timer timer("MethodWeighted::execute");
    eckit::Log::info() << "MethodWeighted::execute" << std::endl;

    // setup sizes & checks
    const size_t
    npts_inp = in.npts(),
    npts_out = out.npts();

    std::vector<double> result;
    result.resize(npts_out);

    const WeightMatrix &W = getMatrix(in, out);

    // TODO: ASSERT matrix size is npts_out * npts_inp

    // multiply interpolant matrix with field vector
    for (size_t i = 0; i < field.dimensions(); i++) {

        eckit::StrStream os;
        os << "Interpolating field ("  << eckit::BigNum(npts_inp) << " -> " << eckit::BigNum(npts_out) << ")" << eckit::StrStream::ends;
        std::string msg(os);
        eckit::Timer t(msg);

        ASSERT(field.values(i).size() == npts_inp);

        std::vector<double> &values = field.values(i);

        Eigen::VectorXd::MapType vi = Eigen::VectorXd::Map( &values[0], npts_inp );
        Eigen::VectorXd::MapType vo = Eigen::VectorXd::Map( &result[0], npts_out );

        if ( field.hasMissing() ) {
            // Assumes compiler does return value optimization
            // otherwise we need to pass result matrix as parameter
            WeightMatrix MW = applyMissingValues(W, field, i);
            vo = MW * vi;
        } else {
            vo = W * vi;
        }

        field.values(result, i);  // Update field with result
    }

    // update if missing values are present
    if (field.hasMissing()) {
        const check_equality< double > check_miss(field.missingValue());
        bool still_has_missing = false;
        for (size_t i = 0; i < field.dimensions() && !still_has_missing; ++i) {
            const std::vector< double > &values = field.values(i);
            still_has_missing = (std::find_if(values.begin(),values.end(),check_miss)!=values.end());
        }
        field.hasMissing(still_has_missing);
    }
}


void MethodWeighted::computeWeights(const atlas::Grid &in, const atlas::Grid &out, WeightMatrix &W) const {
    if (in.same(out)) {
        eckit::Log::info() << "Matrix is indentity" << std::endl;
        W.setIdentity();        // grids are the same, use identity matrix
    } else {
        eckit::Timer timer("Assemble matrix");
        assemble(W, in, out);   // assemble matrix of coefficients
    }
}


WeightMatrix MethodWeighted::applyMissingValues(const WeightMatrix &W, data::MIRField &field, size_t which) const {

    ASSERT(field.hasMissing());

    eckit::Log::info() << "Field has missing values" << std::endl;
    const double missing = field.missingValue();
    std::vector< double > &values = field.values(which);

    // setup sizes & checks
    const check_equality< double > check_miss(missing);
    const size_t
    Nivalues = values.size(),
    Novalues = W.rows();
    std::vector< bool > missvalues(Nivalues);

    std::transform(values.begin(),values.end(),missvalues.begin(),check_miss);
    const size_t count = std::count(missvalues.begin(),missvalues.end(),true);

    if (count == 0) {
        return W;
    }
    eckit::Log::info() << "Field has " << eckit::Plural(count, "missing value") << " out of " << eckit::BigNum(Nivalues) << std::endl;


    // check matrix weigths correctness
    // TODO this check should not be done here? maybe before matrix caching? but I don't know where it goes
    size_t Nprob = 0;
    for (size_t i = 0; i < W.rows(); i++) {
        double sum = 0.;
        for (WeightMatrix::InnerIterator j(W, i); j; ++j) {
            if (missvalues[j.col()])
                sum += j.value();
        }
        const bool
        weights_zero = eckit::FloatCompare::is_equal(sum, 0.),
        weights_one  = eckit::FloatCompare::is_equal(sum, 1.);
        if ( !weights_zero && !weights_one ) {
            ++Nprob;
            eckit::Log::warning() <<  "Missing values: incorrect interpolation weights sum: Sum(W(" << i << ",:)) != {0,1} = " << sum << std::endl;
        }
    }
    if (Nprob) {
        eckit::Log::warning() <<  "Missing values: problem in input weights matrix for " << eckit::Plural(Nprob, "interpolation point") << ", continuing (but shouldn't really)." << std::endl;
    }


    // correct matrix weigths for the missing values (matrix copy happens here)
    WeightMatrix X(W);
    size_t fix_missall  = 0;
    size_t fix_misssome = 0;
    for (size_t i = 0; i < X.rows(); i++) {

        // count missing values and accumulate weights
        double sum = 0.; // accumulated row weight, disregarding field missing values
        size_t
        Nmiss = 0,
        Ncol  = 0;
        for (WeightMatrix::InnerIterator j(X, i); j; ++j, ++Ncol) {
            if (missvalues[j.col()])
                ++Nmiss;
            else
                sum += j.value();
        }
        const bool
        weights_zero = eckit::FloatCompare::is_equal(sum, 0.),
        miss_some = Nmiss,
        miss_all  = (miss_some && (Ncol==Nmiss)) || weights_zero;

        // redistribution
        if ( miss_all ) {
            ++fix_missall;

            // all values are missing (or weights wrongly computed):
            // erase row & force missing value equality
            for (WeightMatrix::InnerIterator j(X, i); j; ++j) {
                j.valueRef() = 0.;
                values[j.col()] = missing;
            }
            WeightMatrix::InnerIterator(X, i).valueRef() = 1.;

        } else if ( miss_some ) {
            ++fix_misssome;

            // apply linear redistribution
            const double invsum = 1./sum;
            for (WeightMatrix::InnerIterator j(X, i); j; ++j) {
                if (missvalues[j.col()]) {
                    values[j.col()] = missing;
                    j.valueRef() = 0.;
                } else {
                    j.valueRef() *= invsum;
                }
            }

        }
    }


    // recheck corrected matrix for problems
    // TODO this check should not be done here? after interpolation? again I don't know where it goes
    if ( Nprob ) {
        Nprob = 0;
        for (size_t i = 0; i < X.rows(); i++) {
            double sum = 0.;
            for (WeightMatrix::InnerIterator j(X, i); j; ++j)
                sum += j.value();
            const bool
            weights_zero = eckit::FloatCompare::is_equal(sum, 0.),
            weights_one  = eckit::FloatCompare::is_equal(sum, 1.);
            if ( !weights_zero && !weights_one ) {
                ++Nprob;
                eckit::Log::warning() <<  "Missing values: incorrect interpolation weights sum: Sum(X(" << i << ",:)) != {0,1} = " << sum << std::endl;
            }
        }
        if (Nprob) {
            eckit::Log::warning() <<  "Missing values: problems still found in corrected input weights matrix for " << eckit::Plural(Nprob, "interpolation point") << ", continuing (but shouldn't really)." << std::endl;
        } else {
            eckit::Log::info() <<  "Missing values: no problems found in corrected input weights matrix." << std::endl;
        }
    }


    // log corrections and return
    eckit::Log::info() << "Missing values correction: " << fix_misssome << '/' << fix_missall << " some/all-missing out of " << eckit::BigNum(Novalues) << std::endl;
    return X;
}


void MethodWeighted::applyMasks(WeightMatrix &W, const lsm::LandSeaMasks &masks) const {

    eckit::Timer timer("MethodWeighted::applyMasks");

    eckit::Log::info() << "======== MethodWeighted::applyMasks(" << masks << ")" << std::endl;
    ASSERT(masks.active());


    const data::MIRField &imask_field = masks.inputField();
    const data::MIRField &omask_field = masks.inputField();
    ASSERT(!imask_field.hasMissing());
    ASSERT(!omask_field.hasMissing());
    ASSERT(imask_field.dimensions()==1);
    ASSERT(omask_field.dimensions()==1);


    // build boolean masks (to isolate algorithm from the logical mask condition)
    const std::vector< double >
    &imask_values = imask_field.values(0),
    &omask_values = imask_field.values(0);

    check_inequality_ge< double > check_lsm(0.5);
    std::vector< bool >
    imask(imask_values.size(), false),
    omask(omask_values.size(), false);
    std::transform(imask_values.begin(), imask_values.end(), imask.begin(), check_lsm);
    std::transform(omask_values.begin(), omask_values.end(), omask.begin(), check_lsm);


    // apply corrections on inequality != (XOR) of logical masks,
    // then redistribute weights
    // - output mask (omask) operates on matrix row index, here i
    // - input mask (imask) operates on matrix column index, here j.col()
    // FIXME: hardcoded to *= 0.2
    size_t fix = 0;
    for (size_t i = 0; i < W.rows(); i++) {
        double sum = 0.;

        // correct weight of non-matching input point weight contribution
        bool row_changed = false;
        for (WeightMatrix::InnerIterator j(W, i); j; ++j) {

            ASSERT(j.col() < imask.size());
            ASSERT(i < omask.size());

            if (omask[i] != imask[j.col()]) {
                j.valueRef() *= 0.2;
                row_changed = true;
            }
            sum += j.value();
        }

        // apply linear redistribution if necessary
        const bool weights_zero = eckit::FloatCompare::is_equal(sum, 0.);
        if (row_changed && !weights_zero) {
            ++fix;
            const double invsum = 1./sum;
            for (WeightMatrix::InnerIterator j(W, i); j; ++j)
                j.valueRef() *= invsum;
        }

    }


    // log corrections
    eckit::Log::info() << "LandSeaMasks correction: " << eckit::BigNum(fix) << " out of " << eckit::BigNum(W.rows()) << std::endl;
}

void MethodWeighted::hash(eckit::MD5& md5) const {
    md5.add(name());
}


}  // namespace method
}  // namespace mir

