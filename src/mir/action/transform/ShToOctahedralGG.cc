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


#include "mir/action/transform/ShToOctahedralGG.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "mir/action/transform/InvtransScalar.h"
#include "mir/action/transform/InvtransVodTouv.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/gauss/reduced/ReducedOctahedral.h"


namespace mir {
namespace action {
namespace transform {


template<class Invtrans>
ShToOctahedralGG<Invtrans>::ShToOctahedralGG(const param::MIRParametrisation& parametrisation):
    ShToGridded(parametrisation) {

    ASSERT(parametrisation_.userParametrisation().get("octahedral", N_));

}


template<class Invtrans>
bool ShToOctahedralGG<Invtrans>::sameAs(const Action& other) const {
    auto o = dynamic_cast<const ShToOctahedralGG*>(&other);
    return o && (N_ == o->N_);
}


template<class Invtrans>
ShToOctahedralGG<Invtrans>::~ShToOctahedralGG() = default;


template<class Invtrans>
void ShToOctahedralGG<Invtrans>::print(std::ostream& out) const {
    out << "ShToOctahedralGG[";
    ShToGridded::print(out);
    out << ",";
    Invtrans::print(out);
    out << ",N=" << N_
        << "]";
}


template<class Invtrans>
void ShToOctahedralGG<Invtrans>::sh2grid(data::MIRField& field, const ShToGridded::atlas_trans_t& trans, const param::MIRParametrisation& parametrisation) const {
    Invtrans::sh2grid(field, trans, parametrisation);
}


template<class Invtrans>
const char* ShToOctahedralGG<Invtrans>::name() const {
    return "ShToOctahedralGG";
}


template<class Invtrans>
const repres::Representation* ShToOctahedralGG<Invtrans>::outputRepresentation() const {
    return new repres::gauss::reduced::ReducedOctahedral(N_);
}


namespace {
static ActionBuilder< ShToOctahedralGG<InvtransScalar> > __action1("transform.sh-scalar-to-octahedral-gg");
static ActionBuilder< ShToOctahedralGG<InvtransVodTouv> > __action2("transform.sh-vod-to-uv-octahedral-gg");
}


}  // namespace transform
}  // namespace action
}  // namespace mir

