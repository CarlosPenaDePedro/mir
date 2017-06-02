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


#ifndef GribFileLSM_H
#define GribFileLSM_H

#include <iosfwd>

#include "eckit/filesystem/PathName.h"

#include "mir/lsm/Mask.h"

namespace mir {
namespace param {
class MIRParametrisation;
}
}

namespace mir {
namespace lsm {

class GribFileLSM : public Mask {
public:

    // -- Exceptions
    // None

    // -- Contructors

    GribFileLSM(
            const std::string& name,
            const eckit::PathName&,
            const param::MIRParametrisation&,
            const atlas::grid::Grid&,
            const std::string& which );

    // -- Destructor

    ~GribFileLSM(); // Change to virtual if base class

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

    static void hashCacheKey(
            eckit::MD5&,
            const eckit::PathName&,
            const param::MIRParametrisation&,
            const atlas::grid::Grid&,
            const std::string& which );

protected:

    // -- Members
    // None

    // -- Methods

    void hash(eckit::MD5&) const;
    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed
    GribFileLSM(const GribFileLSM&);
    GribFileLSM& operator=(const GribFileLSM&);

    // -- Members

    eckit::PathName path_;
    std::vector<bool> mask_;

    // -- Methods
    // None

    // -- Overridden methods

    const std::vector<bool> &mask() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};



}  // namespace lsm
}  // namespace mir
#endif

