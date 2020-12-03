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


#ifndef mir_action_ThreadExecutor_h
#define mir_action_ThreadExecutor_h

#include <iosfwd>

#include "mir/action/plan/Executor.h"


namespace mir {
namespace action {


class ThreadExecutor : public Executor {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ThreadExecutor(const std::string& name);

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

    void print(std::ostream&) const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // No copy allowed
    ThreadExecutor(const ThreadExecutor&);
    ThreadExecutor& operator=(const ThreadExecutor&);

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    void execute(context::Context&, const ActionNode& node) const override;
    void wait() const override;
    void parametrisation(const param::MIRParametrisation&) override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace action
}  // namespace mir


#endif
