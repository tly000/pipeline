#pragma once
/*
 * GtkHeader.h
 *
 *  Created on: May 6, 2016
 *      Author: tly
 */

#include <gtkmm.h>

//fix compile errors for lambdas as signal handlers
//https://developer.gnome.org/libsigc++/stable/group__slot.html#details
namespace sigc {
  SIGC_FUNCTORS_DEDUCE_RESULT_TYPE_WITH_DECLTYPE
}


