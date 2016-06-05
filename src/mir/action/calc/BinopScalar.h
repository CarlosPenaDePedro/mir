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


#ifndef BinopScalar_H
#define BinopScalar_H

#include "mir/action/plan/Action.h"


namespace mir {
namespace action {


template<class T>
class BinopScalar : public Action {
  public:

// -- Exceptions
    // None

// -- Contructors

    BinopScalar(const param::MIRParametrisation&);

// -- Destructor

    virtual ~BinopScalar(); // Change to virtual if base class

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

    BinopScalar(const BinopScalar&);
    BinopScalar& operator=(const BinopScalar&);

// -- Members

    double scalar_;
    size_t param_;

// -- Methods
    // None

// -- Overridden methods

    virtual void execute(data::MIRField & field, util::MIRStatistics& statistics) const;
    virtual bool sameAs(const Action& other) const;


// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    //friend ostream& operator<<(ostream& s,const BinopScalar& p)
    //	{ p.print(s); return s; }

};


}  // namespace action
}  // namespace mir
#endif

