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


#include "mir/output/GeoPointsFileOutputXYVector.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/HandleBuf.h"
#include "eckit/memory/ScopedPtr.h"
#include "mir/action/context/Context.h"
#include "mir/data/MIRField.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace output {


// See https://software.ecmwf.int/wiki/display/METV/Geopoints


GeoPointsFileOutputXYVector::GeoPointsFileOutputXYVector(
        const std::string& path,
        const param::MIRParametrisation&) :
    GeoPointsFileOutput(path) {
}


size_t GeoPointsFileOutputXYVector::copy(const param::MIRParametrisation&, context::Context&) {
    NOTIMP;
}


size_t GeoPointsFileOutputXYVector::save(const param::MIRParametrisation& param, context::Context& ctx) {
    ASSERT(once());

    data::MIRField& field = ctx.field();

    eckit::DataHandle& handle = dataHandle();
    eckit::Offset position = handle.position();

    std::ostream out(new eckit::HandleBuf(handle));

    ASSERT(field.dimensions() % 2 == 0);
    for (size_t j = 0; j < field.dimensions(); j += 2) {

        const std::vector<double>& values_u = field.values(j);
        const std::vector<double>& values_v = field.values(j + 1);
        ASSERT(values_u.size() == values_v.size());

        // eckit::Log::info() << "GeoPointsFileOutputXYVector::save => " << handle << std::endl;


        out << "#GEO"
               "\n#FORMAT XY_VECTOR"
               "\n# lat  lon  height  date  time  u  v"
               "\n#DATA";

        std::string extra;
        for (auto& key : {"height", "date", "time"}) {
            std::string v("0");
            param.get(key, v);
            extra += ' ' + v;
        }

        std::vector<double>::const_iterator u = values_u.cbegin();
        std::vector<double>::const_iterator v = values_v.cbegin();

        eckit::ScopedPtr<repres::Iterator> it(field.representation()->iterator());
        while (it->next()) {
            const repres::Iterator::point_ll_t& p = it->pointUnrotated();
            ASSERT(u != values_u.cend());
            ASSERT(v != values_v.cend());
            out << "\n" << p.lat.value() << ' ' << p.lon.value() << extra << ' ' << *u << ' ' << *v;
            ++u;
            ++v;
        }

        out << std::endl;
    }

    // eckit::Log::info() << "GeoPointsFileOutputXYVector::save <= " << handle.position() - position << std::endl;

    return handle.position() - position;
}


static MIROutputBuilder<GeoPointsFileOutputXYVector> output("geopoints-xy-vector");


}  // namespace output
}  // namespace mir

