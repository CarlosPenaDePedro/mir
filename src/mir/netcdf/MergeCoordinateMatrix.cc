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


#include "mir/netcdf/MergeCoordinateMatrix.h"

#include <ostream>

#include "mir/netcdf/Mapper.h"
#include "mir/netcdf/Type.h"


namespace mir {
namespace netcdf {


MergeCoordinateMatrix::MergeCoordinateMatrix(Matrix* out, Matrix* in, size_t size) :
    Matrix(Type::lookup(out->type(), in->type()), out->name(), size),
    out_(out),
    in_(in) {
    out_->attach();
    in_->attach();
}


MergeCoordinateMatrix::~MergeCoordinateMatrix() {
    out_->detach();
    in_->detach();
}


void MergeCoordinateMatrix::print(std::ostream& out) const {
    out << "MergeCoordinateMatrix[name=" << name_ << ", ptr=" << this << ", size=" << size_ << "]";
}


void MergeCoordinateMatrix::dumpTree(std::ostream& out, size_t depth) const {
    out << std::string(depth, ' ') << *this << std::endl;
    out_->dumpTree(out, depth + 1);
    in_->dumpTree(out, depth + 1);
}


}  // namespace netcdf
}  // namespace mir
