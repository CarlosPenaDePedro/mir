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


#ifndef mir_action_plan_Action_h
#define mir_action_plan_Action_h

#include <iosfwd>
#include <string>


namespace mir {
namespace context {
class Context;
}
namespace param {
class MIRParametrisation;
}
namespace util {
class BoundingBox;
}
namespace api {
class MIREstimation;
}
namespace repres {
class Representation;
}
}  // namespace mir


namespace mir {
namespace action {


class Action {
public:
    // -- Exceptions
    // None

    // -- Contructors

    Action(const param::MIRParametrisation&);
    Action(const Action&) = delete;

    // -- Destructor

    virtual ~Action();

    // -- Convertors
    // None

    // -- Operators

    void operator=(const Action&) = delete;

    // -- Methods

    void perform(context::Context&) const;

    virtual bool sameAs(const Action&) const = 0;
    virtual void custom(std::ostream&) const;
    virtual const char* name() const = 0;

    // For optimising plans
    virtual bool mergeWithNext(const Action&);
    virtual bool deleteWithNext(const Action&);
    virtual bool isEndAction() const;
    virtual bool isCropAction() const;
    virtual bool canCrop() const;
    virtual util::BoundingBox outputBoundingBox() const;

    virtual void estimate(context::Context&, api::MIREstimation& estimation) const;

    // -- Overridden methods
    // None

    // -- Class members
    // Helper function for estimate()

    static void estimateNumberOfGridPoints(context::Context& ctx, api::MIREstimation& estimation,
                                           const repres::Representation&);
    static void estimateMissingValues(context::Context& ctx, api::MIREstimation& estimation,
                                      const repres::Representation&);

    // -- Class methods
    // None

protected:
    // -- Members

    const param::MIRParametrisation& parametrisation_;

    // -- Methods

    virtual void print(std::ostream&) const = 0;

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

    // call perform() for the public interface
    virtual void execute(context::Context&) const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Action& p) {
        p.print(s);
        return s;
    }
};


class ActionFactory {

    std::string name_;

    virtual Action* make(const param::MIRParametrisation&) = 0;

    ActionFactory(const ActionFactory&) = delete;
    ActionFactory& operator=(const ActionFactory&) = delete;

protected:
    ActionFactory(const std::string&);

    virtual ~ActionFactory();

public:
    static Action* build(const std::string&, const param::MIRParametrisation&, bool exact = true);

    static void list(std::ostream&);
};


template <class T>
class ActionBuilder : public ActionFactory {
    virtual Action* make(const param::MIRParametrisation& param) { return new T(param); }

public:
    ActionBuilder(const std::string& name) : ActionFactory(name) {}
};


}  // namespace action
}  // namespace mir


#endif
