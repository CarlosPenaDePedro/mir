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


#include "mir/netcdf/Dimension.h"

#include "mir/netcdf/Exceptions.h"
#include "mir/netcdf/Dataset.h"
#include "mir/netcdf/Remapping.h"

#include <iostream>

namespace mir {
namespace netcdf {

Dimension::Dimension(Dataset &owner, const std::string &name, size_t len):
    owner_(owner),
    name_(name),
    len_(len) {
}

Dimension::~Dimension() = default;

void Dimension::dump(std::ostream &out) const
{
    out << "\t" << name_ << " = " << len_ << " ;" << std::endl;
}

bool Dimension::sameAs(const Dimension &other) const {
    return name_ == other.name_;
}

bool Dimension::inUse() const {
    std::vector<Variable *> v = owner_.variablesForDimension(*this);
    return v.size() != 0;
}

const std::string &Dimension::name() const {
    return name_;
}

size_t Dimension::count() const {
    return len_;
}

void Dimension::grow(size_t)
{
    std::ostringstream os;
    os << "Dimension::grow() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

void Dimension::clone(Dataset &) const {
    std::ostringstream os;
    os << "Dimension::clone() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

void Dimension::create(int) const {
    std::ostringstream os;
    os << "Dimension::create() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

int Dimension::id()const {
    std::ostringstream os;
    os << "Dimension::id() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


void Dimension::realDimensions(std::vector<size_t>& dims) const {
    std::ostringstream os;
    os << "Dimension::realDimensions() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

}  // namespace netcdf
}  // namespace mir
