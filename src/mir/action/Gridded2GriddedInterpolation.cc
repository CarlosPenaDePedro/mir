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

#include "mir/action/Gridded2GriddedInterpolation.h"

#include <memory>

#include "atlas/GridSpec.h"

#include "mir/data/MIRField.h"
#include "mir/method/Method.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"



namespace mir {
namespace action {


Gridded2GriddedInterpolation::Gridded2GriddedInterpolation(const param::MIRParametrisation &parametrisation):
    Action(parametrisation) {
}


Gridded2GriddedInterpolation::~Gridded2GriddedInterpolation() {
}


void Gridded2GriddedInterpolation::execute(data::MIRField &field) const {

    std::string name;
    ASSERT(parametrisation_.get("method", name));

    std::auto_ptr< method::Method > method(method::MethodFactory::build(name, parametrisation_));
    eckit::Log::info() << "Method is " << *method << std::endl;

    const repres::Representation *in = field.representation();
    repres::Representation *out = outputRepresentation(field.representation());

    try {
        // TODO: We should not copy those things around

        std::auto_ptr<atlas::Grid> gin(in->atlasGrid()); // We do it here has ATLAS does not respect constness
        std::auto_ptr<atlas::Grid> gout(out->atlasGrid());

        // eckit::Log::info() << "ingrid  = " << *gin  << std::endl;
        // eckit::Log::info() << "outgrid = " << *gout << std::endl;

        method->execute(field, *gin, *gout);

    } catch (...) {
        delete out;
        throw;
    }

    field.representation(out);

}


}  // namespace action
}  // namespace mir

