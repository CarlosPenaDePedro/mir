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


#include "mir/action/interpolate/Gridded2RotatedOctahedralGG.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/reduced/RotatedOctahedral.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2RotatedOctahedralGG::Gridded2RotatedOctahedralGG(const param::MIRParametrisation &parametrisation):
    Gridded2RotatedGrid(parametrisation),
    N_(0) {
    ASSERT(parametrisation_.userParametrisation().get("octahedral", N_));
    ASSERT(N_ > 0);
}


Gridded2RotatedOctahedralGG::~Gridded2RotatedOctahedralGG() = default;


bool Gridded2RotatedOctahedralGG::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2RotatedOctahedralGG*>(&other);
    return o && (N_ == o->N_) && Gridded2RotatedGrid::sameAs(other);
}


void Gridded2RotatedOctahedralGG::print(std::ostream &out) const {
    out << "Gridded2RotatedOctahedralGG["
           "N=" << N_ << ","
           "rotation=" << rotation() << ",";
    Gridded2RotatedGrid::print(out);
    out << "]";
}


const repres::Representation *Gridded2RotatedOctahedralGG::outputRepresentation() const {
    return new repres::gauss::reduced::RotatedOctahedral(N_, rotation());
}


const char* Gridded2RotatedOctahedralGG::name() const {
    return "Gridded2RotatedOctahedralGG";
}


namespace {
static ActionBuilder< Gridded2RotatedOctahedralGG > grid2grid("interpolate.grid2rotated-octahedral-gg");
}


}  // namespace interpolate
}  // namespace action
}  // namespace mir

