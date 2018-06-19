/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - ECMWF Jan 2015

#include "mir/netcdf/Curvilinear.h"
#include "mir/netcdf/Variable.h"
#include "eckit/types/Types.h"
#include "mir/netcdf/Dimension.h"
#include "mir/netcdf/HyperCube.h"

#include <iostream>

namespace mir {
namespace netcdf {

Curvilinear::Curvilinear(const Variable &variable,
                         const std::vector<double>& latitudes,
                         const std::vector<double>& longitudes):
    GridSpec(variable),
    latitudes_(latitudes),
    longitudes_(longitudes)
{
    ASSERT(latitudes_.size() == longitudes_.size());
    ASSERT(latitudes_.size() >= 2);

    north_ = south_ = latitudes_[0];
    for (double d : latitudes_) {
        north_ = std::max(north_, d);
        south_ = std::min(south_, d);
    }

    east_ = west_ = longitudes_[0];
    for (double d : longitudes_) {
        east_ = std::max(east_, d);
        west_ = std::min(west_, d);
    }
}

Curvilinear::~Curvilinear() = default;


void Curvilinear::print(std::ostream& s) const
{
    s << "Curvilinear[points=" << latitudes_.size() << "]";
}

bool Curvilinear::has(const std::string& name) const {
    // std::cout << "has " << name << std::endl;
    if (name == "gridded") {
        return true;
    }

    // std::cout << "Curvilinear::has " << name << " failed" << std::endl;


    return false;
}

bool Curvilinear::get(const std::string &name, std::vector<double> &values) const {
    // std::cout << "get " << name << std::endl;

    if (name == "latitudes") {
        values = latitudes_;
        return true;
    }

    if (name == "longitudes") {
        values = longitudes_;
        return true;
    }

    return false;

}

bool Curvilinear::get(const std::string&name, long& value) const {
    // std::cout << "get " << name << std::endl;

    // std::cout << "Curvilinear::get " << name << " failed" << std::endl;

    return false;
}

bool Curvilinear::get(const std::string&name, std::string& value) const {
    // std::cout << "get " << name << std::endl;
    if (name == "gridType") {
        value = "unstructured_grid";
        return true;
    }

    // std::cout << "Curvilinear::get " << name << " failed" << std::endl;


    return false;
}

bool Curvilinear::get(const std::string &name, double &value) const {

    if (name == "north") {
        value = north_;
        return true;
    }

    if (name == "south") {
        value = south_;
        return true;
    }

    if (name == "west") {
        value = west_;
        return true;
    }

    if (name == "east") {
        value = east_;
        return true;
    }



    // std::cout << "Curvilinear::get " << name << " failed" << std::endl;


    return false;
}


//================================================================

static bool check_axis(const Variable &variable, const Variable & axis, std::vector<double>& v) {



    auto axis_dimensions = axis.dimensions();
    auto variable_dimensions = variable.dimensions();

    if (axis_dimensions.size() != 2) {
        return false;
    }

    if (variable_dimensions[variable_dimensions.size() - 2 ] != axis_dimensions[0]) {
        return false;
    }

    if (variable_dimensions[variable_dimensions.size() - 1 ] != axis_dimensions[1]) {
        return false;
    }


    axis.values(v);

    return true;
}

inline double sign(double x) {
    return (x > 0) - (x < 0);
}

GridSpec* Curvilinear::guess(const Variable &variable,
                             const Variable &latitudes,
                             const Variable &longitudes) {

    if (variable.numberOfDimensions() < 2) {
        return 0;
    }


    std::vector<double> lats;
    if (!check_axis(variable, latitudes, lats)) {
        return 0;
    }

    std::vector<double> lons;
    if (!check_axis(variable, longitudes, lons)) {
        return 0;
    }

//

    auto dimensions = latitudes.dimensions();
    ASSERT(dimensions.size() == 2);


    // for(size_t i = 0; i < lats.size(); i++) {
    //     std::cout << lats[i] << ' ' << lons[i] << std::endl;
    // }


    struct Index {
        size_t ni_;
        size_t nj_;

        size_t operator()(size_t i, size_t j) { return i + j * ni_; }
    };

    Index index;
    index.ni_ = dimensions[1]->count();
    index.nj_ = dimensions[0]->count();

    /*

        (x1, y1) --------------- (x4, y4)
            |                        |
            |                        |
            |                        |
            |                        |
        (x2, y2) --------------- (x3, y3)
    */


    std::cout << "Curvilinear " << index.ni_ << " " << index.nj_ << std::endl;

    double s;
        for (size_t i = 0; i < index.ni_ - 1; i++) {

    for (size_t j = 0; j < index.nj_ - 1; j++) {


            double x1 = lons[index(i, j)];
            double y1 = lats[index(i, j)];

            double x2 = lons[index(i, j + 1)];
            double y2 = lats[index(i, j + 1)];

            double x3 = lons[index(i + 1, j + 1)];
            double y3 = lats[index(i + 1, j + 1)];

            double x4 = lons[index(i, j + 1)];
            double y4 = lats[index(i, j + 1)];

            double t1 = x1 * y2 - x2 * y1 + x2 * y3 - x3 * y2 + x3 * y1 - x1 * y3;
            double t2 = x1 * y3 - x3 * y1 + x3 * y4 - x4 * y3 + x4 * y1 - x1 * y4;



            if (i == 0 && j == 0) {
                eckit::Log::info() << "First " << t1 << "  " << t2 << std::endl;
                eckit::Log::info() << x1 << "/" << y1 << " ================ " << x4 << "/" << y4 << std::endl;

                eckit::Log::info() << x2 << "/" << y2  << " ================ " << x3 << "/" << y3 << std::endl;
                s = sign(t1?t1:t2);
            }

            if (sign(t1) != s) {
                eckit::Log::info() << "Sign of " << t1 << " is not " << s << std::endl;
                return 0;
            }

            if (sign(t2) != s) {
                eckit::Log::info() << "Sign of " << t2 << " is not " << s << std::endl;
                return 0;
            }

            // double t3 = x1 * y2 - x2 * y1 + x2 * y4 - x4 * y2 + x4 * y1 - x1 * y4;
            // double t4 = * y3 - x3 * y2 + x3 * y4 - x4 * y3 + x4 * y2 - x2 * y4;

            // std::cout << a << std::endl;
        }
    }

    return new Curvilinear(variable, lats, lons);

}


void Curvilinear::reorder(MIRValuesVector& values) const {
    // size_t ni = latitudes_.size();
    // size_t nj = longitudes_.size();

    // if (jScansPositively_) {
    //   ASSERT(values.size() == ni * nj);

    //   std::vector<double> out(values.size());

    //   size_t count = 0;
    //   for (int j = nj - 1 ; j >= 0; --j) {
    //     for (size_t i = 0 ; i <  ni; ++i) {
    //       out[count++] = values[j * ni + i];
    //     }
    //   }
    //   ASSERT(count == out.size());
    //   std::swap(values, out);
    //   return;
    // }
}


static GridSpecGuesserBuilder<Curvilinear> builder(3);


}  // namespace netcdf
}  // namespace mir

