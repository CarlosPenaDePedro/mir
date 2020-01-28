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


#ifndef mir_netcdf_Calendar
#define mir_netcdf_Calendar

#include "eckit/types/DateTime.h"
#include "mir/netcdf/Codec.h"

namespace mir {
namespace netcdf {

class Variable;


class Calendar : public Codec {
public:
    Calendar();
    virtual ~Calendar();

private:
    virtual bool timeAxis() const;
};

}  // namespace netcdf
}  // namespace mir
#endif
