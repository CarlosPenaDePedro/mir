/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_util_RotatedGrid_h
#define mir_util_RotatedGrid_h

#include <cstddef>
#include <vector>
#include "eckit/memory/Builder.h"
#include "eckit/memory/ScopedPtr.h"
#include "atlas/grid/Domain.h"
#include "atlas/grid/Grid.h"


namespace mir {
namespace util {


/**
 * @brief RotatedGrid is a grid where the poles are shifted
 *
 * === WMO specification ===
 * (6) Three parameters define a general latitude/longitude coordinate system, formed by a general rotation
 * of the sphere. One
 *     choice for these parameters is:
 *     (a) The geographic latitude in degrees of the southern pole of the coordinate system, θp for example;
 *
 *     (b) The geographic longitude in degrees of the southern pole of the coordinate system, λp for example;
 *
 *     (c) The angle of rotation in degrees about the new polar axis (measured clockwise when looking from
 *         the southern to the northern pole) of the coordinate system, assuming the new axis to have been
 *         obtained by first rotating the sphere through λp degrees about the geographic polar axis, and then
 *         rotating through (90 + θp) degrees so that the southern pole moved along the (previously rotated)
 *         Greenwich meridian.
 * === end WMO specification ===
 *
 * gribs use the following convention: (from Shahram)
 *
 * Horizontally:  Points scan in the +i (+x) direction
 * Vertically:    Points scan in the -j (-y) direction
 *
 * The way I verified this was to look at our SAMPLE files (which IFS uses). I also verified that IFS does
 * not modify the scanning modes so whatever the samples say, is the convention
 *
 * @todo Do we check the area? Can we assume area is multiple of the grids?
 */
class RotatedGrid : public atlas::grid::Grid {

public: // methods

    RotatedGrid(Grid *grid, double south_pole_latitude, double south_pole_longitude, double south_pole_rotation_angle);

    virtual ~RotatedGrid();

    virtual size_t npts() const;

    virtual void lonlat(std::vector<Point>&) const;

    virtual std::string gridType() const;

    virtual eckit::Properties spec() const;

private:  // methods

    /// Human readable name (may not be unique, especially when BoundBox is different)
    virtual std::string shortName() const;

    /// Adds to the MD5 the information that makes this Grid unique
    virtual void hash(eckit::MD5&) const;

    /// @return area represented by the grid
    virtual const atlas::grid::Domain& domain() const;

    virtual void print(std::ostream&) const;

private: // members

    eckit::ScopedPtr<Grid> grid_;

    double south_pole_latitude_;

    double south_pole_longitude_;

    double south_pole_rotation_angle_;

    mutable std::string shortName_;

};


}  // namespace util
}  // namespace mir


#endif
