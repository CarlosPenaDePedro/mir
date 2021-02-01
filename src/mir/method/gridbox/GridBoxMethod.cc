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


#include "mir/method/gridbox/GridBoxMethod.h"

#include <algorithm>
#include <forward_list>
#include <sstream>
#include <vector>

#include "eckit/log/ResourceUsage.h"
#include "eckit/log/TraceTimer.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/utils/MD5.h"

#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "mir/search/PointSearch.h"
#include "mir/util/Domain.h"
#include "mir/util/Exceptions.h"
#include "mir/util/GridBox.h"
#include "mir/util/Pretty.h"
#include "mir/util/Types.h"


namespace mir {
namespace method {
namespace gridbox {


GridBoxMethod::GridBoxMethod(const param::MIRParametrisation& parametrisation) : MethodWeighted(parametrisation) {
    if (parametrisation.userParametrisation().has("rotation") ||
        parametrisation.fieldParametrisation().has("rotation")) {
        throw exception::UserError("GridBoxMethod: rotated input/output not supported");
    }
}


GridBoxMethod::~GridBoxMethod() = default;


bool GridBoxMethod::sameAs(const Method& other) const {
    auto o = dynamic_cast<const GridBoxMethod*>(&other);
    return (o != nullptr) && name() == o->name() && MethodWeighted::sameAs(*o);
}


void GridBoxMethod::assemble(util::MIRStatistics&, WeightMatrix& W, const repres::Representation& in,
                             const repres::Representation& out) const {
    eckit::Channel& log = Log::debug();
    log << "GridBoxMethod::assemble (input: " << in << ", output: " << out << ")" << std::endl;


    if (!in.domain().contains(out.domain())) {
        std::ostringstream msg;
        msg << "GridBoxMethod: input must contain output (input:" << in.domain() << ", output:" << out.domain() << ")";
        throw exception::UserError(msg.str());
    }

    const Pretty::Plural gridBoxes("grid box", "grid boxes");
    log << "GridBoxMethod: intersect " << Pretty(out.numberOfPoints()) << " from "
        << Pretty(in.numberOfPoints(), gridBoxes) << std::endl;


    // init structure used to fill in sparse matrix
    // TODO: triplets, really? why not writing to the matrix directly?
    std::vector<WeightMatrix::Triplet> weights_triplets;
    std::vector<WeightMatrix::Triplet> triplets;
    std::vector<search::PointSearch::PointValueType> closest;


    // set input and output grid boxes
    struct GridBoxes : std::vector<util::GridBox> {
        GridBoxes(const repres::Representation& rep) : std::vector<util::GridBox>(rep.gridBoxes()) {
            ASSERT(size() == rep.numberOfPoints());
        }
        double getLongestGridBoxDiagonal() const {
            double R = 0.;
            for (const auto& box : *this) {
                R = std::max(R, box.diagonal());
            }
            ASSERT(R > 0.);
            return R;
        }
    };

    const GridBoxes inBoxes(in);
    const GridBoxes outBoxes(out);
    const auto R = inBoxes.getLongestGridBoxDiagonal() + outBoxes.getLongestGridBoxDiagonal();

    size_t nbFailures           = 0;
    using failed_intersection_t = std::pair<size_t, PointLatLon>;
    std::forward_list<failed_intersection_t> failures;


    // set input k-d tree for grid boxes indices
    std::unique_ptr<search::PointSearch> tree;
    {
        eckit::ResourceUsage usage("GridBoxMethod::assemble create k-d tree", log);
        eckit::TraceTimer<LibMir> timer("k-d tree: create");
        tree.reset(new search::PointSearch(parametrisation_, in));
    }

    {
        Pretty::ProgressTimer progress("Intersecting", outBoxes.size(), gridBoxes, log);

        const std::unique_ptr<repres::Iterator> it(out.iterator());
        size_t i = 0;
        while (it->next()) {
            if (++progress) {
                log << *tree << std::endl;
            }


            // lookup
            tree->closestWithinRadius(it->point3D(), R, closest);
            ASSERT(!closest.empty());


            // calculate grid box intersections
            triplets.clear();
            triplets.reserve(closest.size());

            auto& box   = outBoxes.at(i);
            double area = box.area();
            ASSERT(area > 0.);

            double sumSmallAreas = 0.;
            bool areaMatch       = false;
            for (auto& c : closest) {
                auto j        = c.payload();
                auto smallBox = inBoxes.at(j);

                if (box.intersects(smallBox)) {
                    double smallArea = smallBox.area();
                    ASSERT(smallArea > 0.);

                    triplets.emplace_back(WeightMatrix::Triplet(i, j, smallArea / area));
                    sumSmallAreas += smallArea;

                    if ((areaMatch = eckit::types::is_approximately_equal(area, sumSmallAreas, 1. /*m^2*/))) {
                        break;
                    }
                }
            }


            // insert the interpolant weights into the global (sparse) interpolant matrix
            if (areaMatch) {
                std::copy(triplets.begin(), triplets.end(), std::back_inserter(weights_triplets));
            }
            else {
                ++nbFailures;
                failures.push_front({i, it->pointUnrotated()});
            }


            ++i;
        }
    }
    log << "Intersected " << Pretty(weights_triplets.size(), gridBoxes) << std::endl;

    if (nbFailures > 0) {
        auto& warning = Log::warning();
        warning << "Failed to intersect " << Pretty(nbFailures, gridBoxes) << ":";
        size_t count = 0;
        for (const auto& f : failures) {
            warning << "\n\tpoint " << f.first << " " << f.second;
            if (++count > 10) {
                warning << "\n\t...";
                break;
            }
        }
        warning << std::endl;
    }


    // fill sparse matrix
    W.setFromTriplets(weights_triplets);
}


void GridBoxMethod::hash(eckit::MD5& md5) const {
    MethodWeighted::hash(md5);
    md5.add(name());
}


void GridBoxMethod::print(std::ostream& out) const {
    out << "GridBoxMethod["
        << "name=" << name() << ",";
    MethodWeighted::print(out);
    out << "]";
}


bool GridBoxMethod::validateMatrixWeights() const {
    return false;
}


const char* GridBoxMethod::name() const {
    return "grid-box";
}


}  // namespace gridbox
}  // namespace method
}  // namespace mir
