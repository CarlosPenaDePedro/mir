/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef mir_InverseSquare_H
#define mir_InverseSquare_H

#include <Eigen/Sparse>

#include "mir/Weights.h"

//-----------------------------------------------------------------------------

namespace atlas {
namespace grid {
    class Point2;
}
}

namespace mir {

//-----------------------------------------------------------------------------

class InverseSquare: public WeightEngine {

public:

    InverseSquare();
    virtual ~InverseSquare();

    virtual void compute( atlas::Mesh& i_mesh, atlas::Mesh& o_mesh, Eigen::SparseMatrix<double>& W ) const;

    virtual std::string classname() const;

private:

    size_t nclosest_; ///< number of closest points to search for

};

//-----------------------------------------------------------------------------

} // namespace mir

#endif
