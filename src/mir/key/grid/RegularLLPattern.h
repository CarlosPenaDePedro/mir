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


#ifndef mir_key_grid_RegularLLPattern_h
#define mir_key_grid_RegularLLPattern_h

#include "mir/key/grid/GridPattern.h"


namespace mir {
namespace key {
namespace grid {


class RegularLLPattern : public GridPattern {
public:
    // -- Exceptions
    // None

    // -- Constructors

    RegularLLPattern(const std::string& name);
    RegularLLPattern(const RegularLLPattern&) = delete;

    // -- Destructor

    virtual ~RegularLLPattern() override;

    // -- Convertors
    // None

    // -- Operators

    RegularLLPattern& operator=(const RegularLLPattern&) = delete;

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

    virtual void print(std::ostream&) const override;

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
