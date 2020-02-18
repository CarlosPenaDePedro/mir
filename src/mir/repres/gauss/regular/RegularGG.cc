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


#include "mir/repres/gauss/regular/RegularGG.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"

#include "mir/util/GridBox.h"


namespace mir {
namespace repres {
namespace gauss {
namespace regular {


RegularGG::RegularGG(const param::MIRParametrisation& parametrisation) : Regular(parametrisation) {}


RegularGG::RegularGG(size_t N, const util::BoundingBox& bbox, double angularPrecision) :
    Regular(N, bbox, angularPrecision) {}


RegularGG::~RegularGG() = default;


void RegularGG::print(std::ostream& out) const {
    out << "RegularGG["
           "N="
        << N_ << ",Ni=" << Ni_ << ",Nj=" << Nj_ << ",bbox=" << bbox_ << "]";
}


bool RegularGG::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const RegularGG*>(&other);
    return (o != nullptr) && Regular::sameAs(other);
}


Iterator* RegularGG::iterator() const {
    auto Ni = [=](size_t) { return long(4 * N_); };
    return Gaussian::unrotatedIterator(Ni);
}


const Gridded* RegularGG::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RegularGG(N_, bbox, angularPrecision_);
}


std::string RegularGG::factory() const {
    return "regular_gg";
}


std::vector<util::GridBox> RegularGG::gridBoxes() const {
    ASSERT(1 < Ni_);
    ASSERT(1 < Nj_);


    // latitude edges
    std::vector<double> latEdges = calculateUnrotatedGridBoxLatitudeEdges();


    // longitude edges
    bool periodic = isPeriodicWestEast();
    auto lon0     = bbox_.west();
    auto inc      = (bbox_.east() - bbox_.west()) / (Ni_ - 1);
    eckit::Fraction half(1, 2);

    std::vector<double> lonEdges(Ni_ + 1, 0.);
    lonEdges[0] = (lon0 - inc / 2).value();
    for (size_t i = 0; i < Ni_; ++i) {
        lonEdges[i + 1] = (lon0 + (i + half) * inc.fraction()).value();
    }


    // grid boxes
    std::vector<util::GridBox> r;
    r.reserve(Ni_ * Nj_);

    for (size_t j = 0; j < Nj_; ++j) {
        Longitude lon1 = lon0;

        for (size_t i = 0; i < Ni_; ++i) {
            auto l = lon1;
            lon1   = l + Longitude(inc * (i + half));
            r.emplace_back(util::GridBox(latEdges[j], lonEdges[i], latEdges[j + 1], lonEdges[i + 1]));
        }

        ASSERT(periodic ? lon0 == lon1.normalise(lon0) : lon0 < lon1.normalise(lon0));
    }

    ASSERT(r.size() == Ni_ * Nj_);
    return r;
}


static RepresentationBuilder<RegularGG> reducedGG("regular_gg");


}  // namespace regular
}  // namespace gauss
}  // namespace repres
}  // namespace mir
