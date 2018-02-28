/*
 * (C) Copyright 1996- ECMWF.
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


#ifndef mir_repres_gauss_reduced_ReducedOctahedral_h
#define mir_repres_gauss_reduced_ReducedOctahedral_h

#include "mir/repres/gauss/reduced/Octahedral.h"


namespace mir {
namespace repres {
namespace gauss {
namespace reduced {


class ReducedOctahedral : public Octahedral {
public:

    // -- Exceptions
    // None

    // -- Contructors

    ReducedOctahedral(size_t);

    // -- Destructor

    virtual ~ReducedOctahedral(); // Change to virtual if base class

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

    void print(std::ostream&) const; // Change to virtual if base class

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

    virtual Iterator* iterator() const;
    virtual const Reduced* croppedRepresentation(const util::BoundingBox&, const std::vector<long>&) const;
    virtual void makeName(std::ostream&) const;
    virtual bool sameAs(const Representation&) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const ReducedOctahedral& p)
    //  { p.print(s); return s; }

};


}  // namespace reduced
}  // namespace gauss
}  // namespace repres
}  // namespace mir


#endif

