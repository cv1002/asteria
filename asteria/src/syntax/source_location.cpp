// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "source_location.hpp"

namespace Asteria {

void Source_Location::print(std::ostream &os) const
  {
    os << this->get_file() << ':' << this->get_line();
  }

}
