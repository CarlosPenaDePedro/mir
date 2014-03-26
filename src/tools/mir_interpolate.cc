#include <limits>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>

#include <boost/progress.hpp>

#include "atlas/Gmsh.hpp"
#include "atlas/Mesh.hpp"

#include "eckit/exception/Exceptions.h"
#include "eckit/config/Resource.h"
#include "eckit/log/Timer.h"
#include "eckit/runtime/Tool.h"

#include "atlas/grid/GribRead.h"
#include "atlas/grid/GribWrite.h"
#include "atlas/grid/PointIndex3.h"
#include "atlas/grid/PointSet.h"
#include "atlas/grid/TriangleIntersection.h"
#include "atlas/grid/Tesselation.h"

//------------------------------------------------------------------------------------------------------

#if 1
#define DBG     std::cout << Here() << std::endl;
#define DBGX(x) std::cout << #x << " -> " << x << std::endl;
#else
#define DBG
#define DBGX(x)
#endif

//------------------------------------------------------------------------------------------------------

using namespace Eigen;
using namespace atlas;
using namespace eckit;

//------------------------------------------------------------------------------------------------------

void compute_weights( atlas::Mesh& i_mesh,
                      atlas::Mesh& o_mesh,
                      Eigen::SparseMatrix<double>& W )
{
    Timer t("compute weights");

    // generate baricenters of each triangle & insert the baricenters on a kd-tree

    atlas::MeshGen::create_cell_centres( i_mesh );

    std::unique_ptr<PointIndex3> ptree ( create_cell_centre_index<PointIndex3>( i_mesh ) );
    PointIndex3& tree = *ptree;

    // input mesh

    FunctionSpace&  i_nodes  = i_mesh.function_space( "nodes" );
    FieldT<double>& icoords  = i_nodes.field<double>( "coordinates" );

    const size_t inp_npts = i_nodes.bounds()[1];

    FunctionSpace& triags      = i_mesh.function_space( "triags" );
    FieldT<int>& triag_nodes   = triags.field<int>( "nodes" );

    const size_t nb_triags = triags.bounds()[1];

    // output mesh

    FunctionSpace&  o_nodes  = o_mesh.function_space( "nodes" );
    FieldT<double>& ocoords  = o_nodes.field<double>( "coordinates" );

    const size_t out_npts = o_nodes.bounds()[1];

    // weights

    std::vector< Eigen::Triplet<double> > weights_triplets; /* structure to fill-in sparse matrix */

    weights_triplets.reserve( out_npts * 3 ); /* each row has 3 entries: one per vertice of triangle */

    /* search nearest k cell centres */

//    const size_t k = 4;
    const size_t k = 4 + ( inp_npts / 100 );
//    const size_t k = 100;

    std::ofstream of;
    of.open("found.txt");

    boost::progress_display show_progress( out_npts );

    for( size_t ip = 0; ip < out_npts; ++ip )
    {
        std::ostringstream os;

        KPoint3 p ( ocoords.slice(ip) ); // lookup point

#if 0
        std::cout << p << std::endl;
#endif

        // search the nearest 4 triangles

        PointIndex3::NodeList cs = tree.kNearestNeighbours(p,k);

#if 0
        for( size_t i = 0; i < cs.size(); ++i )
        {
            std::cout << cs[i] << std::endl;
        }
#endif

        // find in which triangle the point is contained
        // by computing the intercetion of the point with each nearest triangle

        int idx[3]; /* indexes of the triangle that will contain the point*/

        Vector3d phi;
        Isect uvt;
        Ray ray( p.data() );

        size_t tid = std::numeric_limits<size_t>::max();

        bool found = false;
        for( size_t i = 0; i < cs.size(); ++i )
        {
            tid = cs[i].value().payload();

            KPoint3 tc = cs[i].value().point();

            ASSERT( tid < nb_triags );

            idx[0] = triag_nodes(0,tid);
            idx[1] = triag_nodes(1,tid);
            idx[2] = triag_nodes(2,tid);

            ASSERT( idx[0] < inp_npts && idx[1] < inp_npts && idx[2] < inp_npts );

            Triag triag( icoords.slice(idx[0]), icoords.slice(idx[1]), icoords.slice(idx[2]) );

            if( triag_intersection( triag, ray, uvt ) )
            {
                found = true;
#if 1
                of << "[SUCCESS]" << std::endl
                   << "   i    " << i << std::endl
                   << "   ip   " << ip << std::endl
                   << "   p    " << p << std::endl
                   << "   tc   " << tc << std::endl
                   << "   d    " << KPoint3::distance(tc,p) << std::endl
                   << "   tid  " << tid << std::endl
                   << "   nidx " << idx[0] << " " << idx[1] << " " << idx[2] << std::endl
                   << "   " << KPoint3(icoords.slice(idx[0])) << " / "
                            << KPoint3(icoords.slice(idx[1])) << " / "
                            << KPoint3(icoords.slice(idx[2])) << std::endl
                   << "   uvwt " << uvt << std::endl;
#endif
                // weights are the baricentric cooridnates u,v

                phi[0] = uvt.w();
                phi[1] = uvt.u;
                phi[2] = uvt.v;

                break;
            }
            else
            {
                os << "[FAILED] projection on triangle:" << std::endl
                   << "   i    " << i << std::endl
                   << "   ip   " << ip << std::endl
                   << "   p    " << p << std::endl
                   << "   tc   " << tc << std::endl
                   << "   d    " << KPoint3::distance(tc,p) << std::endl
                   << "   tid  " << tid << std::endl
                   << "   nidx " << idx[0] << " " << idx[1] << " " << idx[2] << std::endl
                   << "   " << KPoint3(icoords.slice(idx[0])) << " / "
                            << KPoint3(icoords.slice(idx[1])) << " / "
                            << KPoint3(icoords.slice(idx[2])) << std::endl
                   << "   uvwt " << uvt << std::endl;
            }
        }

        ++show_progress;

#if 0
        if( found ) { std::cout << os.str() << std::endl; }
#endif

        if( !found )
        {
            std::cout << os.str() << std::endl;
            throw SeriousBug("Could not project point into nearest triangles");
        }

        // insert the interpolant weights into the global (sparse) interpolant matrix

        for(int i = 0; i < 3; ++i)
            weights_triplets.push_back( Eigen::Triplet<double>( ip, idx[i], phi[i] ) );

    }

    // fill-in sparse matrix

    W.setFromTriplets(weights_triplets.begin(), weights_triplets.end());
}

