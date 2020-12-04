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


#ifndef mir_netcdf_VirtualInputDimension_h
#define mir_netcdf_VirtualInputDimension_h

#include "mir/netcdf/Dimension.h"


namespace mir {
namespace netcdf {


class VirtualInputDimension : public Dimension {
public:
    VirtualInputDimension(Dataset& owner, const std::string& name);
    ~VirtualInputDimension() override;

private:
    // Members

    // -- Methods

    // From Dimension
    void print(std::ostream&) const override;
    void clone(Dataset& owner) const override;
    int id() const override;
    void realDimensions(std::vector<size_t>& dims) const override;
};


}  // namespace netcdf
}  // namespace mir


#endif
