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


#ifndef mir_method_Cropping_h
#define mir_method_Cropping_h

#include <iosfwd>

#include "mir/util/BoundingBox.h"


namespace mir {
namespace method {


class Cropping {
public:
    Cropping();
    Cropping(const Cropping&);
    ~Cropping();

    Cropping& operator=(const Cropping&);
    bool operator==(const Cropping&) const;

    operator bool() const { return active_; }

    void hash(eckit::MD5&) const;
    void boundingBox(const util::BoundingBox&);
    const util::BoundingBox& boundingBox() const;

protected:
    void print(std::ostream&) const;

private:
    util::BoundingBox bbox_;
    bool active_;

    friend std::ostream& operator<<(std::ostream& s, const Cropping& p) {
        p.print(s);
        return s;
    }
};


}  // namespace method
}  // namespace mir


#endif
