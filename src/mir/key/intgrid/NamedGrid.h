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


#ifndef mir_key_intgrid_NamedGrid_h
#define mir_key_intgrid_NamedGrid_h

#include "mir/key/intgrid/Intgrid.h"


namespace mir {
namespace key {
namespace intgrid {


class NamedGrid : public Intgrid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    NamedGrid(const std::string& gridname, const param::MIRParametrisation& parametrisation);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    const std::string& gridname() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const std::string gridname_;

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


}  // namespace intgrid
}  // namespace key
}  // namespace mir


#endif
