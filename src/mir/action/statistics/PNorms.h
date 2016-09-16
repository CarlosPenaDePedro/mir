/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#ifndef mir_action_statistics_pNorms_h
#define mir_action_statistics_pNorms_h

#include "eckit/exception/Exceptions.h"
#include "mir/action/statistics/Statistics.h"
#include "mir/action/statistics/detail/ScalarpNormsFn.h"
#include "mir/data/MIRField.h"


namespace mir {
namespace action {
namespace statistics {


/**
 * @brief Calculate statistics on a MIRField
 */
class PNorms : public Statistics {
public:

    // -- Exceptions
    // None

    // -- Constructors

    PNorms(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~PNorms() {}

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    /// Online statistics update
    void operator+=(const PNorms&);

    // -- Overridden methods

    bool sameAs(const Action&) const;

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

    /// Calculate statistics
    void calculate(const data::MIRField&, Results&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    mutable detail::ScalarpNormsFn<double> stats_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace statistics
}  // namespace action
}  // namespace mir


#endif

