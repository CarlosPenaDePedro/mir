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


#include "mir/repres/gauss/reduced/ReducedClassic.h"

#include <iostream>


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


ReducedClassic::ReducedClassic(size_t N, const util::BoundingBox& bbox, double angularPrecision) :
    Classic(N, bbox, angularPrecision) {
}


ReducedClassic::~ReducedClassic() = default;


void ReducedClassic::print(std::ostream &out) const {
    out << "ReducedClassic[N=" << N_ << ",bbox=" << bbox_ << "]";
}


Iterator* ReducedClassic::iterator() const {
    return unrotatedIterator();
}


const Gridded* ReducedClassic::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new ReducedClassic(N_, bbox, angularPrecision_);
}


void ReducedClassic::makeName(std::ostream& out) const {
    Classic::makeName(out);
}


bool ReducedClassic::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const ReducedClassic*>(&other);
    return o && Classic::sameAs(other);
}


std::vector<double> ReducedClassic::calculateGridBoxLatitudeEdges() const {
    return calculateUnrotatedGridBoxLatitudeEdges();
}


std::vector<double> ReducedClassic::calculateGridBoxLongitudeEdges(size_t j) const {
    return calculateUnrotatedGridBoxLongitudeEdges(j);
}


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir

