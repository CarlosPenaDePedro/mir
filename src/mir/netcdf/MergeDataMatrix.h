/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - ECMWF Jan 2015

#ifndef mir_netcdf_MergeDataMatrix
#define mir_netcdf_MergeDataMatrix

#include "mir/netcdf/Matrix.h"


namespace mir {
namespace netcdf {

class MergeDataMatrix : public Matrix {
public:

    MergeDataMatrix(Matrix *out, Matrix *in, size_t size);
    virtual ~MergeDataMatrix();

private:

    Matrix *out_;
    Matrix *in_;

    virtual void print(std::ostream &out) const;

    virtual void fill(Mapper<double> &) const;
    virtual void fill(Mapper<float> &) const;
    virtual void fill(Mapper<long> &) const;
    virtual void fill(Mapper<short> &) const;
    virtual void fill(Mapper<unsigned char> &) const;

    template<class T> void _fill(Mapper<T> &v) const;

    virtual void dumpTree(std::ostream &, size_t) const;

};

}
}
#endif
