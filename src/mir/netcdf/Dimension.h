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


#ifndef mir_netcdf_Dimension
#define mir_netcdf_Dimension

#include <iosfwd>
#include <string>
#include <vector>

namespace mir {
namespace netcdf {

class Dataset;


class Dimension {
public:
    Dimension(Dataset &owner, const std::string &name, size_t len);
    virtual ~Dimension();

    // -- Methods
    virtual int id() const;

    const std::string &name() const;
    size_t count() const;

    virtual bool sameAs(const Dimension &) const;
    virtual void dump(std::ostream &s) const;
    virtual void create(int nc) const;
    virtual void clone(Dataset &owner) const;
    virtual void grow(size_t n);
    virtual bool inUse() const;

    virtual void realDimensions(std::vector<size_t>& dims) const;

protected:

    // -- Members

    Dataset &owner_;
    std::string name_;
    size_t len_;


private:

    // -- Methods
    virtual void print(std::ostream &s) const = 0;

    // -- Friends

    friend std::ostream &operator<<(std::ostream &s, const Dimension &v) {
        v.print(s);
        return s;
    }

};

}
}
#endif
