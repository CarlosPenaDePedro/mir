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


#ifndef mir_util_Types_h
#define mir_util_Types_h

#include "mir/util/Latitude.h"
#include "mir/util/LongitudeDouble.h"
#include "mir/util/LongitudeFraction.h"
#include "mir/util/PointLatLonT.h"

#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"


namespace mir {


// using Longitude = LongitudeDouble;
using Longitude = LongitudeFraction;


using PointLatLon = util::PointLatLonT<Latitude, Longitude>;
using eckit::geometry::Point2;
using eckit::geometry::Point3;


}  // namespace mir


#endif

