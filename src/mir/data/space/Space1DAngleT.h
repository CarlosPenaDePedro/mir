/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_data_dimension_Space1DAngleT_h
#define mir_data_dimension_Space1DAngleT_h

#include "mir/data/Space.h"

namespace mir {
namespace data {
namespace space {


template< int SCALE, int SYMMETRY >
class Space1DAngleT : public Space {
public:

    // -- Exceptions
    // None

    // -- Constructors

    Space1DAngleT();

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    void linearise(const Space::Matrix&, Space::Matrix&, double missingValue) const;

    void unlinearise(const Space::Matrix&, Space::Matrix&, double missingValue) const;

    size_t dimensions() const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Types
    // None

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


}  // namespace space
}  // namespace data
}  // namespace mir


#endif

