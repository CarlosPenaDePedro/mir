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
/// @author Tiago Quintino
///
/// @date Oct 2016

#ifndef mir_caching_InterpolatorLoader_H
#define mir_caching_InterpolatorLoader_H

#include <iosfwd>

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"

namespace mir {

namespace param {
class MIRParametrisation;
}

namespace caching {
namespace interpolator {

class InterpolatorLoader : public eckit::NonCopyable {

public:
    InterpolatorLoader(const param::MIRParametrisation& parametrisation, const eckit::PathName&);

    virtual ~InterpolatorLoader();

    virtual const void* address() const = 0;
    virtual size_t size() const = 0;

protected:
    const param::MIRParametrisation& parametrisation_;
    eckit::PathName path_;

    virtual void print(std::ostream&) const = 0;

private:
    friend std::ostream& operator<<(std::ostream& s, const InterpolatorLoader& p) {
        p.print(s);
        return s;
    }
};

class InterpolatorLoaderFactory {
    std::string name_;
    virtual InterpolatorLoader* make(const param::MIRParametrisation&, const eckit::PathName& path) = 0;

protected:
    InterpolatorLoaderFactory(const std::string&);
    virtual ~InterpolatorLoaderFactory();

public:
    static InterpolatorLoader* build(const param::MIRParametrisation&, const eckit::PathName& path);
    static void list(std::ostream&);
};

template <class T>
class InterpolatorLoaderBuilder : public InterpolatorLoaderFactory {
    virtual InterpolatorLoader* make(const param::MIRParametrisation& param, const eckit::PathName& path) {
        return new T(param, path);
    }

public:
    InterpolatorLoaderBuilder(const std::string& name) : InterpolatorLoaderFactory(name) {}
};

}  // namespace interpolator
}  // namespace caching
}  // namespace mir

#endif
