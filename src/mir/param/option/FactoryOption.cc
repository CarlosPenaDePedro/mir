/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @date Apr 2015


#include "mir/param/option/FactoryOption.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Translator.h"

#include <iostream>

namespace mir {
namespace param {
namespace option {


template<class T>
FactoryOption<T>::FactoryOption(const std::string &name, const std::string &description):
    Option(name, description) {
}

template<class T>
FactoryOption<T>::~FactoryOption() {
}

template<class T>
void FactoryOption<T>::set(const std::string &value, SimpleParametrisation &parametrisation) const {
    parametrisation.set(name_, value);
}

template<class T>
void FactoryOption<T>::print(std::ostream &out) const {
    out << "   --" << name_ << "=name" << " (" << description_ << ")";
    out << std::endl << "     Values are: ";
    T::list(out);
}

} // namespace option
}  // namespace param
}  // namespace mir

