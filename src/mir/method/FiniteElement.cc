/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date May 2015


#include "mir/method/FiniteElement.h"

#include <algorithm>
#include <limits>
#include "eckit/config/Resource.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/ETA.h"
#include "eckit/log/Plural.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/MD5.h"
#include "atlas/grid/Structured.h"
#include "atlas/interpolation/PointIndex3.h"
#include "atlas/interpolation/Quad3D.h"
#include "atlas/interpolation/Ray.h"
#include "atlas/interpolation/Triag3D.h"
#include "atlas/mesh/ElementType.h"
#include "atlas/mesh/Elements.h"
#include "atlas/mesh/Nodes.h"
#include "atlas/mesh/actions/BuildCellCentres.h"
#include "atlas/mesh/actions/BuildXYZField.h"
#include "atlas/util/io/Gmsh.h"
#include "mir/config/LibMir.h"
#include "mir/method/GridSpace.h"
#include "mir/param/MIRParametrisation.h"
#include "eckit/log/ResourceUsage.h"


namespace mir {
namespace method {


namespace {


// try to project to 20% of total number elements before giving up
static const double maxFractionElemsToTry = 0.2;

// epsilon used to scale edge tolerance when projecting ray to intesect element
static const double parametricEpsilon = 1e-16;


enum { LON=0, LAT=1 };


typedef std::vector< WeightMatrix::Triplet > Triplets;


struct MeshStats {

    size_t nb_triags;
    size_t nb_quads;
    size_t inp_npts;
    size_t out_npts;

    MeshStats(): nb_triags(0), nb_quads(0), inp_npts(0), out_npts(0) {}

    size_t size() const {
        return nb_triags + nb_quads;
    }

    void print(std::ostream &s) const {
        s << "MeshStats[nb_triags=" << eckit::BigNum(nb_triags)
          << ",nb_quads=" << eckit::BigNum(nb_quads)
          << ",inp_npts=" << eckit::BigNum(inp_npts)
          << ",out_npts=" << eckit::BigNum(out_npts) << "]";
    }

