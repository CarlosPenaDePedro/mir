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


#include "mir/action/transform/ShTruncate.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/memory/ScopedPtr.h"
#include "mir/action/context/Context.h"
#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace action {
namespace transform {


ShTruncate::ShTruncate(const param::MIRParametrisation &parametrisation):
    Action(parametrisation),
    truncation_(0) {
    ASSERT(parametrisation.userParametrisation().get("truncation", truncation_));

    ASSERT(truncation_ > 0);
}


ShTruncate::~ShTruncate() = default;


bool ShTruncate::sameAs(const Action& other) const {
    auto o = dynamic_cast<const ShTruncate*>(&other);
    return o && (truncation_ == o->truncation_);
}


void ShTruncate::print(std::ostream &out) const {
    out << "ShTruncate["
        <<  "truncation=" << truncation_
        << "]";
}


void ShTruncate::execute(context::Context& ctx) const {
    data::MIRField& field = ctx.field();

    // Keep a pointer on the original representation, as the one in the field will
    // be changed in the loop
    repres::RepresentationHandle representation(field.representation());


    for (size_t i = 0; i < field.dimensions(); i++) {
        const MIRValuesVector& values = field.values(i);
        MIRValuesVector result;

        const repres::Representation* repres = representation->truncate(truncation_, values, result);

        if (repres) { // NULL if nothing happend
            field.representation(repres); // Assumes representation will be the same
            field.update(result, i);
        }
    }
}

const char* ShTruncate::name() const {
    return "ShTruncate";
}

namespace {
static ActionBuilder< ShTruncate > __action("transform.sh-truncate");
}


}  // namespace transform
}  // namespace action
}  // namespace mir

