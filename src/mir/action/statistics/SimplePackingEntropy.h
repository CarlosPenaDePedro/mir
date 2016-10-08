/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Oct 2016


#ifndef mir_action_statistics_SimplePackingEntropy_h
#define mir_action_statistics_SimplePackingEntropy_h

#include "eckit/exception/Exceptions.h"
#include "mir/action/statistics/Statistics.h"
#include "mir/data/MIRField.h"


namespace mir {
namespace action {
namespace statistics {


/**
 * @brief Calculate entropy statistics on a MIRField
 */
class SimplePackingEntropy : public Statistics {
public:

    // -- Exceptions
    // None

    // -- Constructors

    SimplePackingEntropy(const param::MIRParametrisation&);

    // -- Destructor

    ~SimplePackingEntropy() {}

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    bool sameAs(const Action&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    size_t bucketCount_;

    // -- Methods
    // None

    // -- Overridden methods

    /// Calculate statistics
    void calculate(const data::MIRField&, Results&) const;

    void print(std::ostream&) const;


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

