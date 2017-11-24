/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @date May 2017


#ifndef mir_style_IntermediateGaussianGrid_h
#define mir_style_IntermediateGaussianGrid_h

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/ScopedPtr.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/style/IntermediateGrid.h"
#include "mir/style/SpectralOrder.h"


namespace mir {
namespace style {


template< typename GRIDTYPE >
class IntermediateGaussianGrid : public IntermediateGrid {
public:

    // -- Exceptions
    // None

    // -- Contructors

    IntermediateGaussianGrid(const param::MIRParametrisation& parametrisation) : IntermediateGrid(parametrisation) {

        order_ = "linear";
        parametrisation_.get("spectral-order", order_);
        eckit::ScopedPtr<SpectralOrder> order(SpectralOrderFactory::build(order_));
        ASSERT(order);

        long T = 0;
        ASSERT(parametrisation_.get("truncation", T));
        ASSERT(T > 0);

        long N = order->getGaussianNumberFromTruncation(T);
        ASSERT(N > 0);

        gridname_ = gaussianGridTypeLetter() + std::to_string(N);
    }

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    std::string order_;
    std::string gridname_;

    // -- Methods

    std::string gaussianGridTypeLetter() const {
        std::ostringstream os;
        os << "IntermediateGaussianGrid::gaussianGridTypeLetter() not implemented for " << *this;
        throw eckit::SeriousBug(os.str());
    }

    // -- Overridden methods

    std::string getGridname() const {
        return gridname_;
    }

    void print(std::ostream& out) const {
        out << "IntermediateGaussianGrid["
            << ",order=" << order_
            << ",gridname=" << gridname_
            << "]";
    }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace style
}  // namespace mir


#endif
