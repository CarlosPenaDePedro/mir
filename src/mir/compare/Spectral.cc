/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Oct 2016


#include "mir/compare/Spectral.h"

#include <algorithm>
#include <cmath>
#include "mir/config/LibMir.h"
#include "mir/data/MIRField.h"
#include "mir/data/MIRValuesVector.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace compare {


Spectral::Spectral(const param::MIRParametrisation& param1, const param::MIRParametrisation& param2) :
    Comparator(param1, param2) {
    absoluteError_ = getSameParameter<double>("absolute-error");
    ASSERT(absoluteError_ > 0);
}


Spectral::~Spectral() = default;


void Spectral::execute(const data::MIRField& field1, const data::MIRField& field2) const {

    // access unpacked SH values to compare with absolute tolerance
    const repres::Representation *repres1 = field1.representation();
    const repres::Representation *repres2 = field2.representation();


    // compare truncation
    const size_t trunc1 = repres1->truncation();
    const size_t trunc2 = repres2->truncation();
    if (trunc1 != trunc2) {
        std::ostringstream oss;
        oss << "Spectral: truncation mismatch: " << trunc1 << " != " << trunc2;
        throw eckit::UserError(oss.str());
    }


    // compare unpacked coefficients (up to TS, with i >= TS they are packed)
    // FIXME: get unpacked coefficients in a compatible way to spectral_complex/spectral_simple packingType
    const size_t Ts = 20; //std::min(repres1->pentagonalResolutionTs(), repres2->pentagonalResolutionTs());
    if (!Ts) {
        std::ostringstream oss;
        oss << "Spectral: cannot compare unpacked coefficients, as TS=0";
        throw eckit::UserError(oss.str());
    }

    ASSERT(field1.dimensions() == field2.dimensions());
    for (size_t w = 0; w < field1.dimensions(); ++w) {

        const MIRValuesVector& values1 = field1.values(w);
        const MIRValuesVector& values2 = field2.values(w);
        ASSERT(values1.size() == values2.size());
        ASSERT(values1.size() >= Ts);

        for (size_t i = 0; i < Ts; ++i) {

            double diff = std::abs(values1[i] - values2[i]);
            if (diff > absoluteError_) {

                std::ostringstream oss;
                oss << "value mismatch at "
                       "point " << (i+1) << " < TS=" << Ts << ": "
                       "V1=" << values1[i] << ", "
                       "V2=" << values2[i] << ", "
                       "|V2-V1|=" << diff;
                throw eckit::UserError(oss.str());

            }

        }


    }
}


void Spectral::print(std::ostream& out) const {
    out << "Spectral["
        << "absoluteError=" << absoluteError_
        << "]";
}


namespace {
ComparatorBuilder<Spectral> __spectral("spectral");
}


}  // namespace compare
}  // namespace mir


