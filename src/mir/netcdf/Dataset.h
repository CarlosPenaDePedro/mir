/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - ECMWF Jan 2015

#ifndef mir_netcdf_Dataset
#define mir_netcdf_Dataset

#include "mir/netcdf/Endowed.h"

#include <string>
#include <vector>

namespace mir{
namespace netcdf{

class Dimension;
class Variable;
class Field;


class Dataset : public Endowed {
public:

    Dataset(const std::string &);
    virtual ~Dataset();

    // -- Methods

    Dimension *findDimension(int id) const;
    Dimension *findDimension(const std::string &name) const;
    std::vector<Variable *> variablesForDimension(const Dimension &) const;

    virtual void dump(std::ostream &s) const;

    void add(Dimension *);
    void add(Variable *);

    const std::map<std::string, Dimension *> &dimensions() const ;
    const std::map<std::string, Variable *> &variables() const ;


    //
    bool hasVariable(const std::string& name) const;
    const Variable& variable(const std::string& name) const;

    // From Endowed

    virtual const std::string &path() const;

protected:

    // -- Members
    std::string path_;
    std::map<std::string, Dimension *> dimensions_;
    std::map<std::string, Variable *> variables_;


private:

    Dataset(const Dataset &);
    Dataset &operator=(const Dataset &);

    // From Endowed

    virtual int varid() const;
    virtual const std::string &name() const;

    // - Methods

    virtual void print(std::ostream &s) const = 0;

    // -- Friends
    friend std::ostream &operator<<(std::ostream &s, const Dataset &v) {
        v.print(s);
        return s;
    }
};

}
}
#endif
