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


#ifndef mir_action_compare_CompareMetadata_h
#define mir_action_compare_CompareMetadata_h

#include "mir/action/compare/Compare.h"


namespace mir {
namespace data {
class MIRField;
}
namespace param {
class MIRParametrisation;
}
}


namespace mir {
namespace action {
namespace compare {


/**
 * @brief Compare MIRField's using their metadata
 */
class CompareMetadata : public Compare {
public:

    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    CompareMetadata(const param::MIRParametrisation& parametrisation) :
        Compare(parametrisation) {
    }

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

    /// Comparison options
    // None

    // -- Methods

    /// @returns report field metada
    CompareResults getFieldMetadata(const data::MIRField&, const param::MIRParametrisation&) const;

    // -- Overridden methods

    /// @return if fields compare successfuly
    bool compare(
            const data::MIRField& field1, const param::MIRParametrisation& param1,
            const data::MIRField& field2, const param::MIRParametrisation& param2 ) const;

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
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace compare
}  // namespace action
}  // namespace mir


#endif

