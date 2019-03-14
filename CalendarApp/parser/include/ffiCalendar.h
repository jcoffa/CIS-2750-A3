/************************************
 *  Name: Joseph Coffa              *
 *  Student #: 1007320              *
 *  Due Date: March 20, 2019        *
 *                                  *
 *  Assignment 3, CIS*2750          *
 *  ffiCalendar.h                   *
 ************************************/

// A unified header file used in tandem with ffiCalendar.c to create a single library file
// that can be used with the 'ffi' library API in JavaScript.

#ifndef FFICALENDAR_H
#define FFICALENDAR_H

#include "CalendarParser.h"

// Takes a filename and returns a JSON string of a Calendar object, or an error code on a fail.
char *createCalendarWrapper(const char filename[]);

// Takes a calendar JSON, an event JSON, two datetime JSON's, and a filename.
// Returns a string representing the exit status (ICalErrorCode)
char *createCalFileFromJSON(char *calJSON, char *eventJSON, char *startJSON, char *createJSON, \
		                    char *filename);

#endif
