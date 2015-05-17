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

#include "mir/param/MIRArgs.h"

#include <iostream>
#include <map>

#include "eckit/runtime/Context.h"
#include "eckit/parser/Tokenizer.h"

namespace mir {
namespace param {


MIRArgs::MIRArgs(usage_proc usage, int args_count, const ArgOptions* options):
    options_(options) {
    eckit::Context& ctx = eckit::Context::instance();
    const std::string& tool = ctx.runName();
    size_t argc = ctx.argc();
    bool error = false;

    // mir::input::GribFileInput input(ctx.argv(argc - 2));
    // mir::output::GribFileOutput output(ctx.argv(argc - 1))

    std::map<std::string, const ArgOptions*> opts;

    if (options) {
        size_t i = 0;
        while (options_[i].name_) {
            if (options_[i].name_[0]) {
                opts[options_[i].name_] = &options_[i];
            }
            i++;
        }
    }


    eckit::Tokenizer parse("=");
    for (size_t i = 1; i < argc; i++) {

        std::string a = ctx.argv(i);
        if (a.size() > 2 && a[0] == '-' && a[1] == '-') {
            std::vector<std::string> v;
            parse(a.substr(2), v);
            ASSERT(v.size() <= 2) ;
            if (v.size() == 1) {
                set(v[0], true);
            } else {
                set(v[0], v[1]);
            }
            keys_.insert(v[0]);

            if (options_) {
                std::map<std::string, const ArgOptions*>::const_iterator j = opts.find(v[0]);
                if (j == opts.end()) {
                    eckit::Log::info() << "Invalid option --" << v[0] << std::endl;
                    error = true;
                }
            }


        } else {
            args_.push_back(a);
        }
    }

    if (args_count >= 0) {
        if (args_.size() != args_count) {
            error = true;
        }
    }

    if (error) {
        usage(tool);
        if (options_) {
            eckit::Log::info() << std::endl;
            eckit::Log::info() << "Options are:" << std::endl;
            eckit::Log::info() << "===========:" << std::endl ;
            size_t i = 0;
            while (options_[i].name_) {

                if (options_[i].name_[0]) {
                    eckit::Log::info()
                            << "   --" << options_[i].name_
                            << "=" << options_[i].values_;

                    if (options_[i].description_)
                        eckit::Log::info() << " (" << options_[i].description_ << ")" ;
                } else {
                    eckit::Log::info() << std::endl << " " << options_[i].description_ << ":" ;
                }

                eckit::Log::info() << std::endl;
                i++;
            }
            eckit::Log::info() << std::endl;
        }
        ::exit(1);

    }
}


MIRArgs::~MIRArgs() {
}


void MIRArgs::print(std::ostream& out) const {
    out << "MIRArgs[";
    SimpleParametrisation::print(out);
    out << "]";
}

const std::set<std::string>& MIRArgs::keys() const {
    return keys_;
}

const std::vector<std::string>& MIRArgs::args() const {
    return args_;
}

const std::string& MIRArgs::args(size_t i) const {
    ASSERT(i < args_.size());
    return args_[i];
}

bool MIRArgs::has(const std::string& name) const {
    return SimpleParametrisation::has(name);
}

bool MIRArgs::get(const std::string& name, std::string& value) const {
    return SimpleParametrisation::get(name, value);
}

bool MIRArgs::get(const std::string& name, bool& value) const {
    return SimpleParametrisation::get(name, value);
}

bool MIRArgs::get(const std::string& name, long& value) const {
    return SimpleParametrisation::get(name, value);
}

bool MIRArgs::get(const std::string& name, double& value) const {
    return SimpleParametrisation::get(name, value);
}

bool MIRArgs::get(const std::string& name, std::vector<long>& value) const {
    return SimpleParametrisation::get(name, value);
}

bool MIRArgs::get(const std::string& name, std::vector<double>& value) const {
    return SimpleParametrisation::get(name, value);
}


}  // namespace param
}  // namespace mir

