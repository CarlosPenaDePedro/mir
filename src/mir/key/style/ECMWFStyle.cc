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


#include "mir/key/style/ECMWFStyle.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/utils/StringTools.h"

#include "mir/action/io/Copy.h"
#include "mir/action/io/Save.h"
#include "mir/action/plan/ActionPlan.h"
#include "mir/config/LibMir.h"
#include "mir/key/grid/Grid.h"
#include "mir/key/resol/Resol.h"
#include "mir/output/MIROutput.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/param/SameParametrisation.h"
#include "mir/repres/latlon/LatLon.h"
#include "mir/util/DeprecatedFunctionality.h"


namespace mir {
namespace key {
namespace style {


namespace {


struct DeprecatedStyle : ECMWFStyle, util::DeprecatedFunctionality {
    DeprecatedStyle(const param::MIRParametrisation& p) :
        ECMWFStyle(p), util::DeprecatedFunctionality("style 'dissemination' now known as 'ecmwf'") {}
};


bool option(const param::MIRParametrisation& param, const std::string& key, bool dfault) {
    bool value = dfault;
    param.get(key, value);
    return value;
};


}  // namespace


static MIRStyleBuilder<ECMWFStyle> __style("ecmwf");

static MIRStyleBuilder<DeprecatedStyle> __deprecated_style("dissemination");


static std::string target_gridded_from_parametrisation(const param::MIRParametrisation& user,
                                                       const param::MIRParametrisation& field, bool checkRotation) {
    std::unique_ptr<const param::MIRParametrisation> same(new param::SameParametrisation(user, field, true));

    std::vector<double> rotation;
    const bool rotated = checkRotation && user.has("rotation") && !same->get("rotation", rotation);

    bool filter = option(user, "filter", false);
    bool forced = field.has("spectral") || filter || rotated;
    const std::string prefix(user.has("rotation") ? "rotated-" : "");


    std::string grid;
    if (user.get("grid", grid)) {
        auto& g = grid::Grid::lookup(grid);

        if (g.isRegularLL()) {
            std::vector<double> grid_v;
            forced = forced || !field.has("gridded_regular_ll");
            return forced || !same->get("grid", grid_v) || !repres::latlon::LatLon::samePoints(user, field)
                       ? prefix + "regular-ll"
                       : "";
        }

        if (g.isNamed()) {
            forced = forced || !field.has("gridded_named");
            return forced || !same->get("grid", grid) ? prefix + "namedgrid" : "";
        }

        ASSERT(g.isTyped());
        return prefix + "typedgrid";
    }

    if (user.has("reduced")) {
        long N;
        return forced || !same->get("reduced", N) ? prefix + "reduced-gg" : "";
    }

    if (user.has("regular")) {
        long N;
        return forced || !same->get("regular", N) ? prefix + "regular-gg" : "";
    }

    if (user.has("octahedral")) {
        long N;
        return forced || !same->get("octahedral", N) ? prefix + "octahedral-gg" : "";
    }

    if (user.has("pl")) {
        std::vector<long> pl;
        return forced || !same->get("pl", pl) ? prefix + "reduced-gg-pl-given" : "";
    }

    if (user.has("griddef")) {
        if (user.has("rotation")) {
            throw eckit::UserError("ECMWFStyle: option 'rotation' is incompatible with 'griddef'");
        }
        return "griddef";
    }

    if (user.has("latitudes") || user.has("longitudes")) {
        if (user.has("latitudes") != user.has("longitudes")) {
            throw eckit::UserError("ECMWFStyle: options 'latitudes' and 'longitudes' have to be provided together");
        }
        if (user.has("rotation")) {
            throw eckit::UserError("ECMWFStyle: option 'rotation' is incompatible with 'latitudes' and 'longitudes'");
        }
        return "points";
    }

    if (user.has("area") || user.has("rotation")) {
        if (field.has("gridded_regular_ll") && !repres::latlon::LatLon::samePoints(user, field)) {
            return prefix + "regular-ll";
        }
    }

    eckit::Log::debug<LibMir>() << "ECMWFStyle: did not determine target from parametrisation" << std::endl;
    return "";
}


static void add_formula(action::ActionPlan& plan, const param::MIRParametrisation& param,
                        const std::vector<std::string>&& whens) {
    std::string formula;
    for (auto& when : whens) {
        if (param.get("formula." + when, formula)) {
            std::string metadata;  // paramId for the results of formulas
            param.get("formula." + when + ".metadata", metadata);

            plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
            break;
        }
    }
}


ECMWFStyle::ECMWFStyle(const param::MIRParametrisation& parametrisation) : MIRStyle(parametrisation) {}


ECMWFStyle::~ECMWFStyle() = default;


void ECMWFStyle::prologue(action::ActionPlan& plan) const {
    auto& user = parametrisation_.userParametrisation();

    std::string prologue;
    if (parametrisation_.get("prologue", prologue)) {
        plan.add(prologue);
    }

    if (parametrisation_.has("checkerboard")) {
        plan.add("misc.checkerboard");
    }

    if (parametrisation_.has("pattern")) {
        plan.add("misc.pattern");
    }

    if (user.has("statistics") || user.has("input-statistics")) {
        plan.add("filter.statistics", "which-statistics", "input");
    }

    add_formula(plan, user, {"prologue"});
}


void ECMWFStyle::sh2grid(action::ActionPlan& plan) const {
    auto& user  = parametrisation_.userParametrisation();
    auto& field = parametrisation_.fieldParametrisation();

    add_formula(plan, user, {"spectral", "raw"});

    resol::Resol resol(parametrisation_, false);

    long uv       = 0;
    bool uv_input = field.get("is_wind_component_uv", uv) && (uv != 0);

    bool rotation = user.has("rotation");
    bool vod2uv   = option(user, "vod2uv", false);
    bool uv2uv    = option(user, "uv2uv", false) || uv_input;  // where "MIR knowledge of winds" is hardcoded

    if (vod2uv && uv_input) {
        throw eckit::UserError("ECMWFStyle: option 'vod2uv' is incompatible with input U/V");
    }

    if (resol.resultIsSpectral()) {
        resol.prepare(plan);
    }

    auto target = target_gridded_from_parametrisation(user, field, false);
    if (!target.empty()) {
        if (resol.resultIsSpectral()) {

            plan.add("transform." + std::string(vod2uv ? "sh-vod-to-uv-" : "sh-scalar-to-") + target);
        }
        else {

            resol.prepare(plan);

            // if the intermediate grid is the same as the target grid, the interpolation to the
            // intermediate grid is not followed by an additional interpolation
            std::string grid;
            if (rotation || !user.get("grid", grid) || grid != resol.gridname()) {
                plan.add("interpolate.grid2" + target);
            }
        }

        if (vod2uv || uv2uv) {
            ASSERT(vod2uv != uv2uv);

            if (uv2uv) {
                plan.add("filter.adjust-winds-scale-cos-latitude");
            }

            if (rotation) {
                plan.add("filter.adjust-winds-directions");
            }
        }
    }

    add_formula(plan, user, {"gridded"});
}


void ECMWFStyle::sh2sh(action::ActionPlan& plan) const {
    auto& user = parametrisation_.userParametrisation();

    resol::Resol resol(parametrisation_, true);
    eckit::Log::debug<LibMir>() << "ECMWFStyle: resol=" << resol << std::endl;

    // the runtime parametrisation above is needed to satisfy this assertion
    ASSERT(resol.resultIsSpectral());
    resol.prepare(plan);

    add_formula(plan, user, {"spectral", "raw"});

    bool vod2uv = option(user, "vod2uv", false);
    if (vod2uv) {
        plan.add("transform.sh-vod-to-UV");
    }
}


void ECMWFStyle::grid2grid(action::ActionPlan& plan) const {
    auto& user  = parametrisation_.userParametrisation();
    auto& field = parametrisation_.fieldParametrisation();

    bool rotation = user.has("rotation");
    bool vod2uv   = option(user, "vod2uv", false);
    bool uv2uv    = option(user, "uv2uv", false);

    if (vod2uv) {
        eckit::Log::error() << "ECMWFStyle: option 'vod2uv' does not support gridded input" << std::endl;
        ASSERT(!vod2uv);
    }

    add_formula(plan, user, {"gridded", "raw"});

    auto target = target_gridded_from_parametrisation(user, field, rotation);
    if (!target.empty()) {
        plan.add("interpolate.grid2" + target);

        if (vod2uv || uv2uv) {
            ASSERT(vod2uv != uv2uv);

            if (rotation) {
                plan.add("filter.adjust-winds-directions");
            }
        }
    }
}


void ECMWFStyle::epilogue(action::ActionPlan& plan) const {
    auto& user = parametrisation_.userParametrisation();

    bool vod2uv = option(user, "vod2uv", false);
    bool uv2uv  = option(user, "uv2uv", false);

    if (vod2uv || uv2uv) {
        ASSERT(vod2uv != uv2uv);

        bool u_only = option(user, "u-only", false);
        bool v_only = option(user, "v-only", false);

        if (u_only) {
            ASSERT(!v_only);
            plan.add("select.field", "which", long(0));
        }

        if (v_only) {
            ASSERT(!u_only);
            plan.add("select.field", "which", long(1));
        }
    }

    add_formula(plan, user, {"epilogue"});

    std::string metadata;
    if (user.get("metadata", metadata)) {
        plan.add("set.metadata", "metadata", metadata);
    }

    if (user.has("statistics") || user.has("output-statistics")) {
        plan.add("filter.statistics", "which-statistics", "output");
    }

    if (user.has("add-random")) {
        plan.add("filter.add-random");
    }

    std::string epilogue;
    if (parametrisation_.get("epilogue", epilogue)) {
        plan.add(epilogue);
    }
}


void ECMWFStyle::print(std::ostream& out) const {
    out << "ECMWFStyle[]";
}


void ECMWFStyle::prepare(action::ActionPlan& plan, input::MIRInput& input, output::MIROutput& output) const {
    auto& user = parametrisation_.userParametrisation();

    // All the nasty logic goes there
    prologue(plan);

    size_t user_wants_gridded = 0;

    if (user.has("grid")) {
        user_wants_gridded++;
    }

    if (user.has("gridname")) {
        static struct DeprecatedKeyword : util::DeprecatedFunctionality {
            DeprecatedKeyword() : util::DeprecatedFunctionality("keyword 'gridname' is now 'grid'") {}
        } __deprecated_gridname;
        user_wants_gridded++;
    }

    if (user.has("reduced")) {
        user_wants_gridded++;
    }

    if (user.has("regular")) {
        user_wants_gridded++;
    }

    if (user.has("octahedral")) {
        user_wants_gridded++;
    }

    if (user.has("pl")) {
        user_wants_gridded++;
    }

    if (user.has("griddef")) {
        user_wants_gridded++;
    }

    if (user.has("latitudes") || user.has("longitudes")) {
        user_wants_gridded++;
    }

    if (option(user, "pre-globalise", false)) {
        plan.add("filter.globalise");
    }

    ASSERT(user_wants_gridded <= 1);

    bool field_gridded  = parametrisation_.fieldParametrisation().has("gridded");
    bool field_spectral = parametrisation_.fieldParametrisation().has("spectral");

    ASSERT(field_gridded != field_spectral);


    if (field_spectral) {
        if (user_wants_gridded > 0) {
            sh2grid(plan);
        }
        else {
            // "user wants spectral"
            sh2sh(plan);
        }
    }


    if (field_gridded) {
        grid2grid(plan);
    }


    if (field_gridded || (user_wants_gridded > 0)) {

        std::string nabla;
        if (user.get("nabla", nabla)) {
            for (auto operation : eckit::StringTools::split("/", nabla)) {
                plan.add("filter." + operation);
            }
        }

        if (option(user, "globalise", false)) {
            plan.add("filter.globalise");
        }

        if (user.has("area")) {
            plan.add("crop.area");
        }

        if (user.has("bitmap")) {
            plan.add("filter.bitmap");
        }

        if (user.has("frame")) {
            plan.add("filter.frame");
        }

        if (user.has("unstructured")) {
            plan.add("filter.unstructured");
        }
    }


    epilogue(plan);


    output.prepare(parametrisation_, plan, input, output);


    if (!plan.ended()) {
        if (plan.empty()) {
            plan.add(new action::io::Copy(parametrisation_, output));
        }
        else {
            plan.add(new action::io::Save(parametrisation_, input, output));
        }
    }
    ASSERT(plan.ended());
}


}  // namespace style
}  // namespace key
}  // namespace mir
