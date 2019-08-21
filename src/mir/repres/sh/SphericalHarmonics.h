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


#ifndef mir_repres_sh_SphericalHarmonics_h
#define mir_repres_sh_SphericalHarmonics_h

#include "mir/repres/Representation.h"
#include "mir/util/Domain.h"


namespace mir {
namespace repres {
namespace sh {


class SphericalHarmonics : public Representation {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    SphericalHarmonics(const param::MIRParametrisation&);
    SphericalHarmonics(size_t truncation);

    // -- Destructor

    virtual ~SphericalHarmonics();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    static void truncate(
            size_t truncation_from,
            size_t truncation_to,
            const MIRValuesVector& in,
            MIRValuesVector& out);

    static size_t number_of_complex_coefficients(size_t truncation) {
        return (truncation + 1) * (truncation + 2) / 2;
    }

    static void interlace_spectra(
            MIRValuesVector& interlaced,
            const MIRValuesVector& spectra,
            size_t truncation,
            size_t numberOfComplexCoefficients,
            size_t index,
            size_t indexTotal);

    // -- Overridden methods

    util::Domain domain() const {
        return util::Domain();
    }

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


    // No copy allowed

    SphericalHarmonics(const SphericalHarmonics&);
    SphericalHarmonics& operator=(const SphericalHarmonics&);

    // -- Members

    size_t truncation_;

    // -- Methods
    // None


    // -- Overridden methods

    virtual void fill(grib_info&) const;
    virtual void fill(api::MIRJob &) const;
    virtual void estimate(api::MIREstimation&) const;
    virtual std::string factory() const;

    virtual const Representation* truncate(
            size_t truncation,
            const MIRValuesVector&,
            MIRValuesVector&) const;
    virtual size_t truncation() const;

    virtual void comparison(std::string&) const;

    virtual void validate(const MIRValuesVector&) const;

    virtual void setComplexPacking(grib_info&) const;
    virtual void setSimplePacking(grib_info&) const;
    virtual void makeName(std::ostream&) const;
    virtual bool sameAs(const Representation& other) const;

    bool isPeriodicWestEast() const;
    bool includesNorthPole() const;
    bool includesSouthPole() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const SphericalHarmonics& p)
    //  { p.print(s); return s; }

};


}  // namespace sh
}  // namespace repres
}  // namespace mir


#endif

