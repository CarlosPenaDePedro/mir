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


#include <algorithm>
#include <limits>
#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/LinearAlgebra.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/log/Log.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/TraceTimer.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/FactoryOption.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/option/VectorOption.h"

#include "mir/action/context/Context.h"
#include "mir/api/Atlas.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/input/GribFileInput.h"
#include "mir/output/GribFileOutput.h"
#include "mir/param/CombinedParametrisation.h"
#include "mir/param/ConfigurationWrapper.h"
#include "mir/param/DefaultParametrisation.h"
#include "mir/param/RuntimeParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/repres/latlon/RegularLL.h"
#include "mir/search/PointSearch.h"
#include "mir/tools/MIRTool.h"
#include "mir/util/Domain.h"
#include "mir/util/MIRStatistics.h"
#include "mir/util/Pretty.h"


using namespace mir;


class MIRClimateFilter : public mir::tools::MIRTool {
private:
    void execute(const eckit::option::CmdArgs&);

    int numberOfPositionalArguments() const { return 2; }

    void usage(const std::string& tool) const {
        eckit::Log::info()
            << "\nClimate filter (topographic data smoothing operator) for GRIB global regular_ll (no missing values.)"
               "\n"
               "\nUsage: "
            << tool
            << " [--k=min/max] [--distance=real] [--delta=real] [--weight-min=real] input output"
               "\nExamples: "
               "\n  % "
            << tool << " --delta=1000 --distance=5000 --k=4/100 lsm lsm-filtered" << std::endl;
    }

public:
    MIRClimateFilter(int argc, char** argv) : MIRTool(argc, argv) {
        using eckit::linalg::LinearAlgebra;
        options_.push_back(new eckit::option::VectorOption<size_t>(
            "k", "Range of neighbour points to weight (k, default [4, infty[)", 2));
        options_.push_back(new eckit::option::SimpleOption<double>(
            "distance", "Climate filter radius [m] of neighbours to weight (default 1.)"));
        options_.push_back(new eckit::option::SimpleOption<double>(
            "delta", "Climate filter width of filter edge [m], must be greater than 'distance' (default 1000.)"));
        options_.push_back(new eckit::option::SimpleOption<double>(
            "weight-min", "Climate filter point minimum relative weight ([0, 1], default 0.001)"));
        options_.push_back(new eckit::option::SimpleOption<bool>(
            "no-backend", "No linear algebra backend (minimum memory requirements)"));
        options_.push_back(new eckit::option::FactoryOption<LinearAlgebra>(
            "backend", "Linear algebra backend (default '" + LinearAlgebra::backend().name() + "')"));
        options_.push_back(
            new eckit::option::FactoryOption<search::TreeFactory>("point-search-trees", "k-d tree control"));
    }
};


