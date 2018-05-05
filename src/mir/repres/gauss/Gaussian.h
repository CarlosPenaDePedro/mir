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


#ifndef mir_repres_gauss_Gaussian_h
#define mir_repres_gauss_Gaussian_h

#include "mir/repres/Gridded.h"
#include "mir/repres/gauss/GaussianIterator.h"
#include "mir/util/BoundingBox.h"


namespace mir {
namespace repres {


class Gaussian : public Gridded {
public:

    // -- Exceptions
    // None

    // -- Constructors

    Gaussian(size_t N);
    Gaussian(size_t N, const util::BoundingBox&);
    Gaussian(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~Gaussian(); // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    ///@return global Gaussian latitudes
    static const std::vector<double>& latitudes(size_t N);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members

    size_t N_;

    // -- Methods

    ///@return global Gaussian latitudes
    const std::vector<double>& latitudes() const;

    void correctBoundingBox();
    virtual eckit::Fraction getSmallestIncrement() const = 0;

    Iterator* unrotatedIterator(gauss::GaussianIterator::ni_type) const;
    Iterator* rotatedIterator(gauss::GaussianIterator::ni_type, const util::Rotation&) const;

    // -- Overridden methods

    virtual bool sameAs(const Representation& other) const;
    bool includesNorthPole() const;
    bool includesSouthPole() const;
    bool isPeriodicWestEast() const;
    virtual void validate(const std::vector<double>&) const;

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

    virtual std::string atlasMeshGenerator() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const Gaussian& p)
    //  { p.print(s); return s; }

};


}  // namespace repres
}  // namespace mir


#endif

