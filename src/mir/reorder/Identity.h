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


#pragma once

#include "mir/reorder/Reorder.h"


namespace mir::reorder {


struct Identity final : Reorder {
    explicit Identity(size_t N) : N_(N) {}

private:
    Renumber reorder() const override;
    const size_t N_;
};


}  // namespace mir::reorder
