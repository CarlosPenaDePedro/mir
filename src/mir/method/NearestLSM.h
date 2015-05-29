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
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date May 2015


#ifndef mir_method_NearestLSM_H
#define mir_method_NearestLSM_H

#include "mir/method/MethodWeighted.h"


namespace atlas {
class Grid;
}


namespace mir {
namespace lsm {
class LandSeaMasks;
}
}


namespace mir {
namespace method {


class NearestLSM: public MethodWeighted {
public:

    NearestLSM(const param::MIRParametrisation &);

    virtual ~NearestLSM();

private:

    // -- Methods
    // None

    // -- Overridden methods

    virtual const char *name() const;

    virtual void assemble(WeightMatrix &W, const atlas::Grid &in, const atlas::Grid &out) const;

    /// Update interpolation weigths matrix to account for field masked values
    virtual void applyMasks(WeightMatrix &W, const lsm::LandSeaMasks &) const;

    virtual lsm::LandSeaMasks getMasks(const atlas::Grid &in, const atlas::Grid &out) const;

    virtual void print(std::ostream &) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace method
}  // namespace mir

#endif

