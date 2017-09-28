/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date September 2017


#include "mir/lsm/NamedLSM.h"

#include <iostream>
#include "eckit/utils/MD5.h"
#include "mir/config/LibMir.h"
#include "mir/lsm/GribFileLSM.h"
#include "mir/lsm/MappedMask.h"
#include "mir/lsm/TenMinutesLSM.h"


namespace mir {
namespace lsm {


namespace {
static NamedLSM input("named-input");
static NamedLSM output("named-output");
}


NamedLSM::NamedLSM(const std::string &name):
    LSMChooser(name) {
}


NamedLSM::~NamedLSM() {
}


void NamedLSM::print(std::ostream& out) const {
    out << "NamedLSM[" << name_ << "]";
}


std::string NamedLSM::path(const param::MIRParametrisation &parametrisation) const {
    // TODO: Implement clever selection
    return  "~mir/share/mir/masks/lsm.N640.grib";
}


Mask *NamedLSM::create(const std::string &name,
                      const param::MIRParametrisation &param,
                      const repres::Representation& representation,
                      const std::string& which) const {

    // Mask* mask = new TenMinutesLSM(name, param, grid, which);
    Mask* mask = new MappedMask(name, param, representation, which);
    // Mask* mask = new GribFileLSM(name, path(param), param, grid, which);

    eckit::Log::debug<LibMir>() << "NamedLSM::create => " << *mask << std::endl;
    return mask;
}


std::string NamedLSM::cacheKey(const std::string &name,
                              const param::MIRParametrisation &param,
                              const repres::Representation& representation,
                              const std::string& which) const {
    eckit::MD5 md5;
    GribFileLSM::hashCacheKey(md5, path(param), param, representation, which); // We need to take the lsm interpolation method into account
    return "auto." + md5.digest();
}


}  // namespace lsm
}  // namespace mir

