/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/BigNum.h"

namespace mir {


template<class T>
InMemoryCache<T>::InMemoryCache(const std::string& name, size_t capacity):
    name_(name),
    capacity_(eckit::Resource<size_t>(name + "InMemoryCacheCapacity;$CACHE_" + name, capacity)) {
    ASSERT(capacity_ > 0);
}


template<class T>
InMemoryCache<T>::~InMemoryCache() {
    std::cout << "Deleting InMemoryCache " << name_ << " capacity=" << capacity_ << ", entries: " << cache_.size() << std::endl;
    for (typename std::map<std::string, Entry*>::iterator j = cache_.begin(); j != cache_.end(); ++j) {
        std::cout << "Deleting InMemoryCache " << name_ << " " << *((*j).second->ptr_) << std::endl;
        delete (*j).second;
    }
}

template<class T>
T* InMemoryCache<T>::find(const std::string& key) const {
    typename std::map<std::string, Entry*>::const_iterator j = cache_.find(key);
    if (j != cache_.end()) {
        (*j).second->access_++;
        (*j).second->last_ = ::time(0);
        return (*j).second->ptr_.get();
    }
    return 0;
}


template<class T>
T& InMemoryCache<T>::operator[](const std::string& key) {
    T* ptr = find(key);
    if (ptr) {
        return *ptr;
    }
    return create(key);
}

static inline double score(size_t count, size_t recent, size_t age) {
    // count: number of accesses
    // age: age in seconds since creation
    // recent: age in seconds since last access

    // The higher the score, the most likely to be deleted

    return (double(recent) + double(age)) / double(count);
}


template<class T>
T& InMemoryCache<T>::insert(const std::string& key, T* ptr) {
    ASSERT(ptr);
    std::cout << "Insert in InMemoryCache " << *ptr << std::endl;

    typename std::map<std::string, Entry*>::iterator k = cache_.find(key);
    if (k != cache_.end()) {
        delete (*k).second;
        (*k).second = new Entry(ptr);
        return *ptr;
    }

    while (cache_.size() >= capacity_) {

        std::cout << "Evicting entries from InMemoryCache "
                  << name_
                  << " capacity="
                  << capacity_
                  << std::endl;

        time_t now = ::time(0);
        typename std::map<std::string, Entry*>::iterator best = cache_.begin();
        double m = 0;

        for (typename std::map<std::string, Entry*>::iterator j = cache_.begin(); j != cache_.end(); ++j) {
            double s = score((*j).second->access_, now - (*j).second->last_, now - (*j).second->insert_);
            if (s > m) {
                m = s;
                best = j;
            }
        }

        std::cout << "Evicting entries from InMemoryCache "
                  <<  name_
                  << " best="
                  << m
                  << ", " << eckit::BigNum((*best).second->access_)
                  << " " <<  eckit::Seconds(now - (*best).second->last_)
                  << " " << eckit::Seconds(now - (*best).second->insert_)
                  << std::endl;

        std::cout << "Evicting entries from InMemoryCache "
                  <<  name_
                  << " "
                  << *((*best).second->ptr_)
                  << std::endl;

        delete (*best).second;
        cache_.erase(best);
    }

    cache_[key] = new Entry(ptr);
    return *ptr;

}


template<class T>
T& InMemoryCache<T>::create(const std::string& key) {
    return insert(key, new T());
}

}  // namespace mir

