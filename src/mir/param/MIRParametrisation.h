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


#ifndef MIRParametrisation_H
#define MIRParametrisation_H

#include <iosfwd>
#include "eckit/config/Parametrisation.h"
#include "eckit/memory/NonCopyable.h"
#include "mir/util/Types.h"

namespace mir {
namespace param {


class MIRParametrisation : public eckit::Parametrisation, private eckit::NonCopyable {
public:

    // -- Exceptions
    // None

    // -- Contructors

    MIRParametrisation();

    // -- Destructor

    virtual ~MIRParametrisation();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual const MIRParametrisation& userParametrisation() const;
    virtual const MIRParametrisation& fieldParametrisation() const;

    // -- Overridden methods

    // From eckit::Parametrisation
    virtual bool has(const std::string& name) const = 0;

    virtual bool get(const std::string& name, std::string& value) const = 0;
    virtual bool get(const std::string& name, bool& value) const = 0;
    virtual bool get(const std::string& name, int& value) const = 0;
    virtual bool get(const std::string& name, long& value) const = 0;
    virtual bool get(const std::string& name, float& value) const = 0;
    virtual bool get(const std::string& name, double& value) const = 0;

    virtual bool get(const std::string& name, std::vector<int>& value) const = 0;
    virtual bool get(const std::string& name, std::vector<long>& value) const = 0;
    virtual bool get(const std::string& name, std::vector<float>& value) const = 0;
    virtual bool get(const std::string& name, std::vector<double>& value) const = 0;
    virtual bool get(const std::string& name, std::vector<std::string>& value) const = 0;

    bool get(const std::string& name, size_t& value) const;
    bool get(const std::string& name, std::vector<size_t>& value) const;

    bool get(const std::string& name, eckit::Fraction& value) const;
    bool get(const std::string& name, Latitude& value) const;
    bool get(const std::string& name, Longitude& value) const;

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members
    // None

    // -- Methods

    virtual void print(std::ostream&) const = 0; // Change to virtual if base class

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

    friend std::ostream& operator<<(std::ostream& s, const MIRParametrisation& p) {
        p.print(s);
        return s;
    }

};


}  // namespace param
}  // namespace mir


#endif

