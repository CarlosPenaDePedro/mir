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


#include "mir/util/Trace.h"

#include "eckit/config/Resource.h"
#include "eckit/log/ETA.h"
#include "eckit/log/Seconds.h"

#include "mir/config/LibMir.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Pretty.h"


namespace mir {
namespace trace {


template <>
detail::TraceT<eckit::ResourceUsage>::TraceT(const std::string& name, Log::Channel& log) {
    static bool usage = eckit::LibResource<bool, LibMir>(
        "mir-trace-resource-usage;"
        "$MIR_TRACE_RESOURCE_USAGE",
        false);
    if (usage) {
        info_ = new eckit::ResourceUsage(name.c_str(), log);
    }
}


template <>
double detail::TraceT<eckit::ResourceUsage>::elapsed() {
    NOTIMP;
}


ProgressTimer::ProgressTimer(const std::string& name, size_t limit, const Plural& units, Log::Channel& o, double time) :
    Timer(name, o), lastTime_(0.), counter_(0), units_(units), limit_(limit), time_(time) {}


bool ProgressTimer::operator++() {
    bool hasOutput = (0 < counter_) && (lastTime_ + time_ < elapsed());

    if (hasOutput) {
        lastTime_   = elapsed();
        double rate = double(counter_) / lastTime_;
        output() << Pretty(counter_, units_) << " in " << eckit::Seconds(lastTime_) << ", rate: " << rate << " "
                 << units_(counter_) << "/s"
                 << ", ETA: " << eckit::ETA(double(limit_ - counter_) / rate) << std::endl;
    }

    if (counter_ < limit_) {
        ++counter_;
    }

    return hasOutput;
}


}  // namespace trace
}  // namespace mir
