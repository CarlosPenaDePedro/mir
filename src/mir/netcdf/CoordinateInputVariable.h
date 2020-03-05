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


#ifndef mir_netcdf_CoordinateInputVariable_h
#define mir_netcdf_CoordinateInputVariable_h

#include "mir/netcdf/InputVariable.h"


namespace mir {
namespace netcdf {


class CoordinateInputVariable : public InputVariable {
public:
    CoordinateInputVariable(Dataset& owner, const std::string& name, int id, const std::vector<Dimension*>& dimensions);
    virtual ~CoordinateInputVariable();

private:
    Variable* makeOutputVariable(Dataset& owner, const std::string& name,
                                 const std::vector<Dimension*>& dimensions) const;

    virtual Variable* makeCoordinateVariable();
    virtual Variable* makeScalarCoordinateVariable();
    const char* kind() const;

    virtual void print(std::ostream&) const;
};


}  // namespace netcdf
}  // namespace mir


#endif
