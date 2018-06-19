/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - ECMWF Jan 2015

#include "mir/netcdf/Endowed.h"

#include "mir/netcdf/Attribute.h"
#include "mir/netcdf/Exceptions.h"
#include "mir/netcdf/InputAttribute.h"
#include "mir/netcdf/Type.h"

#include <netcdf.h>
#include <string.h>

namespace mir {
namespace netcdf {


Endowed::Endowed() {

}

Endowed::~Endowed() {
    for (auto j = attributes_.begin(); j != attributes_.end(); ++j)
    {
        delete (*j).second;
    }
}

void Endowed::add(Attribute *a) {
    auto j = attributes_.find(a->name());
    if (j != attributes_.end()) {
        delete (*j).second;
    }
    attributes_[a->name()] = a;
}

void Endowed::getAttributes(int nc, int i, int nattr)
{
    int j;
    char value[2048];


    for (j = 0; j < nattr; j++)
    {
        char name[NC_MAX_NAME + 1];
        int type;
        size_t len;


        memset(name, 0, sizeof(name));
        memset(value, 0, sizeof(value));

        NC_CALL(nc_inq_attname(nc, i, j, name), path());


        NC_CALL(nc_inq_att(nc, i, name, &type, &len), path());


        Type &kind = Type::lookup(type);
        Value *value = kind.attributeValue(nc, i, name, len, path());

        add(new InputAttribute(*this, name, value));

    }

}

void Endowed::copyAttributes(const Endowed &other) {

    for (auto j = other.attributes().begin(); j != other.attributes().end(); ++j)
    {
        (*j).second->clone(*this);

    }
}

void Endowed::mergeAttributes(const Endowed &other) {

    for (auto j = other.attributes().begin(); j != other.attributes().end(); ++j)
    {
        for (auto k = attributes_.begin(); k != attributes_.end(); ++k)
        {
            if ((*k).second->sameAs(*(*j).second)) {
                (*k).second->merge(*(*j).second);
            }
        }
    }

    for (auto k = attributes_.begin(); k != attributes_.end(); ++k)
    {
        bool found  = false;
        for (auto j = other.attributes().begin(); j != other.attributes().end(); ++j)
        {
            if ((*k).second->sameAs(*(*j).second)) {
                found = true;
            }
        }

        if (!found) {
            (*k).second->invalidate();
        }
    }
}

const std::map<std::string, Attribute *> &Endowed::attributes() const {
    return attributes_;
}


}  // namespace netcdf
}  // namespace mir
