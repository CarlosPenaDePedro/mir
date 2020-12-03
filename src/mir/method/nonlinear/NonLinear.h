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


#ifndef mir_method_nonlinear_NonLinear_h
#define mir_method_nonlinear_NonLinear_h

#include <iosfwd>
#include <string>

#include "mir/data/MIRValuesVector.h"


namespace eckit {
class MD5;
namespace linalg {
class Matrix;
}
}  // namespace eckit

namespace mir {
namespace method {
class WeightMatrix;
}
namespace param {
class MIRParametrisation;
}
}  // namespace mir


namespace mir {
namespace method {
namespace nonlinear {


class NonLinear {
public:
    using WeightMatrix = method::WeightMatrix;
    using Matrix       = eckit::linalg::Matrix;

    NonLinear(const param::MIRParametrisation&);

    NonLinear(const NonLinear&) = delete;
    void operator=(const NonLinear&) = delete;

    virtual ~NonLinear();

    /// Update interpolation linear system to account for non-linearities
    virtual bool treatment(Matrix& A, WeightMatrix& W, Matrix& B, const data::MIRValuesVector& values,
                           const double& missingValue) const = 0;

    virtual bool sameAs(const NonLinear& other) const = 0;

    virtual void hash(eckit::MD5&) const = 0;

    virtual bool canIntroduceMissingValues() const;

private:
    virtual void print(std::ostream&) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const NonLinear& p) {
        p.print(s);
        return s;
    }
};


class NonLinearFactory {
private:
    std::string name_;
    virtual NonLinear* make(const param::MIRParametrisation&) = 0;

    NonLinearFactory(const NonLinearFactory&) = delete;
    NonLinearFactory& operator=(const NonLinearFactory&) = delete;

protected:
    NonLinearFactory(const std::string& name);
    virtual ~NonLinearFactory();

public:
    static const NonLinear* build(const std::string& name, const param::MIRParametrisation&);
    static void list(std::ostream&);
};


template <class T>
class NonLinearBuilder : public NonLinearFactory {
    virtual NonLinear* make(const param::MIRParametrisation& param) override { return new T(param); }

public:
    NonLinearBuilder(const std::string& name) : NonLinearFactory(name) {}
};


}  // namespace nonlinear
}  // namespace method
}  // namespace mir


#endif
