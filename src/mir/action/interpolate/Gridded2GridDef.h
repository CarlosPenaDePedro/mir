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


#ifndef mir_action_interpolate_Gridded2GridDef_h
#define mir_action_interpolate_Gridded2GridDef_h

#include "mir/action/interpolate/Gridded2UnrotatedGrid.h"


namespace mir {
namespace action {
namespace interpolate {


class Gridded2GridDef : public Gridded2UnrotatedGrid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Gridded2GridDef(const param::MIRParametrisation&);

    // -- Destructor

    ~Gridded2GridDef() override;

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
    bool sameAs(const Action&) const override;
    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    std::string griddef_;

    // -- Methods
    // None

    // -- Overridden methods

    // From Action
    void custom(std::ostream&) const override;
    const char* name() const override;

    // From Gridded2GriddedInterpolation
    const repres::Representation* outputRepresentation() const override;

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
