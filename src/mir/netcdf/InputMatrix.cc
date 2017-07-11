/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - ECMWF Jan 2015

#include "mir/netcdf/InputMatrix.h"

#include "mir/netcdf/Codec.h"
#include "mir/netcdf/Exceptions.h"
#include "mir/netcdf/Mapper.h"
#include "mir/netcdf/NCFile.h"
#include "mir/netcdf/Type.h"
#include "mir/netcdf/Value.h"

#include <ostream>

#include <netcdf.h>

namespace mir {
namespace netcdf {

InputMatrix::InputMatrix(Type &type, int varid, const std::string &name, size_t size, NCFile &file):
    Matrix(type, name, size),
    varid_(varid),
    file_(file)
{
}

InputMatrix::~InputMatrix()
{
}

void InputMatrix::print(std::ostream &out) const {
    out << "InputMatrix[name=" << name_ << ",type=" << *type_ << ", size=" << size_ << "]";
}

template<class V, class G>
static void get(V &v, size_t size, int varid, NCFile &file, G get) {
    v.resize(size);
    int nc = file.open();
    NC_CALL(get(nc, varid, &v[0]), file.path());
    file.close();
}

void InputMatrix::read(std::vector<double> &v) const {
    get(v, size_, varid_, file_, &nc_get_var_double);
    if (codec_) {
        codec_->decode(v);
    }
}

void InputMatrix::read(std::vector<float> &v) const {
    get(v, size_, varid_, file_, &nc_get_var_float);
    if (codec_) {
        codec_->decode(v);
    }
}

void InputMatrix::read(std::vector<long> &v) const {
    get(v, size_, varid_, file_, &nc_get_var_long);
    if (codec_) {
        codec_->decode(v);
    }
}

void InputMatrix::read(std::vector<short> &v) const {
    get(v, size_, varid_, file_, &nc_get_var_short);
    if (codec_) {
        codec_->decode(v);
    }
}

void InputMatrix::read(std::vector<unsigned char> &v) const {
    get(v, size_, varid_, file_, &nc_get_var_ubyte);
    if (codec_) {
        codec_->decode(v);
    }
}

void InputMatrix::read(std::vector<long long> &v) const {
    get(v, size_, varid_, file_, &nc_get_var_longlong);
    if (codec_) {
        codec_->decode(v);
    }
}

//========================================================================

template<class T>
void InputMatrix::_fill(Mapper<T> &v) const {
    std::vector<T> values;
    read(values);
    if (missingValue_) {
        T missing;
        missingValue_->get(missing);
        for (size_t i = 0; i < values.size(); i++) {
            if (values[i] != missing)
            {
                v.set(i, values[i]);
            }
        }
    }
    else {
        for (size_t i = 0; i < values.size(); i++) {
            v.set(i, values[i]);
        }
    }
}

void InputMatrix::fill(Mapper<double> &v) const {
    _fill(v);
}

void InputMatrix::fill(Mapper<float> &v) const {
    _fill(v);
}

void InputMatrix::fill(Mapper<long> &v) const {
    _fill(v);
}

void InputMatrix::fill(Mapper<short> &v) const {
    _fill(v);
}

void InputMatrix::fill(Mapper<unsigned char> &v) const {
    _fill(v);
}

void InputMatrix::fill(Mapper<long long> &v) const {
    _fill(v);
}


template<class V, class G>
static void get_slab(V &v,
                     const std::vector<size_t>& start,
                     const std::vector<size_t>& count,
                     int  varid,
                     NCFile &file,
                     G get) {
    size_t size = std::accumulate(count.begin(), count.end(), 1, std::multiplies<size_t>());

    v.resize(size);
    int nc = file.open();
    NC_CALL(get(nc, varid, start.data(), count.data(), v.data()), file.path());
    file.close();
}



void InputMatrix::read(std::vector<double> &values,
                       const std::vector<size_t>& start,
                       const std::vector<size_t>& count) const  {
    get_slab(values, start, count, varid_, file_, &nc_get_vara_double);
}

void InputMatrix::read(std::vector<float> &,
                       const std::vector<size_t>& start,
                       const std::vector<size_t>& count) const  {
    NOTIMP;
}

void InputMatrix::read(std::vector<long> &,
                       const std::vector<size_t>& start,
                       const std::vector<size_t>& count) const  {
    NOTIMP;
}

void InputMatrix::read(std::vector<short> &,
                       const std::vector<size_t>& start,
                       const std::vector<size_t>& count) const  {
    NOTIMP;
}

void InputMatrix::read(std::vector<unsigned char> &,
                       const std::vector<size_t>& start,
                       const std::vector<size_t>& count) const  {
    NOTIMP;
}

void InputMatrix::read(std::vector<long long> &,
                       const std::vector<size_t>& start,
                       const std::vector<size_t>& count) const  {
    NOTIMP;
}


}
}
