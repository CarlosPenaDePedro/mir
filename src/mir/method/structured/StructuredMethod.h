/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_method_structured_StructuredMethod_h
#define mir_method_structured_StructuredMethod_h

#include "mir/method/MethodWeighted.h"

#include <vector>


namespace mir {
namespace param {
class MIRParametrisation;
}
namespace repres {
class Representation;
}
}  // namespace mir


namespace mir {
namespace method {
namespace structured {


class StructuredMethod : public MethodWeighted {
public:
    StructuredMethod(const param::MIRParametrisation&);
    ~StructuredMethod();

protected:
    // Utility types
    using triplet_vector_t = std::vector<WeightMatrix::Triplet>;

    // Find nearest West-East bounding i indices
    void left_right_lon_indexes(const Longitude& in, const std::vector<PointLatLon>& coords, size_t start, size_t end,
                                size_t& left, size_t& right) const;


    // Normalize weights triplets such that sum(weights) = 1
    void normalise(triplet_vector_t& triplets) const;

    // Get lat/lon point coordinates from representation
    void getRepresentationPoints(const repres::Representation&, std::vector<PointLatLon>&, Latitude& minimum,
                                 Latitude& maximum) const;

    // Get latitudes list from representation
    void getRepresentationLatitudes(const repres::Representation&, std::vector<Latitude>&) const;

    // Find nearest North-South bounding j indices
    void boundNorthSouth(const Latitude&, const std::vector<Latitude>&, size_t& jNorth, size_t& jSouth) const;

    // Find nearest West-East bounding i indices
    void boundWestEast(const Longitude&, const size_t& Ni, const size_t& iStart, size_t& iWest, size_t& iEast) const;

    virtual bool sameAs(const Method& other) const = 0;

    virtual void print(std::ostream&) const override;

private:
    void assemble(util::MIRStatistics&, WeightMatrix&, const repres::Representation& in,
                  const repres::Representation& out) const;
    virtual void assembleStructuredInput(WeightMatrix&, const repres::Representation& in,
                                         const repres::Representation& out) const = 0;
};


}  // namespace structured
}  // namespace method
}  // namespace mir


#endif
