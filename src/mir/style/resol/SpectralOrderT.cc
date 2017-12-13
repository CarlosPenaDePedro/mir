/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @date May 2017


#include "mir/style/resol/SpectralOrderT.h"


namespace mir {
namespace style {
namespace resol {


namespace {
static SpectralOrderBuilder< SpectralOrderT<1> > __order1("linear");
static SpectralOrderBuilder< SpectralOrderT<2> > __order2("quadratic");
static SpectralOrderBuilder< SpectralOrderT<3> > __order3("cubic");
static SpectralOrderBuilder< SpectralOrderT<4> > __order4("quartic");
}


template<> void SpectralOrderT<1>::print(std::ostream& out) const { out << "Linear[]"; }
template<> void SpectralOrderT<2>::print(std::ostream& out) const { out << "Quadratic[]"; }
template<> void SpectralOrderT<3>::print(std::ostream& out) const { out << "Cubic[]"; }
template<> void SpectralOrderT<4>::print(std::ostream& out) const { out << "Quartic[]"; }


}  // namespace resol
}  // namespace style
}  // namespace mir

