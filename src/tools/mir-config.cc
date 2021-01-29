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


#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"

#include "mir/input/GribFileInput.h"
#include "mir/param/CombinedParametrisation.h"
#include "mir/param/DefaultParametrisation.h"
#include "mir/param/FieldParametrisation.h"
#include "mir/tools/MIRTool.h"
#include "mir/util/Log.h"


using namespace mir;


struct MIRConfig : tools::MIRTool {
    MIRConfig(int argc, char** argv) : MIRTool(argc, argv) {
        using eckit::option::SimpleOption;

        options_.push_back(new SimpleOption<long>("param-id", "Display configuration with paramId"));
        options_.push_back(new SimpleOption<std::string>("key", "Display configuration with specific key"));
    }

    int minimumPositionalArguments() const override { return 0; }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool
                    << " [--key=key] [[--param-id=value]|[input1.grib [input2.grib [...]]]]"
                       "\n"
                       "Examples: "
                       "\n"
                       "  % "
                    << tool
                    << ""
                       "\n"
                       "  % "
                    << tool
                    << " --param-id=157"
                       "\n"
                       "  % "
                    << tool << " --key=lsm input1.grib input2.grib" << std::endl;
    }

    void execute(const eckit::option::CmdArgs&) override;

    void display(const param::MIRParametrisation& metadata, const std::string& key) const {
        static param::SimpleParametrisation empty;
        static param::DefaultParametrisation defaults;
        const param::CombinedParametrisation combined(empty, metadata, defaults);
        const param::MIRParametrisation& param(combined);

        long paramId = 0;
        ASSERT(metadata.get("paramId", paramId));

        std::string value = "???";
        param.get(key, value);

        Log::info() << "paramId=" << paramId << ": " << key << "=" << value << std::endl;
    }
};


void MIRConfig::execute(const eckit::option::CmdArgs& args) {

    std::string key("interpolation");
    args.get("key", key);


    // Display configuration for a paramId
    long paramId = 0;
    if (args.get("param-id", paramId) || args.count() == 0) {

        class DummyField : public param::FieldParametrisation {
            long paramId_;
            void print(std::ostream&) const override {}
            bool get(const std::string& name, long& value) const override {
                if (name == "paramId") {
                    value = paramId_;
                    return true;
                }
                return FieldParametrisation::get(name, value);
            }

        public:
            DummyField(long paramId) : paramId_(paramId) {}
        };

        display(DummyField(paramId), key);
    }
    else {

        for (size_t i = 0; i < args.count(); i++) {

            // Display configuration for each input file message(s)
            input::GribFileInput grib(args(i));
            while (grib.next()) {
                input::MIRInput& input = grib;
                display(input.parametrisation(), key);
            }
        }
    }
}


int main(int argc, char** argv) {
    MIRConfig tool(argc, argv);
    return tool.start();
}
