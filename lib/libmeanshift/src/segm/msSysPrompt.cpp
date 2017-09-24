/*******************************************************

                 Mean Shift Analysis Library
	=============================================

	The mean shift library is a collection of routines
	that use the mean shift algorithm. Using this algorithm,
	the necessary output will be generated needed
	to analyze a given input set of data.

  Mean Shift System:
  ==================

	The Mean Shift System class provides a mechanism for the
	mean shift library classes to prompt progress and to
	time its computations. When porting the mean shift library
	to an application the methods of this class may be changed
	such that the output of the mean shift class prompts
	will be given to whatever hardware or software device that
	is desired.

	The definition for the mean shift system class is provided
	below. Its prototype is provided in "msSys.cc".

The theory is described in the papers:

  D. Comaniciu, P. Meer: Mean Shift: A robust approach toward feature
									 space analysis.

  C. Christoudias, B. Georgescu, P. Meer: Synergism in low level vision.

and they are is available at:
  http://www.caip.rutgers.edu/riul/research/papers/

Implemented by Chris M. Christoudias, Bogdan Georgescu
********************************************************/

////////////////////////////////////////////////////////////////////////
// Command Line Version:
//   This version of mean shift system is written for the command
//   line version of EDISON.
////////////////////////////////////////////////////////////////////////

//include the msSystem class prototype
#include	"msSys.h"

//include needed system libraries
#include	<time.h>
#include	<stdio.h>
#include	<stdarg.h>
#include	<stdlib.h>

//define bgLog
extern bool CmCDisplayProgress = false;
extern int percentDone;
void bgLog(const char *PromptStr, ...)
{
	//obtain argument list using ANSI standard...
	va_list	argList;
	va_start(argList, PromptStr);

	//print the output string to stderr using
	if(CmCDisplayProgress) vfprintf(stdout, PromptStr, argList);
	va_end(argList);

	//done.
	return;
}
