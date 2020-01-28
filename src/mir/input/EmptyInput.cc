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


#include <iostream>

#include "mir/data/MIRField.h"
#include "mir/input/EmptyInput.h"

#include "eckit/exception/Exceptions.h"


namespace mir {
namespace input {


EmptyInput::EmptyInput(): calls_(0) {
    parametrisation_.set("gridded", true);
    parametrisation_.set("gridType", "none");
}


EmptyInput::~EmptyInput() = default;


bool EmptyInput::sameAs(const MIRInput& other) const {
    auto o = dynamic_cast<const EmptyInput *>(&other);
    return o;
}


bool EmptyInput::next() {
    return calls_++ == 0;
}


const param::MIRParametrisation &EmptyInput::parametrisation(size_t which) const {
    ASSERT(which == 0);
    return parametrisation_;
}


data::MIRField EmptyInput::field() const {
    return data::MIRField(parametrisation_, false, 999.);
}


void EmptyInput::print(std::ostream &out) const {
    out << "EmptyInput[...]";
}


void EmptyInput::latitudes(std::vector<double>&) const {
}


void EmptyInput::longitudes(std::vector<double>&) const {
}


}  // namespace input
}  // namespace mir

