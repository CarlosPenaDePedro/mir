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


#include "mir/method/Matrix.h"

#include "eckit/log/JSON.h"
#include "eckit/utils/MD5.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Exceptions.h"


namespace mir::method {


static const MethodBuilder<Matrix> __builder("matrix");


Matrix::Matrix(const param::MIRParametrisation& param) : MethodWeighted(param) {
    std::string matrix;
    if (!parametrisation_.get("interpolation-matrix", matrix)) {
        throw exception::UserError("Matrix: option interpolation-matrix missing");
    }

    matrix_ = matrix;
    if (!matrix_.exists()) {
        throw exception::UserError("Matrix: path does not exist '" + matrix + "'");
    }

    matrix_path_ = matrix_.realName().asString();
}


void Matrix::json(eckit::JSON& j) const {
    j.startObject();
    j << "interpolation-matrix=" << matrix_;
    MethodWeighted::json(j);
    j.endObject();
}


const char* Matrix::name() const {
    return "matrix";
}


MethodWeighted::CacheKeys Matrix::getDiskAndMemoryCacheKeys(const repres::Representation& in,
                                                            const repres::Representation& out,
                                                            const lsm::LandSeaMasks&) const {
    return {matrix_path_, matrix_path_};
}


void Matrix::assemble(util::MIRStatistics&, WeightMatrix&, const repres::Representation& in,
                      const repres::Representation& out) const {
    NOTIMP;
}


void Matrix::hash(eckit::MD5& h) const {
    h << matrix_path_;
    MethodWeighted::hash(h);
}


bool Matrix::sameAs(const Method& other) const {
    const auto* o = dynamic_cast<const Matrix*>(&other);
    return (o != nullptr) && matrix_path_ == o->matrix_path_ && MethodWeighted::sameAs(other);
}


void Matrix::print(std::ostream& out) const {
    out << "Matrix["
        << "InterpolationMatrix=" << matrix_ << ",";
    MethodWeighted::print(out);
    out << "]";
}


}  // namespace mir::method
