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


#include "mir/data/MIRField.h"

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"

#include "mir/config/LibMir.h"
#include "mir/data/Field.h"
#include "mir/data/MIRFieldStats.h"


namespace mir {
namespace data {


MIRField::MIRField(const param::MIRParametrisation& param, bool hasMissing, double missingValue) :
    field_(new Field(param, hasMissing, missingValue)) {
    field_->attach();
}


MIRField::MIRField(const repres::Representation* repres, bool hasMissing, double missingValue) :
    field_(new Field(repres, hasMissing, missingValue)) {
    field_->attach();
}


MIRField::MIRField(const MIRField& other) : field_(other.field_) {
    field_->attach();
}


void MIRField::copyOnWrite() {
    if (field_->count() > 1) {
        // eckit::Log::info() << "XXXX copyOnWrite " << *field_ << std::endl;
        Field* f = field_->clone();
        field_->detach();
        field_ = f;
        field_->attach();
    }
}


// Warning: take ownership of values
void MIRField::update(MIRValuesVector& values, size_t which, bool recomputeHasMissing) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    // eckit::Log::info() << "MIRField::update " << *field_ << std::endl;

    copyOnWrite();
    field_->update(values, which, recomputeHasMissing);
}


size_t MIRField::dimensions() const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    return field_->dimensions();
}


void MIRField::dimensions(size_t size) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    copyOnWrite();
    field_->dimensions(size);
}


void MIRField::select(size_t which) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);
    // TODO: Check the if we can select() without copying everything first
    copyOnWrite();
    field_->select(which);
}


MIRField::~MIRField() {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    field_->detach();
}


void MIRField::print(std::ostream& out) const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    out << *field_;
}


const repres::Representation* MIRField::representation() const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    return field_->representation();
}


void MIRField::validate() const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    field_->validate();
}


MIRFieldStats MIRField::statistics(size_t i) const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    return field_->statistics(i);
}


void MIRField::representation(const repres::Representation* representation) {
    // eckit::Log::info() << "MIRField::representation " << *field_ << " => " << *representation << std::endl;
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    copyOnWrite();
    field_->representation(representation);
}


const MIRValuesVector& MIRField::values(size_t which) const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    return field_->values(which);
}


MIRValuesVector& MIRField::direct(size_t which) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    // eckit::Log::info() << "MIRField::direct " << *field_ << std::endl;
    copyOnWrite();
    return field_->direct(which);
}


void MIRField::metadata(size_t which, const std::map<std::string, long>& md) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    // eckit::Log::info() << "MIRField::paramId " << *field_ << std::endl;

    copyOnWrite();
    field_->metadata(which, md);
}

void MIRField::metadata(size_t which, const std::string& name, long value) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    // eckit::Log::info() << "MIRField::paramId " << *field_ << std::endl;

    copyOnWrite();
    field_->metadata(which, name, value);
}

const std::map<std::string, long>& MIRField::metadata(size_t which) const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    return field_->metadata(which);
}


bool MIRField::hasMissing() const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    return field_->hasMissing();
}


double MIRField::missingValue() const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    return field_->missingValue();
}


void MIRField::hasMissing(bool on) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    if (on != hasMissing()) {
        copyOnWrite();
        field_->hasMissing(on);
    }
}


void MIRField::missingValue(double value) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    if (value != missingValue()) {
        copyOnWrite();
        field_->missingValue(value);
    }
}


//=========================================================================


namespace {
static pthread_once_t once                     = PTHREAD_ONCE_INIT;
static eckit::Mutex* local_mutex               = nullptr;
static std::map<std::string, FieldFactory*>* m = nullptr;
static void init() {
    local_mutex = new eckit::Mutex();
    m           = new std::map<std::string, FieldFactory*>();
}
}  // namespace


FieldFactory::FieldFactory(const std::string& name) : name_(name) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    ASSERT(m->find(name) == m->end());
    (*m)[name] = this;
}


FieldFactory::~FieldFactory() {
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    m->erase(name_);
}


void FieldFactory::list(std::ostream& out) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    const char* sep = "";
    for (const auto& j : *m) {
        out << sep << j.first;
        sep = ", ";
    }
}


MIRField* FieldFactory::build(const std::string& name, const param::MIRParametrisation& params, bool hasMissing,
                              double missingValue) {
    pthread_once(&once, init);
    eckit::AutoLock<eckit::Mutex> lock(local_mutex);

    eckit::Log::debug<LibMir>() << "FieldFactory: looking for '" << name << "'" << std::endl;

    auto j = m->find(name);
    if (j == m->end()) {
        list(eckit::Log::error() << "FieldFactory: unknown '" << name << "', choices are: ");
        throw eckit::SeriousBug("FieldFactory: unknown '" + name + "'");
    }

    return (*j).second->make(params, hasMissing, missingValue);
}


}  // namespace data
}  // namespace mir
