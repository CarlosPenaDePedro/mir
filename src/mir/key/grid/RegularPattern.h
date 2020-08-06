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


#ifndef mir_key_grid_RegularPattern_h
#define mir_key_grid_RegularPattern_h

#include "mir/key/grid/NamedGridPattern.h"


namespace mir {
namespace key {
namespace grid {


class RegularPattern : public NamedGridPattern {
public:
    // -- Exceptions
    // None

    // -- Constructors

    RegularPattern(const std::string& name);
    RegularPattern(const RegularPattern&) = delete;

    // -- Destructor

    virtual ~RegularPattern();

    // -- Convertors
    // None

    // -- Operators

    RegularPattern& operator=(const RegularPattern&) = delete;

    // -- Methods
    // None

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

    virtual void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    virtual const Grid* make(const std::string& name) const;

    // -- Friends
    // None
};


}  // namespace grid
}  // namespace key
}  // namespace mir


#endif
