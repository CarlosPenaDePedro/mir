/*
 * (C) Copyright 1996- ECMWF.
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

#include <sstream>
#include "eckit/exception/Exceptions.h"

#include "mir/action/context/Context.h"
#include "mir/data/MIRField.h"
#include "mir/method/Cropping.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"
#include "mir/util/BoundingBox.h"
#include "mir/util/Domain.h"
#include "mir/util/MIRStatistics.h"


namespace mir {
namespace action {
namespace interpolate {


Gridded2GriddedInterpolation::Gridded2GriddedInterpolation(const param::MIRParametrisation& param):
    Action(param) {

    ASSERT(param.get("interpolation", interpolation_));
    method_.reset(method::MethodFactory::build(interpolation_, param));
    ASSERT(method_);

    inputIntersectWithOutput_ = !param.userParametrisation().has("area") &&
                                !param.has("rotation");
}


Gridded2GriddedInterpolation::~Gridded2GriddedInterpolation() = default;


const method::Method& Gridded2GriddedInterpolation::method() const {
    ASSERT(method_);
    return *method_;
}


bool Gridded2GriddedInterpolation::mergeWithNext(const Action& next) {
    if (next.canCrop() && method_->canCrop()) {
        method_->setCropping(next.croppingBoundingBox());
        return true;
    }
    return false;
}


bool Gridded2GriddedInterpolation::canCrop() const {
    return method_->hasCropping();
}


void Gridded2GriddedInterpolation::execute(context::Context& ctx) const {

    eckit::AutoTiming timing(ctx.statistics().timer_, ctx.statistics().grid2gridTiming_);
    data::MIRField& field = ctx.field();

    repres::RepresentationHandle in(field.representation());
    method::Cropping crop;

    auto input = in->domain();
    if (!input.isGlobal()) {

        const auto output(croppingBoundingBox());
        auto out(output);
        if (inputIntersectWithOutput_) {
            input.intersects(out);
        }

        if (!input.contains(out)) {
            std::ostringstream msg;
            msg << "Input does not contain output:"
                << "\n\t" "Input:  " << input
                << "\n\t" "Output: " << output;
            throw eckit::UserError(msg.str());
        }

        crop.boundingBox(out);

    } else if (method_->hasCropping()) {
        crop.boundingBox(method_->getCropping());
    }

    repres::RepresentationHandle out(crop ? outputRepresentation()->croppedRepresentation(crop.boundingBox())
                                          : outputRepresentation());

    method_->execute(ctx, *in, *out);

    field.representation(out);
}


bool Gridded2GriddedInterpolation::sameAs(const Action& other) const {
    auto o = dynamic_cast<const Gridded2GriddedInterpolation*>(&other);
    return o && (interpolation_ == o->interpolation_) && method_->sameAs(*o->method_);
}


void Gridded2GriddedInterpolation::print(std::ostream& out) const {
    out << "interpolation=" << interpolation_
        << ",method=" << *method_;
}


}  // namespace interpolate
}  // namespace action
}  // namespace mir

