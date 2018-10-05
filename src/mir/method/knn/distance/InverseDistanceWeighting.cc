/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/method/knn/distance/InverseDistanceWeighting.h"

#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"
#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace method {
namespace knn {
namespace distance {


InverseDistanceWeighting::InverseDistanceWeighting(const param::MIRParametrisation& parametrisation) {

    power_ = 2.;
    parametrisation.get("distance-weighting-shepard-power", power_);
    ASSERT(power_ >= 0.);

    // half power is used to avoid the eckit::geometry::Point3::distance extra srqt
    halfPower_ = power_ * 0.5;
    ASSERT(halfPower_ >= 0.);
}


void InverseDistanceWeighting::operator()(
        size_t ip,
        const eckit::geometry::Point3& point,
        const std::vector<search::PointSearch::PointValueType>& neighbours,
        std::vector<WeightMatrix::Triplet>& triplets ) const {

    const size_t nbPoints = neighbours.size();
    ASSERT(nbPoints);

    triplets.clear();
    triplets.reserve(nbPoints);

    // calculate neighbour points weights, and their total (for normalisation)
    std::vector<double> weights(nbPoints);
    double sum = 0.;
    for (size_t j = 0; j < nbPoints; ++j) {
        const double d2 = eckit::geometry::Point3::distance2(point, neighbours[j].point());
        if (eckit::types::is_strictly_greater(d2, 0.)) {

            weights[j] = 1. / std::pow(d2, halfPower_);
            sum += weights[j];

        } else {

            // exact match found, use this neighbour only (inverse distance tends to infinity)
            triplets.assign(1, WeightMatrix::Triplet(ip, neighbours[j].payload(), 1.));
            return;

        }
    }

    ASSERT(sum > 0.);

    // normalise all weights according to the total, and set sparse matrix triplets
    for (size_t j = 0; j < nbPoints; ++j) {
        size_t jp = neighbours[j].payload();
        triplets.push_back(WeightMatrix::Triplet(ip, jp, weights[j] / sum));
    }
}


bool InverseDistanceWeighting::sameAs(const DistanceWeighting& other) const {
    auto o = dynamic_cast<const InverseDistanceWeighting*>(&other);
    return o && eckit::types::is_approximately_equal(power_, o->power_);
}


void InverseDistanceWeighting::print(std::ostream& out) const {
    out << "InverseDistanceWeighting[power=" << power_ << "]";
}


void InverseDistanceWeighting::hash(eckit::MD5& h) const {
    std::ostringstream s;
    s << *this;
    h.add(s.str());
}


static DistanceWeightingBuilder<InverseDistanceWeighting> __distance1("inverse-distance-weighting");
static DistanceWeightingBuilder<InverseDistanceWeighting> __distance2("shepard");


}  // namespace distance
}  // namespace knn
}  // namespace method
}  // namespace mir

