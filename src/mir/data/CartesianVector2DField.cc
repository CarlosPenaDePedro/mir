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


#include "mir/data/CartesianVector2DField.h"

#include <cmath>
#include <iostream>
#include <memory>

#include "eckit/exception/Exceptions.h"

#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "mir/util/Angles.h"
#include "mir/util/Rotation.h"


namespace mir {
namespace data {


namespace {

inline double normalise(double x) {
    return std::max(std::min(x, 1.0), -1.0);
}

}  // namespace


CartesianVector2DField::CartesianVector2DField(const repres::Representation* representation, bool hasMissing,
                                               double missingValue) :
    valuesX_(),
    valuesY_(),
    missingValue_(missingValue),
    representation_(representation) {

    ASSERT(representation_);
    representation_->attach();
}


CartesianVector2DField::~CartesianVector2DField() {
    ASSERT(representation_);
    representation_->detach();
}


void CartesianVector2DField::rotate(const util::Rotation& rotation, MIRValuesVector& valuesX,
                                    MIRValuesVector& valuesY) const {

    // setup results vectors
    ASSERT(valuesX.size() == valuesY.size());
    ASSERT(valuesX.size());
    const size_t N = valuesX.size();

    // determine angle between meridians (c) using the (first) spherical law of cosines:
    // https://en.wikipedia.org/wiki/Spherical_law_of_cosines
    // NOTE: uses spherical (not geodetic) coordinates: C = θ = π / 2 - latitude
    ASSERT(rotation.south_pole_rotation_angle() == 0.);  // For now
    const double C = util::degree_to_radian(90. - rotation.south_pole_latitude().value()), cos_C = std::cos(C),
                 sin_C = std::sin(C);

    std::unique_ptr<repres::Iterator> it(representation_->iterator());
    size_t i = 0;

    while (it->next()) {
        ASSERT(i < N);

        if (valuesX[i] == missingValue_ || valuesY[i] == missingValue_) {
            valuesX[i] = valuesY[i] = missingValue_;
            ++i;
            continue;
        }

        const LongitudeDouble lonRotated = rotation.south_pole_longitude().value() - (*(*it))[1];
        const double lon_rotated         = lonRotated.normalise(LongitudeDouble::MINUS_DATE_LINE).value(),
                     lon_unrotated       = it->pointUnrotated().lon().value(),

                     a = util::degree_to_radian(lon_rotated), b = util::degree_to_radian(lon_unrotated),
                     q = (sin_C * lon_rotated < 0.) ? 1. : -1.,  // correct quadrant

            cos_c          = normalise(std::cos(a) * std::cos(b) + std::sin(a) * std::sin(b) * cos_C),
                     sin_c = q * std::sqrt(1. - cos_c * cos_c);

        // TODO: use matrix multiplication
        const double x = cos_c * valuesX[i] - sin_c * valuesY[i], y = sin_c * valuesX[i] + cos_c * valuesY[i];
        valuesX[i] = x;
        valuesY[i] = y;
        ++i;
    }
}


void CartesianVector2DField::print(std::ostream& out) const {
    out << "CartesianVector2DField["
        << "]";
}


}  // namespace data
}  // namespace mir
