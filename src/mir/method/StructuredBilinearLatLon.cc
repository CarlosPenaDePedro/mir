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
/// @date July 2015


#include "mir/method/StructuredBilinearLatLon.h"

#include <vector>
#include "eckit/log/BigNum.h"
#include "eckit/log/Log.h"
#include "eckit/types/FloatCompare.h"
#include "atlas/array/ArrayView.h"
#include "atlas/array_fwd.h"
#include "atlas/grid.h"
#include "mir/action/context/Context.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "mir/util/Compare.h"
#include "mir/util/MIRGrid.h"


namespace mir {
namespace method {


namespace {
static MethodBuilder< StructuredBilinearLatLon > __method("bilinear");
}


StructuredBilinearLatLon::StructuredBilinearLatLon(const param::MIRParametrisation& param) :
    StructuredMethod(param) {
}


StructuredBilinearLatLon::~StructuredBilinearLatLon() {
}


void StructuredBilinearLatLon::assembleStructuredInput(WeightMatrix& W, const repres::Representation& rin, const repres::Representation& rout) const {

    atlas::grid::StructuredGrid in(rin.grid());
    ASSERT(in);

    atlas::Grid out(rout.grid());
    ASSERT(out);


//    eckit::Log::debug<LibMir>() << "StructuredBilinearLatLon::assemble (input: " << in<< ", output: " << out << ")" << std::endl;
    // NOTE: use bilinear interpolation assuming quasi-regular grid
    // (this assumes the points are oriented north-south)
    // FIXME: proper documentation



    const std::vector<long>& lons = in.nx();
    const size_t inpts = in.size();

    ASSERT(lons.size());
    ASSERT(lons.front());
    ASSERT(lons.back());


    // get input coordinates, checking min/max latitudes (Gaussian grids exclude the poles)
    std::vector<point_ll_t> icoords(in.size());
    Latitude min_lat = 0.;
    Latitude max_lat = 0.;
    {
        eckit::ScopedPtr<repres::Iterator> it(rin.unrotatedIterator());
        Latitude lat;
        Longitude lon;
        size_t i = 0;

        while (it->next(lat, lon)) {
            if (!i || lat < min_lat) min_lat = lat;
            if (!i || lat > max_lat) max_lat = lat;
            ASSERT(i < icoords.size());
            icoords[i++] = point_ll_t(lat, lon);
        }

        ASSERT(min_lat < max_lat);
    }
    eckit::Log::debug<LibMir>() << "StructuredBilinearLatLon::assemble max_lat=" << max_lat << ", min_lat=" << min_lat << std::endl;


    // set northern & southern-most parallel point indices
    std::vector<size_t> parallel_north(lons.front());
    std::vector<size_t> parallel_south(lons.back());

    eckit::Log::debug<LibMir>() << "StructuredBilinearLatLon::assemble first row: " << lons.front() << std::endl;
    for (long i = 0; i < lons.front(); ++i) {
        parallel_north[i] = size_t(i);
    }

    eckit::Log::debug<LibMir>() << "StructuredBilinearLatLon::assemble last row: " << lons.back() << std::endl;
    for (long i = lons.back(), j = 0; i > 0; i--, j++) {
        parallel_south[j] = size_t(inpts - i);
    }

//    std::ofstream outfile ("mir.coeffs");
//    outfile.precision(2);


    // fill sparse matrix using triplets (reserve assuming all-quadrilateral interpolations)
    triplets_t triplets; /* structure to fill-in sparse matrix */
    triplets.reserve(4 * out.size());


    // interpolate each output point in turn
    eckit::ScopedPtr<repres::Iterator> it(rout.unrotatedIterator());
    Latitude lat;
    Longitude lon;
    size_t i = 0;

    while (it->next(lat, lon)) {
        ASSERT(i < out.size());

        const bool too_much_north = lat > max_lat;
        const bool too_much_south = lat < min_lat;

        if (too_much_north || too_much_south) {
            ASSERT(too_much_north != too_much_south);

            const std::vector<size_t>& par(too_much_north ? parallel_north : parallel_south);
            ASSERT(par.size());

            const double w = 1. / double(par.size());
            for (const size_t& j: par) {
                triplets.push_back( WeightMatrix::Triplet(i, j, w) );
            }

//            outfile << std::fixed
//                    << " " << (size_t) lat * 100 << " "
//                    << w << " "
//                    << w << " "
//                    << w << " "
//                    << w << std::endl;

        } else {

            // find encompassing latitudes ("bottom/top")

            size_t top_n = 0;  // number of points in top latitude line
            size_t bot_n = 0;  // number of points in bottom latitude line

            size_t top_i = 0;  // index of first point in top latitude line
            size_t bot_i = 0;  // index of first point in bottom latitude line

            Latitude top_lat = 0;
            Latitude bot_lat = 0;

            ASSERT(lons.size() >= 2); // at least 2 lines of latitude

            if( eckit::types::is_approximately_equal(max_lat.value(), lat.value()) ) {

                top_n = lons[0];
                bot_n = lons[1];
                top_i = 0;
                bot_i = top_i + top_n;

            } else if( eckit::types::is_approximately_equal(min_lat.value(), lat.value()) ) {

                top_n = lons[ lons.size() - 2 ];
                bot_n = lons[ lons.size() - 1 ];
                bot_i = inpts - bot_n;
                top_i = bot_i - top_n;

            } else {

                top_lat = icoords[top_i].first;
                bot_lat = icoords[bot_i].first;

                size_t n = 1;
                while ( !( bot_lat < lat && ( top_lat > lat || eckit::types::is_approximately_equal(top_lat.value(), lat.value())))
                        && n != lons.size() ) {

                    top_n = lons[n - 1];
                    bot_n = lons[n];

                    top_i  = bot_i;
                    bot_i += lons[n - 1];

                    top_lat = icoords[top_i].first;
                    bot_lat = icoords[bot_i].first;

                    ASSERT(top_lat > bot_lat);

                    ++n;
                }
            }

            top_lat = icoords[top_i].first;
            bot_lat = icoords[bot_i].first;

            ASSERT( top_lat > bot_lat );

            // find encompassing longitudes ("left/right")
            // -------------------------------------------

            // set left/right point indices, on the upper latitude
            size_t top_i_lft = 0;
            size_t top_i_rgt = 0;

            left_right_lon_indexes(lon, icoords, top_i, top_i + top_n, top_i_lft, top_i_rgt);

            // set left/right point indices, on the lower latitude
            size_t bot_i_lft = 0;
            size_t bot_i_rgt = 0;

            left_right_lon_indexes(lon, icoords, bot_i, bot_i + bot_n , bot_i_lft, bot_i_rgt);

            // now we have the indices of the input points around the output point


            // bilinear interpolation
            // ----------------------

            ASSERT(bot_i_rgt < inpts);
            ASSERT(bot_i_lft < inpts);
            ASSERT(top_i_rgt < inpts);
            ASSERT(top_i_lft < inpts);

            Longitude tl_lon  = icoords[top_i_lft].second;
            Longitude tr_lon  = icoords[top_i_rgt].second;
            Longitude bl_lon  = icoords[bot_i_lft].second;
            Longitude br_lon  = icoords[bot_i_rgt].second;

            if( tr_lon < tl_lon ) tr_lon += 360;
            if( br_lon < bl_lon ) br_lon += 360;

            // calculate the weights
            Longitude w1 =  lon - tl_lon;
            Longitude w2 =  tr_lon - lon;
            Longitude w3 =  lon - bl_lon;
            Longitude w4 =  br_lon - lon;
            ASSERT(w1 >= 0);
            ASSERT(w2 >= 0);
            ASSERT(w3 >= 0);
            ASSERT(w4 >= 0);

            // top and bottom midpoint weights
            Latitude wt = lat - bot_lat;
            Latitude wb = top_lat - lat;
            ASSERT(wt >= 0);
            ASSERT(wb >= 0);

            // weights for the tl, tr, bl, br points
            double w_br =  w3.value() * wb.value();
            double w_bl =  w4.value() * wb.value();
            double w_tr =  w1.value() * wt.value();
            double w_tl =  w2.value() * wt.value();

            //            eckit::Log::info() << " --> LL "
            //                      << lon << " ["
            //                      << tl_lon << "/" << tr_lon << ","
            //                      << bl_lon << "/" << br_lon << "] "
            //                      << lat << " ["
            //                      << top_lat << ","
            //                      << bot_lat << "] : "
            //                      << top_i_lft << " "
            //                      << top_i_rgt << " "
            //                      << bot_i_lft << " "
            //                      << bot_i_rgt << " "
//                      << " *** "
//                      << w1 << " "
//                      << w2 << " "
//                      << w3 << " "
//                      << w4 << " "
//                      << wt << " "
//                      << wb << " "
//                      << std::endl;

            triplets_t trip({ WeightMatrix::Triplet( i, bot_i_rgt, w_br ),
                              WeightMatrix::Triplet( i, bot_i_lft, w_bl ),
                              WeightMatrix::Triplet( i, top_i_rgt, w_tr ),
                              WeightMatrix::Triplet( i, top_i_lft, w_tl ) });

            // insert local point weights (normalized) into matrix "filler"
            normalise(trip);
            std::copy(trip.begin(), trip.end(), std::back_inserter(triplets));
        }

        ++i;
    }

    // outfile.close();

    // fill sparse matrix
    W.setFromTriplets(triplets);
}


const char* StructuredBilinearLatLon::name() const {
    return  "bilinear";
}


void StructuredBilinearLatLon::hash(eckit::MD5& md5) const {
    StructuredMethod::hash(md5);
}


void StructuredBilinearLatLon::print(std::ostream& out) const {
    out << "StructuredBilinearLatLon[]";
}


}  // namespace method
}  // namespace mir

