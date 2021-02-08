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


#ifndef mir_key_grid_NamedGrid_h
#define mir_key_grid_NamedGrid_h

#include "mir/key/grid/Grid.h"


namespace mir {
namespace key {
namespace grid {


class NamedGrid : public Grid {
protected:
    // -- Constructors

    NamedGrid(const std::string& key, bool tentative = false) : Grid(key, named_t, tentative) {}

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    void print(std::ostream&) const override                                           = 0;
    size_t gaussianNumber() const override                                             = 0;
    const repres::Representation* representation() const override                      = 0;
    const repres::Representation* representation(const util::Rotation&) const override = 0;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Friends
    // None
};


}  // namespace grid
}  // namespace key
}  // namespace mir


#endif
