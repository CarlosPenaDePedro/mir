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


#include "mir/method/fe/CalculateCellLongestDiagonal.h"

#include <algorithm>
#include <utility>

#include "eckit/exception/Exceptions.h"

#include "atlas/array/MakeView.h"
#include "atlas/mesh.h"
#include "atlas/runtime/Trace.h"
#include "atlas/util/Earth.h"


namespace mir {
namespace method {
namespace fe {


CalculateCellLongestDiagonal::CalculateCellLongestDiagonal(std::string name, bool force_recompute) :
    name_(std::move(name)),
    force_recompute_(force_recompute) {}


double CalculateCellLongestDiagonal::operator()(atlas::Mesh& mesh) const {
    using namespace atlas;

    bool recompute = force_recompute_ || !mesh.metadata().has(name_);

    if (recompute) {
        ATLAS_TRACE("CalculateCellLongestDiagonal");
        ASSERT(mesh.generated());

        auto& nodes       = mesh.nodes();
        const auto coords = array::make_view<double, 2>(nodes.field("xyz"));
        auto nbRealPts    = nodes.metadata().has("NbRealPts") ? nodes.metadata().get<idx_t>("NbRealPts") : nodes.size();


        // distance, up to Earth radius
        double d          = 0.;
        const double dMax = util::Earth::radius();


        // assumes:
        // - nb_cols == 3 implies triangle
        // - nb_cols == 4 implies quadrilateral
        // - no other element is supported at this time
        PointXYZ P[4];
        const auto& connectivity = mesh.cells().node_connectivity();

        for (idx_t e = 0; e < connectivity.rows(); ++e) {
            auto nb_cols = connectivity.cols(e);
            ASSERT(nb_cols == 3 || nb_cols == 4);

            // test edges and diagonals (quadrilaterals only)
            // (combinations of ni in [0, nb_cols[ and nj in [ni+1, nb_cols[)
            for (idx_t ni = 0; ni < nb_cols; ++ni) {
                auto i = connectivity(e, ni);
                P[ni].assign(coords(i, 0), coords(i, 1), coords(i, 2));
            }

            for (idx_t ni = 0; ni < nb_cols - 1; ++ni) {
                auto i = connectivity(e, ni);
                for (idx_t nj = ni + 1; nj < nb_cols; ++nj) {
                    auto j = connectivity(e, nj);

                    if (i < nbRealPts && j < nbRealPts) {
                        d = std::max(d, util::Earth::distance(P[ni], P[nj]));
                        if (d > dMax) {
                            eckit::Log::warning() << "CalculateCellLongestDiagonal: limited to maximum " << dMax << "m";
                            return dMax;
                        }
                    }
                }
            }
        }

        ASSERT(d > 0.);
        mesh.metadata().set(name_, d);
    }

    return mesh.metadata().getDouble(name_);
}


}  // namespace fe
}  // namespace method
}  // namespace mir
