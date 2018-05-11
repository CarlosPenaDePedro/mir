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


#include "mir/repres/gauss/regular/Regular.h"

#include <cmath>
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Plural.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/utils/MD5.h"
#include "mir/api/MIRJob.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Domain.h"
#include "mir/util/Grib.h"


namespace mir {
namespace repres {
namespace gauss {
namespace regular {


Regular::Regular(const param::MIRParametrisation& parametrisation) :
    Gaussian(parametrisation) {
    setNiNj();
}


Regular::Regular(size_t N, const util::BoundingBox& bbox) :
    Gaussian(N, bbox) {

    const std::vector<double>& lats = latitudes();
    cropToBoundingBox(N_, lats, bbox_);

    setNiNj();
}


Regular::~Regular() {
}


void Regular::cropToBoundingBox(size_t N, const std::vector<double>& latitudes, util::BoundingBox& bbox) {
    ASSERT(N > 0);
    ASSERT(N * 2 == latitudes.size());


    // adjust bounding box North/South
    Latitude n = bbox.north();
    Latitude s = bbox.south();

    double latMin = Latitude::NORTH_POLE.value();
    double latMax = Latitude::SOUTH_POLE.value();

    for (size_t i = 0; i < latitudes.size(); i++) {
        Latitude ll(latitudes[i]);
        if ((ll >= s) && (ll <= n)) {
            if (latMin > latitudes[i]) {
                latMin = latitudes[i];
            }
            if (latMax < latitudes[i]) {
                latMax = latitudes[i];
            }
        }
    }

    ASSERT(latMin <= latMax);
    n = latMax;
    s = latMin;


    // adjust bounding box West/East (actually, only East if periodic)
    Longitude w = bbox.west();
    Longitude e = bbox.east();

    eckit::Fraction inc = eckit::Fraction(90, N);
    ASSERT(inc > 0);

    if (e > w + Longitude::GLOBE - inc) {
        e = w + Longitude::GLOBE - inc;
    }

    // ensure 0 <= East - West < 360
    bool same(e == w);
    if (!same) {
        e = e.normalise(w);
        if (e == w) {
            e = w + Longitude::GLOBE - inc;
        }
    }


    // set bounding box and inform
    util::BoundingBox newbbox = util::BoundingBox(n, w, s, e);

    if (newbbox != bbox) {
        eckit::Channel& log = eckit::Log::debug<LibMir>();
        std::streamsize old = log.precision(12);
        log << "Regular::cropToBoundingBox: "
            << "\n   " << bbox
            << "\n > " << newbbox
            << std::endl;
        log.precision(old);

        bbox = newbbox;
    }
}


void Regular::fill(grib_info& info) const  {

    // See copy_spec_from_ksec.c in libemos for info

    info.grid.grid_type = GRIB_UTIL_GRID_SPEC_REGULAR_GG;

    info.grid.N = long(N_);
    info.grid.iDirectionIncrementInDegrees = getSmallestIncrement();
    info.grid.Ni = long(Ni_);
    info.grid.Nj = long(Nj_);

    bbox_.fill(info);

    /*
        Comment in libemos is:

        "grib_api to set global area in full precision for gaussian grid"

        TODO: check and document

    */

    // for GRIB, a global field is also aligned with Greenwich
    bool westAtGreenwich = bbox_.west() == Longitude::GREENWICH;

    long j = info.packing.extra_settings_count++;
    info.packing.extra_settings[j].type = GRIB_TYPE_LONG;
    info.packing.extra_settings[j].name = "global";
    info.packing.extra_settings[j].long_value = isGlobal() && westAtGreenwich ? 1 : 0;
}


void Regular::fill(api::MIRJob& job) const  {
    std::stringstream os;
    os << "F" << N_;
    job.set("gridname", os.str());
}


void Regular::makeName(std::ostream& out) const {
    out << "F" << N_;
    bbox_.makeName(out);
}


util::BoundingBox Regular::croppedBoundingBox(const util::BoundingBox& bbox) const {

    // crop latitude-wise, ensuring exact Gaussian latitudes
    Latitude n = bbox.north();
    Latitude s = bbox.south();

    const std::vector<double>& lats = latitudes();

    if (n < lats.back()) {
        n = lats.back();
    } else {
        auto best = std::lower_bound(lats.begin(), lats.end(), n.value(),
                                     [](const Latitude& l1, const Latitude& l2) {
            return !(l1 <= l2);
        });
        ASSERT(best != lats.end());
        n = *best;
    }

    if (s > lats.front()) {
        s = lats.front();
    } else {
        auto best = std::lower_bound(lats.rbegin(), lats.rend(), s.value(),
                                     [](const Latitude& l1, const Latitude& l2) {
            return !(l1 >= l2);
        });
        ASSERT(best != lats.rend());
        s = *best;
    }


    // crop longitude-wise
    Longitude w = bbox.west();
    Longitude e = bbox.east();
    ASSERT(w < e);
    ASSERT(w - e < Longitude::GLOBE);

    eckit::Fraction inc = getSmallestIncrement();

    eckit::Fraction west = w.fraction();
    eckit::Fraction::value_type Nw = (west / inc).integralPart();
    if (Nw * inc < west) {
        Nw += 1;
    }

    eckit::Fraction east = e.fraction();
    eckit::Fraction::value_type Ne = (east / inc).integralPart();
    if (Ne * inc > east) {
        Ne -= 1;
    }

    ASSERT(Nw <= Ne);

    w = Nw * inc;
    e = Ne * inc;


    // set bounding box and inform
    util::BoundingBox cropped(n, w, s, e);

    if (cropped != bbox) {
        eckit::Channel& log = eckit::Log::debug<LibMir>();
        std::streamsize old = log.precision(12);
        log << "Regular::croppedBoundingBox: "
            << "\n   " << bbox
            << "\n > " << cropped
            << std::endl;
        log.precision(old);
    }

    return cropped;
}


bool Regular::sameAs(const Representation& other) const {
    const Regular* o = dynamic_cast<const Regular*>(&other);
    return o && (N_ == o->N_) && (bbox_ == o->bbox_);
}


eckit::Fraction Regular::getSmallestIncrement() const {
    ASSERT(N_);
    return eckit::Fraction(90, N_);
}


size_t Regular::numberOfPoints() const {
    ASSERT(Ni_);
    ASSERT(Nj_);
    return Ni_ * Nj_;
}


bool Regular::getLongestElementDiagonal(double& d) const {
    eckit::Fraction inc = getSmallestIncrement();

    double l = inc / 2;
    d = atlas::util::Earth::distance(
                atlas::PointLonLat(-l, -l),
                atlas::PointLonLat( l,  l) );

    return true;
}


atlas::Grid Regular::atlasGrid() const {
    return atlas::grid::RegularGaussianGrid("F" + std::to_string(N_), domain());
}


void Regular::setNiNj() {
    const util::Domain dom = domain();
    ASSERT(N_);

    Ni_ = N_ * 4;
    if (!dom.isPeriodicEastWest()) {
        eckit::Fraction inc = getSmallestIncrement();

        eckit::Fraction w = bbox_.west().fraction();
        eckit::Fraction::value_type Nw = (w / inc).integralPart();
        if (Nw * inc < w) {
            Nw += 1;
        }

        eckit::Fraction e = bbox_.east().fraction();
        eckit::Fraction::value_type Ne = (e / inc).integralPart();
        if (Ne * inc > e) {
            Ne -= 1;
        }

        ASSERT(Ne - Nw + 1 > 0);
        Ni_ = size_t(Ne - Nw + 1);

        ASSERT(2 <= Ni_ && Ni_ <= N_ * 4);
    }

    Nj_ = N_ * 2;
    if (!dom.includesPoleNorth() || !dom.includesPoleSouth()) {
        const Longitude lon_middle = (dom.west() + dom.east()) / 2.;

        Nj_ = 0;
        for (const double& lat : latitudes()) {
            if (dom.contains(lat, lon_middle)) {
                ++Nj_;
            }
        }
        ASSERT(2 <= Nj_ && Nj_ <= N_ * 2);
    }

    eckit::Log::debug<LibMir>()
            << "Regular::setNiNj: Ni*Nj = " << Ni_ << " * " << Nj_ << " = " << (Ni_ * Nj_)
            << std::endl;
}


size_t Regular::frame(std::vector<double>& values, size_t size, double missingValue) const {

    // TODO: Check if that logic cannot also be used for other grid, and therefore move it to a higher class
    validate(values);

    size_t count = 0;

    size_t k = 0;
    for (size_t j = 0; j < Nj_; j++) {
        for (size_t i = 0; i < Ni_; i++) {
            if (!((i < size) || (j < size) || (i >= Ni_ - size) || (j >= Nj_ - size))) { // Check me, may be buggy
                values[k] = missingValue;
                count++;
            }
            k++;
        }
    }

    ASSERT(k == values.size());
    return count;
}


void Regular::shape(size_t& ni, size_t& nj) const {
    ni = Ni_;
    nj = Nj_;
}


}  // namespace regular
}  // namespace gauss
}  // namespace repres
}  // namespace mir

