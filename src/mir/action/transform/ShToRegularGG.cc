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


#include "mir/action/transform/ShToRegularGG.h"

#include <iostream>

#include "mir/action/transform/InvtransScalar.h"
#include "mir/action/transform/InvtransVodTouv.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/regular/RegularGG.h"
#include "mir/util/Exceptions.h"


namespace mir {
namespace action {
namespace transform {


template <class Invtrans>
ShToRegularGG<Invtrans>::ShToRegularGG(const param::MIRParametrisation& parametrisation) :
    ShToGridded(parametrisation) {
    ASSERT(parametrisation_.userParametrisation().get("regular", N_));
}


template <class Invtrans>
ShToRegularGG<Invtrans>::~ShToRegularGG() = default;


template <class Invtrans>
bool ShToRegularGG<Invtrans>::sameAs(const Action& other) const {
    auto o = dynamic_cast<const ShToRegularGG*>(&other);
    return (o != nullptr) && (N_ == o->N_);
}


template <class Invtrans>
void ShToRegularGG<Invtrans>::print(std::ostream& out) const {
    out << "ShToRegularGG[";
    ShToGridded::print(out);
    out << ",";
    Invtrans::print(out);
    out << ",N=" << N_ << "]";
}


template <class Invtrans>
void ShToRegularGG<Invtrans>::sh2grid(data::MIRField& field, const ShToGridded::atlas_trans_t& trans,
                                      const param::MIRParametrisation& parametrisation) const {
    Invtrans::sh2grid(field, trans, parametrisation);
}


template <class Invtrans>
const char* ShToRegularGG<Invtrans>::name() const {
    return "ShToRegularGG";
}


template <class Invtrans>
const repres::Representation* ShToRegularGG<Invtrans>::outputRepresentation() const {
    return new repres::gauss::regular::RegularGG(N_);
}


static ActionBuilder<ShToRegularGG<InvtransScalar> > __action1("transform.sh-scalar-to-regular-gg");
static ActionBuilder<ShToRegularGG<InvtransVodTouv> > __action2("transform.sh-vod-to-uv-regular-gg");


}  // namespace transform
}  // namespace action
}  // namespace mir
