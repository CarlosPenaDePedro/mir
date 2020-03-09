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


#ifndef mir_lsm_TenMinutesMask_h
#define mir_lsm_TenMinutesMask_h

#include <iosfwd>

#include "eckit/filesystem/PathName.h"

#include "mir/lsm/Mask.h"


namespace mir {
namespace param {
class MIRParametrisation;
}
}  // namespace mir


namespace mir {
namespace lsm {

class TenMinutesMask : public Mask {
public:
    // -- Exceptions
    // None

    // -- Constructors

    TenMinutesMask(const std::string& name, const eckit::PathName&, const param::MIRParametrisation&,
                   const repres::Representation&, const std::string& which);

    // -- Destructor

    virtual ~TenMinutesMask();

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

private:
    // -- Members

    std::string name_;
    eckit::PathName path_;
    std::vector<bool> mask_;

    // -- Methods
    // None

    // -- Overridden methods

    virtual bool active() const;
    virtual bool cacheable() const;
    virtual void hash(eckit::MD5&) const;
    virtual const std::vector<bool>& mask() const;
    virtual void print(std::ostream&) const;
    virtual std::string cacheName() const;

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
