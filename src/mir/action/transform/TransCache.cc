/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Jan 2017


#include "mir/action/transform/TransCache.h"

#include <iostream>


namespace mir {
namespace action {
namespace transform {


TransCache::TransCache() :
    loader_(nullptr) {
}


TransCache::~TransCache() {
    if (loader_) {
        delete loader_;
    }
}


TransCache& TransCache::operator=(cache_t&& transCache) {

    // on assignment, loader is discarded
    if (loader_) {
        delete loader_;
    }

    loader_ = nullptr;
    transCache_ = transCache;

    return *this;
}


void TransCache::print(std::ostream& s) const {
    s << "TransCache[";
    if (loader_) {
        s << *loader_;
    }
    s << "]";
}


}  // namespace transform
}  // namespace action
}  // namespace mir
