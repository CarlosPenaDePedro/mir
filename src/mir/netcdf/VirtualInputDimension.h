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


#ifndef mir_netcdf_VirtualInputDimension
#define mir_netcdf_VirtualInputDimension

#include "mir/netcdf/Dimension.h"

namespace mir {
namespace netcdf {

class VirtualInputDimension : public Dimension {
public:
    VirtualInputDimension(Dataset& owner, const std::string& name);
    virtual ~VirtualInputDimension();

private:
    // Members

    // -- Methods

    // From Dimension
    virtual void print(std::ostream&) const;
    virtual void clone(Dataset& owner) const;
    virtual int id() const;
    virtual void realDimensions(std::vector<size_t>& dims) const;
};

}  // namespace netcdf
}  // namespace mir
#endif
