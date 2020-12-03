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


#ifndef mir_packing_JPEG2000_h
#define mir_packing_JPEG2000_h

#include "mir/packing/Packer.h"


namespace mir {
namespace packing {


class JPEG2000 : public Packer {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    using Packer::Packer;

    // -- Destructor

    ~JPEG2000() override;

    // -- Convertors
    // None

    // -- Operators
    // None

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
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods

    void print(std::ostream&) const override;
    void fill(grib_info&, const repres::Representation&) const override;
    std::string type(const repres::Representation*) const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace packing
}  // namespace mir


#endif