    friend std::ostream &operator<<(std::ostream &s, const MeshStats &p) {
        p.print(s);
        return s;
    }
};


static void normalise(Triplets& triplets)
{
    // sum all calculated weights for normalisation
    double sum = 0.0;

    for (size_t j = 0; j < triplets.size(); ++j) {
        sum += triplets[j].value();
    }

    // now normalise all weights according to the total
    const double invSum = 1.0 / sum;
    for (size_t j = 0; j < triplets.size(); ++j) {
        triplets[j].value() *= invSum;
    }
}


/// Find in which element the point is contained by projecting the point with each nearest element
static Triplets projectPointToElements(
        const MeshStats &stats,
        const atlas::array::ArrayView<double, 2> &icoords,
        const atlas::mesh::Elements::Connectivity &triag_nodes,
        const atlas::mesh::Elements::Connectivity &quads_nodes,
        const FiniteElement::Point &p,
        size_t ip,
        size_t firstVirtualPoint,
        atlas::interpolation::ElemIndex3::NodeList::const_iterator start,
        atlas::interpolation::ElemIndex3::NodeList::const_iterator finish ) {

    ASSERT(start != finish);

    Triplets triplets;

    bool mustNormalise = false;
    size_t idx[4];
    double w[4];
    atlas::interpolation::Ray ray( p.data() );

    for (atlas::interpolation::ElemIndex3::NodeList::const_iterator itc = start; itc != finish; ++itc) {

        atlas::interpolation::ElemPayload elem = (*itc).value().payload();

        if ( elem.triangle() ) { /* triags */

            const size_t &tid = elem.id_;

            ASSERT( tid < stats.nb_triags );

            idx[0] = triag_nodes(tid, 0);
            idx[1] = triag_nodes(tid, 1);
            idx[2] = triag_nodes(tid, 2);

            ASSERT( idx[0] < stats.inp_npts && idx[1] < stats.inp_npts && idx[2] < stats.inp_npts );

            atlas::interpolation::Triag3D triag(
                    icoords[idx[0]].data(),
                    icoords[idx[1]].data(),
                    icoords[idx[2]].data());

            // pick an epsilon based on a characteristic length (sqrt(area))
            // (this scales linearly so it better compares with linear weights u,v,w)
            const double edgeEpsilon = parametricEpsilon * std::sqrt(triag.area());
            ASSERT(edgeEpsilon >= 0);

            atlas::interpolation::Intersect is = triag.intersects(ray, edgeEpsilon);

            if (is) {

                // weights are the linear Lagrange function evaluated at u,v (aka barycentric coordinates)
                w[0] = 1. - is.u - is.v;
                w[1] = is.u;
                w[2] = is.v;

                for (size_t i = 0; i < 3; ++i)
                {
                    if(idx[i] < firstVirtualPoint)
                        triplets.push_back( WeightMatrix::Triplet( ip, idx[i], w[i] ) );
                    else
                        mustNormalise = true;
                }

                break; // stop looking for elements
            }

        } else if( elem.quadrilateral() ) {  /* quads */

            const size_t &qid = elem.id_;

            ASSERT( qid < stats.nb_quads );

            idx[0] = quads_nodes(qid, 0);
            idx[1] = quads_nodes(qid, 1);
            idx[2] = quads_nodes(qid, 2);
            idx[3] = quads_nodes(qid, 3);

            ASSERT( idx[0] < stats.inp_npts && idx[1] < stats.inp_npts &&
                    idx[2] < stats.inp_npts && idx[3] < stats.inp_npts );

            atlas::interpolation::Quad3D quad(
                    icoords[idx[0]].data(),
                    icoords[idx[1]].data(),
                    icoords[idx[2]].data(),
                    icoords[idx[3]].data() );

            if ( !quad.validate() ) { // somewhat expensive sanity check
                eckit::Log::warning() << "Invalid Quad : " << quad << std::endl;
                throw eckit::SeriousBug("Found invalid quadrilateral in mesh", Here());
            }

            // pick an epsilon based on a characteristic length (sqrt(area))
            // (this scales linearly so it better compares with linear weights u,v,w)
            const double edgeEpsilon = parametricEpsilon * std::sqrt(quad.area());
            ASSERT(edgeEpsilon >= 0);

            atlas::interpolation::Intersect is = quad.intersects(ray, edgeEpsilon);

            if (is) {

                // weights are the bilinear Lagrange function evaluated at u,v
                w[0] = (1. - is.u) * (1. - is.v);
                w[1] =       is.u  * (1. - is.v);
                w[2] =       is.u  *       is.v ;
                w[3] = (1. - is.u) *       is.v ;


                for (size_t i = 0; i < 4; ++i)
                {
                    if(idx[i] < firstVirtualPoint)
                        triplets.push_back( WeightMatrix::Triplet( ip, idx[i], w[i] ) );
                    else
                        mustNormalise = true;
                }

                break; // stop looking for elements
            }
        } else {
            throw eckit::SeriousBug("Element type is not triangle or quadrilateral", Here());
        }

    } // loop over nearest elements

    // at least one of the nodes of element shouldn't be virtual
    if (!triplets.empty() && mustNormalise) {
        normalise(triplets);
    }

    return triplets;
}


}  // (anonymous namespace)


FiniteElement::MeshGenParams::MeshGenParams() {
    set("three_dimensional", true);
    set("patch_pole",        true);
    set("include_pole",      false);
    set("angle",             0.);
    set("triangulate",       false);
}


FiniteElement::FiniteElement(const param::MIRParametrisation &param) :
    MethodWeighted(param) {
}


FiniteElement::~FiniteElement() {
}


void FiniteElement::hash(eckit::MD5&) const
{
}


void FiniteElement::assemble(context::Context& ctx, WeightMatrix &W, const GridSpace& in, const GridSpace& out) const {

    // FIXME: arguments
    eckit::Log::debug<LibMir>() << "FiniteElement::assemble" << std::endl;

    eckit::Log::debug<LibMir>() << "  Input  Grid: " << in.grid()  << std::endl;
    eckit::Log::debug<LibMir>() << "  Output Grid: " << out.grid() << std::endl;

    const atlas::grid::Domain &inDomain = in.grid().domain();

    eckit::TraceTimer<LibMir> timer("Compute weights");

    // FIXME: using the name() is not the right thing, although it should work, but create too many cached meshes.
    // We need to use the mesh-generator
    {
        eckit::TraceTimer<LibMir> timer("Generate mesh");

        // generateMeshAndCache(in.grid(), in.mesh()); // mesh generation will be done lazily

        static bool dumpMesh = eckit::Resource<bool>("$MIR_DUMP_MESH", false);
        if (dumpMesh) {
            atlas::util::io::Gmsh gmsh;
            gmsh.options.set<std::string>("nodes", "xyz");

            eckit::Log::debug<LibMir>() << "Dumping input mesh to input.msh" << std::endl;
            gmsh.write(in.mesh(), "input.msh");

            eckit::Log::debug<LibMir>() << "Dumping output mesh to output.msh" << std::endl;
            atlas::mesh::actions::BuildXYZField("xyz")(out.mesh());
            gmsh.write(out.mesh(), "output.msh");
        }
    }

    // generate barycenters of each triangle & insert them on a kd-tree
    {
        eckit::ResourceUsage usage("create_cell_centres");
        eckit::TraceTimer<LibMir> timer("Tesselation::create_cell_centres");
        atlas::mesh::actions::BuildCellCentres()(in.mesh());
    }

    eckit::ScopedPtr<atlas::interpolation::ElemIndex3> eTree;
    {
        eckit::ResourceUsage usage("create_element_centre_index");
        eckit::TraceTimer<LibMir> timer("create_element_centre_index");
        eTree.reset( atlas::interpolation::create_element_centre_index(in.mesh()) );
    }

    // input mesh
    MeshStats stats;

    const atlas::mesh::Nodes  &i_nodes  = in.mesh().nodes();
    atlas::array::ArrayView<double, 2> icoords  ( i_nodes.field( "xyz" ));

    atlas::mesh::Elements::Connectivity const *triag_nodes;
    atlas::mesh::Elements::Connectivity const *quads_nodes;

    for( size_t jtype=0; jtype<in.mesh().cells().nb_types(); ++jtype )
    {
        const atlas::mesh::Elements& elements =  in.mesh().cells().elements(jtype);
        if( elements.element_type().name() == "Triangle" )
        {
            stats.nb_triags = elements.size();
            triag_nodes = &elements.node_connectivity();
        }
        if( elements.element_type().name() == "Quadrilateral" )
        {
            stats.nb_quads  = elements.size();
            quads_nodes = &elements.node_connectivity();
        }
    }

    size_t firstVirtualPoint = std::numeric_limits<size_t>::max();
    if( i_nodes.metadata().has("NbRealPts") )
        firstVirtualPoint = i_nodes.metadata().get<size_t>("NbRealPts");

    atlas::array::ArrayView<double, 2> ocoords = out.coordsXYZ();
    atlas::array::ArrayView<double, 2> olonlat = out.coordsLonLat();

    stats.inp_npts  = i_nodes.size();
    stats.out_npts  = out.grid().npts();

    eckit::Log::debug<LibMir>() << stats << std::endl;

    // weights -- one per vertex of element, triangles (3) or quads (4)

    std::vector< WeightMatrix::Triplet > weights_triplets; // structure to fill-in sparse matrix
    weights_triplets.reserve( stats.out_npts * 4 );        // preallocate space as if all elements where quads

    // search nearest k cell centres

    const size_t maxNbElemsToTry = std::max<size_t>(64, stats.size() * maxFractionElemsToTry);
    size_t max_neighbours = 0;

    eckit::Log::debug<LibMir>() << "Projecting " << eckit::Plural(stats.out_npts, "output point") << " to input mesh " << in.grid().shortName() << std::endl;

    {
        eckit::TraceTimer<LibMir> timerProj("Projecting");

        for ( size_t ip = 0; ip < stats.out_npts; ++ip ) {

            if (ip && (ip % 10000 == 0)) {
                double rate = ip / timerProj.elapsed();
                eckit::Log::debug<LibMir>() << eckit::BigNum(ip) << " ..."  << eckit::Seconds(timerProj.elapsed())
                                         << ", rate: " << rate << " points/s, ETA: "
                                         << eckit::ETA( (stats.out_npts - ip) / rate )
                                         << std::endl;
            }

            if (!inDomain.contains(olonlat[ip][LON], olonlat[ip][LAT])) {
                continue;
            }

            Point p ( ocoords[ip].data() ); // lookup point

            size_t kpts = 1;
            bool success = false;

            while (!success && kpts <= maxNbElemsToTry) {

                max_neighbours = std::max(kpts, max_neighbours);

                atlas::interpolation::ElemIndex3::NodeList cs = eTree->kNearestNeighbours(p, kpts);
                Triplets triplets = projectPointToElements(
                            stats,
                            icoords,
                            *triag_nodes,
                            *quads_nodes,
                            p,
                            ip,
                            firstVirtualPoint,
                            cs.begin(),
                            cs.end() );

                if (triplets.size()) {
                    std::copy(triplets.begin(), triplets.end(), std::back_inserter(weights_triplets));
                    success = true;
                }
                kpts *= 2;

            }

            if (!success) {
                // If this fails, consider lowering atlas::grid::parametricEpsilon
                eckit::Log::debug<LibMir>() << "Failed to project point " << ip << " " << p
                                         << " with coordinates lon=" << olonlat[ip][LON] << ", lat=" << olonlat[ip][LAT]
                                         << " after " << eckit::Plural(kpts, "attempt") << std::endl;
                throw eckit::SeriousBug("Could not project point");
            }
        }
    }

    eckit::Log::debug<LibMir>() << "Projected " << eckit::Plural(stats.out_npts, "point") << std::endl;
    eckit::Log::debug<LibMir>() << "Maximum neighbours searched was " << eckit::Plural(max_neighbours, "element") << std::endl;

    W.setFromTriplets(weights_triplets); // fill sparse matrix
}


void FiniteElement::generateMesh(const atlas::grid::Grid &grid, atlas::mesh::Mesh &mesh) const {

    eckit::ResourceUsage usage("FiniteElement::generateMesh");


    std::string meshgenerator( grid.getOptimalMeshGenerator() );

    parametrisation_.get("meshgenerator", meshgenerator); // Override with MIRParametrisation

    eckit::Log::debug<LibMir>() << "MeshGenerator parametrisation is '" << meshgenerator << "'" << std::endl;

    eckit::ScopedPtr<atlas::mesh::generators::MeshGenerator> generator(
                atlas::mesh::generators::MeshGeneratorFactory::build(meshgenerator, meshgenparams_) );
    generator->generate(grid, mesh);

    // If meshgenerator did not create xyz field already, do it now.
    atlas::mesh::actions::BuildXYZField()(mesh);
}


}  // namespace method
}  // namespace mir

