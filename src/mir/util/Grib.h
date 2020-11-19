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


#ifndef mir_util_Grib_h
#define mir_util_Grib_h

#include <eccodes.h>

#include <memory>
#include <vector>

#include "mir/util/Exceptions.h"


inline bool grib_call(int e, const char* call, bool missingOK = false) {
    if (e) {
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


struct codes_values_post_t {
    virtual void set(codes_handle*) = 0;
};


struct post_t : std::vector<std::unique_ptr<codes_values_post_t>> {
    void addBytes(const char* key, const unsigned char* value, size_t length);
    void set(codes_handle*);
};


struct grib_info {
    grib_info();
    codes_util_grid_spec grid;
    codes_util_packing_spec packing;
    post_t post;

private:
    grib_info(const grib_info&) = delete;
    void operator=(const grib_info&) = delete;
};


class HandleDeleter {
    grib_handle* h_;

public:
    HandleDeleter(grib_handle* h) : h_(h) { ASSERT(h); }
    HandleDeleter(const HandleDeleter&) = delete;
    void operator=(const HandleDeleter&) = delete;
    ~HandleDeleter() { codes_handle_delete(h_); }
};


class GKeyIteratorDeleter {
    codes_keys_iterator* h_;

public:
    GKeyIteratorDeleter(codes_keys_iterator* h) : h_(h) {}
    GKeyIteratorDeleter(const GKeyIteratorDeleter&) = delete;
    void operator=(const GKeyIteratorDeleter&) = delete;
    ~GKeyIteratorDeleter() { codes_keys_iterator_delete(h_); }
};


class BKeyIteratorDeleter {
    codes_bufr_keys_iterator* h_;

public:
    BKeyIteratorDeleter(codes_bufr_keys_iterator* h) : h_(h) {}
    BKeyIteratorDeleter(const BKeyIteratorDeleter&) = delete;
    void operator=(const BKeyIteratorDeleter&) = delete;
    ~BKeyIteratorDeleter() { codes_bufr_keys_iterator_delete(h_); }
};


struct GribReorder {
    enum
    {
        iScansNegatively      = 1 << 7,
        jScansPositively      = 1 << 6,
        jPointsAreConsecutive = 1 << 5,
        alternateRowScanning  = 1 << 4
    };

    static void reorder(std::vector<double>& values, long scanningMode, size_t Ni, size_t Nj);
};


struct GribExtraSetting {
    static void set(grib_info&, const char* key, long);
    static void set(grib_info&, const char* key, double);
    static void set(grib_info&, const char* key, const char*);
};


#endif
