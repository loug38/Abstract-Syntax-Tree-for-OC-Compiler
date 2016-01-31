// $Id: syslib.h,v 1.5 2015-07-09 18:10:52-07 - - $

#ifndef __SYSLIB_H__
#define __SYSLIB_H__

#include <string>
using namespace std;

#include <stdarg.h>

// DESCRIPTION
//    Auxiliary library containing miscellaneous useful things.

struct exec {
  static string execname;
  static int exit_status;
  static void set_status (int);
};

// Print the status returned by wait(2) from a subprocess.
void eprint_status (const char* command, int status);

// Print an error message according to the printf format
// specified, using eprintf.  Sets the exitstatus to EXIT_FAILURE.
void errprintf (const char* format, ...);

// Print a message resulting from a bad system call.  The
// object is the name of the object causing the problem and
// the reason is taken from the external variable errno.
// Sets the exit status to EXIT_FAILURE.
void syserrprintf (const char* object);

#endif
