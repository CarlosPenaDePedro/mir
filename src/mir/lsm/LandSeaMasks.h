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


#ifndef mir_lsm_LandSeaMasks_h
#define mir_lsm_LandSeaMasks_h

#include <iosfwd>
#include <vector>


namespace eckit {
class MD5;
}
namespace mir {
namespace lsm {
class Mask;
}
}  // namespace mir
namespace mir {
namespace param {
class MIRParametrisation;
}
namespace repres {
class Representation;
}
}  // namespace mir


namespace mir {
namespace lsm {


class LandSeaMasks {
public:
    // -- Exceptions
    // None

    // -- Contructors

    LandSeaMasks(const Mask& input, const Mask& output);
    LandSeaMasks(const LandSeaMasks&);

    // -- Destructor

    ~LandSeaMasks();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void hash(eckit::MD5&) const;
    std::string cacheName() const;

    bool active() const;
    bool cacheable() const;

    const std::vector<bool>& inputMask() const;
    const std::vector<bool>& outputMask() const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static LandSeaMasks lookup(const param::MIRParametrisation&, const repres::Representation& in,
                               const repres::Representation& out);

    static bool sameLandSeaMasks(const param::MIRParametrisation&, const param::MIRParametrisation&);

protected:
    // -- Members

    const Mask& input_;
    const Mask& output_;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Convertors
    // None

    // -- Operators

    LandSeaMasks& operator=(const LandSeaMasks&) = delete;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const LandSeaMasks& p) {
        p.print(s);
        return s;
    }
};


}  // namespace lsm
}  // namespace mir


#endif
