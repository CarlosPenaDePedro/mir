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


#ifndef mir_method_MethodWeighted_h
#define mir_method_MethodWeighted_h

#include <memory>
#include <vector>

#include "mir/data/MIRValuesVector.h"
#include "mir/method/Cropping.h"
#include "mir/method/Method.h"
#include "mir/method/WeightMatrix.h"


namespace mir {
namespace data {
class Space;
}
namespace lsm {
class LandSeaMasks;
}
namespace method {
namespace nonlinear {
class NonLinear;
}
}  // namespace method
namespace repres {
class Representation;
}
namespace util {
class MIRStatistics;
}
}  // namespace mir


namespace mir {
namespace method {


class MethodWeighted : public Method {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit MethodWeighted(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~MethodWeighted();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual void hash(eckit::MD5&) const;

    virtual int version() const;

    virtual const WeightMatrix& getMatrix(context::Context&, const repres::Representation& in,
                                          const repres::Representation& out) const;

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

    virtual const char* name() const = 0;
    void addNonLinearTreatment(const nonlinear::NonLinear*);

    // -- Overridden methods

    // From Method
    virtual bool sameAs(const Method&) const = 0;
    virtual void print(std::ostream&) const  = 0;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    double lsmWeightAdjustment_;
    double pruneEpsilon_;
    Cropping cropping_;

    std::vector<std::unique_ptr<const nonlinear::NonLinear>> nonLinear_;

    bool matrixValidate_;
    bool matrixAssemble_;

    // -- Methods

    virtual bool canIntroduceMissingValues() const;
    virtual void assemble(util::MIRStatistics&, WeightMatrix&, const repres::Representation& in,
                          const repres::Representation& out) const = 0;
    virtual void applyMasks(WeightMatrix&, const lsm::LandSeaMasks&) const;
    virtual lsm::LandSeaMasks getMasks(const repres::Representation& in, const repres::Representation& out) const;
    virtual bool validateMatrixWeights() const;

    void computeMatrixWeights(context::Context&, const repres::Representation& in, const repres::Representation& out,
                              WeightMatrix&, bool validate) const;
    void createMatrix(context::Context&, const repres::Representation& in, const repres::Representation& out,
                      WeightMatrix&, const lsm::LandSeaMasks&, const Cropping&) const;

    /// Get interpolation operand matrices, from A = W B
    virtual void setOperandMatricesFromVectors(WeightMatrix::Matrix& A, WeightMatrix::Matrix& B,
                                               const MIRValuesVector& Avector, const MIRValuesVector& Bvector,
                                               const double& missingValue, const data::Space&) const;

    /// Get interpolation operand matrices, from A = W B
    virtual void setVectorFromOperandMatrix(const WeightMatrix::Matrix& A, MIRValuesVector& Avector,
                                            const double& missingValue, const data::Space&) const;

    // -- Overridden methods

    // From Method
    virtual void execute(context::Context&, const repres::Representation& in, const repres::Representation& out) const;
    virtual bool canCrop() const;
    virtual void setCropping(const mir::util::BoundingBox&);
    virtual bool hasCropping() const;
    virtual const util::BoundingBox& getCropping() const;


    /// Update matrix to account for field masked values
    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    friend class MatrixCacheCreator;
};


}  // namespace method
}  // namespace mir


#endif
