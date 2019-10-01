/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/method/nonlinear/NoNonLinear.h"

#include <iostream>
#include <sstream>

#include "eckit/utils/MD5.h"


namespace mir {
namespace method {
namespace nonlinear {


NoNonLinear::NoNonLinear(const param::MIRParametrisation& param) :
    NonLinear(param) {
}


bool NoNonLinear::treatment(NonLinear::Matrix&,
                            NonLinear::WeightMatrix&,
                            NonLinear::Matrix&,
                            const mir::data::MIRValuesVector&,
                            const double&) const {
    // no non-linear treatment
    return false;
}


bool NoNonLinear::sameAs(const NonLinear& other) const {
    auto o = dynamic_cast<const NoNonLinear*>(&other);
    return o;
}


void NoNonLinear::print(std::ostream& out) const {
    out << "NoNonLinear[]";
}


void NoNonLinear::hash(eckit::MD5& h) const {
    std::ostringstream s;
    s << *this;
    h.add(s.str());
}


static NonLinearBuilder<NoNonLinear> __nonlinear("no");


}  // namespace nonlinear
}  // namespace method
}  // namespace mir

