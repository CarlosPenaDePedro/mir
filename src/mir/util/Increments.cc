/*
 * (C) Copyright 1996- ECMWF.
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


#include "mir/util/Increments.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/types/Fraction.h"
#include "mir/api/MIRJob.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Grib.h"


namespace mir {
namespace util {


namespace {


static void check(const Increments& inc) {
    ASSERT(inc.west_east().longitude() >= 0);
    ASSERT(inc.south_north().latitude() >= 0);
}


}  // (anonymous namespace)


Increments::Increments() = default;


Increments::Increments(const param::MIRParametrisation& parametrisation) {
    Latitude lat;
    ASSERT(parametrisation.get("south_north_increment", lat));
    south_north_ = lat;

    Longitude lon;
    ASSERT(parametrisation.get("west_east_increment", lon));
    west_east_ = lon;

    check(*this);
}


Increments::Increments(const Increments& other) :
    Increments(other.west_east_, other.south_north_) {
}


Increments::Increments(double westEastIncrement, double southNorthIncrement) :
    Increments(LongitudeIncrement(westEastIncrement), LatitudeIncrement(southNorthIncrement)) {
}


Increments::Increments(const LongitudeIncrement& west_east, const LatitudeIncrement& south_north) :
    west_east_(west_east),
    south_north_(south_north) {
    check(*this);
}


Increments::~Increments() = default;


bool Increments::operator==(const Increments& other) const {
    return  (west_east_.longitude() == other.west_east_.longitude()) &&
            (south_north_.latitude() == other.south_north_.latitude());
}


bool Increments::operator!=(const Increments& other) const {
    return  (west_east_.longitude() != other.west_east_.longitude()) ||
            (south_north_.latitude() != other.south_north_.latitude());
}


Increments& Increments::operator=(const Increments&) = default;


bool Increments::isPeriodic() const {
    return (Longitude::GLOBE.fraction() / west_east_.longitude().fraction()).integer();
}


bool Increments::isShifted(const BoundingBox& bbox) const {
    return isLatitudeShifted(bbox) || isLongitudeShifted(bbox);
}


void Increments::print(std::ostream& out) const {
    out << "Increments["
        << "west_east=" << west_east_.longitude()
        << ",south_north=" << south_north_.latitude()
        << "]";
}


void Increments::fill(grib_info& info) const  {
    // Warning: scanning mode not considered
    info.grid.iDirectionIncrementInDegrees = west_east_.longitude().value();
    info.grid.jDirectionIncrementInDegrees = south_north_.latitude().value();
}


void Increments::fill(api::MIRJob& job) const  {
    job.set("grid", west_east_.longitude().value(), south_north_.latitude().value());
}


void Increments::makeName(std::ostream& out) const {
    out << "-" << west_east_.longitude().value()
        << "x" << south_north_.latitude().value();
}


bool Increments::isLatitudeShifted(const BoundingBox& bbox) const {
    auto& inc = south_north_.latitude();
    if (inc == 0) {
        return false;
    }
    return !(bbox.south().fraction() / inc.fraction()).integer();
}


bool Increments::isLongitudeShifted(const BoundingBox& bbox) const {
    auto& inc = west_east_.longitude();
    if (inc == 0) {
        return false;
    }
    return !(bbox.west().fraction() / inc.fraction()).integer();
}


}  // namespace util
}  // namespace mir

