/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Jan 2017


#include "TransInitor.h"

#include "eckit/exception/Exceptions.h"
#include "atlas/internals/atlas_config.h"

#ifdef ATLAS_HAVE_TRANS
#include "transi/trans.h"
#endif


namespace mir {
namespace action {
namespace transform {


const TransInitor& TransInitor::instance() {
    static TransInitor initor;
    return initor;
}


TransInitor::TransInitor() {
#ifdef ATLAS_HAVE_TRANS
    trans_use_mpi(false); // So that even if MPI is enabled, we don't use it.
    trans_init();
#else
    throw eckit::SeriousBug("Spherical harmonics transforms are not supported. "
                            "Please recompile ATLAS with TRANS support enabled.");
#endif
}


TransInitor::~TransInitor() {
#ifdef ATLAS_HAVE_TRANS
    trans_use_mpi(false); // So that even if MPI is enabled, we don't use it.
    trans_finalize();
#else
    throw eckit::SeriousBug("Spherical harmonics transforms are not supported. "
                            "Please recompile ATLAS with TRANS support enabled.");
#endif
}


}  // namespace transform
}  // namespace action
}  // namespace mir
