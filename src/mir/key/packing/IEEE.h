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


#pragma once

#include "mir/key/packing/Packing.h"


namespace mir::key::packing {


class IEEE : public Packing {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    IEEE(const std::string&, const param::MIRParametrisation&);

    // -- Destructor
    // None

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

    long precision_;
    bool definePrecision_;

    // -- Methods
    // None

    // -- Overridden methods

    void fill(const repres::Representation*, grib_info&) const override;
    void set(const repres::Representation*, grib_handle*) const override;
    bool printParametrisation(std::ostream&) const override;
    bool empty() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace mir::key::packing
