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

#include "mir/action/Sh2RegularGG.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Translator.h"
#include "mir/param/MIRParametrisation.h"

#include "mir/repres/RegularGG.h"


namespace mir {
namespace action {


Sh2RegularGG::Sh2RegularGG(const param::MIRParametrisation& parametrisation):
    Sh2GriddedTransform(parametrisation) {
}


Sh2RegularGG::~Sh2RegularGG() {
}


void Sh2RegularGG::print(std::ostream& out) const {
    out << "Sh2RegularGG[]";
}


repres::Representation* Sh2RegularGG::outputRepresentation(const repres::Representation* inputRepres) const {
    long N;
    ASSERT(parametrisation_.get("regular", N));
    return new repres::RegularGG(N);
}


namespace {
static ActionBuilder< Sh2RegularGG > grid2grid("transform.sh2regular-gg");
}


}  // namespace action
}  // namespace mir

