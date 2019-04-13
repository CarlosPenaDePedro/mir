/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/method/knn/pick/NClosestWithMaxDistance.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/MD5.h"

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace method {
namespace knn {
namespace pick {


NClosestWithMaxDistance::NClosestWithMaxDistance(const param::MIRParametrisation& param) :
    distance_(param) {
    nclosest_ = 4;
    param.get("nclosest", nclosest_);
    ASSERT(nclosest_ > 0);
}


void NClosestWithMaxDistance::pick(const search::PointSearch& tree,
                    const eckit::geometry::Point3& p,
                    Pick::neighbours_t& closest) const {
    // TODO: improve the KDTree; this is slow because distance might be excessive
    distance_.pick(tree, p, closest);
    if (closest.size() > nclosest_) {
        // closest.resize(nClosest_);  // NOTE: non-resizable
        closest.erase(closest.begin() + neighbours_t::difference_type(nclosest_), closest.end());
        ASSERT(closest.size() == nclosest_);
    }
}


size_t NClosestWithMaxDistance::n() const {
    return nclosest_;
}


bool NClosestWithMaxDistance::sameAs(const Pick& other) const {
    auto o = dynamic_cast<const NClosestWithMaxDistance*>(&other);
    return o
            && nclosest_ == o->nclosest_
            && distance_.sameAs(o->distance_);
}


void NClosestWithMaxDistance::print(std::ostream& out) const {
    out << "NClosestWithMaxDistance["
            "nclosest=" << nclosest_
        << ",distance=" << distance_
        << "]";
}


void NClosestWithMaxDistance::hash(eckit::MD5& h) const {
    std::ostringstream s;
    s << *this;
    h.add(s.str());
}


static PickBuilder<NClosestWithMaxDistance> __pick("nclosest-with-max-distance");


}  // namespace pick
}  // namespace knn
}  // namespace method
}  // namespace mir