void MIRClimateFilter::execute(const eckit::option::CmdArgs& args) {

    // statistics, timers and options
    eckit::ResourceUsage usage("MIRClimateFilter");
    eckit::TraceTimer<LibMir> timer("MIRClimateFilter");

    std::unique_ptr<input::MIRInput> in(new input::GribFileInput(args(0)));
    ASSERT(in);

    std::unique_ptr<output::MIROutput> out(new output::GribFileOutput(args(1)));
    ASSERT(out);

    auto& log = eckit::Log::info();
    util::MIRStatistics statistics;
    context::Context ctx(*in, statistics);

    static const param::DefaultParametrisation defaults;
    static const param::ConfigurationWrapper commandLine(args);
    param::RuntimeParametrisation user(commandLine);
    user.set("filter", true);

    const bool noBackend = args.has("no-backend");

    std::vector<size_t> k{4, std::numeric_limits<size_t>::max()};
    args.get("k", k);
    ASSERT(k.size() == 2 && k[0] <= k[1]);

    double distance = 1.;
    args.get("distance", distance);
    ASSERT(distance > 0.);

    double delta = 1000.;
    args.get("delta", delta);
    ASSERT(delta > 0.);

    double weightMin = 0.001;
    args.get("weight-min", weightMin);
    ASSERT(0 <= weightMin && weightMin <= 1.);


    int field = 0;
    while (in->next()) {
        log << "============> field: " << ++field << std::endl;

        // setup parametrisation
        std::unique_ptr<param::MIRParametrisation> param(
            new param::CombinedParametrisation(user, in->parametrisation(), defaults));
        ASSERT(param);


        // input/output field values
        double t = timer.elapsed();
        ASSERT(ctx.field().dimensions() == 1);

        size_t Ni = 0;
        size_t Nj = 0;
        repres::RepresentationHandle rep(ctx.field().representation());
        {
            auto ll = dynamic_cast<const repres::latlon::RegularLL*>(static_cast<const repres::Representation*>(rep));
            if (!ll || !rep->domain().isGlobal() || ctx.field().hasMissing()) {
                throw eckit::UserError("MIRClimateFilter: input field should be global regular_ll, no missing values");
            }
            Ni = ll->Ni();
            Nj = ll->Nj();
        }
        ASSERT(Nj > 1);
        ASSERT(Ni > 1);
        ASSERT(Ni * Nj == rep->numberOfPoints());

        auto& input = ctx.field().values(0);
        MIRValuesVector output(input.size());

        log << "input/output field values: " << eckit::Seconds(timer.elapsed() - t) << std::endl;


        // k-d tree
        t = timer.elapsed();
        search::PointSearch tree(*param, *rep);
        log << "k-d tree: " << eckit::Seconds(timer.elapsed() - t) << std::endl;


        // coordinates
        t = timer.elapsed();
        std::vector<double> lat(Nj);
        std::vector<double> lon(Ni);
        {
            long scan;
            param->get("scanningMode", scan);
            ASSERT(scan == 0L);

            double max;
            double min;
            ASSERT(param->get("south", min = 0));
            ASSERT(param->get("north", max = 0));
            ASSERT(min < max);
            for (size_t j = 0; j < Nj; ++j) {
                lat[Nj - j - 1] = (max * double(j) + min * double(Nj - 1 - j)) / double(Nj - 1);
            }

            ASSERT(param->get("west", min = 0));
            ASSERT(param->get("east", max = 0));
            ASSERT(min < max);
            for (size_t i = 0; i < Ni; ++i) {
                lon[i] = (max * double(i) + min * double(Ni - 1 - i)) / double(Ni - 1);
            }
        }
        log << "coordinates: " << eckit::Seconds(timer.elapsed() - t) << std::endl;


        {
            Pretty::ProgressTimer progress("Locating", Nj, {"row"}, log);
            double farthest = 0;
            double tClosest = 0;
            double tMatrixA = 0;
            double tVectorY = 0;
            double tVectorX = 0;

            for (size_t j = 0; j < Nj; ++j) {
                if (++progress) {
                    log << "   latitude: " << lat[j] << " degree"
                        << "\n   farthest: " << farthest << " m"
                        << "\n   closest: " << eckit::Seconds(tClosest) << "\n   matrix A: " << eckit::Seconds(tMatrixA)
                        << "\n   vector Y: " << eckit::Seconds(tVectorY)
                        << "\n   vector X: " << eckit::Seconds(tVectorX) << "\n"
                        << tree << std::endl;
                    tClosest = tMatrixA = tVectorY = tVectorX = farthest = 0;
                }

                // search neighbour points to P (start of j-th row)
                t = timer.elapsed();

                Point3 P;
                atlas::util::Earth::convertSphericalToCartesian({lon[0], lat[j]}, P);

                std::vector<search::PointSearch::PointValueType> closest;
                tree.closestWithinRadius(P, distance, closest);
                if (closest.size() < k[0]) {
                    tree.closestNPoints(P, k[1], closest);
                }
                tClosest += timer.elapsed() - t;


                // x = A y: set weights vector y with filter weights (to P), might not use all neighbour points
                t = timer.elapsed();

                size_t Nw = std::min(closest.size(), k[1]);
                std::vector<double> weights;
                weights.reserve(Nw);
                {
                    const double halfDelta = distance / 2.;

                    double sum = 0.;
                    for (size_t w = 0; w < Nw; ++w) {
                        auto r = Point3::distance(P, closest[w].point());
                        auto h = r < halfDelta - delta
                                     ? 1.
                                     : halfDelta + delta < r
                                           ? 0.
                                           : 0.5 + 0.5 * std::cos(M_PI_2 * (r - halfDelta + delta) / delta);
                        // h = std::max(0., std::min(0.99, h));

                        if (k[0] <= w && h < weightMin * (sum + h)) {
                            break;
                        }
                        farthest = r;  // closest points are ordered in increasing distance
                        weights.emplace_back(h);
                        sum += h;
                    }
                    Nw = weights.size();
                    ASSERT(0 < Nw && Nw <= closest.size());

                    if (sum > 0.) {
                        double invSum = 1. / sum;
                        std::for_each(weights.begin(), weights.end(), [=](double& w) { w *= invSum; });
                    }
                }

                eckit::linalg::Vector y(weights.data(), Nw);
                tVectorY += timer.elapsed() - t;

                if (noBackend) {
                    t = timer.elapsed();

                    for (size_t w = 0; w < Nw; ++w) {
                        auto a = weights[w];
                        auto n = closest[w].payload();
                        auto r = n / Ni;
                        auto c = n % Ni;

                        for (size_t i = 0, i0 = j * Ni, c0 = r * Ni; i < Ni; ++i) {
                            if (!w) {
                                output[i0 + i] = a * input[c0 + (c + i) % Ni];
                            }
                            else {
                                output[i0 + i] += a * input[c0 + (c + i) % Ni];
                            }
                        }
                    }

                    tVectorX += timer.elapsed() - t;
                    continue;
                }


                // x = A y: set input values matrix A (dense), assembled by shifting closest neighbours by i
                // size(A) = (number of weights, number of points in row j) -- could be a very large allocation
                t = timer.elapsed();
                eckit::linalg::Matrix A(Ni, Nw);
                for (size_t w = 0; w < Nw; ++w) {
                    auto n = closest[w].payload();
                    auto r = n / Ni;
                    auto c = n % Ni;
                    for (size_t i = 0, c0 = r * Ni; i < Ni; ++i) {
                        A(i, w) = input[c0 + (c + i) % Ni];
                    }
                }
                tMatrixA += timer.elapsed() - t;


                // x = A y: set weights vector y and output values vector x
                t = timer.elapsed();
                eckit::linalg::Vector x(output.data() + j * Ni, Ni);
                static const auto& la(eckit::linalg::LinearAlgebra::backend());
                la.gemv(A, y, x);

                tVectorX += timer.elapsed() - t;
            }

            ctx.field().update(output, 0);
            out->save(*param, ctx);
            statistics.report(log);
        }


        log << Pretty(field, {"field"}) << " in " << eckit::Seconds(timer.elapsed())
            << ", rate: " << double(field) / double(timer.elapsed()) << " "
            << "field/s" << std::endl;
    }
}

int main(int argc, char** argv) {
    MIRClimateFilter tool(argc, argv);
    return tool.start();
}
