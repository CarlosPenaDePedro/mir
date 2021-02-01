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


#include "mir/caching/legendre/LegendreLoader.h"

#include <map>
#include <mutex>

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"


namespace mir {
namespace caching {
namespace legendre {


LegendreLoader::LegendreLoader(const param::MIRParametrisation& parametrisation, const eckit::PathName& path) :
    parametrisation_(parametrisation), path_(path.realName()) {}


LegendreLoader::~LegendreLoader() = default;


static std::once_flag once;
static std::recursive_mutex* local_mutex                = nullptr;
static std::map<std::string, LegendreLoaderFactory*>* m = nullptr;
static void init() {
    local_mutex = new std::recursive_mutex();
    m           = new std::map<std::string, LegendreLoaderFactory*>();
}


LegendreLoaderFactory::LegendreLoaderFactory(const std::string& name) : name_(name) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    if (m->find(name) != m->end()) {
        throw exception::SeriousBug("LegendreLoaderFactory: duplicate '" + name + "'");
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


LegendreLoaderFactory::~LegendreLoaderFactory() {
    std::lock_guard<std::recursive_mutex> lock(*local_mutex);

    m->erase(name_);
}


LegendreLoader* LegendreLoaderFactory::build(const param::MIRParametrisation& params, const eckit::PathName& path) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> guard(*local_mutex);

    std::string name = "mapped-memory";
    params.get("legendre-loader", name);

    Log::debug() << "LegendreLoaderFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(Log::error() << "LegendreLoaderFactory: unknown '" << name << "', choices are: ");
        throw exception::SeriousBug("LegendreLoaderFactory: unknown '" + name + "'");
    }

    return j->second->make(params, path);
}


bool LegendreLoaderFactory::inSharedMemory(const param::MIRParametrisation& params) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> guard(*local_mutex);

    std::string name = "mapped-memory";
    params.get("legendre-loader", name);

    Log::debug() << "LegendreLoaderFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(Log::error() << "LegendreLoaderFactory: unknown '" << name << "', choices are: ");
        throw exception::SeriousBug("LegendreLoaderFactory: unknown '" + name + "'");
    }

    return j->second->shared();
}


void LegendreLoaderFactory::list(std::ostream& out) {
    std::call_once(once, init);
    std::lock_guard<std::recursive_mutex> guard(*local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace legendre
}  // namespace caching
}  // namespace mir
