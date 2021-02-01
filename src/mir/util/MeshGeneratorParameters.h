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


#ifndef mir_util_MeshGeneratorParameters_h
#define mir_util_MeshGeneratorParameters_h

#include <iosfwd>
#include <string>

#include "mir/util/Types.h"


namespace mir {
namespace param {
class MIRParametrisation;
}  // namespace param
}  // namespace mir


namespace mir {
namespace util {


class MeshGeneratorParameters : public atlas::MeshGenerator::Parameters {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    MeshGeneratorParameters();
    MeshGeneratorParameters(const std::string& label, const param::MIRParametrisation&);
    MeshGeneratorParameters(const MeshGeneratorParameters&) = default;

    // -- Destructor

    virtual ~MeshGeneratorParameters() = default;

    // -- Convertors
    // None

    // -- Operators

    MeshGeneratorParameters& operator=(const MeshGeneratorParameters&) = default;

    // -- Members

    std::string meshGenerator_;
    std::string fileLonLat_;
    std::string fileXY_;
    std::string fileXYZ_;
    bool meshCellCentres_;
    bool meshCellLongestDiagonal_;
    bool meshNodeLumpedMassMatrix_;
    bool meshNodeToCellConnectivity_;

    // -- Methods

    bool sameAs(const MeshGeneratorParameters&) const;
    void hash(eckit::Hash&) const;
    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const MeshGeneratorParameters& p) {
        p.print(s);
        return s;
    }
};


}  // namespace util
}  // namespace mir


#endif
