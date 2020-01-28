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


#include "mir/netcdf/Variable.h"

#include "mir/netcdf/Attribute.h"
#include "mir/netcdf/Codec.h"
#include "mir/netcdf/Dataset.h"
#include "mir/netcdf/Dimension.h"
#include "mir/netcdf/Exceptions.h"
#include "mir/netcdf/Matrix.h"
#include "mir/netcdf/MergePlan.h"
#include "mir/netcdf/Type.h"
#include "mir/netcdf/Value.h"

#include <iostream>

namespace mir {
namespace netcdf {

Variable::Variable(Dataset& owner, const std::string& name, const std::vector<Dimension*>& dimensions) :
    dataset_(owner),
    name_(name),
    matrix_(0),
    scalar_(dimensions.size() == 0),
    dimensions_(dimensions) {}

Variable::~Variable() {
    if (matrix_) {
        matrix_->detach();
    }
}


bool Variable::identified() const {
    // Only SimpleInputVariable are not identified
    return true;
}

const Dataset& Variable::dataset() const {
    return dataset_;
}

Dataset& Variable::dataset() {
    return dataset_;
}

void Variable::setMatrix(Matrix* matrix) {
    if (matrix) {
        matrix->attach();
    }
    if (matrix_) {
        matrix_->detach();
    }
    matrix_ = matrix;

    if (matrix_) {
        auto j = attributes_.find("_FillValue");
        auto k = attributes_.find("missing_value");
        if (j != attributes_.end() && k != attributes_.end()) {
            eckit::Log::warning() << "Variable '" << name() << "' has both 'missing_value' and '_FillValue' attributes"
                                  << std::endl;
            // throw MergeError(std::string("Variable ") + name() + " has both 'missing_value' and '_FillValue'
            // attributes");
        }
        if (j == attributes_.end()) {
            j = k;
        }
        if (j != attributes_.end()) {
            matrix_->missingValue((*j).second->value().clone());
        }
    }
}

size_t Variable::numberOfValues() const {
    size_t count = 1;
    for (auto j = dimensions_.begin(); j != dimensions_.end(); ++j) {
        count *= (*j)->count();
    }
    return count;
}

std::vector<std::string> Variable::coordinates() const {
    std::ostringstream os;
    os << "Variable::coordinates() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}


std::vector<std::string> Variable::cellMethods() const {
    std::vector<std::string> result;
    auto j = attributes_.find("bounds");
    if (j != attributes_.end()) {
        std::string s = (*j).second->asString();
        result.push_back(s);
    }
    return result;
}

void Variable::dump(std::ostream& out) const {

    out << std::endl;
    out << "\t// Kind is " << kind() << std::endl;
    out << "\t// Class is " << *this << std::endl;

    if (matrix_->codec()) {
        out << "\t// Codec is " << *matrix_->codec() << std::endl;
    }

    dumpAttributes(out, "\t// ");
    out << std::endl;

    out << "\t";
    if (matrix_) {
        matrix_->type().dump(out);
    }
    else {
        out << "unknown";
    }
    out << " " << name_;

    if (dimensions_.size()) {
        std::string sep = "(";
        for (auto j = dimensions_.begin(); j != dimensions_.end(); ++j) {
            out << sep << (*j)->name();
            sep = ", ";
        }
        out << ")";
    }
    out << " ;" << std::endl;

    for (auto j = attributes_.begin(); j != attributes_.end(); ++j) {
        (*j).second->dump(out);
    }
}

void Variable::dumpAttributes(std::ostream& s, const char* prefix) const {
    // empty
}


const char* Variable::kind() const {
    return "unknown";
}


void Variable::dumpData(std::ostream& out) const {
    out << " " << name_ << " = " << std::endl;
    matrix_->dump(out);
    out << "  ;" << std::endl;
}

bool Variable::sameAs(const Variable& other) const {

    if (dummy()) {
        return sameAsDummy(other);
    }

    if (other.dummy()) {
        return other.sameAsDummy(*this);
    }

    return ncname() == other.ncname();
}

const std::string& Variable::path() const {
    return dataset_.path();
}

const std::string& Variable::name() const {
    return name_;
}

const std::vector<Dimension*>& Variable::dimensions() const {
    return dimensions_;
}

bool Variable::scalar() const {
    return scalar_;
}

Matrix* Variable::matrix() const {
    if (!matrix_) {
        std::cout << "Variable::matrix " << *this << std::endl;
    }
    ASSERT(matrix_ != 0);
    return matrix_;
}

bool Variable::coordinate() const {
    // A coordinate variable as the same name as its unique dimension
    return (dimensions_.size() == 1 && dimensions_[0]->name() == name_);
}


void Variable::addVirtualDimension(size_t where, Dimension* dim) {
    where = std::min(where, dimensions_.size());
    dimensions_.insert(dimensions_.begin() + where, dim);
}

Dimension* Variable::getVirtualDimension() {
    std::ostringstream os;
    os << "Variable::getVirtualDimension() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

void Variable::addCoordinateVariable(const Variable*) {
    std::ostringstream os;
    os << "Variable::addCoordinateVariable() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

Variable* Variable::addMissingCoordinates() {
    // empty
    return this;
}


/*
See http://www.unidata.ucar.edu/software/netcdf/docs/netcdf/Attribute-Conventions.html
*/

static const char* not_supported[] = {"signedness", "valid_range", 0};


void Variable::validate() const {
    size_t i = 0;
    while (not_supported[i]) {
        auto j = attributes_.find(not_supported[i]);
        if (j != attributes_.end()) {
            throw MergeError(std::string("Variable ") + name_ + " has an unsupported attribute: " + not_supported[i]);
        }
        i++;
    }
}

void Variable::create(int nc) const {
    std::ostringstream os;
    os << "Variable::create() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

void Variable::save(int nc) const {
    std::ostringstream os;
    os << "Variable::save() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

Variable* Variable::clone(Dataset& owner) const {
    std::ostringstream os;
    os << "Variable::clone() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

void Variable::merge(const Variable& other, MergePlan& plan) {
    plan.link(*this, other);
    mergeAttributes(other);
}

Variable* Variable::makeDataVariable() {
    std::ostringstream os;
    os << "Variable::makeDataVariable() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

Variable* Variable::makeCoordinateVariable() {
    std::ostringstream os;
    os << "Variable::makeCoordinateVariable() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

Variable* Variable::makeSimpleVariable() {
    std::ostringstream os;
    os << "Variable::makeSimpleVariable() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

Variable* Variable::makeCellMethodVariable() {
    std::ostringstream os;
    os << "Variable::makeCellMethodVariable() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

Variable* Variable::makeScalarCoordinateVariable() {
    std::ostringstream os;
    os << "Variable::makeScalarCoordinateVariable() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

void Variable::initCodecs() {

    auto k = attributes_.find("calendar");
    if (k != attributes_.end()) {
        std::string calendar = (*k).second->asString();
        matrix()->codec(CodecFactory::build(calendar, *this));
    }

    if ((attributes_.find("scale_factor") != attributes_.end()) ||
        (attributes_.find("add_offset") != attributes_.end())) {
        matrix()->codec(CodecFactory::build("packing", *this));
    }
}


bool Variable::dummy() const {
    return false;
}

bool Variable::sameAsDummy(const Variable&) const {
    std::ostringstream os;
    os << "Variable::sameAsDummy() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

const std::string& Variable::ncname() const {
    return name_;
}

bool Variable::sharesDimensions(const Variable& other) const {
    for (auto j = dimensions_.begin(); j != dimensions_.end(); ++j) {
        for (auto k = other.dimensions_.begin(); k != other.dimensions_.end(); ++k) {
            if (*j == *k) {
                return true;
            }
        }
    }
    return false;
}

bool Variable::timeAxis() const {
    return (matrix_->codec() && matrix_->codec()->timeAxis());
}

void Variable::collectField(std::vector<Field*>&) const {
    // Ignore
}

// CF part ------------------------

void Variable::getAttribute(const std::string& name, std::string& s) const {
    auto j = attributes_.find(name);
    if (j == attributes_.end()) {
        s = "<UNDEFINED>";
        return;
    }

    (*j).second->value().get(s);
}

void Variable::getAttribute(const std::string& name, double& s) const {
    auto j = attributes_.find(name);
    if (j == attributes_.end()) {
        s = 0;
    }

    (*j).second->value().get(s);
}

size_t Variable::numberOfDimensions() const {
    return dimensions_.size();
}

void Variable::values(std::vector<double>& v) const {
    v = matrix_->values<double>();
}

void Variable::get2DValues(MIRValuesVector&, size_t) const {
    std::ostringstream os;
    os << "Variable::get2DValues() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

size_t Variable::count2DValues() const {
    std::ostringstream os;
    os << "Variable::count2DValues() not implemented for " << *this;
    throw eckit::SeriousBug(os.str());
}

bool Variable::hasMissing() const {
    return matrix_->missingValue() != 0;
}

double Variable::missingValue() const {
    double v = 9999;
    if (matrix_->missingValue()) {
        matrix_->missingValue()->get(v);
    }
    return v;
}

}  // namespace netcdf
}  // namespace mir
