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

#include "mir/method/knn/distance/DistanceWeightingWithLSM.h"


namespace mir::lsm {
class LandSeaMasks;
}  // namespace mir::lsm


namespace mir::method::knn::distance {


// Note: DistanceWeightingFactory cannot instantiate this because it
// requires the LandSeaMasks from the interpolation method
struct NearestLSMWithLowestIndex : DistanceWeightingWithLSM {
    NearestLSMWithLowestIndex(const param::MIRParametrisation&, const lsm::LandSeaMasks&);
    void operator()(size_t ip, const Point3& point, const std::vector<search::PointSearch::PointValueType>& neighbours,
                    std::vector<WeightMatrix::Triplet>& triplets) const override;

private:
    bool sameAs(const DistanceWeighting&) const override;
    void json(eckit::JSON&) const override;
    void print(std::ostream&) const override;
    void hash(eckit::MD5&) const override;

    const std::vector<bool>& imask_;
    const std::vector<bool>& omask_;
};


}  // namespace mir::method::knn::distance
