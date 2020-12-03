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


#ifndef mir_data_MIRField_h
#define mir_data_MIRField_h

#include <iosfwd>
#include <map>
#include <string>

#include "eckit/thread/Mutex.h"

#include "mir/data/MIRValuesVector.h"


namespace mir {
namespace data {
class Field;
class MIRFieldStats;
}  // namespace data
namespace param {
class MIRParametrisation;
}
namespace repres {
class Representation;
}
}  // namespace mir


namespace mir {
namespace data {


class MIRField {
public:
    // -- Exceptions
    // None

    // -- Constructors

    /// @note not in Field
    MIRField(const MIRField& other);
    MIRField(const param::MIRParametrisation&, bool hasMissing = false, double missingValue = 0);
    MIRField(const repres::Representation*, bool hasMissing = false, double missingValue = 0);

    // -- Destructor

    ~MIRField();  // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators

    /// @note not in Field
    MIRField& operator=(const MIRField& other);

    // -- Methods

    size_t dimensions() const;
    void dimensions(size_t);

    /// Resize to one, and keep only which
    void select(size_t which);

    void representation(const repres::Representation*);
    const repres::Representation* representation() const;

    /// @warning Takes ownership of the vector
    void update(MIRValuesVector&, size_t which, bool recomputeHasMissing = false);

    const MIRValuesVector& values(size_t which) const;
    MIRValuesVector& direct(size_t which);  // Non-const version for direct update (Filter)

    void metadata(size_t which, const std::map<std::string, long>&);
    void metadata(size_t which, const std::string& name, long value);
    const std::map<std::string, long>& metadata(size_t which) const;

    void missingValue(double value);
    double missingValue() const;

    void hasMissing(bool on);
    bool hasMissing() const;

    void validate() const;

    void handle(size_t which, size_t handle);
    size_t handle(size_t which) const;

    MIRFieldStats statistics(size_t i) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods

    void print(std::ostream&) const;  // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    mutable eckit::Mutex mutex_;
    Field* field_;

    // -- Methods

    /// @note note in Field
    void copyOnWrite();

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const MIRField& p) {
        p.print(s);
        return s;
    }
};


class FieldFactory {
    std::string name_;
    virtual MIRField* make(const param::MIRParametrisation&, bool hasMissing, double missingValue) = 0;

    FieldFactory(const FieldFactory&) = delete;
    FieldFactory& operator=(const FieldFactory&) = delete;

protected:
    FieldFactory(const std::string&);
    virtual ~FieldFactory();

public:
    static void list(std::ostream&);
    static MIRField* build(const std::string& name, const param::MIRParametrisation&, bool hasMissing,
                           double missingValue);
};


template <class T>
class FieldBuilder : public FieldFactory {
    virtual MIRField* make(const param::MIRParametrisation& param, bool hasMissing, double missingValue) override {
        return new T(param, hasMissing, missingValue);
    }

public:
    FieldBuilder(const std::string& name) : FieldFactory(name) {}
};


}  // namespace data
}  // namespace mir


#endif