//------------------------------------------------------------------------------------------------------

class MirInterpolate : public eckit::Tool {

    virtual void run();

    void grib_load( const std::string& fname, atlas::Mesh& mesh, bool read_field = true );

public:

    MirInterpolate(int argc,char **argv): eckit::Tool(argc,argv) {}

};

//------------------------------------------------------------------------------------------------------

/// @todo this will become an expression object
void MirInterpolate::grib_load( const std::string& fname, atlas::Mesh& mesh, bool read_field )
{
    FILE* fh = ::fopen( fname.c_str(), "r" );
    if( fh == 0 )
        throw ReadError( std::string("error opening file ") + fname );

    int err = 0;
    grib_handle* h = grib_handle_new_from_file(0,fh,&err);

    if( h == 0 || err != 0 )
        throw ReadError( std::string("error reading grib file ") + fname );

    GribRead::read_nodes_from_grib( h, mesh );

    if( read_field )
        GribRead::read_field_from_grib( h, mesh, "field" );

    grib_handle_delete(h);

    // close file handle

    if( ::fclose(fh) == -1 )
        throw ReadError( std::string("error closing file ") + fname );
}

//------------------------------------------------------------------------------------------------------

void MirInterpolate::run()
{    
    std::string in_filename = Resource<std::string>("-in","");
    if( in_filename.empty() )
        throw UserError(Here(),"missing input filename, parameter -in");

    std::string out_filename = Resource<std::string>("-out","");
    if( out_filename.empty() )
        throw UserError(Here(),"missing output filename, parameter -out");

    std::string clone_grid = Resource<std::string>("-grid","");
    if( clone_grid.empty() )
        throw UserError(Here(),"missing clone grid filename, parameter -grid");

    std::cout.precision(std::numeric_limits< double >::digits10);
    std::cout << std::fixed;

    // input grid + field

    std::cout << ">>> reading input grid + field ..." << std::endl;

    std::unique_ptr< atlas::Mesh > in_mesh ( new Mesh() );

    grib_load( in_filename, *in_mesh );

    FunctionSpace&  i_nodes = in_mesh->function_space( "nodes" );
    FieldT<double>& ifield = i_nodes.field<double>("field");

    const size_t nb_i_nodes = i_nodes.bounds()[1];

    std::cout << "points " << nb_i_nodes << std::endl;

    // output grid + field

    std::cout << ">>> reading output grid ..." << std::endl;

    std::unique_ptr< atlas::Mesh > out_mesh ( new Mesh() );

    grib_load( out_filename, *out_mesh, false );

    FunctionSpace&  o_nodes = out_mesh->function_space( "nodes" );
    FieldT<double>& ofield = o_nodes.create_field<double>("field",1);

    const size_t nb_o_nodes = o_nodes.bounds()[1];

    std::cout << "points " << nb_o_nodes << std::endl;

    // generate mesh ...

    Tesselation::tesselate( *in_mesh );

    // input mesh --> gmsh

    atlas::Gmsh::write3dsurf( *in_mesh, "input.msh" );

    // compute weights for each point in output grid

    std::cout << ">>> computing weights ..." << std::endl;

    Eigen::SparseMatrix<double> W( nb_o_nodes, nb_i_nodes );

    compute_weights( *in_mesh, *out_mesh, W );

    // interpolation -- multiply interpolant matrix with field vector

    std::cout << ">>> interpolating ..." << std::endl;

    {
        Timer t("interpolation");

        VectorXd::MapType fi = VectorXd::Map( &(ifield.data())[0], ifield.data().size() );
        VectorXd::MapType fo = VectorXd::Map( &(ofield.data())[0], ofield.data().size() );

        fo = W * fi;
    }

    // output mesh --> gmsh

    std::cout << ">>> output to gmsh" << std::endl;

    Tesselation::tesselate( *out_mesh );
    atlas::Gmsh::write3dsurf( *out_mesh, std::string("output.msh") );

    // output mesh --> grib

    std::cout << ">>> output to grib" << std::endl;

    GribWrite::clone(*out_mesh,clone_grid,out_filename);

}

//------------------------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
    MirInterpolate tool(argc,argv);
    tool.start();
    return 0;
}

