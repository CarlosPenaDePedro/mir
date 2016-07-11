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


#ifndef MIRField_H
#define MIRField_H

#include <iosfwd>
#include <vector>
#include "eckit/thread/Mutex.h"

namespace mir {
namespace repres {
class Representation;
}
namespace param {
class MIRParametrisation;
}
namespace data {

class MIRFieldStats;
class Field;

class MIRField {

public:

    // -- Exceptions
    // None

    // -- Contructors
    MIRField(const MIRField& other);
    //

    MIRField(const param::MIRParametrisation&, bool hasMissing = false, double missingValue = 0);
    MIRField(const repres::Representation*, bool hasMissing = false, double missingValue = 0);

    // -- Destructor

    ~MIRField(); // Change to virtual if base class

    // -- Convertors

    // -- Operators
    MIRField& operator=(const MIRField& other);

    // -- Methods

    size_t dimensions() const;
    void dimensions(size_t);

    // Resize to one, and keep only which
    void select(size_t which);

    void representation(const repres::Representation *);
    const repres::Representation *representation() const;

    void update(std::vector<double> &, size_t which /*=0*/);  // Warning Takes ownership of the vector

    const std::vector<double> &values(size_t which /*=0*/) const;
    std::vector<double> &direct(size_t which /*=0*/);   // Non-const version for direct update (Filter)


    size_t paramId(size_t which) const;
    void paramId(size_t which, size_t param);


    void missingValue(double value);
    double missingValue() const;

    void hasMissing(bool on);
    bool hasMissing() const ;

    void validate() const;

    MIRFieldStats statistics(size_t i) const;

    //

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

    void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    // No copy allowed



    // -- Members

    mutable eckit::Mutex mutex_;
    Field *field_;

    // -- Methods

    void copyOnWrite();

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream &operator<<(std::ostream &s, const MIRField &p) {
        p.print(s);
        return s;
    }

};


}  // namespace data
}  // namespace mir
#endif

