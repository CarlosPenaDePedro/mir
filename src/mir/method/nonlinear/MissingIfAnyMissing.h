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


#ifndef mir_method_nonlinear_MissingIfAnyMissing_h
#define mir_method_nonlinear_MissingIfAnyMissing_h

#include "mir/method/nonlinear/NonLinear.h"


namespace mir {
namespace method {
namespace nonlinear {


struct MissingIfAnyMissing : NonLinear {
    MissingIfAnyMissing(const param::MIRParametrisation&);

private:
    bool treatment(MethodWeighted::Matrix& A, MethodWeighted::WeightMatrix& W, MethodWeighted::Matrix& B,
                   const data::MIRValuesVector& values, const double& missingValue) const override;
    bool sameAs(const NonLinear&) const override;
    void print(std::ostream&) const override;
    void hash(eckit::MD5&) const override;
};


}  // namespace nonlinear
}  // namespace method
}  // namespace mir


#endif
