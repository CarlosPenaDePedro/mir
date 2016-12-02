/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Sep 2016


#include <iostream>
#include <cmath>

#include "mir/compare/FieldInfo.h"
#include "eckit/exception/Exceptions.h"
#include "mir/compare/FieldSet.h"


namespace mir {
namespace compare {


FieldInfo::FieldInfo(const std::string& path, off_t offset, size_t length):
    path_(path),
    offset_(offset),
    length_(length) {}

void FieldInfo::print(std::ostream &out) const {
    out << "FieldInfo[path=" << path_ << ",offset=" << offset_ << ",length=" << length_ << "]";
}


off_t FieldInfo::offset() const {
    return offset_;
}


size_t FieldInfo::length() const {
    return length_;
}


const std::string& FieldInfo::path() const {
    return path_;
}


}  // namespace compare
}  // namespace mir

