/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/Log.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/option/VectorOption.h"

#include "mir/action/context/Context.h"
#include "mir/action/misc/AreaCropper.h"
#include "mir/data/MIRField.h"
#include "mir/input/GribFileInput.h"
#include "mir/param/ConfigurationWrapper.h"
#include "mir/param/RuntimeParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "mir/tools/MIRTool.h"
#include "mir/util/MIRStatistics.h"


class MIRCount : public mir::tools::MIRTool {
private:
    void execute(const eckit::option::CmdArgs&);
    void usage(const std::string& tool) const;
    int minimumPositionalArguments() const {
        return 1;
    }
public:
    MIRCount(int argc, char **argv) : mir::tools::MIRTool(argc, argv) {
        using eckit::option::SimpleOption;
        using eckit::option::VectorOption;
        //options_.push_back(new SimpleOption< bool >("sizes", "compare sizes of coordinates and values vectors, default false"));
        options_.push_back(new VectorOption<double>("area", "Specify the cropping area: north/west/south/east", 4));
    }
};


void MIRCount::usage(const std::string &tool) const {
    eckit::Log::info()
            << "\nCount MIR representation number of values, compared to the GRIB numberOfValues."
               "\n"
               "\nUsage: " << tool << " [--area=N/W/S/E] file.grib [file.grib [...]]"
               "\nExamples:"
               "\n  % " << tool << " 1.grib"
               "\n  % " << tool << " --area=6/0/0/6 1.grib 2.grib"
            << std::endl;
}


void MIRCount::execute(const eckit::option::CmdArgs& args) {
    using mir::repres::Iterator;
    using mir::action::AreaCropper;


    // setup area crop, disabling crop cache
    const mir::param::ConfigurationWrapper args_wrap(args);
    mir::param::RuntimeParametrisation param(args_wrap);
    param.set("caching", false);

    eckit::ScopedPtr< AreaCropper > area_cropper;
    if (args.has("area")) {
        area_cropper.reset(new AreaCropper(param));
        ASSERT(area_cropper);
    }


    // count each file(s) message(s)
    bool printedHeader = false;
    for (size_t i = 0; i < args.count(); ++i) {
        eckit::Log::info() << args(i) << std::endl;

        mir::input::GribFileInput grib(args(i));
        const mir::input::MIRInput& input = grib;

        if (!printedHeader) {
            printedHeader = true;
            eckit::Log::info() << "MIR" "\t" "GRIB" << std::endl;
        }

        size_t count = 0;
        while (grib.next()) {

            mir::data::MIRField field = input.field();
            ASSERT(field.dimensions() == 1);

            mir::repres::RepresentationHandle rep(field.representation());

            if (area_cropper) {
                mir::util::MIRStatistics dummy;
                mir::context::Context ctx(field, dummy);
                area_cropper->execute(ctx);
            }

            size_t count_grib = field.values(0).size();
            size_t count_mir = 0;

            eckit::ScopedPtr< Iterator > it(rep->iterator());
            while (it->next()) {
                ++count_mir;
            }

            eckit::Log::info() << count_mir << "\t" << count_grib << std::endl;
        }

        eckit::Log::info() << ++count << " messages in " << args(i) << std::endl;
    }

}


int main(int argc, char **argv) {
    MIRCount tool(argc, argv);
    return tool.start();
}

