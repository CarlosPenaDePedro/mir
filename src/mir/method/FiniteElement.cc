/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date Apr 2015

#include "mir/method/FiniteElement.h"

#include <iostream>
#include <string>

#include "eckit/log/Timer.h"

#include "atlas/Tesselation.h"
#include "atlas/geometry/TriangleIntersection.h"
#include "atlas/util/IndexView.h"

using atlas::geometry::TriangleIntersection;

namespace mir {
namespace method {

//----------------------------------------------------------------------------------------------------------------------

FiniteElement::FiniteElement(const param::MIRParametrisation& param) :
    MethodWeighted(param) {
}


FiniteElement::~FiniteElement() {
}

const char* FiniteElement::name() const {
    return "finite-element";
}

void FiniteElement::hash( eckit::MD5& md5) const {
    MethodWeighted::hash(md5);
}

bool FiniteElement::project_point_to_triangle(Point& p, Eigen::Vector3d& phi, int idx[3], const size_t k) const {

    bool found = false;

    atlas::IndexView<int,2> triag_nodes ( *ptriag_nodes );
    atlas::ArrayView<double,2> icoords  ( *picoords     );

    if (k > 1000000) {
        eckit::Log::info() << "FiniteElement::project_point_to_triangle " << k << std::endl;
    }
    atlas::PointIndex3::NodeList cs = pTree_->kNearestNeighbours(p, k);

    // find in which triangle the point is contained
    // by computing the intercetion of the point with each nearest triangle

    TriangleIntersection::Intersection uvt;
    TriangleIntersection::Ray ray( p.data() );

    size_t tid = std::numeric_limits<size_t>::max();

    for ( size_t i = 0; i < cs.size(); ++i ) {

        tid = cs[i].value().payload();

        ASSERT( tid < nb_triags_ );

        idx[0] = triag_nodes(tid, 0);
        idx[1] = triag_nodes(tid, 1);
        idx[2] = triag_nodes(tid, 2);

        ASSERT( idx[0] < inp_npts_ && idx[1] < inp_npts_ && idx[2] < inp_npts_ );

        TriangleIntersection triag( icoords[idx[0]].data() , icoords[idx[1]].data(), icoords[idx[2]].data() );

        found = triag.intersects( ray, uvt );

        if (found) { // weights are the baricentric cooridnates u,v
            phi[0] = uvt.w();
            phi[1] = uvt.u;
            phi[2] = uvt.v;
            break;
        }

    } // loop over nearest triangles

    return found;
}


void FiniteElement::assemble(WeightMatrix& W, const atlas::Grid& in, const atlas::Grid& out) const {
    // FIXME arguments:
    eckit::Log::info() << "FiniteElement::assemble" << std::endl;



    const atlas::Mesh& i_mesh = in.mesh();
    const atlas::Mesh& o_mesh = out.mesh();

    eckit::Timer t("compute weights");

    // generate mesh ...

    atlas::Tesselation::tesselate( const_cast<atlas::Grid&>(in) ); // OOPS!

    // generate baricenters of each triangle & insert the baricenters on a kd-tree

    atlas::Tesselation::create_cell_centres( const_cast<atlas::Mesh&>(i_mesh) ); // OOPS!

    pTree_.reset( create_cell_centre_index( const_cast<atlas::Mesh&>(i_mesh) ) ); // OOPS!

    // input mesh

    atlas::FunctionSpace&  i_nodes  = i_mesh.function_space( "nodes" );
    picoords = &i_nodes.field<double>( "xyz" );

    atlas::FunctionSpace& triags = i_mesh.function_space( "triags" );

    ptriag_nodes = &triags.field<int>( "nodes" );

    nb_triags_ = triags.shape(0);
    inp_npts_ = i_nodes.shape(0);

    // output mesh

    atlas::FunctionSpace&  o_nodes  = o_mesh.function_space( "nodes" );
    atlas::ArrayView<double, 2> ocoords ( o_nodes.field( "xyz" ) );

    const size_t out_npts = o_nodes.shape(0);

    // weights

    std::vector< Eigen::Triplet<double> > weights_triplets; /* structure to fill-in sparse matrix */

    weights_triplets.reserve( out_npts * 3 ); /* each row has 3 entries: one per vertice of triangle */

    /* search nearest k cell centres */

    // boost::progress_display show_progress( out_npts );

    for ( ip_ = 0; ip_ < out_npts; ++ip_ ) {

        int idx[3]; /* indexes of the triangle that will contain the point*/
        Eigen::Vector3d phi;
        Point p ( ocoords[ip_].data() ); // lookup point

        size_t factorial = 1;
        size_t n = 1;
        while ( ! project_point_to_triangle( p, phi, idx, factorial ) ) {
            factorial *= ++n;
        }

//        ++show_progress;

        // insert the interpolant weights into the global (sparse) interpolant matrix

        for (int i = 0; i < 3; ++i)
            weights_triplets.push_back( Eigen::Triplet<double>( ip_, idx[i], phi[i] ) );
    }

    // fill-in sparse matrix

    W.setFromTriplets(weights_triplets.begin(), weights_triplets.end());
}


void FiniteElement::print(std::ostream& out) const {
    out << "FiniteElement[]";
}


namespace {
static MethodBuilder< FiniteElement > __finiteelement("finite-element");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace method
}  // namespace mir

