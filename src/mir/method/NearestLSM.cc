/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date May 2015

#include "mir/method/NearestLSM.h"

#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"

#include "mir/method/GridSpace.h"
#include "mir/config/LibMir.h"
#include "mir/lsm/LandSeaMasks.h"
#include "mir/param/RuntimeParametrisation.h"
#include "mir/util/Compare.h"
#include "mir/util/PointSearch.h"


using eckit::Log;

namespace mir {
namespace method {

//----------------------------------------------------------------------------------------------------------------------

NearestLSM::NearestLSM(const param::MIRParametrisation &param) :
    MethodWeighted(param) {
}


NearestLSM::~NearestLSM() {
}


const char *NearestLSM::name() const {
    return  "nearest-lsm";
}


void NearestLSM::assemble(context::Context& ctx, WeightMatrix &W, const GridSpace& in, const GridSpace& out) const {

    eckit::TraceTimer<LibMir> timer("NearestLSM::assemble");
    eckit::Log::debug<LibMir>() << "NearestLSM::assemble" << std::endl;


    // get the land-sea masks, with boolean masking on point (node) indices
    double here = timer.elapsed();

    const lsm::LandSeaMasks masks = getMasks(ctx, in.grid(), out.grid());
    ASSERT(masks.active());

    Log::debug<LibMir>() << "NearestLSM compute LandSeaMasks " << timer.elapsed() - here << std::endl;


    // compute masked/not-masked search trees
    here = timer.elapsed();

    const std::vector< bool > &imask = masks.inputMask();
    const std::vector< bool > &omask = masks.outputMask();
    ASSERT(imask.size() == W.cols());
    ASSERT(omask.size() == W.rows());

    util::PointSearch sptree_masked    (in, util::compare::IsMaskedFn   (imask));
    util::PointSearch sptree_notmasked (in, util::compare::IsNotMaskedFn(imask));

    Log::debug<LibMir>() << "NearestLSM compute masked/not-masked search trees " << timer.elapsed() - here << std::endl;


    // compute the output nodes coordinates
    here = timer.elapsed();

    atlas::array::ArrayView< double, 2 > ocoords = atlas::array::make_view< double, 2 >(out.coordsXYZ());

    Log::debug<LibMir>() << "NearestLSM compute the output nodes coordinates " << timer.elapsed() - here << std::endl;


    // search nearest neighbours matching in/output masks
    // - output mask (omask) operates on matrix row index (i)
    // - input mask (imask) operates on matrix column index (j)
    here = timer.elapsed();

    std::vector<WeightMatrix::Triplet> mat;
    mat.reserve(W.rows());
    for (WeightMatrix::Size i=0; i<W.rows(); ++i) {

        // pick the (input) search tree matching the output mask
        util::PointSearch& sptree(
                    omask[i]? sptree_masked
                            : sptree_notmasked );

        // perform nearest neighbour search
        // - p: output grid node to look neighbours for
        // - q: input grid node closest to p (accessed as qmeta.point())
        // - j: index of q in input grid (or input field)
        const util::PointSearch::PointType      p(ocoords[i].data());
        const util::PointSearch::PointValueType qmeta = sptree.closestPoint(p);
        const size_t j = qmeta.payload();

        // insert entry into uncompressed matrix structure
        mat.push_back(WeightMatrix::Triplet( i, j, 1. ));

    }
    Log::debug<LibMir>() << "NearestLSM search nearest neighbours matching in/output masks " << timer.elapsed() - here << std::endl;


    // fill-in sparse matrix
    here = timer.elapsed();
    W.setFromTriplets(mat);
    Log::debug<LibMir>() << "NearestLSM fill-in sparse matrix " << timer.elapsed() - here << std::endl;
}


lsm::LandSeaMasks NearestLSM::getMasks(context::Context& ctx, const atlas::Grid &in, const atlas::Grid &out) const {
    param::RuntimeParametrisation runtime(parametrisation_);
    runtime.set("lsm", true); // Force use of LSM
    return lsm::LandSeaMasks::lookup(runtime, in, out);
}


void NearestLSM::applyMasks(WeightMatrix &W, const lsm::LandSeaMasks &, util::MIRStatistics& statistics) const {
    // FIXME this function should not be overriding to do nothing
}


void NearestLSM::print(std::ostream &out) const {
    out << "NearestLSM[]";
}


namespace {
static MethodBuilder< NearestLSM > __method("nearest-lsm");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace method
}  // namespace mir

