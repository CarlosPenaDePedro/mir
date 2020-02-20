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


#include "mir/repres/regular/Lambert.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace regular {


static RepresentationBuilder<Lambert> __builder("lambert");


Lambert::Lambert(const param::MIRParametrisation& param) : RegularGrid(param, make_projection(param)) {}


RegularGrid::Projection Lambert::make_projection(const param::MIRParametrisation& param) {
    auto spec = make_proj_spec(param);
    if (!spec.empty()) {
        return spec;
    }

    double LaDInDegrees;
    double LoVInDegrees;
    double Latin1InDegrees;
    double Latin2InDegrees;
    ASSERT(param.get("LaDInDegrees", LaDInDegrees));
    ASSERT(param.get("LoVInDegrees", LoVInDegrees));
    param.get("Latin1InDegrees", Latin1InDegrees = LaDInDegrees);
    param.get("Latin2InDegrees", Latin2InDegrees = LaDInDegrees);

    return Projection::Spec("type", "lambert_conformal_conic")
        .set("latitude1", Latin1InDegrees)
        .set("latitude2", Latin2InDegrees)
        .set("latitude0", LaDInDegrees)
        .set("longitude0", LoVInDegrees);
}


void Lambert::fill(grib_info& info) const {

    info.grid.grid_type = CODES_UTIL_GRID_SPEC_LAMBERT_CONFORMAL;

    ASSERT(x_.size() > 1);
    ASSERT(y_.size() > 1);
    auto Dx = std::lround((x_.max() - x_.min()) * 1000. / (x_.size() - 1.));
    auto Dy = std::lround((y_.max() - y_.min()) * 1000. / (y_.size() - 1.));

    Point2 first     = {firstPointBottomLeft_ ? x_.min() : x_.front(), firstPointBottomLeft_ ? y_.min() : y_.front()};
    Point2 firstLL   = grid_.projection().lonlat(first);
    Point2 reference = grid_.projection().lonlat({0., 0.});

    info.grid.Ni                                 = long(x_.size());
    info.grid.Nj                                 = long(y_.size());
    info.grid.latitudeOfFirstGridPointInDegrees  = firstLL[LLCOORDS::LAT];
    info.grid.longitudeOfFirstGridPointInDegrees = firstLL[LLCOORDS::LON];

    GribExtraSetting::set(info, "Dx", Dx);
    GribExtraSetting::set(info, "Dy", Dy);
    GribExtraSetting::set(info, "LoVInDegrees", reference[LLCOORDS::LON]);
    GribExtraSetting::set(info, "LaDInDegrees", reference[LLCOORDS::LAT]);  // LaD
    GribExtraSetting::set(info, "Latin1InDegrees", reference[LLCOORDS::LAT]);
    GribExtraSetting::set(info, "Latin2InDegrees", reference[LLCOORDS::LAT]);

    // some extra keys are edition-specific, so parent call is here
    RegularGrid::fill(info);
}


}  // namespace regular
}  // namespace repres
}  // namespace mir
