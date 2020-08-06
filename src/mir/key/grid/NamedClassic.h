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


#ifndef mir_key_grid_NamedClassic_h
#define mir_key_grid_NamedClassic_h

#include "mir/key/grid/Grid.h"


namespace mir {
namespace key {
namespace grid {


class NamedClassic : public Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    NamedClassic(const std::string& name, size_t N);
    NamedClassic(const NamedClassic&) = delete;

    // -- Destructor

    virtual ~NamedClassic();

    // -- Convertors
    // None

    // -- Operators

    NamedClassic& operator=(const NamedClassic&) = delete;

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
    // None

    // -- Overridden methods

    virtual void print(std::ostream&) const;
    virtual size_t gaussianNumber() const;
    virtual const repres::Representation* representation() const;
    virtual const repres::Representation* representation(const util::Rotation&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    size_t N_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace grid
}  // namespace key
}  // namespace mir


#endif
