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


#ifndef mir_key_grid_NamedOctahedral_h
#define mir_key_grid_NamedOctahedral_h

#include "mir/key/grid/NamedGrid.h"


namespace mir {
namespace key {
namespace grid {


class NamedOctahedral : public NamedGrid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    NamedOctahedral(const std::string& name, size_t N);
    NamedOctahedral(const NamedOctahedral&) = delete;

    // -- Destructor

    virtual ~NamedOctahedral() override;

    // -- Convertors
    // None

    // -- Operators

    NamedOctahedral& operator=(const NamedOctahedral&) = delete;

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

    virtual void print(std::ostream&) const override;
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
