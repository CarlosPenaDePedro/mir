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


#include "mir/action/interpolate/Gridded2NamedGrid.h"

#include <iostream>

#include "mir/key/grid/Grid.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/util/Exceptions.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2NamedGrid::Gridded2NamedGrid(const param::MIRParametrisation& parametrisation) :
    Gridded2UnrotatedGrid(parametrisation) {
    ASSERT(parametrisation_.userParametrisation().get("grid", grid_));
}


Gridded2NamedGrid::~Gridded2NamedGrid() = default;


bool Gridded2NamedGrid::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2NamedGrid*>(&other);
    return (o != nullptr) && (grid_ == o->grid_) && Gridded2GriddedInterpolation::sameAs(other);
}


void Gridded2NamedGrid::print(std::ostream& out) const {
    out << "Gridded2NamedGrid[grid=" << grid_ << ",";
    Gridded2UnrotatedGrid::print(out);
    out << "]";
}


const repres::Representation* Gridded2NamedGrid::outputRepresentation() const {
    const auto& ng = key::grid::Grid::lookup(grid_);
    return ng.representation();
}


const char* Gridded2NamedGrid::name() const {
    return "Gridded2NamedGrid";
}


static ActionBuilder<Gridded2NamedGrid> grid2grid("interpolate.grid2namedgrid");


}  // namespace interpolate
}  // namespace action
}  // namespace mir
