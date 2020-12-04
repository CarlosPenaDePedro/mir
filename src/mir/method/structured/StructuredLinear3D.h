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


#ifndef mir_method_structured_StructuredLinear3D_h
#define mir_method_structured_StructuredLinear3D_h

#include "mir/method/structured/StructuredMethod.h"


namespace mir {
namespace method {
namespace structured {


class StructuredLinear3D : public StructuredMethod {
public:
    StructuredLinear3D(const param::MIRParametrisation&);
    ~StructuredLinear3D();

private:
    void assembleStructuredInput(WeightMatrix&, const repres::Representation& in,
                                 const repres::Representation& out) const override;

    const char* name() const override;

    void hash(eckit::MD5&) const override;

    void print(std::ostream&) const override;

    bool sameAs(const Method&) const override;
};


}  // namespace structured
}  // namespace method
}  // namespace mir


#endif
