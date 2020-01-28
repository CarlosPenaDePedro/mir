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


#ifndef mir_action_interpolate_Gridded2ReducedGGPLGiven_h
#define mir_action_interpolate_Gridded2ReducedGGPLGiven_h

#include <vector>
#include "mir/action/interpolate/Gridded2UnrotatedGrid.h"


namespace mir {
namespace action {
namespace interpolate {


class Gridded2ReducedGGPLGiven : public Gridded2UnrotatedGrid {
public:

    // -- Exceptions
    // None

    // -- Contructors

    Gridded2ReducedGGPLGiven(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~Gridded2ReducedGGPLGiven();

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

    // From Gridded2GriddedInterpolation
    virtual bool sameAs(const Action&) const;
    virtual void print(std::ostream&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    std::vector<long> pl_;

    // -- Methods
    // None

    // -- Overridden methods

    // From Action
    virtual const char* name() const;

    // From Gridded2GriddedInterpolation
    virtual const repres::Representation* outputRepresentation() const;

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

