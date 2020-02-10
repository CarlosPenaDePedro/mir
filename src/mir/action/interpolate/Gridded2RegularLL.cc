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


#include "mir/action/interpolate/Gridded2RegularLL.h"

#include <iostream>
#include <vector>

#include "eckit/exception/Exceptions.h"

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/latlon/RegularLL.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2RegularLL::Gridded2RegularLL(const param::MIRParametrisation& parametrisation) :
    Gridded2UnrotatedGrid(parametrisation),
    reference_(0, 0) {

    std::vector<double> value;
    ASSERT(parametrisation_.get("grid", value));

    ASSERT(value.size() == 2);
    increments_ = util::Increments(value[0], value[1]);

    if (parametrisation_.userParametrisation().get("area", value)) {
        ASSERT(value.size() == 4);
        bbox_      = util::BoundingBox(value[0], value[1], value[2], value[3]);
        reference_ = PointLatLon(bbox_.south(), bbox_.west());
    }

    repres::latlon::LatLon::globaliseBoundingBox(bbox_, increments_, reference_);

    eckit::Log::debug<LibMir>() << "Gridded2RegularLL: globalise:"
                                << "\n\t" << increments_ << "\n\t" << bbox_
                                << "\n\t"
                                   "shifted in latitude? "
                                << increments_.isLatitudeShifted(bbox_)
                                << "\n\t"
                                   "shifted in longitude? "
                                << increments_.isLongitudeShifted(bbox_) << std::endl;
}


Gridded2RegularLL::~Gridded2RegularLL() = default;


bool Gridded2RegularLL::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2RegularLL*>(&other);
    return o && (increments_ == o->increments_) && (bbox_ == o->bbox_) && Gridded2GriddedInterpolation::sameAs(*o);
}


void Gridded2RegularLL::print(std::ostream& out) const {
    out << "Gridded2RegularLL["
           "increments="
        << increments_
        << ","
           "bbox="
        << bbox_ << ",";
    Gridded2UnrotatedGrid::print(out);
    out << "]";
}


const repres::Representation* Gridded2RegularLL::outputRepresentation() const {
    return new repres::latlon::RegularLL(increments_, bbox_, reference_);
}


const char* Gridded2RegularLL::name() const {
    return "Gridded2RegularLL";
}


static ActionBuilder<Gridded2RegularLL> grid2grid("interpolate.grid2regular-ll");


}  // namespace interpolate
}  // namespace action
}  // namespace mir
