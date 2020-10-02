/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/action/misc/Noop.h"

#include <iostream>


namespace mir {
namespace action {


Noop::Noop(const param::MIRParametrisation& parametrisation) : Action(parametrisation) {}


Noop::~Noop() = default;


bool Noop::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Noop*>(&other);
    return (o != nullptr);
}


void Noop::print(std::ostream& out) const {
    out << "Noop[]";
}


void Noop::execute(context::Context&) const {}


const char* Noop::name() const {
    return "Noop";
}


static ActionBuilder<Noop> __action("noop");


}  // namespace action
}  // namespace mir
