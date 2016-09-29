/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"
#include "mir/config/LibMir.h"
#include "mir/method/decompose/Decompose.h"


namespace mir {
namespace method {
namespace decompose {


namespace {


static eckit::Mutex* local_mutex = 0;
static std::map< std::string, DecomposeFactory* > *m = 0;
static pthread_once_t once = PTHREAD_ONCE_INIT;


static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map< std::string, DecomposeFactory* >();
}


}  // (anonymous namespace)


Decompose::Decompose() {
}


DecomposeFactory::DecomposeFactory(const std::string& name) :
    name_(name) {
    pthread_once(&once, init);

    eckit::AutoLock< eckit::Mutex > lock(local_mutex);

    if (m->find(name) != m->end()) {
        throw eckit::SeriousBug("DecomposeFactory: duplicated Decompose: " + name);
    }

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


DecomposeFactory::~DecomposeFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(name_);
}


Decompose* DecomposeFactory::build(const std::string& name) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    eckit::Log::debug<LibMir>() << "Looking for DecomposeFactory [" << name << "]" << std::endl;

    std::map< std::string, DecomposeFactory* >::const_iterator j = m->find(name);
    if (j == m->end()) {
        eckit::Log::error() << "No DecomposeFactory for [" << name << "]"
                               "\nDecomposeFactories are:" << std::endl;
        list(eckit::Log::error());
        throw eckit::SeriousBug("No DecomposeFactory called \"" + name + "\"");
    }

    return (*j).second->make();
}


void DecomposeFactory::list(std::ostream& out) {
    std::map< std::string, DecomposeFactory* >::const_iterator j;
    for (j = m->begin(); j != m->end(); ++j) {
        out << "   " << (*j).first << "\n";
    }
    eckit::Log::error() << std::endl;
}


}  // namespace decompose
}  // namespace method
}  // namespace mir

