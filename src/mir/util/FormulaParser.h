/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date   April 2016


#ifndef mir_util_FormulaParser_h
#define mir_util_FormulaParser_h

#include <vector>

#include "eckit/parser/StreamParser.h"


namespace mir {
namespace param {
class MIRParametrisation;
}
namespace util {
class Formula;
}
}


namespace mir {
namespace util {
class FormulaParser : public eckit::StreamParser {

public: // methods

    FormulaParser(std::istream&);

    Formula* parse(const param::MIRParametrisation&);

private: // methods

    Formula* parseAtom(const param::MIRParametrisation&);
    Formula* parseTest(const param::MIRParametrisation&);
    Formula* parsePower(const param::MIRParametrisation&);
    std::vector<Formula*> parseList(const param::MIRParametrisation&);
    Formula* parseFactor(const param::MIRParametrisation&);
    Formula* parseTerm(const param::MIRParametrisation&);
    std::string parseIdent(const param::MIRParametrisation&);
    Formula* parseString(const param::MIRParametrisation&);
    Formula* parseNumber(const param::MIRParametrisation&);

};


}  // namespace util
}  // namespace mir


#endif
