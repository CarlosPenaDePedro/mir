/*
 * (C) Copyright 1996-2015 ECMWF.
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


#ifndef RotatedGG_H
#define RotatedGG_H

#include "mir/repres/gauss/regular/Regular.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Rotation.h"


namespace mir {
namespace repres {
namespace regular {

class RotatedGG : public Regular {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    RotatedGG(const param::MIRParametrisation &);
    RotatedGG(size_t, const util::BoundingBox &, const util::Rotation&);

    // -- Destructor

    virtual ~RotatedGG(); // Change to virtual if base class

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
    util::Rotation rotation_;

    // -- Methods

    void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:



    // No copy allowed

    RotatedGG(const RotatedGG &);
    RotatedGG &operator=(const RotatedGG &);

    // -- Members

    // -- Methods
    // None


    // -- Overridden methods

    virtual void fill(grib_info &) const;
    virtual void fill(api::MIRJob &) const;
    virtual atlas::grid::Grid *atlasGrid() const;
    virtual Iterator* rotatedIterator() const;

    virtual const Gridded *cropped(const util::BoundingBox &bbox) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const RotatedGG& p)
    //  { p.print(s); return s; }

};

}
}  // namespace repres
}  // namespace mir
#endif

