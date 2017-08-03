/*
 * (C) Copyright 1996-2017 ECMWF.
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
#include <vector>
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Timer.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "mir/action/context/Context.h"
#include "mir/action/transform/TransCache.h"
#include "mir/action/transform/TransInitor.h"
#include "mir/caching/InMemoryCache.h"
#include "mir/caching/LegendreCache.h"
#include "mir/caching/legendre/LegendreLoader.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/MIRStatistics.h"



namespace mir {
namespace action {
namespace transform {


namespace {


static eckit::Mutex amutex;
static mir::InMemoryCache<TransCache> trans_handles("mirCoefficient",
        8L * 1024 * 1024 * 1024,
        "$MIR_COEFFICIENT_CACHE",
        false); // Don't cleanup at exit: the Fortran part will dump core


static void fillTrans(struct Trans_t& trans,
                      trans_options_t& options,
                      const repres::Representation& representation) {

    ASSERT(trans_new(&trans) == 0);
    trans.flt = int(options.flt);

    ASSERT(trans_set_trunc(&trans, options.truncation) == 0);

    representation.initTrans(trans);

}


static void createCoefficients(const eckit::PathName& path,
                               trans_options_t& options,
                               const repres::Representation& representation,
                               context::Context& ctx) {
    eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().createCoeffTiming_);

    struct Trans_t tmp_trans;
    fillTrans(tmp_trans, options, representation);

    ASSERT(trans_set_write(&tmp_trans, path.asString().c_str()) == 0);
    ASSERT(trans_setup(&tmp_trans) == 0); // This will create the cache

    trans_delete(&tmp_trans);

}


}  // (anonymous namespace)


void ShToGridded::transform(
        data::MIRField& field,
        const repres::Representation& representation,
        context::Context& ctx,
        const std::string& key,
        trans_options_t& options ) const {
    if (trans_handles.find(key) == trans_handles.end()) {

        eckit::PathName path;

        {   // Block for timers

            eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().coefficientTiming_);

            class LegendreCacheCreator: public caching::LegendreCache::CacheContentCreator {

                trans_options_t options_;
                const repres::Representation& representation_;
                context::Context & ctx_;

                virtual void create(const eckit::PathName& path, int& ignore, bool& saved) {
                    createCoefficients(path, options_, representation_, ctx_);
                }
            public:
                LegendreCacheCreator(trans_options_t& options,
                                     const repres::Representation& representation,
                                     context::Context& ctx):
                    options_(options), representation_(representation), ctx_(ctx) {}
            };

            static caching::LegendreCache cache;
            LegendreCacheCreator creator(options, representation, ctx);

            int dummy = 0;
            path = cache.getOrCreate(key, creator, dummy);
        }

        {   // Block for timers

            eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().loadCoeffTiming_);

            eckit::Timer timer("Loading coefficients");

            TransCache &tc = trans_handles[key];

            struct Trans_t &trans = tc.trans_;
            fillTrans(trans, options, representation);

            tc.inited_ = true;
            tc.loader_ = caching::legendre::LegendreLoaderFactory::build(parametrisation_, path);
            // eckit::Log::info() << "LegendreLoader " << *tc.loader_ << std::endl;

            ASSERT(trans_set_cache(&trans, tc.loader_->address(), tc.loader_->size()) == 0);

            ASSERT(trans.ndgl > 0 && (trans.ndgl % 2) == 0);

            ASSERT(trans_setup(&trans) == 0);
        }

        // trans_handles.footprint(key, after - before);


    }


    eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().sh2gridTiming_);
    eckit::Timer timer("SH2GRID");

    TransCache& tc = trans_handles[key];
    struct Trans_t& trans = tc.trans_;


    // transform sp to gp fields
    sh2grid(trans, field);


    // trans_delete(&trans);

}


void ShToGridded::transform(data::MIRField& field, const repres::Representation& representation, context::Context& ctx) const {
    eckit::AutoLock<eckit::Mutex> lock(amutex); // To protect trans_handles

    TransInitor::instance(); // Will init trans if needed

    trans_options_t options(transOptions_);
    options.truncation = field.representation()->truncation();

    std::ostringstream os;
    os << "T" << options.truncation
       << ":" << "flt" << options.flt
       << ":" << representation.uniqueName();
    std::string key(os.str());

    try {
        transform(field, representation, ctx, key, options);
    } catch (std::exception& e) {
        eckit::Log::error() << "Error while running SH2GRID: " << e.what() << std::endl;
        trans_handles.erase(key);
        throw;
    }
}


ShToGridded::ShToGridded(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {

    // MIR-183: optimal solution is setting flt = -1 to let Trans decide the best Legendre transform method
    transOptions_.flt = 0;
    parametrisation.get("trans-fast-legendre-transform", transOptions_.flt);
}


ShToGridded::~ShToGridded() {
}


void ShToGridded::execute(context::Context& ctx) const {

    // Make sure another thread to no evict anything from the cache while we are using it
    InMemoryCacheUser<TransCache> use(trans_handles, ctx.statistics().transHandleCache_);

    repres::RepresentationHandle out(outputRepresentation());

    transform(ctx.field(), *out, ctx);

    ctx.field().representation(out);
}


}  // namespace transform
}  // namespace action
}  // namespace mir

