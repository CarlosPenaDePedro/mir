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


#include "mir/netcdf/InputDataset.h"

#include <netcdf.h>

#include "eckit/exception/Exceptions.h"

#include "mir/netcdf/Exceptions.h"
#include "mir/netcdf/InputDimension.h"
#include "mir/netcdf/InputMatrix.h"
#include "mir/netcdf/NCFile.h"
#include "mir/netcdf/NCFileCache.h"
#include "mir/netcdf/SimpleInputVariable.h"
#include "mir/netcdf/Type.h"


namespace mir {
namespace netcdf {


InputDataset::InputDataset(const std::string& path, NCFileCache& cache) : Dataset(path), cache_(cache) {
    std::cout << "Dataset: pass1..." << std::endl;


    char name[NC_MAX_NAME + 1];

    NCFile& file = cache.lookUp(path_);
    int nc       = file.open();

    NC_CALL(nc_inq(nc, &number_of_dimensions_, &number_of_variables_, &number_of_global_attributes_,
                   &id_of_unlimited_dimension_),
            path_);

    NC_CALL(nc_inq_format(nc, &format_), path_);

    for (size_t i = 0; i < number_of_dimensions_; i++) {
        size_t count;
        NC_CALL(nc_inq_dim(nc, i, name, &count), path_);
        add(new InputDimension(*this, name, i, count));
    }

    for (size_t i = 0; i < number_of_variables_; i++) {
        int type;
        int ndims;
        int nattr;
        int dims[NC_MAX_VAR_DIMS];

        NC_CALL(nc_inq_var(nc, i, name, &type, &ndims, dims, &nattr), path_);
        ASSERT(ndims >= 0);

        Type& kind = Type::lookup(type);

        std::vector<Dimension*> dimensions;
        dimensions.reserve(ndims);

        for (int j = 0; j < ndims; j++) {
            dimensions.push_back(findDimension(dims[j]));
        }

        Variable* v = new SimpleInputVariable(*this, name, i, dimensions);
        v->setMatrix(new InputMatrix(kind, i, name, v->numberOfValues(), file));
        v->getAttributes(nc, i, nattr);
        add(v);
    }

    getAttributes(nc, NC_GLOBAL, number_of_global_attributes_);

    file.close();

    std::cout << "Dataset: pass1 done" << std::endl;
    std::cout << "Dataset: pass2..." << std::endl;

    // Finalise...


    //-----------------------------------------------------------------------
    // Mark coordinate and cell methods

    for (auto j = variables_.begin(); j != variables_.end(); ++j) {
        Variable* v = (*j).second;

        if (v->identified()) {
            continue;
        }

        auto coordinates = (*j).second->coordinates();

        if (!coordinates.empty()) {
            Variable* w = v->makeDataVariable();
            if (w != v) {
                delete v;
                (*j).second = w;
            }
            v = w;
        }

        for (auto k = coordinates.begin(); k != coordinates.end(); ++k) {
            // This is a coordinate variable
            auto m = variables_.find(*k);
            if (m == variables_.end()) {
                eckit::Log::error() << "Coordinate '" << *k << "' of " << *v << "has no corresponding variable"
                                    << std::endl;

                continue;
            }
            Variable* t = (*m).second;
            Variable* w = t->makeCoordinateVariable();
            if (w != t) {
                delete t;
                (*m).second = w;
            }
            v->addCoordinateVariable(w);
        }


        auto cellMethods = (*j).second->cellMethods();
        for (auto k = cellMethods.begin(); k != cellMethods.end(); ++k) {
            // This is a coordinate variable
            auto m = variables_.find(*k);
            if (m == variables_.end()) {
                eckit::Log::error() << "Cell method '" << *k << "' of " << *v << "has no corresponding variable"
                                    << std::endl;

                continue;
            }
            Variable* t = (*m).second;
            Variable* w = t->makeCellMethodVariable();
            if (w != t) {
                delete t;
                (*m).second = w;
            }
        }
    }


    // Check the variables which name is the same as the dimension name
    for (auto j = variables_.begin(); j != variables_.end(); ++j) {

        Variable* v = (*j).second;

        if (v->identified()) {
            continue;
        }

        if (v->coordinate()) {

            eckit::Log::warning() << "Assuming that " << *v << " is a coordinate variable" << std::endl;

            // This is a coordinate variable
            Variable* w = v->makeCoordinateVariable();
            if (w != v) {
                delete v;
                (*j).second = w;
            }
        }
    }

    for (auto j = variables_.begin(); j != variables_.end(); ++j) {
        Variable* v = (*j).second;
        Variable* w = v->addMissingCoordinates();
        if (w != v) {
            delete v;
            (*j).second = w;
        }
    }

    std::cout << "Dataset: pass2..." << std::endl;

    // Check is the variable needs a special codec (calendar or scale_factor/add_offset)
    for (auto j = variables_.begin(); j != variables_.end(); ++j) {
        (*j).second->initCodecs();
    }


    std::cout << "Dataset: pass4..." << std::endl;

    for (auto j = variables_.begin(); j != variables_.end(); ++j) {
        Variable* v = (*j).second;
        v->validate();
    }


    std::cout << "Dataset: pass4 done" << std::endl;
    dump(std::cout, false);
}

InputDataset::~InputDataset() = default;


void InputDataset::print(std::ostream& out) const {
    out << "InputDataset[path=" << path_ << "]";
}


std::vector<Field*> InputDataset::fields() const {

    std::vector<Field*> result;

    for (auto j = variables_.begin(); j != variables_.end(); ++j) {
        Variable* v = (*j).second;
        v->collectField(result);
    }

    return result;
}


}  // namespace netcdf
}  // namespace mir
