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


#ifndef mir_repres_StretchedGG_h
#define mir_repres_StretchedGG_h

#include "mir/repres/Gridded.h"


namespace mir {
namespace repres {


class StretchedGG : public Gridded {
public:
    // -- Exceptions
    // None

    // -- Constructors

    StretchedGG(const param::MIRParametrisation&);

    // -- Destructor

    ~StretchedGG() override;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

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

    void print(std::ostream&) const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    StretchedGG();

    // No copy allowed

    StretchedGG(const StretchedGG&);
    StretchedGG& operator=(const StretchedGG&);

    // -- Members


    // -- Methods
    // None


    // -- Overridden methods

    void fill(grib_info&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace repres
}  // namespace mir


#endif
