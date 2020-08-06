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


#ifndef mir_util_SpectralOrderT_h
#define mir_util_SpectralOrderT_h

#include <cmath>

#include "eckit/exception/Exceptions.h"

#include "mir/util/SpectralOrder.h"


namespace mir {
namespace util {


template <int ORDER>
class SpectralOrderT : public SpectralOrder {
public:
    // -- Exceptions
    // None

    // -- Constructors
    SpectralOrderT() { ASSERT(ORDER); }

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    long getTruncationFromGaussianNumber(const long& N) const {
        ASSERT(N);

        long T = long(ceil(4. / double(ORDER + 1) * N) - 1);
        ASSERT(T);

        return T;
    }

    long getGaussianNumberFromTruncation(const long& T) const {
        ASSERT(T);

        long N = long(double(T + 1) * double(ORDER + 1) / 4.);
        ASSERT(N);

        return N;
    }

    void print(std::ostream& out) const { out << "SpectralOrderT<ORDER=" << ORDER << ">[]"; }

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

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
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace util
}  // namespace mir


#endif
