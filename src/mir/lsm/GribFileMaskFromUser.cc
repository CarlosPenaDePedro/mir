/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/lsm/GribFileMaskFromUser.h"


namespace mir {
namespace lsm {


bool GribFileMaskFromUser::cacheable() const {
    return false;
}


}  // namespace lsm
}  // namespace mir

