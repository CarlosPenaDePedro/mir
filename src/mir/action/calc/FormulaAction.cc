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

#include "mir/action/calc/FormulaAction.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "mir/action/context/Context.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/MIRStatistics.h"
#include "mir/util/FormulaParser.h"
#include "mir/util/Formula.h"
#include "mir/data/MIRField.h"

#include "eckit/utils/Tokenizer.h"
#include "eckit/types/Types.h"
#include "eckit/utils/Translator.h"
namespace mir {
namespace action {

FormulaAction::FormulaAction(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {

    std::string formula;
    ASSERT(parametrisation.get("formula", formula));

    std::string metadata;
    ASSERT(parametrisation.get("formula.metadata", metadata));

    // TODO: create a parser
    eckit::Tokenizer parse1(",");
    eckit::Tokenizer parse2("=");
    eckit::Translator<std::string, long> s2l;

    std::vector<std::string> v;
    parse1(metadata, v);

    for (auto j = v.begin(); j != v.end(); ++j) {
        std::vector<std::string> w;
        parse2(*j, w);
        ASSERT(w.size() == 2);

        metadata_[w[0]] = s2l(w[1]);
    }

    std::istringstream in(formula);
    mir::util::FormulaParser p(in);
    formula_.reset(p.parse(parametrisation));
}


FormulaAction::~FormulaAction() = default;


bool FormulaAction::sameAs(const Action& other) const {
    auto o = dynamic_cast<const FormulaAction*>(&other);
    return o && (formula_->sameAs(*o->formula_)) && (metadata_ == o->metadata_);
}


void FormulaAction::print(std::ostream &out) const {
    out << "FormulaAction[" << *formula_ << ", metadata=" << metadata_ << "]";
}


void FormulaAction::execute(context::Context & ctx) const {

    eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().calcTiming_);

    formula_->perform(ctx);

    data::MIRField& field = ctx.field();
    for (size_t i = 0; i < field.dimensions(); i++) {
        field.metadata(i, metadata_);
    }

}

const char* FormulaAction::name() const {
    return "FormulaAction";
}

static ActionBuilder< FormulaAction > __action("calc.formula");




}  // namespace action
}  // namespace mir

