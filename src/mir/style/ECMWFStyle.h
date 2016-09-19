/*
 * (C) Copyright 1996-2015 ECMWF.
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


#ifndef ECMWFStyle_H
#define ECMWFStyle_H

#include "mir/style/MIRStyle.h"


namespace mir {
namespace style {

struct Requirements;

class ECMWFStyle : public MIRStyle {
public:

// -- Exceptions
    // None

// -- Contructors

    ECMWFStyle(const param::MIRParametrisation&);

// -- Destructor

    ~ECMWFStyle(); // Change to virtual if base class

// -- Convertors
    // None

// -- Operators
    // None

// -- Methods



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


    virtual void prologue(action::ActionPlan&) const;
    virtual void sh2sh(action::ActionPlan&) const = 0;
    virtual void sh2grid(action::ActionPlan&) const = 0;
    virtual void grid2grid(action::ActionPlan&) const;
    virtual void epilogue(action::ActionPlan&) const;

    virtual void selectWindComponents(action::ActionPlan&) const;

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

private:

// No copy allowed

    ECMWFStyle(const ECMWFStyle&);
    ECMWFStyle& operator=(const ECMWFStyle&);

// -- Members

// -- Methods

// -- Overridden methods

    virtual void prepare(action::ActionPlan&) const;


// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    // friend std::ostream& operator<<(std::ostream& s, const ECMWFStyle& p)
    // { p.print(s); return s; }

};


}  // namespace style
}  // namespace mir
#endif

