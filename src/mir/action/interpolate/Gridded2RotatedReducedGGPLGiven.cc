/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/action/interpolate/Gridded2RotatedReducedGGPLGiven.h"

#include <iostream>
#include <vector>
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/reduced/RotatedFromPL.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2RotatedReducedGGPLGiven::Gridded2RotatedReducedGGPLGiven(const param::MIRParametrisation& parametrisation):
    Gridded2RotatedGrid(parametrisation) {
    ASSERT(parametrisation_.userParametrisation().get("pl", pl_));
    ASSERT(!pl_.empty());
}


Gridded2RotatedReducedGGPLGiven::~Gridded2RotatedReducedGGPLGiven() = default;


bool Gridded2RotatedReducedGGPLGiven::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2RotatedReducedGGPLGiven*>(&other);
    return o && (pl_ == o->pl_) && Gridded2RotatedGrid::sameAs(other);
}


void Gridded2RotatedReducedGGPLGiven::print(std::ostream& out) const {
    out << "Gridded2RotatedReducedGGPLGiven["
        "pl=" << pl_.size() << ","
        "rotation=" << rotation() << ",";
    Gridded2RotatedGrid::print(out);
    out << "]";
}


const repres::Representation* Gridded2RotatedReducedGGPLGiven::outputRepresentation() const {
    size_t N = pl_.size() / 2;
    return new repres::gauss::reduced::RotatedFromPL(N, pl_, rotation());
}


const char* Gridded2RotatedReducedGGPLGiven::name() const {
    return "Gridded2RotatedReducedGGPLGiven";
}


namespace {
static ActionBuilder< Gridded2RotatedReducedGGPLGiven > grid2grid("interpolate.grid2rotated-reduced-gg-pl-given");
}


}  // namespace interpolate
}  // namespace action
}  // namespace mir

