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


#include "mir/method/WeightMatrix.h"

#include <cmath>

#include "eckit/linalg/LinearAlgebra.h"
#include "eckit/linalg/Vector.h"
#include "eckit/types/FloatCompare.h"

#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/Types.h"


namespace mir {
namespace method {


WeightMatrix::WeightMatrix(SparseMatrix::Allocator* alloc) : SparseMatrix(alloc) {}


WeightMatrix::WeightMatrix(const eckit::PathName& path) {
    load(path);
}


WeightMatrix::WeightMatrix(WeightMatrix::Size rows, WeightMatrix::Size cols) : SparseMatrix(rows, cols) {}


void WeightMatrix::setFromTriplets(const std::vector<WeightMatrix::Triplet>& triplets) {
    ASSERT(rows());
    ASSERT(cols());

    SparseMatrix M(rows(), cols(), triplets);

    swap(M);
}


void WeightMatrix::print(std::ostream& os) const {
    os << "WeightMatrix[";
    SparseMatrix::print(os);
    os << "]";
}


void WeightMatrix::multiply(const WeightMatrix::Vector& values, WeightMatrix::Vector& result) const {

    /// @todo linear algebra backend should depend on parametrisation
    eckit::linalg::LinearAlgebra::backend().spmv(*this, values, result);
}


void WeightMatrix::multiply(const WeightMatrix::Matrix& values, WeightMatrix::Matrix& result) const {

    // Log::debug() << "MethodWeighted::multiply: "
    //                                "A[" << rows()        << ',' << cols()        << "] * "
    //                                "B[" << values.rows() << ',' << values.cols() << "] = "
    //                                "C[" << result.rows() << ',' << result.cols() << "]" << std::endl;

    // Log::info() << "Multiply: "
    //                                "A[" << rows()        << ',' << cols()        << "] * "
    //                                "B[" << values.rows() << ',' << values.cols() << "] = "
    //                                "C[" << result.rows() << ',' << result.cols() << "]" << std::endl;

    ASSERT(values.rows() == cols());
    ASSERT(result.rows() == rows());
    ASSERT(values.cols() == result.cols());

    // when interpolating, the general case is for single-column values/result vectors
    if (values.cols() == 1) {
        // FIXME: remove this const cast once Vector provides read-only view
        eckit::linalg::Vector vi(const_cast<double*>(values.data()), values.rows());
        eckit::linalg::Vector vo(result.data(), result.rows());

        eckit::linalg::LinearAlgebra::backend().spmv(*this, vi, vo);
    }
    else {
        eckit::linalg::LinearAlgebra::backend().spmm(*this, values, result);
    }
}


void WeightMatrix::cleanup(const double& pruneEpsilon) {

    size_t fixed = 0;
    size_t count = 0;

    for (Size i = 0; i < rows(); ++i) {

        double removed  = 0;
        size_t non_zero = 0;

        for (iterator it = begin(i); it != end(i); ++it) {
            const double a = *it;
            if (fabs(a) < pruneEpsilon) {
                if (fabs(a) > 0) {
                    removed += a;
                    *it = 0;
                    fixed++;
                }
            }
            else {
                non_zero++;
            }
            count++;
        }

        if ((removed != 0.) && (non_zero > 0)) {
            double d = removed / double(non_zero);
            for (iterator it = begin(i); it != end(i); ++it) {
                const double a = *it;
                if (a != 0.) {
                    *it = a + d;
                }
            }
        }
    }

    if (fixed > 0) {
        size_t r     = rows();
        size_t c     = cols();
        size_t total = r * c;
        Log::debug() << "WeightMatrix::cleanup fixed " << Log::Pretty(fixed, {"value"}) << " out of "
                     << Log::Pretty(count) << " (matrix is " << Log::Pretty(r) << "x" << Log::Pretty(c)
                     << ", total=" << Log::Pretty(total) << ")" << std::endl;
    }

    prune(0.);
}


void WeightMatrix::validate(const char* when) const {

    bool logErrors = (Log::debug());

    size_t errors = 0;

    for (Size i = 0; i < rows(); i++) {

        // check for W(i,j)<0, or W(i,j)>1, or sum(W(i,:))!=(0,1)
        double sum = 0.;
        bool ok    = true;

        for (const_iterator it = begin(i); it != end(i); ++it) {
            const double& a = *it;
            ok &= eckit::types::is_approximately_greater_or_equal(a, 0.) &&
                  eckit::types::is_approximately_greater_or_equal(1., a);
            sum += a;
        }

        ok &= (eckit::types::is_approximately_equal(sum, 0.) || eckit::types::is_approximately_equal(sum, 1.));

        // log issues, per row
        if (!ok && logErrors) {

            if (errors < 50) {
                if (errors == 0) {
                    Log::debug() << "WeightMatrix::validate(" << when << ") failed " << std::endl;
                }

                Log::debug() << "Row: " << i;
                size_t n = 0;
                for (const_iterator it = begin(i); it != end(i); ++it, ++n) {
                    if (n > 10) {
                        Log::debug() << " ...";
                        break;
                    }
                    Log::debug() << " [" << *it << "]";
                }

                Log::debug() << " sum=" << sum << ", 1-sum " << (1 - sum) << std::endl;
            }
            else if (errors == 50) {
                Log::debug() << "..." << std::endl;
            }
            errors++;
        }
    }
}


}  // namespace method
}  // namespace mir
