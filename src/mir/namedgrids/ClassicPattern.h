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


#ifndef ClassicPattern_H
#define ClassicPattern_H


#include "mir/namedgrids/NamedGridPattern.h"

namespace mir {
namespace namedgrids {


class ClassicPattern : public NamedGridPattern {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    ClassicPattern(const std::string &name);

    // -- Destructor

    virtual ~ClassicPattern(); // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods


  protected:

    // -- Members

    // -- Methods


    virtual void print(std::ostream &) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    // No copy allowed

    ClassicPattern(const ClassicPattern &);
    ClassicPattern &operator=(const ClassicPattern &);

    // -- Members
    // None

    // -- Methods

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    virtual const NamedGrid* make(const std::string& name) const;

    // -- Friends



};


}  // namespace namedgrids
}  // namespace mir
#endif

