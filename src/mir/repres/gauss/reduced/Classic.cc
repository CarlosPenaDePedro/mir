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
/// @author Pedro Maciel
/// @date Apr 2015

#include "mir/repres/gauss/reduced/Classic.h"

#include "atlas/Grid.h"
#include "atlas/grids/grids.h"
#include "atlas/grids/GaussianLatitudes.h"
#include "mir/util/Grib.h"
#include "eckit/log/Timer.h"
#include "mir/api/MIRJob.h"

namespace mir {
namespace repres {
namespace reduced {

Classic::Classic(size_t N):
    Reduced(N) {

}

Classic::~Classic() {
}

Classic::Classic(size_t N, const util::BoundingBox &bbox):
    Reduced(N, bbox) {

}

void Classic::fill(grib_info &info) const  {
    Reduced::fill(info);
// NOTE: We assume that grib_api will put the proper PL
}

void Classic::fill(api::MIRJob &job) const  {
    Reduced::fill(job);
    std::stringstream os;
    os << "N" << N_;
    job.set("gridname", os.str());
}

atlas::Grid *Classic::atlasGrid() const {
    ASSERT(globalDomain()); // Atlas support needed for non global grids
    std::ostringstream os;
    os << "rgg.N" << N_;
    return atlas::Grid::create(os.str());
}

const std::vector<long> &Classic::pls() const {
    if (pl_.size() == 0) {
        std::ostringstream os;
        os << "rgg.N" << N_;
        eckit::ScopedPtr<atlas::grids::ReducedGrid> grid(dynamic_cast<atlas::grids::ReducedGrid *>(atlas::Grid::create(os.str())));

        ASSERT(grid.get());

        const std::vector<int> &v = grid->npts_per_lat();
        pl_.resize(v.size());
        for (size_t i = 0; i < v.size(); i++) {
            pl_[i] = v[i];
        }
    }
    return pl_;
}

}  // namespace reduced
}  // namespace repres
}  // namespace mir

