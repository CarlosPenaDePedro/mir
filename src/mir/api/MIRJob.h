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


#ifndef MIRJob_H
#define MIRJob_H

#include <string>


#include "mir/param/SimpleParametrisation.h"
#include "eckit/config/Configured.h"

namespace mir {
namespace input {
class MIRInput;
}
namespace output {
class MIROutput;
}


namespace util {
class MIRStatistics;
}

namespace api {


class MIRJob : public param::SimpleParametrisation, public eckit::Configured {
  public:

// -- Exceptions
    // None

// -- Contructors

    MIRJob();

// -- Destructor

    virtual ~MIRJob();

// -- Convertors
    // None

// -- Operators
    // None

// -- Methods

    void execute(input::MIRInput&, output::MIROutput&) const;
    void execute(input::MIRInput&, output::MIROutput&, util::MIRStatistics& statistics) const;

    MIRJob& set(const std::string& name, const char* value);
    MIRJob& set(const std::string& name, const std::string& value);
    MIRJob& set(const std::string& name, bool value);
    MIRJob& set(const std::string& name, long value);
    MIRJob& set(const std::string& name, size_t value);

    MIRJob& set(const std::string& name, double value);
    MIRJob& set(const std::string& name, param::DelayedParametrisation* value);
    MIRJob& set(const std::string& name, double v1, double v2);
    MIRJob& set(const std::string& name, double v1, double v2, double v3, double v4);

    MIRJob& set(const std::string& name, const std::vector<long>&);
    MIRJob& set(const std::string& name, const std::vector<size_t>&);
    MIRJob& set(const std::string& name, const std::vector<double>& value);

    MIRJob& clear(const std::string& name);
    MIRJob& reset();

    MIRJob& representationFrom(input::MIRInput&);


    // For debugging only
    void mirToolCall(std::ostream&) const;

    void json(eckit::JSON&) const;

    // Used by Job

    bool empty() const;
    bool matches(const param::MIRParametrisation&) const;


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



// -- Overridden methods


// -- Class members
    // None

// -- Class methods
    // None

  private:


// -- Members


// -- Methods


// -- Overridden methods

    // From MIRParametrisation

    virtual void print(std::ostream&) const;

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

};


}  // namespace api
}  // namespace mir
#endif

