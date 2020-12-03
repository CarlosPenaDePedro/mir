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


#ifndef mir_action_GlobaliseFilter_h
#define mir_action_GlobaliseFilter_h

#include "mir/action/plan/Action.h"


namespace mir {
namespace action {


class GlobaliseFilter : public Action {
public:
    // -- Exceptions
    // None

    // -- Constructors

    GlobaliseFilter(const param::MIRParametrisation&);

    // -- Destructor

    ~GlobaliseFilter() override;

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

    void print(std::ostream&) const override;
    bool deleteWithNext(const Action&) override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // No copy allowed

    GlobaliseFilter(const GlobaliseFilter&);
    GlobaliseFilter& operator=(const GlobaliseFilter&);

    // -- Members


    // -- Methods
    // None

    // -- Overridden methods

    void execute(context::Context&) const override;
    bool sameAs(const Action&) const override;
    const char* name() const override;


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
