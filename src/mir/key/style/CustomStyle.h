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


#ifndef mir_key_style_CustomStyle_h
#define mir_key_style_CustomStyle_h

#include "mir/key/style/MIRStyle.h"


namespace mir {
namespace key {
namespace style {


class CustomStyle : public MIRStyle {
public:
    // -- Exceptions
    // None

    // -- Constructors

    CustomStyle(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~CustomStyle() override;

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

    virtual void print(std::ostream&) const override;

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

    void prepare(action::ActionPlan&, input::MIRInput&, output::MIROutput&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend std::ostream& operator<<(std::ostream& s, const CustomStyle& p)
    // { p.print(s); return s; }
};


}  // namespace style
}  // namespace key
}  // namespace mir


#endif
