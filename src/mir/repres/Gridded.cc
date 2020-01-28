/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/repres/Gridded.h"

#include "mir/util/Domain.h"
#include "mir/util/Grib.h"
#include "mir/api/MIREstimation.h"


namespace mir {
namespace repres {


Gridded::Gridded(const param::MIRParametrisation& parametrisation) :
    bbox_(parametrisation) {
}


Gridded::Gridded(const util::BoundingBox& bbox) :
    bbox_(bbox) {
}


Gridded::~Gridded() = default;


Gridded::Gridded() = default;


void Gridded::setComplexPacking(grib_info& info) const {
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_COMPLEX;
}


void Gridded::setSimplePacking(grib_info& info) const {
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE;
}


void Gridded::setGivenPacking(grib_info&) const {
    // The packing_type is set by the caller
}


util::Domain Gridded::domain() const {

    const Latitude& n = includesNorthPole()? Latitude::NORTH_POLE : bbox_.north();
    const Latitude& s = includesSouthPole()? Latitude::SOUTH_POLE : bbox_.south();
    const Longitude& w = bbox_.west();
    const Longitude& e = isPeriodicWestEast()? bbox_.west() + Longitude::GLOBE : bbox_.east();

    return util::Domain(n, w, s, e);
}


const util::BoundingBox& Gridded::boundingBox() const {
    return bbox_;
}


bool Gridded::getLongestElementDiagonal(double&) const {
    return false;
}

void Gridded::estimate(api::MIREstimation& estimation) const {
    estimation.packing("grid_simple"); // Will be overriden
    estimation.representation(factory());
}


}  // namespace repres
}  // namespace mir

