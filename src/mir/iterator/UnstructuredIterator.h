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


#ifndef mir_iterator_UnstructuredIterator_h
#define mir_iterator_UnstructuredIterator_h

#include "mir/repres/Iterator.h"

#include "eckit/exception/Exceptions.h"


namespace mir {
namespace iterator {


class UnstructuredIterator : public repres::Iterator {
public:
    // -- Exceptions
    // None

    // -- Constructors
    UnstructuredIterator(const std::vector<double>& latitudes, const std::vector<double>& longitudes) :
        i_(0), size_(latitudes.size()), latitudes_(latitudes), longitudes_(longitudes) {
        ASSERT(latitudes_.size() == longitudes_.size());
    }

    UnstructuredIterator(const UnstructuredIterator&) = delete;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    UnstructuredIterator& operator=(const UnstructuredIterator&) = delete;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    size_t i_;
    const size_t size_;
    const std::vector<double>& latitudes_;
    const std::vector<double>& longitudes_;

    // -- Methods
    // None

    // -- Overridden methods

    // From Iterator

    void print(std::ostream& out) const {
        out << "UnstructuredGridIterator[";
        Iterator::print(out);
        out << "]";
    }

    bool next(Latitude& lat, Longitude& lon) {
        if (i_ < size_) {
            lat = latitudes_[i_];
            lon = longitudes_[i_];
            i_++;
            return true;
        }
        return false;
    }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace iterator
}  // namespace mir


#endif
