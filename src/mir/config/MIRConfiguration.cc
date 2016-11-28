/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Nov 2016


#include "mir/config/MIRConfiguration.h"

#include <iostream>
#include "eckit/exception/Exceptions.h"
#include "eckit/parser/JSONParser.h"
#include "mir/config/LibMir.h"
#include "mir/param/InheritParametrisation.h"
#include "mir/param/SimpleParametrisation.h"


namespace mir {
namespace config {


namespace  {


struct Defaults : param::SimpleParametrisation {
    Defaults() {
        // these options are (can be) overridden by the configuration file

        set("configuration-fill", "");  // very meta

        set("style", "mars");
        set("legendre-loader", "mapped-memory");
        set("executor", "simple");

        set("interpolation", "linear"); // The word 'method' is used in grib
        set("decomposition", "none");
        set("stats", "Scalar");
        set("caching", true);

        set("prune-epsilon", 1e-10);
        set("nclosest", 4L);

        set("lsm-selection", "auto");
        set("lsm-interpolation", "nearest-neighbour");
        set("lsm-weight-adjustment", 0.2);
        set("lsm-value-threshold", 0.5);

        set("autoresol", false);
    }
};


}  // (anonymous namespace)


MIRConfiguration& MIRConfiguration::instance() {
    static MIRConfiguration instance_;
    return instance_;
}


void MIRConfiguration::configure(const eckit::PathName& path) {
    if (configPath_ == path.asString()) {
        return;
    }


    eckit::Log::debug<LibMir>() << "MIRConfiguration: loading configuration from '" << path << "'" << std::endl;
    std::ifstream in(path.asString().c_str());
    if (!in) {
        throw eckit::CantOpenFile(path);
    }

    eckit::JSONParser parser(in);
    const eckit::ValueMap j = parser.parse();


    // create hierarchy, usin filling keys and defaults (not overwriting)
    root_.reset(new param::InheritParametrisation());
    ASSERT(root_);

    root_->fill(j);
    std::string fill_label;
    if (root_->get("configuration-fill", fill_label) && fill_label.length()) {
        root_->fill(root_->pick(fill_label));
        root_->clear("configuration-fill");
    }

    Defaults().copyValuesTo(*root_, false);


    configPath_ = path;
    //    eckit::Log::debug<LibMir>() << "MIRConfiguration: " << *root_ << std::endl;
}


MIRConfiguration::MIRConfiguration() {

    // Always start with defaults
    root_.reset(new param::InheritParametrisation());
    ASSERT(root_);

    Defaults().copyValuesTo(*root_, false);
}


void MIRConfiguration::print(std::ostream& out) const {
    out << "MIRConfiguration["
        <<  "configPath=" << configPath_
        << ",root=" << *root_
        << "]";
}


const param::MIRParametrisation* MIRConfiguration::lookup(const long& paramId, const param::MIRParametrisation& metadata) const {

    // inherit from most-specific paramId/metadata individual and its parents
    param::SimpleParametrisation* param = new param::SimpleParametrisation();
    ASSERT(root_);
    root_->pick(paramId, metadata).inherit(*param);
    return param;
}


const param::MIRParametrisation* MIRConfiguration::defaults() const {

    // inherit from top-level only (where defaults are held)
    param::SimpleParametrisation* param = new param::SimpleParametrisation();
    ASSERT(root_);
    root_->inherit(*param);
    return param;
}


}  // namespace config
}  // namespace mir
