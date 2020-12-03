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


#ifndef mir_netcdf_UnstructuredGrid_h
#define mir_netcdf_UnstructuredGrid_h

#include "mir/netcdf/GridSpec.h"


namespace mir {
namespace netcdf {


class UnstructuredGrid : public GridSpec {
public:
    UnstructuredGrid(const Variable&, const std::vector<double>& latitudes, const std::vector<double>& longitudes);

    virtual ~UnstructuredGrid() override;

    // -- Methods

    static GridSpec* guess(const Variable& variable, const Variable& latitudes, const Variable& longitudes);


protected:
    // -- Members

    std::vector<double> latitudes_;
    std::vector<double> longitudes_;

    double north_;
    double west_;
    double south_;
    double east_;

private:
    UnstructuredGrid(const UnstructuredGrid&);
    UnstructuredGrid& operator=(const UnstructuredGrid&);

    // - Methods

    virtual void print(std::ostream& s) const override;

    // From GridSpec
    virtual bool has(const std::string& name) const override;
    virtual bool get(const std::string&, long&) const override;
    virtual bool get(const std::string&, std::string&) const override;
    virtual bool get(const std::string& name, double& value) const override;
    virtual bool get(const std::string& name, std::vector<double>& value) const override;
    virtual void reorder(MIRValuesVector& values) const override;
};


}  // namespace netcdf
}  // namespace mir


#endif
