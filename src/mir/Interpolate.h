/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Jun 2014

#ifndef mir_Interpolate_H
#define mir_Interpolate_H

#include "eckit/value/Properties.h"

#include "atlas/grid/FieldSet.h"

//------------------------------------------------------------------------------------------------------

namespace mir {

//------------------------------------------------------------------------------------------------------

/// @todo this class will become an eckit::maths::Expression

class Interpolate : private eckit::NonCopyable {

    typedef atlas::grid::FieldSet FieldSet;

public: // methods

    Interpolate( const eckit::Properties& );

    virtual ~Interpolate();

    FieldSet::Ptr eval( const FieldSet::Ptr& in ) const;

private: // members

    eckit::Properties context_;

};

//------------------------------------------------------------------------------------------------------

} // namespace mir

#endif
