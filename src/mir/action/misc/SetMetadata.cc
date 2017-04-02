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

#include "mir/action/misc/SetMetadata.h"

#include <iostream>

// #include "eckit/memory/ScopedPtr.h"

#include "mir/action/context/Context.h"
#include "mir/param/MIRParametrisation.h"
// #include "mir/repres/Iterator.h"
// #include "mir/repres/Representation.h"
#include "mir/data/MIRField.h"

#include "eckit/parser/Tokenizer.h"
#include "eckit/types/Types.h"
#include "eckit/utils/Translator.h"

namespace mir {
namespace action {


SetMetadata::SetMetadata(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {

    std::string metadata;
    ASSERT(parametrisation.get("metadata", metadata));

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

}


SetMetadata::~SetMetadata() {
}


bool SetMetadata::sameAs(const Action& other) const {
    const SetMetadata* o = dynamic_cast<const SetMetadata*>(&other);
    return o && (metadata_ == o->metadata_);
}

void SetMetadata::print(std::ostream &out) const {
    out << "SetMetadata[" << metadata_ << "]";
}


void SetMetadata::execute(context::Context & ctx) const {
    data::MIRField& field = ctx.field();
     for (size_t i = 0; i < field.dimensions(); i++) {
        field.metadata(i, metadata_);
    }
}


namespace {
static ActionBuilder< SetMetadata > action("set.parameter");
}


}  // namespace action
}  // namespace mir

