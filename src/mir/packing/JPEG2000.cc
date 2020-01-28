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


#include "mir/packing/JPEG2000.h"

#include <iostream>

#include "mir/util/Grib.h"


namespace mir {
namespace packing {


JPEG2000::JPEG2000(const std::string& name) : Packer(name) {}


JPEG2000::~JPEG2000() = default;

void JPEG2000::print(std::ostream& out) const {
    out << "JPEG2000[]";
}

void JPEG2000::fill(grib_info& info, const repres::Representation&) const {
    info.packing.packing      = GRIB_UTIL_PACKING_USE_PROVIDED;
    info.packing.packing_type = GRIB_UTIL_PACKING_TYPE_JPEG;
}


namespace {
static JPEG2000 packing1("grid_jpeg");
static JPEG2000 packing2("jpeg");

}  // namespace


}  // namespace packing
}  // namespace mir
