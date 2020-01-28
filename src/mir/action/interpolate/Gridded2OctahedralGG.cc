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


#include "mir/action/interpolate/Gridded2OctahedralGG.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/repres/gauss/reduced/ReducedOctahedral.h"
#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2OctahedralGG::Gridded2OctahedralGG(const param::MIRParametrisation& parametrisation):
    Gridded2UnrotatedGrid(parametrisation) {
    ASSERT(parametrisation_.userParametrisation().get("octahedral", N_));
}


Gridded2OctahedralGG::~Gridded2OctahedralGG() = default;


bool Gridded2OctahedralGG::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2OctahedralGG*>(&other);
    return o && (N_ == o->N_) && Gridded2GriddedInterpolation::sameAs(other);
}


void Gridded2OctahedralGG::print(std::ostream& out) const {
    out << "Gridded2OctahedralGG["
           "N=" << N_ << ",";
    Gridded2UnrotatedGrid::print(out);
    out  << "]";
}


const repres::Representation* Gridded2OctahedralGG::outputRepresentation() const {
    return new repres::gauss::reduced::ReducedOctahedral(N_);
}

const char* Gridded2OctahedralGG::name() const {
    return "Gridded2OctahedralGG";
}


namespace {
static ActionBuilder< Gridded2OctahedralGG > grid2grid("interpolate.grid2octahedral-gg");
}


}  // namespace interpolate
}  // namespace action
}  // namespace mir

