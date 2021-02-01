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


#include "mir/key/intgrid/Intgrid.h"

#include <iostream>
#include <map>
#include <mutex>

#include "mir/key/grid/GridPattern.h"
#include "mir/key/intgrid/NamedGrid.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"


namespace mir {
namespace key {
namespace intgrid {


static std::once_flag once;
static std::mutex* local_mutex                   = nullptr;
static std::map<std::string, IntgridFactory*>* m = nullptr;
static void init() {
    local_mutex = new std::mutex();
    m           = new std::map<std::string, IntgridFactory*>();
}


IntgridFactory::IntgridFactory(const std::string& name) : name_(name) {
    std::call_once(once, init);

    std::lock_guard<std::mutex> lock(*local_mutex);

    if (m->find(name) != m->end()) {
        throw exception::SeriousBug("IntgridFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


IntgridFactory::~IntgridFactory() {
    std::lock_guard<std::mutex> lock(*local_mutex);

    m->erase(name_);
}


Intgrid* IntgridFactory::build(const std::string& name, const param::MIRParametrisation& parametrisation,
                               long targetGaussianN) {
    std::call_once(once, init);
    std::lock_guard<std::mutex> lock(*local_mutex);

    Log::debug() << "IntgridFactory: looking for '" << name << "'" << std::endl;
    ASSERT(!name.empty());

    auto j = m->find(name);
    if (j != m->end()) {
        return j->second->make(parametrisation, targetGaussianN);
    }

    // Look for NamedGrid pattern matching
    if (grid::GridPattern::match(name)) {
        return new intgrid::NamedGrid(name, parametrisation);
    }

    list(Log::error() << "IntgridFactory: unknown '" << name << "', choices are: ");
    throw exception::SeriousBug("IntgridFactory: unknown '" + name + "'");
}


void IntgridFactory::list(std::ostream& out) {
    std::call_once(once, init);
    std::lock_guard<std::mutex> lock(*local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }

    grid::GridPattern::list(out << sep);
}


}  // namespace intgrid
}  // namespace key
}  // namespace mir
