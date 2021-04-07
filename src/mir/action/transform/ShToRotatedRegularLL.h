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

#include "mir/action/transform/ShToGridded.h"
#include "mir/util/Increments.h"
#include "mir/util/Rotation.h"


namespace mir {
namespace action {
namespace transform {


template <class Invtrans>
class ShToRotatedRegularLL : public ShToGridded, Invtrans {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ShToRotatedRegularLL(const param::MIRParametrisation&);

    // -- Destructor

    ~ShToRotatedRegularLL() override;

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

    util::BoundingBox bbox_;
    util::Increments increments_;
    util::Rotation rotation_;

    // -- Methods
    // None

    // -- Overridden methods

    bool sameAs(const Action&) const override;
    const char* name() const override;
    const repres::Representation* outputRepresentation() const override;
    void print(std::ostream&) const override;
    void sh2grid(data::MIRField& field, const atlas_trans_t& trans, const param::MIRParametrisation&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace transform
}  // namespace action
}  // namespace mir
