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


#ifndef mir_compat_GribCompatibility_h
#define mir_compat_GribCompatibility_h

#include <iosfwd>
#include <map>
#include <string>


struct grib_info;
struct grib_handle;

namespace metkit {
namespace mars {
class MarsRequest;
}
}  // namespace metkit

namespace mir {
namespace param {
class MIRParametrisation;
}
namespace output {
class MIROutput;
}
}  // namespace mir


namespace mir {
namespace compat {


class GribCompatibility {
public:
    GribCompatibility(const GribCompatibility&) = delete;
    void operator=(const GribCompatibility&) = delete;

    virtual void execute(const output::MIROutput&, const param::MIRParametrisation&, grib_handle*,
                         grib_info&) const = 0;

    virtual void printParametrisation(std::ostream&, const param::MIRParametrisation&) const = 0;

    virtual bool sameParametrisation(const param::MIRParametrisation&, const param::MIRParametrisation&) const = 0;

    virtual void initialise(const metkit::mars::MarsRequest&, std::map<std::string, std::string>& postproc) const = 0;

    static const GribCompatibility& lookup(const std::string&);

    static void list(std::ostream& out);

protected:
    GribCompatibility(const std::string&);
    virtual ~GribCompatibility();

    virtual void print(std::ostream&) const = 0;

private:
    std::string name_;

    friend std::ostream& operator<<(std::ostream& s, const GribCompatibility& p) {
        p.print(s);
        return s;
    }
};


}  // namespace compat
}  // namespace mir


#endif
