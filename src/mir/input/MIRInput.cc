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


#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Once.h"
#include "eckit/thread/Mutex.h"
#include "eckit/exception/Exceptions.h"
#include "mir/config/LibMir.h"
#include "mir/util/Grib.h"
#include "mir/input/GribFileInput.h"

#include "mir/input/MIRInput.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdFile.h"

namespace mir {
namespace input {


static eckit::Mutex *local_mutex = 0;
static std::map<unsigned long, MIRInputFactory *> *m = 0;
static pthread_once_t once = PTHREAD_ONCE_INIT;


static void init() {
    local_mutex = new eckit::Mutex();
    m = new std::map<unsigned long, MIRInputFactory *>();
}


MIRInput::MIRInput() {
}


MIRInput::~MIRInput() {
}


grib_handle *MIRInput::gribHandle(size_t which) const {
    ASSERT(which == 0);
    static grib_handle *handle = 0;
    if (!handle) {
        handle = grib_handle_new_from_samples(0, "GRIB1");
        ASSERT(handle);
    }
    return handle;
}


bool MIRInput::next() {
    std::ostringstream os;
    MIRInput &self = *this;
    os << "MIRInput::next() not implemented for " << self;
    throw eckit::SeriousBug(os.str());
}

size_t MIRInput::copy(double *values, size_t size) const {
    std::ostringstream os;
    os << "MIRInput::copy() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


size_t MIRInput::dimensions() const {
    std::ostringstream os;
    os << "MIRInput::dimensions() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

//======================================

MIRInputFactory::MIRInputFactory(unsigned long magic):
    magic_(magic) {
    pthread_once(&once, init);

    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if (m->find(magic) != m->end()) {
        std::ostringstream oss;
        oss << "MIRInputFactory: duplication factory: " << std::hex << magic;
        throw eckit::SeriousBug(oss.str());
    }

    // ASSERT(m->find(magic) == m->end());
    (*m)[magic] = this;
}


MIRInputFactory::~MIRInputFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);
    m->erase(magic_);

}

static void put(std::ostream& out, unsigned long magic) {
    out << "0x" << std::hex <<  std::setfill('0') << std::setw(8)  << magic << std::dec;

    char p[5] = {0,};

    for (int i = 3; i >= 0; i--) {
        unsigned char c = magic & 0xff;
        magic >>= 8;
        if (isprint(c)) {
            p[i] = c;
        } else {
            p[i] = '.';
        }
    }

    out << " (" << p << ")";
}


MIRInput *MIRInputFactory::build(const std::string& path) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    eckit::StdFile f(path);
    unsigned long magic = 0;
    char smagic[] = "????";
    char *p = smagic;

    for (size_t i = 0; i < 4; i++) {
        unsigned char c;
        if (fread(&c, 1, 1, f)) {
            magic <<= 8;
            magic |= c;

            if (isprint(c)) {
                *p++ = c;
            } else {
                *p++ = '.';
            }
        }
    }


    eckit::Log::debug<LibMir>() << "Looking for MIRInputFactory [0x"
                                << std::hex
                                << magic
                                << std::dec
                                << " ("
                                << smagic
                                << ")"
                                << "]" << std::endl;

    auto j = m->find(magic);
    if (j == m->end()) {
        if (j == m->end()) {
            eckit::Log::error() << "No MIRInputFactory for [0x" << std::hex
                                << magic
                                << std::dec
                                << " ("
                                << smagic
                                << ")"
                                << "]" << std::endl;

            eckit::Log::error() << "MIRInputFactory are:" << std::endl;
            for (j = m->begin() ; j != m->end() ; ++j) {
                put(eckit::Log::error(), (*j).first);
                eckit::Log::error() << std::endl;
            }
            eckit::Log::error() << "MIRInputFactory assuming grib" << std::endl;
            return new GribFileInput(path);
        }
    }

    return (*j).second->make(path);
}


// void MIRInputFactory::list(std::ostream& out) {
//     pthread_once(&once, init);
//     eckit::AutoLock<eckit::Mutex> lock(local_mutex);

//     const char* sep = "";
//     for (std::map<std::string, MIRInputFactory *>::const_iterator j = m->begin() ; j != m->end() ; ++j) {
//         out << sep << (*j).first;
//         sep = ", ";
//     }
// }

}  // namespace input
}  // namespace mir

