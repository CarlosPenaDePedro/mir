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


#pragma once

#include "mir/action/io/EndAction.h"


namespace mir {
namespace input {
class MIRInput;
}
}  // namespace mir


namespace mir {
namespace action {
namespace io {


class Set : public EndAction {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Set(const param::MIRParametrisation&, output::MIROutput&);
    Set(const Set&) = delete;

    // -- Destructor

    ~Set() override;

    // -- Convertors
    // None

    // -- Operators

    Set& operator=(const Set&) = delete;

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

    // From Action
    void execute(context::Context&) const override;
    const char* name() const override;
    void estimate(context::Context&, api::MIREstimation&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace io
}  // namespace action
}  // namespace mir
