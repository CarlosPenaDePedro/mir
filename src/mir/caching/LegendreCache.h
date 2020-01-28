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


#ifndef mir_caching_LegendreCache_h
#define mir_caching_LegendreCache_h

#include "eckit/container/CacheManager.h"


namespace mir {
namespace caching {


struct LegendreCacheTraits {

    using value_type = int;  // dummy
    using Locker = eckit::CacheManagerFileFlock;

    static const char* name();
    static int version();
    static const char* extension();

    static void save(const eckit::CacheManagerBase&, const value_type&, const eckit::PathName&);
    static void load(const eckit::CacheManagerBase&, value_type&, const eckit::PathName&);

};

class LegendreCache : public eckit::CacheManager<LegendreCacheTraits> {
public:  // methods
    LegendreCache();
};


}  // namespace caching
}  // namespace mir


#endif
