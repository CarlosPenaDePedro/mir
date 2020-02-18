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


#ifndef mir_tools_MIRTool_h
#define mir_tools_MIRTool_h

#include <vector>

#include "eckit/runtime/Tool.h"


namespace eckit {
namespace option {
class CmdArgs;
class Option;
}  // namespace option
}  // namespace eckit


namespace mir {
namespace tools {


class MIRTool : public eckit::Tool {
protected:
    using options_t = std::vector<eckit::option::Option*>;

    virtual void run();

    virtual void execute(const eckit::option::CmdArgs& args) = 0;

    virtual int numberOfPositionalArguments() const { return -1; }

    virtual int minimumPositionalArguments() const { return numberOfPositionalArguments(); }

    virtual void init(const eckit::option::CmdArgs&);

    virtual void finish(const eckit::option::CmdArgs&);

    options_t options_;

public:
    virtual void usage(const std::string& tool) const = 0;

    MIRTool(int argc, char** argv);
};


}  // namespace tools
}  // namespace mir


#endif
