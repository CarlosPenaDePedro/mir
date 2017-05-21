/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date   May 2016


#ifndef mir_method_GridSpace_h
#define mir_method_GridSpace_h

#include <vector>
#include "eckit/memory/NonCopyable.h"


namespace atlas {
namespace grid { class Grid; }
namespace mesh { class Mesh; }
namespace array { template< typename DATA_TYPE, int RANK > class ArrayView; }
}
namespace mir {
namespace method {
class MethodWeighted;
}
}


namespace mir {
namespace method {


class GridSpace : private eckit::NonCopyable {
public:

    // -- Contructors

    GridSpace(const atlas::grid::Grid&, const MethodWeighted&);

    // -- Methods

    const atlas::grid::Grid& grid() const;
    atlas::mesh::Mesh& mesh() const;
    const std::vector<double>& coords() const;
    atlas::array::ArrayView<double,2> coordsLonLat() const;
    atlas::array::ArrayView<double,2> coordsXYZ() const;

private:

    // -- Members

    const MethodWeighted& method_;
    const atlas::grid::Grid& grid_;
    mutable atlas::mesh::Mesh* mesh_;
    mutable std::vector<double> coordsLonLat_;
    mutable std::vector<double> coordsXYZ_;

};


}  // namespace method
}  // namespace mir


#endif

