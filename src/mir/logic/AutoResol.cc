/*
 * (C) Copyright 1996-2015 ECMWF.
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


#include "mir/logic/AutoResol.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace logic {

//==========================================================
AutoResol::AutoResol(const param::MIRParametrisation &parametrisation):
    parametrisation_(parametrisation) {
}


AutoResol::~AutoResol() {

}

void AutoResol::get(const std::string &name, long &value) const {
    eckit::Log::info() << "AutoResol::get(" << name << ")" << std::endl;
    ASSERT(name == "truncation"); // For now

    double step = 0;

    std::vector<double> grid;
    if (parametrisation_.get("user.grid", grid)) {
        ASSERT(grid.size() == 2);
        step = std::min(grid[0], grid[1]);
    }

    long N;
    if (parametrisation_.get("user.reduced", N)) {
        step = 90.0 / N;
    }

    if (parametrisation_.get("user.regular", N)) {
        step = 90.0 / N;
    }

    if (step == 0) {
        throw eckit::SeriousBug("AutoResol: cannot establish truncation");
    }

    // From emoslib
    // TODO: put in config file


    if ( step >= 2.5 ) {
        value = 63;
    }

    else if ( step >= 1.5 ) {
        value = 106;
    }

    else if ( step >= 0.6 ) {
        value = 213;
    }

    else if ( step >= 0.4 ) {
        value = 319;
    }

    else if ( step >= 0.3 ) {
        value = 511;
    }

    else if ( step >= 0.15 ) {
        value = 799;
    }


    else if ( step >= 0.09 ) {
        value = 1279;
    }
else
   { value = 2047;}

    eckit::Log::info() << "AutoResol: step is " << step << ", selecting truncation: " << value << std::endl;

}

void AutoResol::print(std::ostream &out) const {
    out << "<AutoResol>";
}

}  // namespace param
}  // namespace mir

