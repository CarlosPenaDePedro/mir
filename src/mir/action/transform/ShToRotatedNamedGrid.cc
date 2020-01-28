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


#include "mir/action/transform/ShToRotatedNamedGrid.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/action/transform/InvtransScalar.h"
#include "mir/action/transform/InvtransVodTouv.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/namedgrids/NamedGrid.h"

namespace mir {
namespace action {
namespace transform {


template<class Invtrans>
ShToRotatedNamedGrid<Invtrans>::ShToRotatedNamedGrid(const param::MIRParametrisation& parametrisation):
    ShToGridded(parametrisation) {

    ASSERT(parametrisation_.userParametrisation().get("gridname", gridname_));

    std::vector<double> value;
    ASSERT(parametrisation_.userParametrisation().get("rotation", value));
    ASSERT(value.size() == 2);
    rotation_ = util::Rotation(value[0], value[1]);
}


template<class Invtrans>
ShToRotatedNamedGrid<Invtrans>::~ShToRotatedNamedGrid() = default;


template<class Invtrans>
bool ShToRotatedNamedGrid<Invtrans>::sameAs(const Action& other) const {
    auto o = dynamic_cast<const ShToRotatedNamedGrid*>(&other);
    return o && (gridname_ == o->gridname_) && (rotation_ == o->rotation_) && ShToGridded::sameAs(other);
}


template<class Invtrans>
void ShToRotatedNamedGrid<Invtrans>::print(std::ostream& out) const {
    out << "ShToRotatedNamedGrid[";
    ShToGridded::print(out);
    out << ",";
    Invtrans::print(out);
    out << ",gridname=" << gridname_
        << ",rotation=" << rotation_
        << "]";
}


template<class Invtrans>
void ShToRotatedNamedGrid<Invtrans>::sh2grid(data::MIRField& field, const ShToGridded::atlas_trans_t& trans, const param::MIRParametrisation& parametrisation) const {
    Invtrans::sh2grid(field, trans, parametrisation);
}


template<class Invtrans>
const char* ShToRotatedNamedGrid<Invtrans>::name() const {
    return "ShToRotatedNamedGrid";
}


template<class Invtrans>
const repres::Representation* ShToRotatedNamedGrid<Invtrans>::outputRepresentation() const {
    const namedgrids::NamedGrid& ng = namedgrids::NamedGrid::lookup(gridname_);
    return ng.representation(rotation_);
}


namespace {
static ActionBuilder< ShToRotatedNamedGrid<InvtransScalar> > __action1("transform.sh-scalar-to-rotated-namedgrid");
static ActionBuilder< ShToRotatedNamedGrid<InvtransVodTouv> > __action2("transform.sh-vod-to-uv-rotated-namedgrid");
}


}  // namespace transform
}  // namespace action
}  // namespace mir

