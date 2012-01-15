/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "tools/library.hpp"

namespace oos {

library::library()
  : handle_(0)
{}

library::library(const std::string &lib)
  : lib_(lib)
  , handle_(0)
{}

library::~library()
{
  unload();
}

bool library::load()
{
#ifdef WIN32
  handle_ = LoadLibrary((lib_ + ".dll").c_str());
#else
  handle_ = dlopen((lib_ + ".so").c_str(), RTLD_LAZY);
#endif
  if (!handle_) {
    return false;
  }
  return true;
}

bool library::load(const std::string &lib)
{
  if (handle_) {
    return false;
  }
  lib_ = lib;
  return load();
}

bool library::unload()
{
  if (handle_) {
#ifdef WIN32
    return FreeLibrary(handle_) == TRUE;
#else
    return dlclose(handle_) == 0;
#endif
  }
  return true;
}

func_ptr library::function(const std::string &f) const
{
#ifdef WIN32
  return GetProcAddress(handle_, f.c_str());
#else
  return dlsym(handle_, f.c_str());
#endif
}

}
