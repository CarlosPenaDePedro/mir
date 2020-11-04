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


#include "mir/method/FailMethod.h"

#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"


namespace mir {
namespace method {


static MethodBuilder<FailMethod> __method("fail");


bool FailMethod::canCrop() const {
    return false;
}


bool FailMethod::hasCropping() const {
    return false;
}


bool FailMethod::sameAs(const Method& other) const {
    auto o = dynamic_cast<const FailMethod*>(&other);
    return (o != nullptr);
}


const util::BoundingBox& FailMethod::getCropping() const {
    NOTIMP;
}


void FailMethod::execute(context::Context&, const repres::Representation&, const repres::Representation&) const {
    std::string msg = "FailMethod: this interpolation method fails intentionally";
    eckit::Log::error() << msg << std::endl;
    throw eckit::UserError(msg);
}


void FailMethod::hash(eckit::MD5&) const {}


int FailMethod::version() const {
    NOTIMP;
}


void FailMethod::print(std::ostream& out) const {
    out << "FailMethod[]";
}


void FailMethod::setCropping(const util::BoundingBox&) {
    NOTIMP;
}


}  // namespace method
}  // namespace mir
