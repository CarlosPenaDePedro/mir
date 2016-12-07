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


#include "mir/style/AutoGaussian.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/config/LibMir.h"


namespace mir {
namespace style {


//==========================================================
AutoGaussian::AutoGaussian(const param::MIRParametrisation &parametrisation):
    parametrisation_(parametrisation) {
}


AutoGaussian::~AutoGaussian() {

}

void AutoGaussian::get(const std::string &name, long &value) const {
    eckit::Log::debug<LibMir>() << "AutoGaussian::get(" << name << ")" << std::endl;
    ASSERT(name == "octahedral"); // For now

    long truncation = 0;

    ASSERT(parametrisation_.get("field.truncation", truncation));

    // TODO: a config file
    value = (truncation == 1279)? 1280
          : (truncation ==  639)?  640
          : (truncation ==  319)?  320
          : (truncation ==  255)?  256  // TODO: Should be N256, not O256
          : (truncation ==   63)?   64  // TODO: Should be N64, not O64
          :                          0;

    if (value == 0) {
        std::ostringstream oss;
        oss << "AutoGaussian: cannot establish N for truncation " << truncation;
        throw eckit::SeriousBug(oss.str());
    }

    // eckit::Log::debug<LibMir>() << "AutoGaussian: N is " << N << ", selecting reduced N" << value << std::endl;
}

void AutoGaussian::print(std::ostream &out) const {
    out << "<AutoGaussian>";
}

}  // namespace param
}  // namespace mir

