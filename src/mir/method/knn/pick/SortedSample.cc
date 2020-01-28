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


#include "mir/method/knn/pick/SortedSample.h"

#include <algorithm>

#include "eckit/utils/MD5.h"


namespace mir {
namespace method {
namespace knn {
namespace pick {


SortedSample::SortedSample(const param::MIRParametrisation& param) : sample_(param) {}


void SortedSample::pick(const search::PointSearch& tree, const eckit::geometry::Point3& p,
                        Pick::neighbours_t& closest) const {
    sample_.pick(tree, p, closest);

    std::sort(closest.begin(), closest.end(),
              [&p](const Pick::neighbours_t::value_type& a, const Pick::neighbours_t::value_type& b) {
                  return Point3::distance2(a.point(), p) < Point3::distance2(b.point(), p);
              });
}

size_t SortedSample::n() const { return sample_.n(); }


bool SortedSample::sameAs(const Pick& other) const {
    auto o = dynamic_cast<const SortedSample*>(&other);
    return o && sample_.sameAs(*o);
}


void SortedSample::hash(eckit::MD5& h) const {
    h << "sample-sorted";
    h << sample_;
}


void SortedSample::print(std::ostream& out) const {
    out << "SortedSample[sample=" << sample_ << "]";
}


static PickBuilder<SortedSample> __pick("sorted-sample");


}  // namespace pick
}  // namespace knn
}  // namespace method
}  // namespace mir
