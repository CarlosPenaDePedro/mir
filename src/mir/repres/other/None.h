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


#ifndef mir_repres_other_None_h
#define mir_repres_other_None_h

#include "mir/repres/Representation.h"


namespace mir {
namespace repres {
namespace other {


class None : public Representation {
public:
    // -- Exceptions
    // None

    // -- Constructors

    None();
    None(const param::MIRParametrisation&);
    None(const None&) = delete;

    // -- Destructor

    virtual ~None();

    // -- Convertors
    // None

    // -- Operators

    None& operator=(const None&) = delete;

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

    // from Representation
    void print(std::ostream&) const;

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
    // None

    // -- Friends
    // None
};


}  // namespace other
}  // namespace repres
}  // namespace mir


#endif
