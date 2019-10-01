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


#include "mir/action/misc/SelectField.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/action/context/Context.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/data/MIRField.h"


namespace mir {
namespace action {


SelectField::SelectField(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {

    ASSERT(parametrisation_.get("which", which_));
}


SelectField::~SelectField() = default;


bool SelectField::sameAs(const Action& other) const {
    auto o = dynamic_cast<const SelectField*>(&other);
    return o && (which_ == o->which_);
}

void SelectField::print(std::ostream &out) const {
    out << "SelectField[" << which_ << "]";
}


void SelectField::execute(context::Context & ctx) const {
    data::MIRField& field = ctx.field();
    field.select(which_);
}

const char* SelectField::name() const {
    return "SelectField";
}


namespace {
static ActionBuilder< SelectField > action("select.field");
}


}  // namespace action
}  // namespace mir

