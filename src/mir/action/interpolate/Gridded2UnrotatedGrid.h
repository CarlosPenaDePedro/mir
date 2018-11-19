/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_action_interpolate_Gridded2UnrotatedGrid_h
#define mir_action_interpolate_Gridded2UnrotatedGrid_h

#include "mir/action/interpolate/Gridded2GriddedInterpolation.h"


namespace mir {
namespace action {
namespace interpolate {


class Gridded2UnrotatedGrid : public Gridded2GriddedInterpolation {
public:

    // -- Exceptions
    // None

    // -- Contructors

    using Gridded2GriddedInterpolation::Gridded2GriddedInterpolation;

    // -- Destructor

    virtual ~Gridded2UnrotatedGrid();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    // From Gridded2GriddedInterpolation
    virtual util::BoundingBox croppingBoundingBox() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace interpolate
}  // namespace action
}  // namespace mir


#endif

