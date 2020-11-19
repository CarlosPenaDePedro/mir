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


#include <iomanip>
#include <memory>
#include <sstream>

#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"

#include "mir/api/Atlas.h"
#include "mir/data/MIRField.h"
#include "mir/input/GribFileInput.h"
#include "mir/param/CombinedParametrisation.h"
#include "mir/param/ConfigurationWrapper.h"
#include "mir/param/DefaultParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/tools/MIRTool.h"
#include "mir/util/MeshGeneratorParameters.h"

#include "mir/caching/InMemoryMeshCache.h"
#include "mir/util/MIRStatistics.h"


class MIRMeshGen : public mir::tools::MIRTool {
private:
    void execute(const eckit::option::CmdArgs&);
    void usage(const std::string& tool) const;
    int minimumPositionalArguments() const { return 0; }

public:
    MIRMeshGen(int argc, char** argv) : mir::tools::MIRTool(argc, argv) {
        options_.push_back(new eckit::option::SimpleOption<std::string>("output", "Gmsh output file"));
        using eckit::option::SimpleOption;

        options_.push_back(new SimpleOption<std::string>("mesh-generator", "Mesh generator"));
        options_.push_back(new SimpleOption<bool>("mesh-cell-centres", "Calculate cell centres"));
        options_.push_back(new SimpleOption<bool>("mesh-cell-longest-diagonal", "Calculate cells longest diagonal"));
        options_.push_back(new SimpleOption<bool>("mesh-node-lumped-mass-matrix", "Calculate node-lumped mass matrix"));
        options_.push_back(
            new SimpleOption<bool>("mesh-node-to-cell-connectivity", "Calculate node-to-cell connectivity"));

        options_.push_back(new SimpleOption<bool>("mesh-generator-three-dimensional", "Generate 3-dimensional mesh"));
        options_.push_back(new SimpleOption<bool>("mesh-generator-triangulate", "Generate triangulated mesh"));
        options_.push_back(
            new SimpleOption<double>("mesh-generator-angle", "Generate with quadrilateral tolerance angle"));
        options_.push_back(
            new SimpleOption<bool>("mesh-generator-force-include-north-pole", "Generate including North pole"));
        options_.push_back(
            new SimpleOption<bool>("mesh-generator-force-include-south-pole", "Generate including South pole"));

        options_.push_back(
            new SimpleOption<std::string>("coordinates", "Write coordinates field, 'lonlat' (default), 'xy' or 'xyz'"));
        options_.push_back(new SimpleOption<bool>("ghost", "Write ghost nodes/elements (default true)"));
        options_.push_back(new SimpleOption<bool>("values", "Write field values (default true)"));
        options_.push_back(
            new SimpleOption<bool>("overwrite", "Overwrite output file '<input file>.msh' (default true))"));
    }
};


void MIRMeshGen::usage(const std::string& tool) const {
    eckit::Log::info() << "\nMesh generation, writing .msh files from input GRIBs."
                          "\n"
                          "\nUsage:"
                          "\n\t"
                       << tool << " [--mesh-generator=delaunay] [...] file.grib [file.grib [...]]" << std::endl;
}


void MIRMeshGen::execute(const eckit::option::CmdArgs& args) {
    using namespace mir::param;


    // Setup options
    static DefaultParametrisation defaults;
    const ConfigurationWrapper commandLine(args);

    mir::util::MIRStatistics statistics;
    auto overwrite = args.getBool("overwrite", true);
    auto values    = args.getBool("values", true);

    atlas::util::Config config;
    config.set("coordinates", args.getString("coordinates", "xyz"));
    config.set("ghost", args.getBool("ghost", true));


    // Loop over each file(s) message(s)
    for (size_t a = 0; a < args.count(); ++a) {
        eckit::PathName path = args(a);

        for (std::unique_ptr<mir::input::MIRInput> input(new mir::input::GribFileInput(path)); input->next();) {
            const CombinedParametrisation param(commandLine, input->parametrisation(), defaults);

            // Creating grid
            auto field = input->field();
            mir::repres::RepresentationHandle rep(field.representation());

            atlas::Grid grid;
            {
                eckit::Timer time("Creating grid");
                grid = rep->atlasGrid();
            }


            // Generating mesh
            mir::util::MeshGeneratorParameters meshGenParams(param);
            rep->fill(meshGenParams);

            atlas::Mesh mesh;
            {
                eckit::Timer time("Generating mesh");
                mesh = mir::caching::InMemoryMeshCache::atlasMesh(statistics, grid, meshGenParams);
            }


            // Write mesh & field values
            auto out = path + ".msh";
            for (size_t counter = 1; !overwrite && out.exists(); ++counter) {
                std::ostringstream name;
                name << path << "." << std::setw(4) << std::setfill('0') << counter << ".msh";
                out = name.str();
            }

            {
                eckit::Timer time("Writing '" + out + "'");
                atlas::output::Gmsh gmsh(out, config);
                gmsh.write(mesh);

                for (size_t which = 0; values && which < field.dimensions(); ++which) {
                    auto& v = field.values(which);
                    atlas::Field f("values", const_cast<double*>(v.data()), atlas::array::make_shape(v.size()));
                    if (field.hasMissing()) {
                        f.metadata().set("missing_value_type", "equals").set("missing_value", field.missingValue());
                    }

                    gmsh.write(f);
                }
            }
        }
    }
}


int main(int argc, char** argv) {
    MIRMeshGen tool(argc, argv);
    return tool.start();
}
