/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <iostream>
#include <memory>

#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"

#include "mir/api/Atlas.h"
#include "mir/namedgrids/NamedGrid.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/latlon/RegularLL.h"
#include "mir/util/Domain.h"


namespace mir {
namespace tests {
namespace unit {


using Handle = repres::RepresentationHandle;


CASE("Test number of points representation <=> grid") {
    auto& log = eckit::Log::info();

    using repres::latlon::RegularLL;
    using namedgrids::NamedGrid;


   Handle representations[] = {
       new RegularLL(util::Increments(1., 1.)),
       new RegularLL(util::Increments(1., 1.), util::BoundingBox(90, 0, 90, 360)),
       NamedGrid::lookup("O16").representation(),
       NamedGrid::lookup("O1280").representation(),
    };

    for (const auto& repres : representations) {
        size_t n1 = repres->numberOfPoints();
        log << "#=" << n1 << "\tfrom " << *repres << std::endl;

        auto grid = repres->atlasGrid();
        size_t n2 = grid.size();
        log << "#=" << n2 << "\tfrom " << grid.spec() << std::endl;

        EXPECT(n1 == n2);
    }
}


CASE("Test number of points representation <=> cropped grid") {
    auto& log = eckit::Log::info();

    using repres::latlon::RegularLL;

    const util::Domain domains[] = {
        {  90,    0,  90,  360 },  // North pole
        {  90,    0,  89,  360 },
        {   0,    0,   0,  360 },
        { -89,    0, -90,  360 },
        { -90,    0, -90,  360 },  // South pole
        {  90,    0, -90,    0 },  // Greenwhich
        {  90,    0, -90,    1 },
        {  90,  180, -90,  180 },  // date line
        {  90, -180, -90, -180 },  // date line
        {  90,   -1, -90,    0 },
        {  90,   -1, -90,   -1 },
    };

    for (const auto& dom : domains) {
        ASSERT(!dom.isGlobal());

        Handle repres(new RegularLL(util::Increments(1., 1.)));
        Handle represCropped(repres->croppedRepresentation(dom));

        size_t n1 = represCropped->numberOfPoints();
        log << "#=" << n1 << "\tfrom " << *represCropped << std::endl;

        auto grid = repres->atlasGrid();
        ASSERT(grid.domain().global());

        auto gridCropped = atlas::Grid(grid, dom);
        ASSERT(!gridCropped.domain().global());

        size_t n2 = gridCropped.size();
        log << "#=" << n2 << "\tfrom " << gridCropped.spec() << std::endl;

        EXPECT(n1 == n2);
    }
}


CASE("MIR-374") {
    auto& log  = eckit::Log::info();
    auto old   = log.precision(16);
    double eps = 1.e-6;

    const util::Domain domains[] = {
        {90, -180, -90, 180},
        {90, -180, -60, 180},
        {10, 0, 0, 10},
    };

    const std::string names[] = {"O16", "O640"};

    for (auto& domain : domains) {
        for (auto& name : names) {
            Handle repr = namedgrids::NamedGrid::lookup(name).representation();
            Handle crop = repr->croppedRepresentation(domain);

            std::unique_ptr<repres::Iterator> it(crop->iterator());
            ASSERT(it->next());
            PointLatLon p = it->pointUnrotated();
            log << p << std::endl;

            auto grid            = crop->atlasGrid();
            atlas::PointLonLat q = *(grid.lonlat().begin());

            log << "Compare p=" << p << " with q=" << q << std::endl;
            EXPECT(eckit::types::is_approximately_equal(p.lat().value(), q.lat(), eps));
            EXPECT(eckit::types::is_approximately_equal(p.lon().value(), q.lon(), eps));
        }
    }

    log.precision(old);
}


}  // namespace unit
}  // namespace tests
}  // namespace mir


int main(int argc, char **argv) {
    return eckit::testing::run_tests(argc, argv);
}

