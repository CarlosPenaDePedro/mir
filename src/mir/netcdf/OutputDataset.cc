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

#include "mir/netcdf/OutputDataset.h"

#include "mir/netcdf/Attribute.h"
#include "mir/netcdf/Dimension.h"
#include "mir/netcdf/DummyInputVariable.h"
#include "mir/netcdf/DummyMatrix.h"
#include "mir/netcdf/Exceptions.h"
#include "mir/netcdf/MergePlan.h"
#include "mir/netcdf/Variable.h"

#include <iostream>

#include <netcdf.h>

namespace mir {
namespace netcdf {

OutputDataset::OutputDataset(const std::string &path, NCFileCache &cache, int format):
    Dataset(path),
    format_(format ? format : NC_FORMAT_NETCDF4_CLASSIC),
    cache_(cache)
{
}

OutputDataset::~OutputDataset() = default;


void OutputDataset::print(std::ostream &out) const
{
    out << "OutputDataset[path=" << path_ << "]";
}

void OutputDataset::merge( Dataset &other) {

    if (dimensions_.size() == 0 && attributes_.size() == 0 && variables_.size() == 0) {
        // First time, just adopt
        for (auto j = other.dimensions().begin(); j != other.dimensions().end(); ++j)
        {
            (*j).second->clone(*this);
        }

        for (auto j = other.attributes().begin(); j != other.attributes().end(); ++j)
        {
            (*j).second->clone(*this);
        }

        for (auto j = other.variables().begin(); j != other.variables().end(); ++j)
        {
            (*j).second->clone(*this);
        }
        return;
    }

    bool more = true;
    while (more) {
        more = false;
        for (auto j = other.variables().begin(); j != other.variables().end(); ++j)
        {
            bool found = false;
            for (auto k = variables_.begin(); k != variables_.end(); ++k)
            {
                if ((*k).second->sameAs(*(*j).second)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                // Variable not in output file
                std::cout << "MISSING in output " << *((*j).second) << std::endl;
                Variable *v = (*j).second;
                v->clone(*this)->setMatrix(new DummyMatrix(*v));
                more = true;
                break;
            }
        }
    }

    more = true;
    while (more) {
        more = false;
        for (auto k = variables_.begin(); k != variables_.end(); ++k)
        {
            bool found = false;
            for (auto j = other.variables().begin(); j != other.variables().end(); ++j)
            {
                if ((*k).second->sameAs(*(*j).second)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                // Variable not in input file
                std::cout << "MISSING in input " << *((*k).second) << std::endl;
                other.add(new DummyInputVariable(other, *(*k).second));
                more = true;
                break;
            }
        }
    }
    // ===============================
    MergePlan plan(*this);

    mergeAttributes(other);

    for (auto j = other.variables().begin(); j != other.variables().end(); ++j)
    {
        bool found = false;
        for (auto k = variables_.begin(); k != variables_.end(); ++k)
        {
            if ((*k).second->sameAs(*(*j).second)) {
                (*k).second->merge(*(*j).second, plan);
                found = true;
            }
        }
        if (!found) {
            ASSERT(found);
        }
    }

    plan.execute();
}

void OutputDataset::save() const
{
    int flags = 0;
    switch (format_)
    {
    case NC_FORMAT_CLASSIC:
        flags = 0;
        break;

    case NC_FORMAT_64BIT:
        flags = NC_64BIT_OFFSET;
        break;

    case NC_FORMAT_NETCDF4:
        flags = NC_NETCDF4;
        break;

    case NC_FORMAT_NETCDF4_CLASSIC:
        flags = NC_NETCDF4 | NC_CLASSIC_MODEL;
        break;
    }

    int nc;

    NC_CALL(nc_create(path_.c_str(), flags | NC_WRITE, &nc), path_);
    NC_CALL(nc_set_fill(nc, NC_NOFILL, NULL), path_);


    // std::cout << "Save dimensions" << std::endl;
    for (auto j = dimensions_.begin(); j != dimensions_.end(); ++j)
    {
        if ((*j).second->inUse()) {
            // std::cout << "Define " << *((*j).second) << std::endl;
            (*j).second->create(nc);
        }
    }

    // std::cout << "Save attributes" << std::endl;
    for (auto j = attributes_.begin(); j != attributes_.end(); ++j)
    {
        // std::cout << "Define " << *((*j).second) << std::endl;
        (*j).second->create(nc);
    }

    // std::cout << "Save variables" << std::endl;

    for (auto j = variables_.begin(); j != variables_.end(); ++j)
    {
        // std::cout << "Define " << *((*j).second) << std::endl;
        (*j).second->create(nc);
    }

    NC_CALL(nc_enddef(nc), path_);

    for (auto j = variables_.begin(); j != variables_.end(); ++j)
    {
        std::cout << "Save " << *((*j).second) << std::endl;
        (*j).second->save(nc);
    }

    NC_CALL(nc_close(nc), path_);
}

}  // namespace netcdf
}  // namespace mir
