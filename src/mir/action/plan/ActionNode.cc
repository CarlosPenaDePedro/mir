/*
 * (C) Copyright 1996-2015 ECMWF.
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

#include "mir/action/plan/ActionNode.h"
#include "mir/action/plan/Action.h"
#include "mir/param/RuntimeParametrisation.h"
#include "mir/data/MIRField.h"
#include "mir/log/MIR.h"
#include "mir/api/MIRWatcher.h"

#include "eckit/exception/Exceptions.h"



namespace mir {
namespace action {


ActionNode::ActionNode(const Action& action, api::MIRWatcher *watcher):
    action_(action),
    watcher_(watcher) {
}


ActionNode::~ActionNode() {

}

void ActionNode::execute(data::MIRField& field, util::MIRStatistics& statistics) const {
    // std::cout << " -----> " << action_ << std::endl << "      ---> " << field << std::endl;
    bool ok = false;
    try {
        action_.execute(field, statistics);
        if(watcher_ && graph_.empty()) {
            watcher_->success();
        }
        ok = true;
    } catch (std::exception& e) {

        eckit::Log::error() << e.what() << " while executing " << action_ << std::endl;

        bool rethrow = true;
        notifyFailure(e, action_, watcher_, rethrow);
        if (rethrow) {
            throw;
        }
    }
    if(ok) {
        graph_.execute(field, statistics);
    }
}

void ActionNode::notifyFailure(std::exception& e, const Action& action, api::MIRWatcher *watcher, bool& rethrow) const {
    graph_.notifyFailure(e, action, watcher_, rethrow);
}


const action::Action &ActionNode::action() const {
    return action_;
}

ActionGraph& ActionNode::graph() {
    return graph_;
}

void ActionNode::dump(std::ostream& out, size_t depth) const {
    for (size_t i = 0; i < depth; i++) {
        out << "   ";
    }
    out << action_ << std::endl;
    graph_.dump(out, depth + 1);
}


}  // namespace action
}  // namespace mir

