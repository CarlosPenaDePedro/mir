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


#ifndef mir_param_RuntimeParametrisation_h
#define mir_param_RuntimeParametrisation_h

#include <set>
#include <string>
#include "mir/param/SimpleParametrisation.h"


namespace mir {
namespace param {


class RuntimeParametrisation : public SimpleParametrisation {
public:

    // -- Exceptions
    // None

    // -- Contructors

    RuntimeParametrisation(const param::MIRParametrisation& owner);

    // -- Destructor

    ~RuntimeParametrisation(); // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void unset(const std::string& name);

    MIRParametrisation& set(const std::string& name, const std::string &value);
    MIRParametrisation& set(const std::string& name, const char *value);
    MIRParametrisation& set(const std::string& name, float value);
    MIRParametrisation& set(const std::string& name, double value);
    MIRParametrisation& set(const std::string& name, int value);
    MIRParametrisation& set(const std::string& name, long value);
    MIRParametrisation& set(const std::string& name, long long value);
    MIRParametrisation& set(const std::string& name, bool value);
    MIRParametrisation& set(const std::string& name, size_t value);

    MIRParametrisation& set(const std::string& name, const std::vector<int>& value);
    MIRParametrisation& set(const std::string& name, const std::vector<long>& value);
    MIRParametrisation& set(const std::string& name, const std::vector<long long>& value);
    MIRParametrisation& set(const std::string& name, const std::vector<size_t>& value);
    MIRParametrisation& set(const std::string& name, const std::vector<float>& value);
    MIRParametrisation& set(const std::string& name, const std::vector<double>& value);
    MIRParametrisation& set(const std::string& name, const std::vector<std::string>& value);

    // -- Overridden methods

    // From MIRParametrisation
    virtual bool has(const std::string& name) const;

    // From SimpleParametrisation
    virtual bool get(const std::string& name, std::string& value) const;
    virtual bool get(const std::string& name, bool& value) const;
    virtual bool get(const std::string& name, int& value) const;
    virtual bool get(const std::string& name, long& value) const;
    virtual bool get(const std::string& name, float& value) const;
    virtual bool get(const std::string& name, double& value) const;

    virtual bool get(const std::string& name, std::vector<int>& value) const;
    virtual bool get(const std::string& name, std::vector<long>& value) const;
    virtual bool get(const std::string& name, std::vector<float>& value) const;
    virtual bool get(const std::string& name, std::vector<double>& value) const;
    virtual bool get(const std::string& name, std::vector<std::string>& value) const;

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

    const param::MIRParametrisation& owner_;
    std::set<std::string> hidden_;

    // -- Methods

    template<class T>
    void _set(const std::string&, const T&);

    template<class T>
    void _set(const std::string&, const std::vector<T>&);

    template<class T>
    bool _get(const std::string&, T&) const;

    // -- Overridden methods

    // From MIRParametrisation
    virtual void print(std::ostream&) const;
    virtual const MIRParametrisation& userParametrisation() const;
    virtual const MIRParametrisation& fieldParametrisation() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

};


}  // namespace param
}  // namespace mir


#endif

