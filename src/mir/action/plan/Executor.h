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


#ifndef mir_action_plan_Executor_h
#define mir_action_plan_Executor_h

#include <iosfwd>
#include <string>


namespace mir {
namespace context {
class Context;
}
namespace param {
class MIRParametrisation;
}
namespace action {
class ActionNode;
}
}  // namespace mir


namespace mir {
namespace action {
class Executor {
public:
    // -- Exceptions
    // None

    // -- Constructors
    // None

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual void execute(context::Context&, const ActionNode&) const = 0;
    virtual void wait() const                                        = 0;
    virtual void parametrisation(const param::MIRParametrisation&)   = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static const Executor& lookup(const param::MIRParametrisation&);
    static void list(std::ostream&);

protected:
    Executor(const std::string&);
    virtual ~Executor();

    // -- Members

    std::string name_;

    // -- Methods

    virtual void print(std::ostream&) const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // No copy allowed
    Executor(const Executor&);
    Executor& operator=(const Executor&);

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

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Executor& p) {
        p.print(s);
        return s;
    }
};


}  // namespace action
}  // namespace mir


#endif
