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


#include "mir/action/misc/ReferencePattern.h"

#include <iostream>
#include <cmath>
#include <memory>

#include "eckit/exception/Exceptions.h"

#include "mir/action/context/Context.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "mir/data/MIRField.h"
#include "mir/util/Angles.h"


namespace mir {
namespace action {


ReferencePattern::ReferencePattern(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {
}


ReferencePattern::~ReferencePattern() = default;


bool ReferencePattern::sameAs(const Action&) const {
    return false;
}


void ReferencePattern::print(std::ostream &out) const {
    out << "ReferencePattern[]";
}


void ReferencePattern::execute(context::Context & ctx) const {
    data::MIRField& field = ctx.field();

    repres::RepresentationHandle representation(field.representation());
    bool normalize = false;
    parametrisation_.get("0-1", normalize);

    std::vector<long> frequencies;
    if(!parametrisation_.get("frequencies", frequencies)) {
        frequencies.push_back(6);
        frequencies.push_back(3);
    }

    bool hasMissing = field.hasMissing();
    double missingValue = field.missingValue();

    for (size_t k = 0; k < field.dimensions(); k++) {
        MIRValuesVector& values = field.direct(k);

        double minvalue = 0;
        double maxvalue = 0;

        size_t first = 0;
        size_t count = 0;
        for (; first < values.size(); ++first) {
            if (!hasMissing || values[first] != missingValue) {
                minvalue = values[first];
                maxvalue = values[first];
                count++;
                break;
            }
        }

        if (first == values.size()) {
            // Only missing values
            continue;
        }

        for (size_t i = first; i < values.size(); ++i) {
            if (!hasMissing || values[i] != missingValue) {
                minvalue = std::min(minvalue, values[i]);
                maxvalue = std::max(maxvalue, values[i]);
                count++;
            }
        }

        if (normalize) {
            maxvalue = 1;
            minvalue = 0;
        }

        double median = (minvalue + maxvalue) / 2;
        double range = maxvalue - minvalue;

        double f1 = frequencies[0] / 2.0;
        double f2 = frequencies[1];

        std::unique_ptr<repres::Iterator> iter(representation->iterator());
        size_t j = 0;

        while (iter->next()) {
            const auto& p = iter->pointUnrotated();

            if (!hasMissing || values[j] != missingValue) {
                values[j] = range
                            * sin(f1 * util::degree_to_radian(p.lon().value()))
                            * cos(f2 * util::degree_to_radian(p.lat().value()))
                            * 0.5 + median;
            }

            j++;
        }

        ASSERT(j == values.size());

    }
}


const char* ReferencePattern::name() const {
    return "ReferencePattern";
}


namespace {
static ActionBuilder< ReferencePattern > action("misc.pattern");
}


}  // namespace action
}  // namespace mir

