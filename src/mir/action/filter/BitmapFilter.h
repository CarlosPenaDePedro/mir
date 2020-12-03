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


#ifndef mir_action_BitmapFilter_h
#define mir_action_BitmapFilter_h

#include "mir/action/plan/Action.h"


namespace mir {
namespace util {
class Bitmap;
}
namespace action {


class BitmapFilter : public Action {
public:
    // -- Exceptions
    // None

    // -- Constructors

    BitmapFilter(const param::MIRParametrisation&);

    // -- Destructor

    ~BitmapFilter() override;

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
    // No copy allowed

    BitmapFilter(const BitmapFilter&);
    BitmapFilter& operator=(const BitmapFilter&);

    // -- Members

    std::string path_;

    // -- Methods

    util::Bitmap& bitmap() const;

    // -- Overridden methods

    void execute(context::Context&) const override;
    bool sameAs(const Action&) const override;
    const char* name() const override;
    void estimate(context::Context&, api::MIREstimation&) const override;


    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace action
}  // namespace mir


#endif
