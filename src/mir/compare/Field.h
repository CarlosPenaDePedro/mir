/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   Field.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef mir_compare_Field_H
#define mir_compare_Field_H

#include <iosfwd>
#include <string>
#include <set>
#include <vector>

#include "mir/compare/FieldInfo.h"
#include "eckit/memory/Counted.h"


namespace eckit {
namespace option {
class Option;
class CmdArgs;
}
}

namespace mir {
namespace compare {

class FieldSet;
class GribField;
class BufrField;

//----------------------------------------------------------------------------------------------------------------------
//

class FieldBase : public eckit::Counted {
public:

    FieldBase(const std::string& path, off_t offset, size_t length);

    off_t offset() const;
    size_t length() const;
    const std::string& path() const;

private:

    FieldInfo info_;

    virtual void print(std::ostream &out) const = 0;

    friend std::ostream &operator<<(std::ostream &s, const FieldBase &x) {
        x.print(s);
        return s;
    }

};

class Field {
public:

    Field(FieldBase* field);
    Field(const Field& other);

    ~Field();

    Field& operator=(const Field& other);


    bool operator<(const Field& other) const;
    static void addOptions(std::vector<eckit::option::Option*>& options);
    static void setOptions(const eckit::option::CmdArgs &args);

    const GribField& asGribField() const;
    const BufrField& asBufrField() const;

    GribField& asGribField(); 
    BufrField& asBufrField() ;

    off_t offset() const;
    size_t length() const;
    const std::string& path() const;


private:

    FieldBase* field_;

    void print(std::ostream &out) const;

    friend std::ostream &operator<<(std::ostream &s, const Field &x) {
        x.print(s);
        return s;
    }
};


//----------------------------------------------------------------------------------------------------------------------

} // namespace mir
} // namespace compare

#endif
