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


#ifndef mir_method_fe_FELinear_h
#define mir_method_fe_FELinear_h

#include "mir/method/fe/FiniteElement.h"


namespace mir {
namespace method {
namespace fe {


class FELinear : public FiniteElement {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    FELinear(const param::MIRParametrisation&, const std::string& label = "input");

    // -- Destructor
    // None

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

    // From MethodWeighted
    const char* name() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace fe
}  // namespace method
}  // namespace mir


#endif
