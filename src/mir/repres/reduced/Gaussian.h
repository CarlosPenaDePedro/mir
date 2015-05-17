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


#ifndef Gaussian_H
#define Gaussian_H

#include "mir/repres/Gridded.h"
#include "mir/util/BoundingBox.h"


namespace mir {
namespace repres {
namespace reduced {

class Gaussian : public Gridded {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    Gaussian(const param::MIRParametrisation &parametrisation);
    Gaussian(size_t N);
    Gaussian(size_t N, const util::BoundingBox &);

    // -- Destructor

    virtual ~Gaussian(); // Change to virtual if base class

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

    size_t N_;
    util::BoundingBox bbox_;

    // -- Methods


    // void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods

    virtual void fill(grib_info &) const;

    // -- Class members
    // None

    // -- Class methods
    // None

  private:



    // No copy allowed

    Gaussian(const Gaussian &);
    Gaussian &operator=(const Gaussian &);

    // -- Members

    mutable std::vector<double> latitudes_;

    // -- Methods

    const std::vector<double>& latitudes() const;
    virtual Gaussian *cropped(const util::BoundingBox &bbox, const std::vector<long> &) const ;


    virtual const std::vector<long> &pls() const = 0;
    virtual Iterator *iterator() const;


    // -- Overridden methods

    Gridded *cropped(const util::BoundingBox &bbox) const ;
    void validate(const std::vector<double> &values) const;


    // Representation *crop(const util::BoundingBox &bbox, const std::vector<double> &in, std::vector<double> &out) const;


    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const Gaussian& p)
    //  { p.print(s); return s; }

};

} // namespace
}  // namespace repres
}  // namespace mir
#endif

