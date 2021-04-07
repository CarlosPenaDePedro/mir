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


#pragma once

#include "mir/method/Method.h"


namespace mir {
namespace method {


class FailMethod final : public Method {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    using Method::Method;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    // From Method
    bool canCrop() const override;
    bool hasCropping() const override;
    bool sameAs(const Method&) const override;
    const util::BoundingBox& getCropping() const override;
    void execute(context::Context&, const repres::Representation& in, const repres::Representation& out) const override;
    void hash(eckit::MD5&) const override;
    int version() const override;
    void print(std::ostream&) const override;
    void setCropping(const util::BoundingBox&) override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace method
}  // namespace mir
