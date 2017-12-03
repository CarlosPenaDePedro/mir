/*
 * (C) Copyright 1996-2016 ECMWF.
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


#include "mir/action/interpolate/Gridded2GriddedInterpolation.h"

#include "eckit/memory/ScopedPtr.h"
#include "mir/action/context/Context.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/method/Method.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/MIRStatistics.h"


namespace mir {
namespace action {


Gridded2GriddedInterpolation::Gridded2GriddedInterpolation(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {

    ASSERT(parametrisation_.get("interpolation", interpolation_));
    method_.reset(method::MethodFactory::build(interpolation_, parametrisation_));

}



Gridded2GriddedInterpolation::~Gridded2GriddedInterpolation() {
}


void Gridded2GriddedInterpolation::execute(context::Context& ctx) const {

    eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().grid2gridTiming_);
    data::MIRField& field = ctx.field();

    eckit::Log::debug<LibMir>() << "Method is " << *method_ << std::endl;

    repres::RepresentationHandle in(field.representation());
    repres::RepresentationHandle out(outputRepresentation());

    method_->execute(ctx, *in, *out);

    field.representation(out);

    // Make sure results are cropped to the input
    in->crop(parametrisation_, ctx);

}

bool Gridded2GriddedInterpolation::sameAs(const Action& other) const {
    const Gridded2GriddedInterpolation* o = dynamic_cast<const Gridded2GriddedInterpolation*>(&other);
    return o && (interpolation_ == o->interpolation_) && method_->sameAs(*o->method_);
}


void Gridded2GriddedInterpolation::print(std::ostream& out) const {
    out << "interpolation=" << interpolation_;
    out << ",method=" << *method_;
}


}  // namespace action
}  // namespace mir

