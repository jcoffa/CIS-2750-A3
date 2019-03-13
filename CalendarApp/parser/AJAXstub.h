/************************************
 *  Name: Joseph Coffa              *
 *  Student #: 1007320              *
 *  Due Date: March 20, 2019        *
 *                                  *
 *  Assignment 3, CIS*2750          *
 *  AJAXstub.h                      *
 ************************************/

// Just a bunch of dummy functions that return random JSON strings of data that can be coerced
// into information that looks like it could have come from a real event, or alarm, or calendar, etc.

#ifndef AJAXSTUB
#define AJAXSTUB

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_WORDS 38

//const char *words[NUM_WORDS] = {"cold","elephant","military","teeny","safe","mighty","cynical","office","thaw","synapse","swim","ragged","bell","switch","admit","tax","mark","volleyball","mysterious","job","tough","bejewel","committee","geese","cough","pushy","circle","sprout","wings","satisfy","ask","pastoral","action","obnoxious","flame","obedient","offer","neat"};

char *fakeText(bool spaces, int numWords);

char *fakeDT();

char *fakeAlarm();

char *fakeEvent();

char *fakeCal();

#endif
