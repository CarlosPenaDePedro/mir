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


#include "mir/action/interpolate/Gridded2ReducedGG.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/reduced/ReducedClassic.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2ReducedGG::Gridded2ReducedGG(const param::MIRParametrisation& parametrisation) :
    Gridded2UnrotatedGrid(parametrisation) {
    ASSERT(parametrisation_.userParametrisation().get("reduced", N_));
}


Gridded2ReducedGG::~Gridded2ReducedGG() = default;


bool Gridded2ReducedGG::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2ReducedGG*>(&other);
    return o && (N_ == o->N_) && Gridded2GriddedInterpolation::sameAs(other);
}

void Gridded2ReducedGG::print(std::ostream& out) const {
    out << "Gridded2ReducedGG["
           "N="
        << N_ << ",";
    Gridded2UnrotatedGrid::print(out);
    out << "]";
}


const repres::Representation* Gridded2ReducedGG::outputRepresentation() const {
    return new repres::gauss::reduced::ReducedClassic(N_);
}

const char* Gridded2ReducedGG::name() const {
    return "Gridded2ReducedGG";
}


namespace {
static ActionBuilder<Gridded2ReducedGG> grid2grid("interpolate.grid2reduced-gg");
}


}  // namespace interpolate
}  // namespace action
}  // namespace mir
