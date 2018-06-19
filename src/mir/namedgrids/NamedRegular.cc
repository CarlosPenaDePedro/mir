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



#include "mir/namedgrids/NamedRegular.h"
#include "mir/repres/gauss/regular/RegularGG.h"
#include "mir/repres/gauss/regular/RotatedGG.h"
#include "eckit/exception/Exceptions.h"
#include <iostream>

namespace mir {
namespace namedgrids {


NamedRegular::NamedRegular(const std::string& name, size_t N):
    NamedGrid(name),
    N_(N) {
}


NamedRegular::~NamedRegular() = default;

void NamedRegular::print(std::ostream& out) const {
    out << "NamedRegular[name=" << name_ << ",N=" << N_ << "]";
}

const repres::Representation *NamedRegular::representation() const {
    return new repres::gauss::regular::RegularGG(N_);
}

const repres::Representation *NamedRegular::representation(const util::Rotation& rotation) const {
    return new repres::gauss::regular::RotatedGG(N_, rotation);
}

size_t NamedRegular::gaussianNumber() const {
    return N_;
}

}  // namespace namedgrids
}  // namespace mir

