/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Peter Bispham
/// @author Tiago Quintino
/// @date May 2015

#ifndef mir_caching_InMemoryCache_H
#define mir_caching_InMemoryCache_H

#include "mir/caching/InMemoryCacheBase.h"
#include "eckit/config/Resource.h"
#include "eckit/memory/ScopedPtr.h"

namespace mir {

//----------------------------------------------------------------------------------------------------------------------

template<class T>
class InMemoryCache : public InMemoryCacheBase {

public:  // methods

    typedef T* iterator;

    explicit InMemoryCache(const std::string& name, unsigned long long capacity, const char* variable);

    ~InMemoryCache();

    iterator find(const std::string& key) const;
    T& insert(const std::string& key, T*);
    T& operator[](const std::string& key);

    iterator end() const { return 0; }

    void footprint(const std::string& key, size_t size);

    void erase(const std::string& key);

    void startUsing(InMemoryCacheStatistics&);
    void stopUsing();

private:

    void purge();
    T& create(const std::string& key);

    virtual unsigned long long footprint() const;
    virtual unsigned long long capacity() const;
    virtual size_t purge(size_t count);
    virtual const std::string& name() const;

    std::string name_;
    eckit::Resource<unsigned long long> capacity_;

    size_t users_;

    mutable InMemoryCacheStatistics* statistics_;
    mutable eckit::Mutex mutex_;
    mutable std::map<std::string, unsigned long long> keys_;

    struct Entry {
        eckit::ScopedPtr<T> ptr_;
        size_t hits_;
        double last_;
        double insert_;
        size_t footprint_;
        Entry(T* ptr): ptr_(ptr), hits_(1), last_(::time(0)), insert_(::time(0)), footprint_(1) {}
    };

    std::map<std::string, Entry*> cache_;

};

template<class T>
class InMemoryCacheUser {
    InMemoryCache<T>& cache_;
public:
    InMemoryCacheUser(InMemoryCache<T>& cache, InMemoryCacheStatistics& statistics):
        cache_(cache) {
        cache_.startUsing(statistics);
    }
    ~InMemoryCacheUser() {
        cache_.stopUsing();
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace mir

#include "mir/caching/InMemoryCache.cc"

#endif
