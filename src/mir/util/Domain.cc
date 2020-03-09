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


#include "mir/util/Domain.h"

#include <iostream>


namespace mir {
namespace util {


bool Domain::includesPoleNorth() const {
    return north() == Latitude::NORTH_POLE;
}


bool Domain::includesPoleSouth() const {
    return south() == Latitude::SOUTH_POLE;
}


Domain::operator RectangularDomain() const {
    return RectangularDomain({{west().value(), east().value()}}, {{south().value(), north().value()}});
}


void Domain::print(std::ostream& os) const {
    os << "Domain["
       << "north=" << north() << ",west=" << west() << ",south=" << south() << ",east=" << east()
       << ",isGlobal=" << isGlobal() << "]";
}


}  // namespace util
}  // namespace mir
