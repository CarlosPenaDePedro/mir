/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/param/rules/Rules.h"


namespace mir {
namespace param {
namespace rules {


Rules::Rules() {
}


Rules::~Rules() {
}


const MIRParametrisation& Rules::operator[](const long& paramId) const {
    const auto i = container_t::find(paramId);

    if (i == container_t::end()) {
        static const SimpleParametrisation empty;
        return empty;
    }

    return *(i->second);
}


SimpleParametrisation& Rules::lookup(const std::string& ruleName, long ruleValue) {

    ASSERT(ruleName == "paramId");
    long paramId = ruleValue;

    auto& p = container_t::operator[](paramId);

    if (!p) {
        p.reset(new CountedParametrisation());
    }

    return *p;
}


void Rules::print(std::ostream& s) const {
    const char* sep = "";
    s << "Rules=[";
    for (const auto& pid : *this) {
        s << sep << "\n\t" << pid.first << "={" << *pid.second << '}';
        sep = ",";
    }
    s << "]";
}


}  // namespace rules
}  // namespace param
}  // namespace mir

