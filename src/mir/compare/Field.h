/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date   May 2016


#ifndef mir_compare_Field_h
#define mir_compare_Field_h

#include <iosfwd>
#include <string>
#include <vector>

#include "mir/compare/FieldInfo.h"
#include "eckit/memory/Counted.h"


namespace eckit {
class JSON;
namespace option {
class Option;
class CmdArgs;
}
}

namespace mir {
namespace compare {

class FieldSet;

//----------------------------------------------------------------------------------------------------------------------
//

class FieldBase : public eckit::Counted {
public:

    FieldBase(const std::string& path, off_t offset, size_t length);

    off_t offset() const;
    size_t length() const;
    const std::string& path() const;


    virtual bool wrapped() const = 0;
    virtual bool less_than(const FieldBase& other) const = 0;
    virtual void whiteListEntries(std::ostream&) const = 0;
    virtual size_t differences(const FieldBase& other) const = 0;
    virtual std::ostream& printDifference(std::ostream&, const FieldBase& other) const = 0;
    virtual void compareExtra(std::ostream&, const FieldBase& other) const = 0;
    virtual bool same(const FieldBase& other) const = 0;
    virtual bool match(const FieldBase& other) const = 0;
    virtual std::ostream& printGrid(std::ostream&) const = 0;
    virtual bool match(const std::string&, const std::string&) const = 0;
    virtual size_t numberOfPoints() const = 0;
    virtual const std::string& format() const = 0;
    virtual bool canCompareFieldValues() const = 0;

    static double normaliseLongitude(double longitude);

protected:

    FieldInfo info_;

    virtual void print(std::ostream &out) const = 0;
    virtual void json(eckit::JSON& json) const = 0;

    friend std::ostream &operator<<(std::ostream &s, const FieldBase &x) {
        x.print(s);
        return s;
    }


    friend eckit::JSON &operator<<(eckit::JSON &s, const FieldBase &x) {
        x.json(s);
        return s;
    }

};

class Field {
public:

    Field(FieldBase* field = 0);
    Field(const Field& other);

    ~Field();

    Field& operator=(const Field& other);


    bool operator<(const Field& other) const;
    bool operator==(const Field& other) const;

    operator bool() const;

    off_t offset() const;
    size_t length() const;
    const std::string& path() const;


    bool same(const Field& other) const;
    bool match(const Field& other) const;

    std::vector<Field> bestMatches(const FieldSet & fields) const;
    std::vector<Field> sortByDifference(const FieldSet & fields) const;



    void whiteListEntries(std::ostream&) const;
    size_t differences(const Field& other) const;
    std::ostream& printDifference(std::ostream&, const Field& other) const;

    std::ostream& printGrid(std::ostream&) const;
    bool match(const std::string&, const std::string&) const;
    size_t numberOfPoints() const;
    const std::string& format() const;
    bool canCompareFieldValues() const;

    bool wrapped() const;
    void compareExtra(std::ostream&, const Field& other) const;



    static void addOptions(std::vector<eckit::option::Option*>& options);
    static void setOptions(const eckit::option::CmdArgs &args);

private:

    FieldBase* field_;

    void print(std::ostream &out) const;
    void json(eckit::JSON& json) const;

    friend std::ostream &operator<<(std::ostream &s, const Field &x) {
        x.print(s);
        return s;
    }

    friend eckit::JSON &operator<<(eckit::JSON &s, const Field &x) {
        x.json(s);
        return s;
    }

};


//----------------------------------------------------------------------------------------------------------------------

} // namespace mir
} // namespace compare

#endif
