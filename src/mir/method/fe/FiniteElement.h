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


#ifndef mir_method_fe_FiniteElement_h
#define mir_method_fe_FiniteElement_h

#include "mir/api/Atlas.h"
#include "mir/method/MethodWeighted.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace method {
namespace fe {


class FiniteElement : public MethodWeighted {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    FiniteElement(const param::MIRParametrisation&, const std::string& label = "input");

    // -- Destructor

    virtual ~FiniteElement() override;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

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

    const util::MeshGeneratorParameters& meshGeneratorParams() const { return meshGeneratorParams_; }
    util::MeshGeneratorParameters& meshGeneratorParams() { return meshGeneratorParams_; }

    atlas::Mesh atlasMesh(util::MIRStatistics&, const repres::Representation&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    util::MeshGeneratorParameters meshGeneratorParams_;

    // -- Methods
    // None

    // -- Overridden methods

    // From MethodWeighted
    void hash(eckit::MD5&) const;
    void assemble(util::MIRStatistics&, WeightMatrix&, const repres::Representation& in,
                  const repres::Representation& out) const;
    bool sameAs(const Method&) const;
    void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend class L2Projection;
};


class FiniteElementFactory : public MethodFactory {
    std::string name_;
    virtual FiniteElement* make(const param::MIRParametrisation&, const std::string& label) = 0;
    virtual FiniteElement* make(const param::MIRParametrisation&)                           = 0;

    FiniteElementFactory(const FiniteElementFactory&) = delete;
    FiniteElementFactory& operator=(const FiniteElementFactory&) = delete;

protected:
    FiniteElementFactory(const std::string&);
    virtual ~FiniteElementFactory() override;

public:
    static void list(std::ostream&);
    static FiniteElement* build(const std::string& method, const std::string& label, const param::MIRParametrisation&);
};


template <class T>
class FiniteElementBuilder : public FiniteElementFactory {
    virtual FiniteElement* make(const param::MIRParametrisation& param, const std::string& label) {
        return new T(param, label);
    }

    virtual FiniteElement* make(const param::MIRParametrisation& param) { return new T(param); }

public:
    FiniteElementBuilder(const std::string& name) : FiniteElementFactory(name) {}
};


}  // namespace fe
}  // namespace method
}  // namespace mir


#endif
