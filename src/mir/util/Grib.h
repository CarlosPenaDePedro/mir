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


#pragma once

#include <eccodes.h>

#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>

#include "mir/util/Exceptions.h"


inline bool grib_call(int e, const char* call, bool missingOK = false) {
    if (static_cast<bool>(e)) {
        if (missingOK && (e == CODES_NOT_FOUND)) {
            return false;
        }

        std::ostringstream os;
        os << call << ": " << codes_get_error_message(e);
        throw mir::exception::SeriousBug(os.str());
    }
    return true;
}


#define GRIB_CALL(a) grib_call(a, #a)
#define GRIB_GET(a) grib_call(a, #a, true)
#define GRIB_ERROR(a, b) grib_call(a, b)


struct grib_info {
    grib_info();

    void extra_set(const char* key, long);
    void extra_set(const char* key, double);
    void extra_set(const char* key, const char*);

    codes_util_grid_spec grid;
    codes_util_packing_spec packing;

private:
    grib_info(const grib_info&)      = delete;
    void operator=(const grib_info&) = delete;

    std::vector<std::string> strings_;
    const size_t extra_settings_size_;
};


class HandleDeleter {
    grib_handle* h_;

public:
    HandleDeleter(grib_handle* h) : h_(h) { ASSERT(h); }
    HandleDeleter(const HandleDeleter&)  = delete;
    void operator=(const HandleDeleter&) = delete;
    ~HandleDeleter() { codes_handle_delete(h_); }
};


class GKeyIteratorDeleter {
    codes_keys_iterator* h_;

public:
    GKeyIteratorDeleter(codes_keys_iterator* h) : h_(h) {}
    GKeyIteratorDeleter(const GKeyIteratorDeleter&) = delete;
    void operator=(const GKeyIteratorDeleter&)      = delete;
    ~GKeyIteratorDeleter() { codes_keys_iterator_delete(h_); }
};


class BKeyIteratorDeleter {
    codes_bufr_keys_iterator* h_;

public:
    BKeyIteratorDeleter(codes_bufr_keys_iterator* h) : h_(h) {}
    BKeyIteratorDeleter(const BKeyIteratorDeleter&) = delete;
    void operator=(const BKeyIteratorDeleter&)      = delete;
    ~BKeyIteratorDeleter() { codes_bufr_keys_iterator_delete(h_); }
};


void grib_reorder(std::vector<double>& values, long scanningMode, size_t Ni, size_t Nj);


void grib_get_unique_missing_value(const std::vector<double>& values, double& missingValue);


namespace eckit {
class Fraction;
}


namespace mir::util::grib {


struct Fraction {
    using value_type = long;

    explicit Fraction(double);
    explicit Fraction(const eckit::Fraction&);
    Fraction(value_type _numerator, value_type _denominator) :
        num(_numerator), den(_denominator == 0 || _numerator == 0 ? 1 : _denominator) {}

    const value_type num;
    const value_type den;
};


Fraction::value_type lcm(Fraction::value_type a, Fraction::value_type b);


template <typename... Longs>
Fraction::value_type lcm(Fraction::value_type a, Fraction::value_type b, Longs... cde) {
    return lcm(a, lcm(b, cde...));
}


long gcd(long a, long b);


template <typename... Longs>
long gcd(long a, long b, Longs... cde) {
    return gcd(a, gcd(b, cde...));
}


struct BasicAngle : Fraction {
    using Fraction::Fraction;
    BasicAngle(Fraction, Fraction, Fraction, Fraction, Fraction, Fraction);
    explicit BasicAngle(const grib_info&);

    void fillGrib(grib_info&) const;
    Fraction::value_type numerator(const Fraction&) const;

    static void list(std::ostream&);
};


}  // namespace mir::util::grib
