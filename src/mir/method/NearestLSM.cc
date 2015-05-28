/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date May 2015


#include "mir/method/NearestLSM.h"

#include <string>
#include <algorithm>

#include "eckit/log/Log.h"
#include "eckit/utils/MD5.h"

#include "atlas/grids/ReducedGaussianGrid.h"
#include "mir/lsm/LandSeaMasks.h"
#include "mir/param/RuntimeParametrisation.h"


namespace mir {
namespace method {


NearestLSM::NearestLSM(const param::MIRParametrisation &param) :
    Nearest(param) {
}


NearestLSM::~NearestLSM() {
}

lsm::LandSeaMasks NearestLSM::getMasks(const atlas::Grid &in, const atlas::Grid &out) const {
    param::RuntimeParametrisation runtime(parametrisation_);
    runtime.set("lsm", true); // Force use of LSM
    return lsm::LandSeaMasks::lookup(runtime, in, out);
}

const char *NearestLSM::name() const {
    return  "nearest-lsm";
}

size_t NearestLSM::nclosest() const {
    return 16;
}

void NearestLSM::hash( eckit::MD5 &md5) const {
    MethodWeighted::hash(md5);
}


void NearestLSM::assemble(WeightMatrix &W, const atlas::Grid &in, const atlas::Grid &out) const {
    Nearest::assemble(W, in, out);
}

void NearestLSM::applyMasks(WeightMatrix &W, const lsm::LandSeaMasks &) const {
}


void NearestLSM::print(std::ostream &out) const {
    out << "NearestLSM[]";
}


namespace {
static MethodBuilder< NearestLSM > __method("nearest-lsm");
}


}  // namespace method
}  // namespace mir

