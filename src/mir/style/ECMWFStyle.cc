/*
 * (C) Copyright 1996-2017 ECMWF.
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


#include "mir/style/ECMWFStyle.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "mir/action/plan/ActionPlan.h"
#include "mir/api/MIRJob.h"
#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/param/RuntimeParametrisation.h"
#include "mir/style/Resol.h"
#include "mir/style/Truncation.h"
#include "mir/util/DeprecatedFunctionality.h"


namespace mir {
namespace style {


namespace {

static MIRStyleBuilder<ECMWFStyle> __style("ecmwf");

struct DeprecatedStyle : ECMWFStyle, util::DeprecatedFunctionality {
    DeprecatedStyle(const param::MIRParametrisation& p) : ECMWFStyle(p), util::DeprecatedFunctionality("style 'dissemination' now known as 'ecmwf'") {}
};

static MIRStyleBuilder<DeprecatedStyle> __deprecated_style("dissemination");

static std::string target_gridded_from_parametrisation(const param::MIRParametrisation& parametrisation) {
    static const std::vector< std::pair< const char*, const char* > > keys_targets = {
        { "grid",       "regular-ll" },
        { "reduced",    "reduced-gg" },
        { "regular",    "regular-gg" },
        { "octahedral", "octahedral-gg" },
        { "pl",         "reduced-gg-pl-given" },
        { "gridname",   "namedgrid" },
        { "griddef",    "griddef" },
        { "points",     "points" }
    };

    for (auto& kt : keys_targets) {
        if (parametrisation.has(kt.first)) {
            return kt.second;
        }
    }

    return "";
}

}  // (anonymous namespace)


ECMWFStyle::ECMWFStyle(const param::MIRParametrisation& parametrisation):
    MIRStyle(parametrisation) {
}


ECMWFStyle::~ECMWFStyle() {
}


void ECMWFStyle::prologue(action::ActionPlan& plan) const {

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

    std::string formula;
    if (parametrisation_.userParametrisation().get("formula.prologue", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.prologue.metadata", metadata);
        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }
}


void ECMWFStyle::sh2grid(action::ActionPlan& plan) const {

    std::string formula;
    if (parametrisation_.userParametrisation().get("formula.spectral", formula) ||
            parametrisation_.userParametrisation().get("formula.raw", formula)
       ) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.spectral.metadata", metadata);
        parametrisation_.userParametrisation().get("formula.raw.metadata", metadata);

        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }

    std::string resolution;
    parametrisation_.get("resol", resolution);
    eckit::ScopedPtr<Resol> resol(ResolFactory::build(resolution, parametrisation_));
    eckit::Log::debug<LibMir>() << "ECMWFStyle: resol=" << *resol << std::endl;

    bool rotation = parametrisation_.userParametrisation().has("rotation");

    bool vod2uv = false;
    parametrisation_.userParametrisation().get("vod2uv", vod2uv);

    bool wind = false;
    parametrisation_.userParametrisation().get("wind", wind);

    // completed later
    const std::string transform = "transform." + std::string(vod2uv ? "sh-vod-to-uv-" : "sh-scalar-to-");
    const std::string interpolate = "interpolate.grid2";
    const std::string target = target_gridded_from_parametrisation(parametrisation_.userParametrisation());

    bool rotation_not_supported = (target == "griddef" || target == "points");
    if (rotation && rotation_not_supported) {
        throw eckit::UserError("'rotation' is incompatible with options 'griddef' and 'points'");
    }

    if (resol->resultIsSpectral()) {
        resol->prepare(plan);
    }

    if (!target.empty()) {
        if (rotation_not_supported) {

            // TODO: this is temporary
            plan.add(transform + "octahedral-gg", "octahedral", 64L);
            plan.add(interpolate + target);

        } else if (resol->resultIsSpectral()) {

            plan.add(transform + target);
            if (rotation) {
                plan.add(interpolate + "rotated-" + target);
            }

        } else {

            resol->prepare(plan);
            plan.add(interpolate + (rotation ? "rotated-" : "") + target);

        }
    }

    if (rotation && (wind || vod2uv)) {
        plan.add("filter.adjust-winds-directions");
        selectWindComponents(plan);
    }

    if (isWindComponent()) {
        plan.add("filter.adjust-winds-scale-cos-latitude");
    }

    if (!rotation) {
        selectWindComponents(plan);
    }

    if (parametrisation_.userParametrisation().get("formula.gridded", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.gridded.metadata", metadata);
        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }
}


void ECMWFStyle::sh2sh(action::ActionPlan& plan) const {

    param::RuntimeParametrisation runtime(parametrisation_);
    runtime.set("intgrid", "none");

    std::string resolution;
    parametrisation_.get("resol", resolution);
    eckit::ScopedPtr<Resol> resol(ResolFactory::build(resolution, runtime));
    eckit::Log::debug<LibMir>() << "ECMWFStyle: resol=" << *resol << std::endl;

    // the runtime parametrisation above is needed to satisfy this assertion
    ASSERT(resol->resultIsSpectral());
    resol->prepare(plan);

    std::string formula;
    if (parametrisation_.userParametrisation().get("formula.spectral", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.spectral.metadata", metadata);

        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }

    bool vod2uv = false;
    parametrisation_.userParametrisation().get("vod2uv", vod2uv);

    if (vod2uv) {
        plan.add("transform.sh-vod-to-UV");
    }

    selectWindComponents(plan);
}


void ECMWFStyle::grid2grid(action::ActionPlan& plan) const {

    bool rotation = parametrisation_.userParametrisation().has("rotation");

    bool vod2uv = false;
    parametrisation_.userParametrisation().get("vod2uv", vod2uv);

    bool wind = false;
    parametrisation_.userParametrisation().get("wind", wind);

    std::string formula;
    if (parametrisation_.userParametrisation().get("formula.gridded", formula) ||
            parametrisation_.userParametrisation().get("formula.raw", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.gridded.metadata", metadata);
        parametrisation_.userParametrisation().get("formula.raw.metadata", metadata);

        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }

    bool rotation_not_supported =
            parametrisation_.userParametrisation().has("griddef") ||
            parametrisation_.userParametrisation().has("points");

    if (rotation && rotation_not_supported) {
        throw eckit::UserError("'rotation' is incompatible with options 'griddef' and 'points'");
    }

    // completed later
    const std::string interpolate = "interpolate.grid2";
    const std::string target = target_gridded_from_parametrisation(parametrisation_.userParametrisation());

    if (!target.empty()) {
        plan.add(interpolate + (rotation ? "rotated-":"") + target);

        if (rotation && (wind || vod2uv)) {
            plan.add("filter.adjust-winds-directions");
            selectWindComponents(plan);
        }
    }
}


void ECMWFStyle::epilogue(action::ActionPlan& plan) const {

    bool globalise = false;
    parametrisation_.userParametrisation().get("globalise", globalise);

    if (globalise) {
        plan.add("filter.globalise");
    }

    if (parametrisation_.userParametrisation().has("area")) {
        plan.add("crop.area");
    }

    if (parametrisation_.userParametrisation().has("bitmap")) {
        plan.add("filter.bitmap");
    }

    if (parametrisation_.userParametrisation().has("frame")) {
        plan.add("filter.frame");
    }

    std::string formula;
    if (parametrisation_.userParametrisation().get("formula.epilogue", formula)) {
        std::string metadata;
        // paramId for the results of formulas
        parametrisation_.userParametrisation().get("formula.epilogue.metadata", metadata);
        plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
    }

    std::string metadata;
    if (parametrisation_.userParametrisation().get("metadata", metadata)) {
        plan.add("set.metadata", "metadata", metadata);
    }

    std::string epilogue;
    if (parametrisation_.get("epilogue", epilogue)) {
        plan.add(epilogue);
    }
}


void ECMWFStyle::print(std::ostream& out) const {
    out << "ECMWFStyle[]";
}


bool ECMWFStyle::isWindComponent() const {
    long id = 0;
    parametrisation_.get("paramId", id);

    if (id == 0) {
        return false;
    }

    const eckit::Configuration& config = LibMir::instance().configuration();
    const long id_u = config.getLong("parameter-id-u", 131);
    const long id_v = config.getLong("parameter-id-v", 132);

    return (id == id_u || id == id_v);
}


bool ECMWFStyle::selectWindComponents(action::ActionPlan& plan) const {
    bool u_only = false;
    if (parametrisation_.userParametrisation().get("u-only", u_only) && u_only) {
        plan.add("select.field", "which", long(0));
    }
    bool v_only = false;
    if (parametrisation_.userParametrisation().get("v-only", v_only) && v_only) {
        ASSERT(!u_only);
        plan.add("select.field", "which", long(1));
    }
    return (u_only || v_only);
}


void ECMWFStyle::prepare(action::ActionPlan& plan) const {

    // All the nasty logic goes there
    prologue(plan);

    size_t user_wants_gridded = 0;

    if (parametrisation_.userParametrisation().has("grid")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("reduced")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("regular")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("octahedral")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("pl")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("gridname")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("griddef")) {
        user_wants_gridded++;
    }

    if (parametrisation_.userParametrisation().has("points")) {
        user_wants_gridded++;
    }

    ASSERT(user_wants_gridded <= 1);

    bool field_gridded  = parametrisation_.fieldParametrisation().has("gridded");
    bool field_spectral = parametrisation_.fieldParametrisation().has("spectral");

    ASSERT(field_gridded != field_spectral);


    if (field_spectral) {
        if (user_wants_gridded) {
            sh2grid(plan);
        } else {
            // "user wants spectral"
            sh2sh(plan);
        }
    }


    if (field_gridded) {

        std::string formula;
        if (parametrisation_.userParametrisation().get("formula.gridded", formula)) {
            std::string metadata;
            // paramId for the results of formulas
            parametrisation_.userParametrisation().get("formula.gridded.metadata", metadata);
            parametrisation_.userParametrisation().get("formula.raw.metadata", metadata);

            plan.add("calc.formula", "formula", formula, "formula.metadata", metadata);
        }
        grid2grid(plan);
    }

    epilogue(plan);
}


bool ECMWFStyle::postProcessingRequested(const api::MIRJob& job) const {
    static const char *force[] = {
        "accuracy",
        "bitmap",
        "checkerboard",
        "griddef",
        "points",
        "edition",
        "formula",
        "frame",
        "packing",
        "pattern",
        "vod2uv",
        0
    };

    for (size_t i = 0; force[i]; ++i) {
        if (job.has(force[i])) {
            return true;
        }
    }

    return false;
}


}  // namespace style
}  // namespace mir

