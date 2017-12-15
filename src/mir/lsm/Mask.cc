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
/// @author Tiago Quintino
/// @date Apr 2015


#include "mir/lsm/Mask.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/MD5.h"
#include "mir/config/LibMir.h"
#include "mir/lsm/NoneLSM.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/param/RuntimeParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/repres/latlon/RegularLL.h"
#include "mir/repres/other/None.h"


namespace mir {
namespace lsm {


namespace {


static eckit::Mutex *local_mutex = 0;

static std::map<std::string, Mask *> *cache = 0;

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void init() {
    local_mutex = new eckit::Mutex();
    cache = new std::map<std::string, Mask *>();
}

static void setParametrisation(param::RuntimeParametrisation& runtime) {
    runtime.set("Ni", 360L);
    runtime.set("Nj", 181L);
}

static const param::SimpleParametrisation noParametrisation;
static const repres::other::None noRepresentation;


}  // (anonymous namespace)


Mask::Mask() :
    parametrisation_(noParametrisation),
    representation_(noRepresentation) {
}


Mask::Mask(const eckit::PathName& path,
           const param::MIRParametrisation& parametrisation,
           const repres::Representation& representation,
           const std::string& which) :
    path_(path),
    parametrisation_(parametrisation),
    representation_(representation),
    which_(which) {
}


Mask::~Mask() {
}


void Mask::hash(eckit::MD5& md5) const {
    md5.add(path_.asString());
    md5.add(representation_.uniqueName());
}


void Mask::hashCacheKey(eckit::MD5& md5,
                        const eckit::PathName& path,
                        const param::MIRParametrisation& parametrisation,
                        const repres::Representation& representation,
                        const std::string& which) {

    std::string interpolation;
    if (!parametrisation.get("lsm-interpolation-" + which, interpolation)) {
        if (!parametrisation.get("lsm-interpolation", interpolation)) {
            throw eckit::SeriousBug("No interpolation method defined for land-sea mask");
        }
    }

    md5 << path.asString();
    md5 << interpolation;
    md5 << representation.uniqueName();
}


Mask& Mask::lookup(const param::MIRParametrisation& parametrisation, const repres::Representation& representation, const std::string& which) {

    // lsm = true is a requirement for lsm processing
    bool lsm = false;
    parametrisation.get("lsm", lsm);

    if (!lsm) {
        return NoneLSM::noMask();
    }


    std::string name;
    if (!parametrisation.get("lsm-selection-" + which, name)) {
        if (!parametrisation.get("lsm-selection", name)) {
            throw eckit::SeriousBug("No lsm selection method provided");
        }
    }

    const LSMSelection& chooser = LSMSelection::lookup(name);
    std::string key = chooser.cacheKey(parametrisation, representation, which);

    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    eckit::Log::debug<LibMir>() << "Mask::lookup(" << key << ")" << std::endl;
    auto j = cache->find(key);
    if (j != cache->end()) {
        return *(*j).second;
    }

    Mask *mask = chooser.create(parametrisation, representation, which);

    (*cache)[key] = mask;

    return *(*cache)[key];
}


Mask& Mask::lookupInput(const param::MIRParametrisation& parametrisation, const repres::Representation& representation) {
    return lookup(parametrisation, representation, "input");
}


Mask& Mask::lookupOutput(const param::MIRParametrisation& parametrisation, const repres::Representation& representation) {
    return lookup(parametrisation, representation, "output");
}


static bool same(const param::MIRParametrisation& parametrisation1,
                 const param::MIRParametrisation& parametrisation2,
                 const std::string& which) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    // Check 'master' lsm key
    bool lsm1 = false;
    parametrisation1.get("lsm", lsm1);

    bool lsm2 = false;
    parametrisation2.get("lsm", lsm2);

    if (lsm1 != lsm2) {
        return false;
    }

#if 0
    // Check LSM selection method
    std::string name1;
    parametrisation1.get("lsm-selection-" + which, name1) || parametrisation1.get("lsm-selection", name1);

    std::string name2;
    parametrisation2.get("lsm-selection-" + which, name2) || parametrisation2.get("lsm-selection", name2);

    if (name1 != name2) {
        return false;
    }


    // Check LSM cache key
    // TODO: this creates a dummy representation, could be better...
    param::RuntimeParametrisation runtime1(parametrisation1);
    setParametrisation(runtime1);
    repres::latlon::RegularLL representation1(runtime1);

    param::RuntimeParametrisation runtime2(parametrisation2);
    setParametrisation(runtime2);
    repres::latlon::RegularLL representation2(runtime2);

    ASSERT(name1 == name2);
    const LSMSelection& chooser = LSMSelection::lookup(name1);
    const std::string key1 = chooser.cacheKey(parametrisation1, representation1, which);
    const std::string key2 = chooser.cacheKey(parametrisation2, representation2, which);

    if (key1 != key2) {
        return false;
    }
#endif

    return true;
}


bool Mask::sameInput(const param::MIRParametrisation& parametrisation1,
                     const param::MIRParametrisation& parametrisation2) {
    return same(parametrisation1, parametrisation2, "input");

}


bool Mask::sameOutput(const param::MIRParametrisation& parametrisation1,
                      const param::MIRParametrisation& parametrisation2) {
    return same(parametrisation1, parametrisation2, "output");
}


void Mask::print(std::ostream& out) const {
    out << "Mask=["
        <<  "path=" << path_
        << ",representation=" << representation_
        << ",which=" << which_
        << "]";
}


}  // namespace lsm
}  // namespace mir

