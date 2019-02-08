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


#include "mir/action/io/Copy.h"

#include <iostream>

#include "eckit/log/Statistics.h"

#include "mir/action/context/Context.h"
#include "mir/output/MIROutput.h"
#include "mir/util/MIRStatistics.h"


namespace mir {
namespace action {
namespace io {


Copy::Copy(const param::MIRParametrisation& parametrisation, output::MIROutput& output):
    Action(parametrisation),
    output_(output) {
}


Copy::~Copy() = default;


bool Copy::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Copy*>(&other);
    return o && output_.sameAs(o->output_);
}


void Copy::print(std::ostream& out) const {
    out << "Copy[output=" << output_ << "]";
}


void Copy::custom(std::ostream& out) const {
    out << "Copy[...]";
}


void Copy::execute(context::Context& ctx) const {
    eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().saveTiming_);
    output_.copy(parametrisation_, ctx);
}


const char* Copy::name() const {
    return "Copy";
}


bool Copy::isEndAction() const {
    return true;
}


}  // namespace io
}  // namespace action
}  // namespace mir

