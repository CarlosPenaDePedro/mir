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


#include "mir/method/nonlinear/NonLinear.h"

#include <map>

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"


namespace mir {
namespace method {
namespace nonlinear {


static eckit::Mutex* local_mutex                   = nullptr;
static std::map<std::string, NonLinearFactory*>* m = nullptr;
static pthread_once_t once                         = PTHREAD_ONCE_INIT;
static void init() {
    local_mutex = new eckit::Mutex();
    m           = new std::map<std::string, NonLinearFactory*>();
}


NonLinear::NonLinear(const param::MIRParametrisation&) {}


bool NonLinear::canIntroduceMissingValues() const {
    return false;
}


NonLinear::~NonLinear() = default;


NonLinearFactory::NonLinearFactory(const std::string& name) : name_(name) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if (m->find(name) == m->end()) {
        (*m)[name] = this;
        return;
    }
    throw exception::SeriousBug("NonLinearFactory: duplicated NonLinear '" + name + "'");
}


NonLinearFactory::~NonLinearFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);
}


const NonLinear* NonLinearFactory::build(const std::string& name, const param::MIRParametrisation& param) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    Log::debug() << "NonLinearFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(Log::error() << "No NonLinearFactory '" << name << "', choices are:\n");
        throw exception::SeriousBug("No NonLinearFactory '" + name + "'");
    }

    return j->second->make(param);
}


void NonLinearFactory::list(std::ostream& out) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace nonlinear
}  // namespace method
}  // namespace mir
