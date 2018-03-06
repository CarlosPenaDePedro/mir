/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/action/transform/LocalShVodTouvGridded.h"

#include <vector>
#include "eckit/exception/Exceptions.h"
#include "eckit/log/ResourceUsage.h"
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace action {
namespace transform {


LocalShVodTouvGridded::LocalShVodTouvGridded(const param::MIRParametrisation& parametrisation):
    LocalShToGridded(parametrisation) {
}


LocalShVodTouvGridded::~LocalShVodTouvGridded() {
}


void LocalShVodTouvGridded::sh2grid(data::MIRField& field, atlas::trans::Trans& trans, const atlas::Grid& grid) const {
    eckit::Timer timer("LocalShVodTouvGridded::sh2grid", eckit::Log::debug<mir::LibMir>());

    size_t number_of_fields = field.dimensions();
    ASSERT(number_of_fields == 2);

    const int number_of_grid_points = int(grid.size());
    ASSERT(number_of_grid_points > 0);

    const eckit::Configuration& config = LibMir::instance().configuration();
    const long id_u = config.getLong("parameter-id-u", 131);
    const long id_v = config.getLong("parameter-id-v", 132);
    ASSERT(id_u > 0);
    ASSERT(id_v > 0);

    // do inverse transform and set gridded values
    std::vector<double> output(number_of_grid_points * 2);
    const std::vector<double>& input_vo = field.values(0);
    const std::vector<double>& input_d  = field.values(1);

    trans.invtrans(1, input_vo.data(), input_d.data(),  output.data(), atlas::option::global() );

    // set u/v field values and paramId (u/v are contiguous, they are saved as separate vectors)
    std::vector<double> output_field;

    auto here = output.cbegin();
    output_field.assign(here, here + number_of_grid_points);
    field.update(output_field, 0);
    field.metadata(0, "paramId", id_u);

    here += number_of_grid_points;
    output_field.assign(here, here + number_of_grid_points);
    field.update(output_field, 1);
    field.metadata(1, "paramId", id_v);
}


}  // namespace transform
}  // namespace action
}  // namespace mir

