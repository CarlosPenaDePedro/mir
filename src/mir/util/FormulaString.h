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


#ifndef mir_util_FormulaString_h
#define mir_util_FormulaString_h

#include <string>

#include "mir/util/Formula.h"


namespace mir {
namespace util {


class FormulaString : public Formula {
public:
    FormulaString(const param::MIRParametrisation& parametrisation, const std::string& value);
    ~FormulaString() override;

private:
    void print(std::ostream&) const override;
    void execute(context::Context&) const override;
    bool sameAs(const Action&) const override;
    const char* name() const override;

private:  // members
    std::string value_;
};


}  // namespace util
}  // namespace mir


#endif
