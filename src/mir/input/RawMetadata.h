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


#ifndef mir_input_RawMetadata_h
#define mir_input_RawMetadata_h

#include <string>
#include <vector>

#include "mir/util/BoundingBox.h"


namespace mir {
namespace input {


class RawMetadata {
public:

    // -- Exceptions
    // None

    // -- Contructors

    RawMetadata();

    // -- Destructor

    virtual ~RawMetadata();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual const std::vector<long>& pl() const = 0;

    virtual size_t N() const = 0;
    virtual size_t truncation() const = 0;

    virtual const util::BoundingBox& bbox() const = 0;
    virtual const std::string& gridType() const = 0;

    virtual bool gridded() const = 0;
    virtual bool spectral() const = 0;

    virtual size_t nj() const = 0;
    virtual size_t paramId() const = 0;

    virtual bool hasMissing() const = 0;
    virtual double missingValue() const = 0;

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


}  // namespace input
}  // namespace mir


#endif

