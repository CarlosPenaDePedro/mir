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

#include "mir/action/transform/Sh2ReducedGG.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/reduced/ReducedClassic.h"


namespace mir {
namespace action {
namespace transform {


Sh2ReducedGG::Sh2ReducedGG(const param::MIRParametrisation& parametrisation):
    Sh2GriddedTransform(parametrisation) {

    ASSERT(parametrisation_.get("user.reduced", N_));

}


Sh2ReducedGG::~Sh2ReducedGG() {
}


bool Sh2ReducedGG::sameAs(const Action& other) const {
    const Sh2ReducedGG* o = dynamic_cast<const Sh2ReducedGG*>(&other);
    return o && (N_ == o->N_);
}

void Sh2ReducedGG::print(std::ostream& out) const {
    out << "Sh2ReducedGG[N=" << N_ << "]";
}


const repres::Representation* Sh2ReducedGG::outputRepresentation() const {
    return new repres::reduced::ReducedClassic(N_);
}


namespace {
static ActionBuilder< Sh2ReducedGG > grid2grid("transform.sh2reduced-gg");
}


}  // namespace transform
}  // namespace action
}  // namespace mir

