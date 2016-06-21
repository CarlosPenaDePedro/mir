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


#include "eckit/runtime/Tool.h"
#include "mir/util/FormulaParser.h"
#include "mir/util/Formula.h"
#include "mir/action/context/Context.h"
#include "mir/param/SimpleParametrisation.h"


class MIRFormula : public eckit::Tool {

    virtual void run();


public:
    MIRFormula(int argc, char **argv) :
        eckit::Tool(argc, argv) {
    }

};


void MIRFormula::run() {
    // std::istringstream in("sqrt(-(-2 + 3 - 4   - 5*10/2))");
    std::istringstream in("2 ^ 10");
    mir::util::FormulaParser p(in);

    mir::param::SimpleParametrisation param;

    mir::util::Formula * f = p.parse(param);
    std::cout << (*f) << std::endl;

    mir::context::Context ctx;
    f->execute(ctx);

    std::cout << ctx << std::endl;

}


int main( int argc, char **argv ) {
    MIRFormula tool(argc, argv);
    return tool.start();
}


