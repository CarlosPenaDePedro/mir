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


#include "mir/style/intgrid/NamedGrid.h"

#include "eckit/exception/Exceptions.h"


namespace mir {
namespace style {
namespace intgrid {


NamedGrid::NamedGrid(const std::string& gridname, const param::MIRParametrisation& parametrisation) :
    Intgrid(parametrisation),
    gridname_(gridname) {
    ASSERT(!gridname_.empty());
}


const std::string& NamedGrid::gridname() const {
    return gridname_;
}


}  // namespace intgrid
}  // namespace style
}  // namespace mir
