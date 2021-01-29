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


#include "mir/action/interpolate/Gridded2RotatedRegularGG.h"

#include <iostream>

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/regular/RotatedGG.h"
#include "mir/util/Exceptions.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2RotatedRegularGG::Gridded2RotatedRegularGG(const param::MIRParametrisation& parametrisation) :
    Gridded2RotatedGrid(parametrisation), N_(0) {
    ASSERT(parametrisation_.userParametrisation().get("regular", N_));
    ASSERT(N_ > 0);
}


Gridded2RotatedRegularGG::~Gridded2RotatedRegularGG() = default;


bool Gridded2RotatedRegularGG::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2RotatedRegularGG*>(&other);
    return (o != nullptr) && (N_ == o->N_) && Gridded2RotatedGrid::sameAs(other);
}


void Gridded2RotatedRegularGG::print(std::ostream& out) const {
    out << "Gridded2RotatedRegularGG["
           "N="
        << N_
        << ","
           "rotation="
        << rotation() << ",";
    Gridded2RotatedGrid::print(out);
    out << "]";
}


const repres::Representation* Gridded2RotatedRegularGG::outputRepresentation() const {
    return new repres::gauss::regular::RotatedGG(N_, rotation());
}


const char* Gridded2RotatedRegularGG::name() const {
    return "Gridded2RotatedRegularGG";
}


static ActionBuilder<Gridded2RotatedRegularGG> grid2grid("interpolate.grid2rotated-regular-gg");


}  // namespace interpolate
}  // namespace action
}  // namespace mir
