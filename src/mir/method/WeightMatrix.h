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


#pragma once

#include <iosfwd>
#include <vector>

#include "eckit/linalg/SparseMatrix.h"


namespace mir::method {


class WeightMatrix final : public eckit::linalg::SparseMatrix {
public:  // types
    using Triplet = eckit::linalg::Triplet;
    using Scalar  = eckit::linalg::Scalar;
    using Size    = eckit::linalg::Size;

public:  // methods
    WeightMatrix(SparseMatrix::Allocator* = nullptr);

    WeightMatrix(const eckit::PathName&);

    WeightMatrix(Size rows, Size cols);

    void setFromTriplets(const std::vector<WeightMatrix::Triplet>&);

    void cleanup(const double& pruneEpsilon = 0);

    void validate(const char* when) const;

private:  // members
    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& out, const WeightMatrix& m) {
        m.print(out);
        return out;
    }
};


}  // namespace mir::method
