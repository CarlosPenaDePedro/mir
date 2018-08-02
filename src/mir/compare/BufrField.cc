/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <iostream>
#include <cmath>

#include "mir/compare/BufrField.h"
#include "mir/compare/FieldSet.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Colour.h"

#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"


namespace mir {
namespace compare {




void BufrField::addOptions(std::vector<eckit::option::Option*>& options) {
    using namespace eckit::option;


}




void BufrField::setOptions(const eckit::option::CmdArgs &args) {

}


BufrField::BufrField(const std::string& path, off_t offset, size_t length):
    FieldBase(path, offset, length){

    }


Field BufrField::field(const char* buffer, size_t size,
                       const std::string& path, off_t offset,
                       const std::vector<std::string>& ignore) {

    BufrField* field = new BufrField(path, offset, size);
    Field result(field);
    return result;
}

void BufrField::print(std::ostream &out) const {
    out << "BufrField[]";
}

//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
}  // namespace compare

}  // namespace mir
