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


#ifndef mir_output_GeoPointsOutput_h
#define mir_output_GeoPointsOutput_h

#include <vector>

#include "mir/output/MIROutput.h"


namespace eckit {
class DataHandle;
}


namespace mir {
namespace output {


class GeoPointsOutput : public MIROutput {
public:

    // -- Exceptions
    // None

    // -- Contructors

    GeoPointsOutput();

    // -- Destructor

    virtual ~GeoPointsOutput();

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

    bool once();

    virtual eckit::DataHandle& dataHandle() const = 0;

    // -- Overridden methods

    // From MIROutput
    virtual bool sameParametrisation(const param::MIRParametrisation&,
                                     const param::MIRParametrisation&) const;

    virtual bool printParametrisation(std::ostream&, const param::MIRParametrisation&) const;

    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    bool once_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const GeoPointsOutput& p)
    // { p.print(s); return s; }

};


}  // namespace output
}  // namespace mir


#endif

