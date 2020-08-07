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


#include "mir/action/interpolate/Gridded2TypedGrid.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/key/grid/Grid.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2TypedGrid::Gridded2TypedGrid(const param::MIRParametrisation& parametrisation) :
    Gridded2UnrotatedGrid(parametrisation) {
    ASSERT(parametrisation_.userParametrisation().get("grid", grid_));

    const auto& g = key::grid::Grid::lookup(grid_);
    ASSERT(g.isTyped());

    g.parametrisation(grid_, gridParams_);
    ASSERT(gridParams_.has("gridType"));
}


Gridded2TypedGrid::~Gridded2TypedGrid() = default;


bool Gridded2TypedGrid::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2TypedGrid*>(&other);

    // NOTE: SimpleParametrisation::matches() is not commutative, and grid_ is not compared because gridParams_ contains
    // all useful information for building Representation
    return (o != nullptr) && gridParams_.matches(o->gridParams_) && o->gridParams_.matches(gridParams_) &&
           Gridded2GriddedInterpolation::sameAs(other);
}


void Gridded2TypedGrid::print(std::ostream& out) const {
    out << "Gridded2TypedGrid[grid=" << gridParams_ << ",";
    Gridded2UnrotatedGrid::print(out);
    out << "]";
}


const repres::Representation* Gridded2TypedGrid::outputRepresentation() const {
    const auto& g = key::grid::Grid::lookup(grid_);
    return g.representation(gridParams_);
}


const char* Gridded2TypedGrid::name() const {
    return "Gridded2TypedGrid";
}


static ActionBuilder<Gridded2TypedGrid> grid2grid("interpolate.grid2typedgrid");


}  // namespace interpolate
}  // namespace action
}  // namespace mir
