#pragma once
/*
 * GtkHeader.h
 *
 *  Created on: May 6, 2016
 *      Author: tly
 */

#ifdef __GNUC__
//remove compiler warnings in cl.hpp

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#include <gtkmm.h>
#pragma GCC diagnostic pop

#else
#include <gtkmm.h>
#endif

//fix compile errors for lambdas as signal handlers
//https://developer.gnome.org/libsigc++/stable/group__slot.html#details
namespace sigc {
  SIGC_FUNCTORS_DEDUCE_RESULT_TYPE_WITH_DECLTYPE
}


