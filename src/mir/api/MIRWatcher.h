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


#ifndef mir_api_MIRWatcher_h
#define mir_api_MIRWatcher_h

#include <exception>
#include <iosfwd>


namespace mir {
namespace action {
class Action;
}
}


namespace mir {
namespace api {


class MIRWatcher {
public:
    // -- Exceptions
    // None

    // -- Contructors

    MIRWatcher();
    MIRWatcher(const MIRWatcher&) = delete;

    // -- Destructor

    virtual ~MIRWatcher();

    // -- Convertors
    // None

    // -- Operators
    void operator=(const MIRWatcher&) = delete;

    // -- Methods

    // Return true to rethrow, false to continue
    virtual bool failure(std::exception&, const action::Action&) = 0;

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

    // From MIRParametrisation
    virtual void print(std::ostream&) const = 0;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const MIRWatcher& p) {
        p.print(s);
        return s;
    }
};


}  // namespace api
}  // namespace mir


#endif
