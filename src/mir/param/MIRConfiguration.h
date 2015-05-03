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


#ifndef MIRConfiguration_H
#define MIRConfiguration_H

#include <iosfwd>


namespace mir {
namespace param {


class MIRConfiguration {
  public:

// -- Exceptions
    // None

// -- Contructors

    MIRConfiguration();

// -- Destructor

    ~MIRConfiguration(); // Change to virtual if base class

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

    void print(std::ostream&) const; // Change to virtual if base class

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  private:

// No copy allowed

    MIRConfiguration(const MIRConfiguration&);
    MIRConfiguration& operator=(const MIRConfiguration&);

// -- Members

// -- Methods
    // None

// -- Overridden methods


// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    friend std::ostream& operator<<(std::ostream& s,const MIRConfiguration& p)
    	{ p.print(s); return s; }

};


}  // namespace param
}  // namespace mir
#endif

