/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "atlas/grid/Grid.h"
#include "eckit/log/Log.h"
#include "Weights.h"

//-----------------------------------------------------------------------------

namespace mir {

InverseSquare::InverseSquare() 
{
    eckit::Log::info() << "Build a InverseSquare" << std::endl;
}

InverseSquare::~InverseSquare() 
{
    eckit::Log::info() << "Destroy a InverseSquare" << std::endl;
}

void InverseSquare::generate(const Point& ref, const std::vector<Point>& closests, std::vector<double>& weights) const
{
    /// @todo take epsilon from some general config
	const double epsilon = 1e-08;
	
    weights.resize(closests.size(), 0.0);
    double sum = 0.0;

    for( size_t j = 0; j < closests.size(); j++)
    {
        const double d2 = Point::distance2(ref, closests[j]);
        weights[j] = 1.0 / ( epsilon + d2 );
        sum += weights[j];
    }

    // now normalise these
    for( size_t j = 0; j < closests.size(); j++)
    {
        ASSERT(sum != 0.0);
        weights[j] /= sum;
    }
}

} // namespace mir
