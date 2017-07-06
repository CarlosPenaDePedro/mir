/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015

#include "mir/input/NetcdfFileInput.h"

#include "eckit/exception/Exceptions.h"

#include "mir/api/mir_config.h"
#include "mir/data/MIRField.h"
#include "metkit/netcdf/Variable.h"




#ifdef HAVE_NETCDF

// JUST A DEMO !!!!
// Assumes that netcdf is single 2D variable, that dimensions are called "latitude" and "longitude"
// ... and many more assumptions.

#include <netcdf.h>


namespace mir {
namespace input {
namespace {


inline int _nc_call(int e, const char *call, const std::string &path) {
    if (e) {
        std::ostringstream os;
        os << call << ": " << nc_strerror(e) << " (" << path << ")";
        throw eckit::SeriousBug(os.str());
    }
    return e;
}


}  // (anonymous namespace)


NetcdfFileInput::NetcdfFileInput(const eckit::PathName &path):
    path_(path),
    field_(path, cache_) {

    auto& variables = field_.variables();

    for (auto j  = variables.begin(); j != variables.end(); ++j ) {
        std::cout << "NC " << (*j).first << " " << *(*j).second << std::endl;
    }

}


NetcdfFileInput::~NetcdfFileInput() {
}


void NetcdfFileInput::print(std::ostream &out) const {
    out << "NetcdfFileInput[path=" << path_ << "]";
}


const param::MIRParametrisation &NetcdfFileInput::parametrisation(size_t which) const {
    ASSERT(which == 0);
    return *this;
}


void NetcdfFileInput::getVariable(const std::string &variable, std::vector<double> &values) const {
#if 0
    if (nc_ == -1) {
        NC_CALL(nc_open(path_.asString().c_str(), NC_NOWRITE, &nc_), path_);
    }

    int varid = -1;

    char name[NC_MAX_NAME + 1];

    int number_of_dimensions;
    int number_of_variables;
    int number_of_global_attributes;
    int id_of_unlimited_dimension;

    NC_CALL(nc_inq(nc_, &number_of_dimensions, &number_of_variables,
                   &number_of_global_attributes,
                   &id_of_unlimited_dimension), path_);


    for (size_t i = 0; i < number_of_dimensions; i++) {
        size_t count;
        NC_CALL(nc_inq_dim(nc_, i, name, &count), path_);
    }

    for (int i = 0; i < number_of_variables; i++) {
        int type;
        int ndims, nattr;
        int dims[NC_MAX_VAR_DIMS];

        NC_CALL(nc_inq_var(nc_, i, name, &type, &ndims, dims, &nattr), path_);

        if (variable == name) {
            varid = i;

            size_t size = 1;
            for (int d = 0; d < ndims; d++ ) {
                size_t count;
                NC_CALL(nc_inq_dim(nc_, dims[d], name, &count), path_);
                size *= count;
            }

            std::vector<double> v(size);

            NC_CALL(nc_get_var_double( nc_, varid, &v[0]), path_);

            std::swap(v, values);
            return;

        }

        // Type &kind = Type::lookup(type);

        // std::vector<Dimension *> dimensions;
        // for (size_t j = 0; j < ndims; j++) {
        //     dimensions.push_back(findDimension(dims[j]));
        // }

        // Variable *v = new SimpleInputVariable(*this, name, i, dimensions);
        // v->setMatrix(new InputMatrix(kind, i, name, v->numberOfValues(), file));
        // v->getAttributes(nc, i, nattr);
        // add(v);
    }

    std::ostringstream os;
    os <<  "NetcdfFileInput: cannot find variable " << variable;
    throw eckit::SeriousBug(os.str());
#endif
}


data::MIRField NetcdfFileInput::field() const {
#if 0
    std::vector<double> values;
    getVariable(variable_, values);

    bool hasMissing = false; // Should check!
    double missingValue = 9999; // Read from file

    data::MIRField field(*this, hasMissing, missingValue);
    field.update(values, 0);

    return field;
#endif
}


bool NetcdfFileInput::has(const std::string& name) const {
    if (name == "gridded") {
        return true;
    }
    if (name == "spectral") {
        return false;
    }
    return FieldParametrisation::has(name);
}

bool NetcdfFileInput::get(const std::string &name, std::string &value) const {

    if (name == "gridType") {
        value = "regular_ll";
        return true;
    }
    return FieldParametrisation::get(name, value);
}

bool NetcdfFileInput::get(const std::string &name, double &value) const {

    // Extremly inefficient code:

    if (latitude_.size() == 0) {
        getVariable("latitude", latitude_);
        ASSERT(latitude_.size() >= 2);
        ASSERT(latitude_[0] > latitude_[1]);
    }

    if (longitude_.size() == 0) {
        getVariable("longitude", longitude_);
        ASSERT(longitude_.size() >= 2);
        ASSERT(longitude_[0] < longitude_[1]);
    }

    if (name == "north") {
        value = latitude_[0];
        return true;
    }

    if (name == "south") {
        value = latitude_[latitude_.size() - 1];
        return true;
    }

    if (name == "west") {
        value = longitude_[0];
        return true;
    }

    if (name == "east") {
        value = longitude_[longitude_.size() - 1];
        return true;
    }

    if (name == "west_east_increment") {
        value = longitude_[1] - longitude_[0];
        return true;
    }

    if (name == "south_north_increment") {
        value = latitude_[0] - latitude_[1];
        return true;
    }


    return FieldParametrisation::get(name, value);
}

 bool NetcdfFileInput::sameAs(const MIRInput& other) const {
    NOTIMP;
    return false;
}

#undef NC_CALL

static MIRInputBuilder< NetcdfFileInput > netcdf4(0x89484446); // ".HDF"
static MIRInputBuilder< NetcdfFileInput > netcdf3(0x43444601); // "CDF."



}  // namespace input
}  // namespace mir

#endif
