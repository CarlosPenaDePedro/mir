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


#ifndef mir_action_io_Set_h
#define mir_action_io_Set_h

#include "mir/action/plan/Action.h"


namespace mir {
namespace input {
class MIRInput;
}
namespace output {
class MIROutput;
}
}  // namespace mir


namespace mir {
namespace action {
namespace io {


class Set : public Action {
public:
    // -- Exceptions
    // None

    // -- Constructors

    Set(const param::MIRParametrisation&, input::MIRInput&, output::MIROutput&);
    Set(const Set&) = delete;

    // -- Destructor

    virtual ~Set() override;

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

    void print(std::ostream&) const override;
    void custom(std::ostream&) const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    input::MIRInput& input_;
    output::MIROutput& output_;

    // -- Methods
    // None

    // -- Overridden methods

    // From Action
    virtual void execute(context::Context&) const override;
    virtual bool sameAs(const Action&) const override;
    virtual const char* name() const override;
    virtual bool isEndAction() const override;
    virtual void estimate(context::Context&, api::MIREstimation&) const override;

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


#endif
