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


#ifndef mir_action_transform_ShVodToUV_h
#define mir_action_transform_ShVodToUV_h

#include "mir/action/plan/Action.h"
#include "mir/api/Atlas.h"


namespace mir {
namespace action {
namespace transform {


class ShVodToUV : public Action {
public:
    // -- Types

    using atlas_config_t = atlas::util::Config;

    // -- Exceptions
    // None

    // -- Constructors

    ShVodToUV(const param::MIRParametrisation&);

    // -- Destructor

    ~ShVodToUV() override;

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

    void print(std::ostream&) const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    atlas_config_t options_;

    // -- Methods
    // None

    // -- Overridden methods

    bool sameAs(const Action&) const override;
    void execute(context::Context&) const override;
    const char* name() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace transform
}  // namespace action
}  // namespace mir


#endif
