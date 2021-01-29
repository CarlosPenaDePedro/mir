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

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"

#include "mir/key/grid/GridPattern.h"
#include "mir/key/intgrid/NamedGrid.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"


namespace mir {
namespace key {
namespace intgrid {


static pthread_once_t once                       = PTHREAD_ONCE_INIT;
static eckit::Mutex* local_mutex                 = nullptr;
static std::map<std::string, IntgridFactory*>* m = nullptr;
static void init() {
    local_mutex = new eckit::Mutex();
    m           = new std::map<std::string, IntgridFactory*>();
}


IntgridFactory::IntgridFactory(const std::string& name) : name_(name) {
    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if (m->find(name) != m->end()) {
        throw exception::SeriousBug("IntgridFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


IntgridFactory::~IntgridFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    m->erase(name_);
}


Intgrid* IntgridFactory::build(const std::string& name, const param::MIRParametrisation& parametrisation,
                               long targetGaussianN) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

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
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

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
