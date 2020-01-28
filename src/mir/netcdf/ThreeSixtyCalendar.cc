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


#include "mir/netcdf/ThreeSixtyCalendar.h"

#include "mir/netcdf/Exceptions.h"
#include "mir/netcdf/OutputAttribute.h"
#include "mir/netcdf/Value.h"
#include "mir/netcdf/Variable.h"

#include <ostream>
#include <sstream>
#include <algorithm>

#include <netcdf.h>

namespace mir {
namespace netcdf {


ThreeSixtyCalendar::ThreeSixtyCalendar(const Variable& variable)
{
}

ThreeSixtyCalendar::~ThreeSixtyCalendar() = default;

void ThreeSixtyCalendar::print(std::ostream &out) const {
    out << "ThreeSixtyCalendar[]";
}

static CodecBuilder<ThreeSixtyCalendar> builder("360_day");

}  // namespace netcdf
}  // namespace mir
