/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#include "mir/action/statistics/AngleDecomposition.h"

#include <limits>
#include <sstream>
#include "eckit/exception/Exceptions.h"
#include "mir/action/statistics/detail/AngleStatistics.h"
#include "mir/data/MIRField.h"
#include "mir/method/decompose/DecomposeToCartesian.h"


#if 0
#include "eckit/utils/Translator.h"
namespace eckit {
template<>
struct Translator< std::complex<double>, std::string > {
    std::string operator()(const std::complex<double>& from)  {
        std::ostringstream o;
        o << from;
        return o.str();
    }
};
}  // namespace eckit (temporary)


namespace {
static eckit::Translator< std::complex<double>, std::string > pretty_complex;
}  // (anonymous namespace)
#else
namespace {
std::string pretty_complex(const std::complex<double>& complex) {
    std::ostringstream o;
    o << complex;
    return o.str();
}
}  // (anonymous namespace)
#endif


namespace mir {
namespace action {
namespace statistics {


AngleDecomposition::AngleDecomposition(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation),
    decomposition_("") {
    parametrisation.get("decomposition", decomposition_);
}


void AngleDecomposition::operator+=(const AngleDecomposition& other) {
//    stats_ += other.stats_;
}


bool AngleDecomposition::sameAs(const action::Action& other) const {
    const AngleDecomposition* o = dynamic_cast<const AngleDecomposition*>(&other);
    return o; //(o && options_ == o->options_);
}


void AngleDecomposition::calculate(const data::MIRField& field, Results& results) const {
    results.reset();

    // set statistics calculation based on decomposition
    const method::decompose::DecomposeToCartesian& decomp = method::decompose::DecomposeToCartesianChooser::lookup(decomposition_);
    const double missingValue = field.hasMissing()? field.missingValue() : std::numeric_limits<double>::quiet_NaN();

    detail::AngleStatistics stats(decomp, missingValue);

    // analyse
    for (size_t w = 0; w < field.dimensions(); ++w) {

        const std::vector<double>& values = field.values(w);
        for (size_t i = 0; i < values.size(); ++i) {
            stats(values[i]);
        }

        std::string head;
        if (field.dimensions()>1) {
            std::ostringstream s;
            s << '#' << (w+1) << ' ';
            head = s.str();
        }

        results.set(head + "mean",              stats.mean());
        results.set(head + "standardDeviation", stats.standardDeviation());
        results.set(head + "variance",          stats.variance());
        results.set(head + "skewness",          pretty_complex(stats.skewness()));
        results.set(head + "kurtosis",          pretty_complex(stats.kurtosis()));

        results.set(head + "count",   stats.countNonMissing());
        results.set(head + "missing", stats.countMissing());

    }
}


namespace {
static StatisticsBuilder<AngleDecomposition> __angleDecomposition( "AngleDecomposition" );
}


}  // namespace statistics
}  // namespace action
}  // namespace mir

