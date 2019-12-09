/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/input/MIRInput.h"

#include <iomanip>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/StdFile.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "mir/config/LibMir.h"
#include "mir/input/DummyInput.h"
#include "mir/input/GribFileInput.h"
#include "mir/util/Grib.h"


namespace mir {
namespace input {


MIRInput::MIRInput() = default;


MIRInput::~MIRInput() = default;


grib_handle* MIRInput::gribHandle(size_t) const {
    // ASSERT(which == 0);
    static grib_handle* handle = nullptr;
    if (!handle) {
        handle = grib_handle_new_from_samples(nullptr, "GRIB1");
        grib_set_long(handle, "paramId", 255);
        ASSERT(handle);
    }
    return handle;
}


void MIRInput::setAuxiliaryInformation(const std::string&) {
    std::ostringstream os;
    os << "MIRInput::setAuxiliaryInformation() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


bool MIRInput::next() {
    std::ostringstream os;
    os << "MIRInput::next() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


size_t MIRInput::copy(double*, size_t) const {
    std::ostringstream os;
    os << "MIRInput::copy() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


size_t MIRInput::dimensions() const {
    std::ostringstream os;
    os << "MIRInput::dimensions() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

//=========================================================================


namespace {
static pthread_once_t once                          = PTHREAD_ONCE_INIT;
static eckit::Mutex* local_mutex                    = nullptr;
static std::map<unsigned long, MIRInputFactory*>* m = nullptr;
static void init() {
    local_mutex = new eckit::Mutex();
    m           = new std::map<unsigned long, MIRInputFactory*>();
}
}  // namespace


MIRInputFactory::MIRInputFactory(unsigned long magic) : magic_(magic) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    if (m->find(magic) != m->end()) {
        std::ostringstream oss;
        oss << "MIRInputFactory: duplicate '" << std::hex << magic << "'";
        throw eckit::SeriousBug(oss.str());
    }

    (*m)[magic] = this;
}


MIRInputFactory::~MIRInputFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    m->erase(magic_);
}


static void put(std::ostream& out, unsigned long magic) {
    out << "0x" << std::hex << std::setfill('0') << std::setw(8) << magic << std::dec << std::setfill(' ');

    unsigned char p[5] = {
        0,
    };

    for (int i = 3; i >= 0; i--) {
        unsigned char c = magic & 0xff;
        magic >>= 8;
        p[i] = isprint(c) ? c : '.';
    }

    out << " (" << p << ")";
}


MIRInput* MIRInputFactory::build(const std::string& path, const param::MIRParametrisation& parametrisation) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    std::string input;
    parametrisation.userParametrisation().get("input", input);

    // attach information after construction (pe. extra files), so virtual methods are specific to child class
    auto aux = [&input](MIRInput* in) {
        ASSERT(in);
        if (!input.empty()) {
            in->setAuxiliaryInformation(input);
        }
        return in;
    };

    bool dummy = false;
    if (parametrisation.userParametrisation().get("dummy", dummy) && dummy) {
        return aux(new DummyInput());
    }

    eckit::AutoStdFile f(path);
    unsigned long magic    = 0;
    unsigned char smagic[] = "????";
    unsigned char* p       = smagic;

    for (size_t i = 0; i < 4; i++) {
        unsigned char c;
        if (fread(&c, 1, 1, f)) {
            magic <<= 8;
            magic |= c;
            p[i] = isprint(c) ? c : '.';
        }
    }

    std::ostringstream oss;
    oss << "0x" << std::hex << magic << std::dec << " (" << smagic << ")";
    eckit::Log::debug<LibMir>() << "MIRInputFactory: looking for '" << oss.str() << "'" << std::endl;

    auto j = m->find(magic);
    if (j == m->end()) {
        list(eckit::Log::warning() << "MIRInputFactory: unknown '" << oss.str() << "', choices are: ");
        eckit::Log::warning() << std::endl;

        eckit::Log::warning() << "MIRInputFactory: assuming 'GRIB'" << std::endl;
        return aux(new GribFileInput(path));
    }

    return aux((*j).second->make(path));
}


void MIRInputFactory::list(std::ostream& out) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep;
        put(out, j.first);
        sep = ", ";
    }
}


}  // namespace input
}  // namespace mir
