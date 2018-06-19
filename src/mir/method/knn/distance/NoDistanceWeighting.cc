/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/method/knn/distance/NoDistanceWeighting.h"

#include "eckit/utils/MD5.h"


namespace mir {
namespace method {
namespace knn {
namespace distance {


NoDistanceWeighting::NoDistanceWeighting(const param::MIRParametrisation&) {
}


void NoDistanceWeighting::operator()(
        size_t ip,
        const eckit::geometry::Point3&,
        const std::vector<util::PointSearch::PointValueType>& neighbours,
        std::vector<WeightMatrix::Triplet>& triplets ) const {

    ASSERT(!neighbours.empty());

    triplets.clear();
    triplets.reserve(neighbours.size());

    // average neighbour points
    const double weight = 1. / neighbours.size();
    for (auto& n : neighbours) {
        triplets.push_back(WeightMatrix::Triplet(ip, n.payload(), weight));
    }
}


bool NoDistanceWeighting::sameAs(const DistanceWeighting& other) const {
    auto o = dynamic_cast<const NoDistanceWeighting*>(&other);
    return o;
}


void NoDistanceWeighting::print(std::ostream& out) const {
    out << "NoDistanceWeighting[]";
}


void NoDistanceWeighting::hash(eckit::MD5& h) const {
    std::ostringstream s;
    s << *this;
    h.add(s.str());
}


static DistanceWeightingBuilder<NoDistanceWeighting> __distance1("no-distance-weighting");
static DistanceWeightingBuilder<NoDistanceWeighting> __distance2("no");


}  // namespace distance
}  // namespace knn
}  // namespace method
}  // namespace mir

