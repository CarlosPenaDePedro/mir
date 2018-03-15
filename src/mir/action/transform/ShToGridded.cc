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
/// @author Tiago Quintino
///
/// @date Apr 2015


#include "mir/action/transform/ShToGridded.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/UnitSphere.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/log/Timer.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/MD5.h"
#include "mir/action/context/Context.h"
#include "mir/action/plan/Action.h"
#include "mir/action/transform/TransCache.h"
#include "mir/caching/InMemoryCache.h"
#include "mir/caching/LegendreCache.h"
#include "mir/caching/legendre/LegendreLoader.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/param/SimpleParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/MIRStatistics.h"
#include "mir/util/function/FunctionParser.h"


namespace mir {
namespace action {
namespace transform {


static eckit::Mutex amutex;


static InMemoryCache<TransCache> trans_cache("mirCoefficient",
        8L * 1024 * 1024 * 1024,
        8L * 1024 * 1024 * 1024,
        "$MIR_COEFFICIENT_CACHE",
        false); // Don't cleanup at exit: the Fortran part will dump core


static ShToGridded::atlas_trans_t getTrans(
        const param::MIRParametrisation& parametrisation,
        context::Context& ctx,
        const std::string& key,
        const atlas::Grid& grid,
        size_t truncation,
        const ShToGridded::atlas_config_t& options) {


    InMemoryCache<TransCache>::iterator j = trans_cache.find(key);
    if (j != trans_cache.end()) {
        return j->trans_;
    }


    // Make sure we have enough space in cache to add new coefficients
    // otherwise we may get killed by OOM thread
    size_t estimate = truncation * truncation * truncation / 2 * sizeof(double);
    trans_cache.reserve(estimate, caching::legendre::LegendreLoaderFactory::inSharedMemory(parametrisation));

    if (!parametrisation.has("caching")) {
        TransCache& tc = trans_cache[key];
        ShToGridded::atlas_trans_t& trans = tc.trans_;

        trans = ShToGridded::atlas_trans_t(grid, int(truncation), options);
        ASSERT(trans);
        return trans;
    }

    eckit::PathName path;
    ShToGridded::atlas_config_t optionsModified(options);

    {   // Block for timers

        eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().coefficientTiming_);

        class LegendreCacheCreator: public caching::LegendreCache::CacheContentCreator {

            context::Context& ctx_;
            const atlas::Grid& grid_;
            const size_t truncation_;
            const ShToGridded::atlas_config_t& options_;

            void create(const eckit::PathName& path, caching::LegendreCacheTraits::value_type& /*ignore*/, bool& saved) {
                eckit::TraceResourceUsage<LibMir> usage("Create legendre coefficients");

                eckit::Log::info() << "Create legendre coefficients: " << path << std::endl;

                eckit::AutoTiming timing(ctx_.statistics().timer_, ctx_.statistics().createCoeffTiming_);

                ShToGridded::atlas_config_t write(options_);
                write.set(atlas::option::write_legendre(path));

                // This will create the cache
                ShToGridded::atlas_trans_t tmp(grid_, int(truncation_), write);
                ASSERT(tmp);

                saved = path.exists();
            }
        public:
            LegendreCacheCreator(
                        context::Context& ctx,
                        const atlas::Grid& grid,
                        size_t truncation,
                        const ShToGridded::atlas_config_t& options) :
                ctx_(ctx),
                grid_(grid),
                truncation_(truncation),
                options_(options) {
                ASSERT(!options_.has("read_legendre"));
            }
        };

        static caching::LegendreCache cache;
        LegendreCacheCreator creator(ctx, grid, truncation, options);

        int dummy = 0;
        path = cache.getOrCreate(key, creator, dummy);
    }


    TransCache& tc = trans_cache[key];
    ShToGridded::atlas_trans_t& trans = tc.trans_;

    {
        eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().loadCoeffTiming_);

        eckit::TraceResourceUsage<LibMir> usage("SH2GG load coefficients");
        eckit::system::MemoryInfo before = eckit::system::SystemInfo::instance().memoryUsage();

        tc.inited_ = true;
        tc.loader_ = caching::legendre::LegendreLoaderFactory::build(parametrisation, path);
        eckit::Log::debug<LibMir>() << "SH2GG LegendreLoader " << *tc.loader_ << std::endl;

        trans = ShToGridded::atlas_trans_t(tc.loader_->transCache(), grid, int(truncation), options);

        eckit::system::MemoryInfo after = eckit::system::SystemInfo::instance().memoryUsage();
        after.delta(eckit::Log::info(), before);
    }

    ASSERT(tc.loader_);
    size_t memory = 0;
    size_t shared = 0;
    (tc.loader_->inSharedMemory() ? memory : shared) += tc.loader_->size();
    trans_cache.footprint(key, InMemoryCacheUsage(memory, shared));

