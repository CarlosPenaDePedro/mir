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


#ifndef mir_netcdf_MergeDataMatrix_h
#define mir_netcdf_MergeDataMatrix_h

#include "mir/netcdf/Matrix.h"


namespace mir {
namespace netcdf {


class MergeDataMatrix : public Matrix {
public:
    MergeDataMatrix(Matrix* out, Matrix* in, size_t size);
    ~MergeDataMatrix() override;

private:
    Matrix* out_;
    Matrix* in_;

    void print(std::ostream&) const override;

    void dumpTree(std::ostream&, size_t) const override;
};


}  // namespace netcdf
}  // namespace mir


#endif
