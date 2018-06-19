/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/param/MIRParametrisation.h"

#include "mir/repres/unsupported/StretchedSH.h"


namespace mir {
namespace repres {


StretchedSH::StretchedSH(const param::MIRParametrisation &parametrisation) {
}


StretchedSH::StretchedSH() {
}


StretchedSH::~StretchedSH() = default;


void StretchedSH::print(std::ostream &out) const {
    out << "StretchedSH["
        << "]";
}


void StretchedSH::fill(grib_info &info) const  {
    NOTIMP;
}


namespace {
static RepresentationBuilder<StretchedSH> stretchedSH("stretched_sh"); // Name is what is returned by grib_api
}


}  // namespace repres
}  // namespace mir