    ASSERT(trans);
    return trans;
}


void ShToGridded::transform(data::MIRField& field, const repres::Representation& representation, context::Context& ctx) const {
    eckit::AutoLock<eckit::Mutex> lock(amutex); // To protect trans_cache

    // Make sure another thread to no evict anything from the cache while we are using it
    // FIXME check if it should be in ::execute()
    InMemoryCacheUser<TransCache> use(trans_cache, ctx.statistics().transHandleCache_);


    const size_t truncation = field.representation()->truncation();
    ASSERT(truncation);

    const atlas::Grid grid = representation.atlasGrid();
    ASSERT(grid);

    eckit::MD5 md5;
    options_.hash(md5);

    const std::string key =
            "T" + std::to_string(truncation)
            + ":" + representation.uniqueName()
            + ":" + md5.digest();

    atlas_trans_t trans;
    try {
        eckit::Timer time("ShToGridded::transform: setup", eckit::Log::debug<LibMir>());

        if (local()) {
            trans = atlas_trans_t(grid, int(truncation), options_);
        } else {

            trans = getTrans(parametrisation_,
                             ctx,
                             key,
                             grid,
                             truncation,
                             options_);
        }

    } catch (std::exception& e) {
        eckit::Log::error() << "ShToGridded::transform: setup: " << e.what() << std::endl;
        trans_cache.erase(key);
        throw;
    }
    ASSERT(trans);

    try {

        eckit::AutoTiming time(ctx.statistics().timer_, ctx.statistics().sh2gridTiming_);
        sh2grid(field, trans, grid);

    } catch (std::exception& e) {
        eckit::Log::error() << "ShToGridded::transform: invtrans: " << e.what() << std::endl;
        throw;
    }
}


ShToGridded::ShToGridded(const param::MIRParametrisation& parametrisation) :
    Action(parametrisation) {
    const param::MIRParametrisation& user = parametrisation.userParametrisation();

    std::string compressIf;
    if (user.get("transform-compress-if", compressIf)) {
        std::istringstream in(compressIf);
        util::function::FunctionParser p(in);
        compressIf_.reset(p.parse());
    }

    if (user.has("atlas-trans-local")) {
        local(true);
    }

    // TODO: MIR-183 let Trans decide the best Legendre transform method
    bool flt = false;
    user.get("atlas-trans-flt", flt);
    options_.set("flt", flt);

    // no partitioning
    options_.set(atlas::option::global());
}


ShToGridded::~ShToGridded() {
}


void ShToGridded::print(std::ostream& out) const {
    out << "ShToGridded=["
            "cropping=" << cropping_
        << ",options=" << options_
        << "]";
}


void ShToGridded::execute(context::Context& ctx) const {

    repres::RepresentationHandle out(outputRepresentation());

    transform(ctx.field(), *out, ctx);

    ctx.field().representation(out);
}


bool ShToGridded::mergeWithNext(const Action& next) {
    if (!cropping_.active()) {
        if (next.isCropAction() || next.isInterpolationAction()) {

            const util::BoundingBox& bbox = next.croppingBoundingBox();

            bool compress = local();
            if (local()) {
                compress = true;
            } else if (compressIf_) {
                using namespace eckit::geometry;

                // evaluate according to bounding box and area ratio to globe
                Point2 WestNorth(bbox.west().value(), bbox.north().value());
                Point2 EastSouth(bbox.east().value(), bbox.south().value());

                double ar = UnitSphere::area(WestNorth, EastSouth) /
                            UnitSphere::area();

                param::SimpleParametrisation vars;
                vars.set("N", WestNorth[1]);
                vars.set("W", WestNorth[0]);
                vars.set("S", EastSouth[1]);
                vars.set("E", EastSouth[0]);
                vars.set("ar", ar);

                compress = bool(compressIf_->eval(vars));
            } else {
                static util::BoundingBox global;
                compress = bbox != global;
            }

            if (compress) {

                repres::RepresentationHandle out(outputRepresentation());
                const util::BoundingBox extended = out->extendedBoundingBox(bbox);

                eckit::Log::debug<LibMir>()
                        << "ShToGridded::mergeWithNext: "
                        << "\n\t   " << *this
                        << "\n\t + " << next
                        << "\n\t extendedBoundingBox:"
                        << "\n\t   " << bbox
                        << "\n\t > " << extended
                        << std::endl;

                // Magic super-powers!
                cropping_.boundingBox(extended);
                local(true);

                return true;
            }
        }
    }
    return false;
}


void ShToGridded::local(bool l) {
    if (!l) {
        if (local()) {
            throw eckit::SeriousBug("ShToGridded::local: Atlas/Trans 'local' has been set, cannot revert");
        }
        return;
    }
    options_.set(atlas::option::type("local"));
}


bool ShToGridded::local() const {
    return options_.has("type") && options_.getString("type") == "local";
}


}  // namespace transform
}  // namespace action
}  // namespace mir

