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


#ifndef mir_param_SimpleParametrisation_h
#define mir_param_SimpleParametrisation_h

#include <map>
#include <string>
#include "mir/param/MIRParametrisation.h"


namespace eckit {
class JSON;
}
namespace mir {
namespace param {
class Setting;
}
}  // namespace mir


namespace mir {
namespace param {


class SimpleParametrisation : public MIRParametrisation {
public:
    // -- Exceptions
    // None

    // -- Contructors

    SimpleParametrisation();

    // -- Destructor

    virtual ~SimpleParametrisation();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void copyValuesTo(SimpleParametrisation&) const;

    SimpleParametrisation& set(const std::string& name, const std::string& value);
    SimpleParametrisation& set(const std::string& name, const char* value);
    SimpleParametrisation& set(const std::string& name, float value);
    SimpleParametrisation& set(const std::string& name, double value);
    SimpleParametrisation& set(const std::string& name, int value);
    SimpleParametrisation& set(const std::string& name, long value);
    SimpleParametrisation& set(const std::string& name, long long value);
    SimpleParametrisation& set(const std::string& name, bool value);
    SimpleParametrisation& set(const std::string& name, size_t value);

    SimpleParametrisation& set(const std::string& name, const std::vector<int>& value);
    SimpleParametrisation& set(const std::string& name, const std::vector<long>& value);
    SimpleParametrisation& set(const std::string& name, const std::vector<long long>& value);
    SimpleParametrisation& set(const std::string& name, const std::vector<size_t>& value);
    SimpleParametrisation& set(const std::string& name, const std::vector<float>& value);
    SimpleParametrisation& set(const std::string& name, const std::vector<double>& value);
    SimpleParametrisation& set(const std::string& name, const std::vector<std::string>& value);

    virtual SimpleParametrisation& clear(const std::string& name);
    SimpleParametrisation& reset();

    // Used by Job

    virtual bool empty() const;
    bool matches(const MIRParametrisation&) const;

    // -- Overridden methods

    // From MIRParametrisation
    virtual bool has(const std::string& name) const;

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

    virtual void print(std::ostream&) const;
    void json(eckit::JSON&) const;
    size_t size() const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Types

    using SettingsMap = std::map<std::string, Setting*>;

    // -- Members

    SettingsMap settings_;

    // -- Methods

    template <class T>
    bool _get(const std::string& name, T& value) const;

    template <class T>
    void _set(const std::string& name, const T& value);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend eckit::JSON& operator<<(eckit::JSON& s, const SimpleParametrisation& p) {
        p.json(s);
        return s;
    }
};


}  // namespace param
}  // namespace mir


#endif
