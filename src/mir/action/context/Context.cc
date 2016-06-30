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


#include <iostream>

#include "mir/action/context/Context.h"
#include "mir/data/MIRField.h"
#include "mir/input/MIRInput.h"
#include "mir/util/MIRStatistics.h"
#include "eckit/thread/AutoLock.h"


namespace mir {
namespace context {

namespace {
class MissingInput : public input::MIRInput
{
    virtual const param::MIRParametrisation& parametrisation(size_t which = 0) const {
        NOTIMP;
    }

    virtual bool sameAs(const MIRInput& other) const {
        NOTIMP;
    }

    virtual void print(std::ostream& out) const {
        out << "MissingInput[]";
    }

    virtual data::MIRField field() const {
        NOTIMP;
    }


public:
    MissingInput() {}
    ~MissingInput() {}

};

static MissingInput missing;
static util::MIRStatistics stats;

}

class Content  {

    virtual void print(std::ostream &) const = 0; // Change to virtual if base class

public:
    Content() {}
    virtual ~Content() {}

    virtual data::MIRField& field() {
        std::ostringstream oss;
        oss << "Cannot get field from " << *this;
        throw eckit::SeriousBug(oss.str());
    }

    virtual double scalar() const {
        std::ostringstream oss;
        oss << "Cannot get field from " << *this;
        throw eckit::SeriousBug(oss.str());
    }

    virtual bool isField() const {
        return false;
    }

    virtual bool isScalar() const {
        return false;
    }

    virtual Content* clone() const = 0;

    friend std::ostream &operator<<(std::ostream &s, const Content &p) {
        p.print(s);
        return s;
    }
};

class ScalarContent : public Content {

    double value_;

    virtual void print(std::ostream& out) const {
        out << "ScalarContent[value=" << value_ << "]";
    }

    virtual double scalar() const {
        return value_;
    }

    virtual bool isScalar() const {
        return true;
    }

    virtual Content* clone() const  {
        return new ScalarContent(value_);
    }


public:

    ScalarContent(double value): value_(value) {}

};

class FieldContent : public Content {
    data::MIRField field_;

    data::MIRField& field() {
        return field_;
    }

    virtual void print(std::ostream& out) const {
        out << "FieldContent[field=" << field_ << "]";
    }

    virtual bool isField() const {
        return true;
    }

    virtual Content* clone() const  {
        return new FieldContent(field_);
    }

public:
    FieldContent(const data::MIRField& field):
        field_(field) {  }


};


Context::Context():
    input_(missing),
    statistics_(stats),
    content_(0) {

}


Context::Context(const Context& other):
    input_(other.input_),
    statistics_(other.statistics_),
    content_(other.content_ ? other.content_->clone() : 0) {
}

Context::Context(mir::data::MIRField& field,
                 mir::util::MIRStatistics& statistics):
    input_(missing),
    statistics_(statistics),
    content_(new FieldContent(field)) {

}


Context::Context(input::MIRInput &input,
                 util::MIRStatistics& statistics):
    input_(input),
    statistics_(statistics),
    content_(0)  {

}

Context::~Context() {
}

bool Context::isField() const {

    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    if (!content_) {
        return false;
    }
    return content_->isField();
}

bool Context::isScalar() const {

    eckit::AutoLock<eckit::Mutex> lock(mutex_);


    if (!content_) {
        return false;
    }
    return content_->isScalar();
}

input::MIRInput &Context::input() {
    return input_;
}

util::MIRStatistics& Context::statistics() {
    return statistics_;
}

data::MIRField& Context::field() {

    eckit::AutoLock<eckit::Mutex> lock(mutex_);
    if (!content_) {
        content_.reset(new FieldContent(input_.field()));
    }
    return content_->field();
}

void Context::scalar(double value) {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    content_.reset(new ScalarContent(value));
}

double Context::scalar() const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    ASSERT(content_);
    return content_->scalar();
}

void Context::print(std::ostream& out) const {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);
    out << "Context[content=";
    if (content_) {
        out << *content_;
    }
    else {
        out << "null";
    }
    out << "]";
}


Context& Context::push() {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    stack_.push_back(Context(*this));
    return stack_.back();
}


Context Context::pop() {
    eckit::AutoLock<eckit::Mutex> lock(mutex_);

    ASSERT(stack_.size());
    Context ctx = stack_.back();
    stack_.pop_back();
    return ctx;
}

}  // namespace action
}  // namespace mir

