/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_method_decompose_DecomposeNone_h
#define mir_method_decompose_DecomposeNone_h

#include "mir/method/decompose/Decompose.h"


namespace mir {
namespace method {
namespace decompose {


class DecomposeNone : public Decompose {
public:

    // -- Exceptions
    // None

    // -- Constructors

    DecomposeNone();

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    void decompose(const WeightMatrix::Matrix&, WeightMatrix::Matrix&, double missingValue=std::numeric_limits<double>::quiet_NaN()) const;

    void recompose(const WeightMatrix::Matrix&, WeightMatrix::Matrix&, double missingValue=std::numeric_limits<double>::quiet_NaN()) const;

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


}  // namespace decompose
}  // namespace method
}  // namespace mir


#endif

