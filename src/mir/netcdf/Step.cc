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


#include "mir/netcdf/Step.h"

namespace mir {
namespace netcdf {

Step::Step() {}

Step::~Step() = default;

bool Step::merge(Step* /*other*/) {
    return false;
}

}  // namespace netcdf
}  // namespace mir
