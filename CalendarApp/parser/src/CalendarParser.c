/************************************
 *  Name: Joseph Coffa              *
 *  Student #: 1007320              *
 *  Due Date: February 27, 2019     *
 *                                  *
 *  Assignment 2, CIS*2750          *
 *  CalendarParser.c                *
 ************************************/

#include "CalendarParser.h"
#include "CalendarHelper.h"
#include "LinkedListAPI.h"
#include "Parsing.h"
#include "Initialize.h"

/** Function to create a Calendar object based on the contents of an iCalendar file.
 *@pre File name cannot be an empty string or NULL.  File name must have the .ics extension.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid calendar has been created, its address was stored in the variable obj, and OK was returned
		or
		An error occurred, the calendar was not created, all temporary memory was freed, obj was set to NULL, and the
		appropriate error code was returned
 *@return the error code indicating success or the error encountered when parsing the calendar
 *@param fileName - a string containing the name of the iCalendar file
 *@param a double pointer to a Calendar struct that needs to be allocated
**/
ICalErrorCode createCalendar(char* fileName, Calendar** obj) {
    FILE *fin;
    ICalErrorCode error;
    bool version, prodID, method, beginCal, endCal, foundEvent;
    char *parse, *name, *descr;
	char delim[] = ";:";
    version = prodID = method = beginCal = endCal = foundEvent = false;

	debugMsg("-----START createCalendar()-----\n");

    // Prof said not to check for obj being NULL, but you can't dereference a NULL pointer,
    // so I think he meant "don't worry if *obj = NULL, since it is being overwritten", and in
    // order to dereference it then the double pointer passed into the function can't be NULL.
    if (obj == NULL) {
		errorMsg("\tprovided obj is NULL\n");
        return OTHER_ERROR;
    }

    // filename can't be null or an empty string, and must end with the '.ics' extension
    if (fileName == NULL || strcmp(fileName, "") == 0 || !endsWith(fileName, ".ics")) {
		errorMsg("\tInvalid fileName. fileName = \"%s\"\n", fileName);
        *obj = NULL;
		notifyMsg("\tRETURNING INV_FILE\n");
        return INV_FILE;
    }

    fin = fopen(fileName, "r");

    // Check that file was found/opened correctly
    if (fin == NULL) {
		errorMsg("\tFile could not be found/opened properly\n");
        // On a failure, the obj argument is set to NULL and an error code is returned
        *obj = NULL;
		notifyMsg("\tRETURNING INV_FILE\n");
        return INV_FILE;
    }

    // allocate memory for the Calendar and all its components
    if ((error = initializeCalendar(obj)) != OK) {
		errorMsg("\tCould not initializeCalendar() for some reason\n");
        return error;
    }

    char line[10000];
    while (!feof(fin)) {
        // readFold returns NULL when the raw line does not end with a \r\n sequence
        // (i.e. the file has invalid line endings)
        if ((error = readFold(line, 10000, fin)) != OK) {
			errorMsg("\treadFold() failed for some reason\n");
            cleanup(obj, NULL, fin);
            return error;
        }

		debugMsg("\tLine read : \"%s\"\n", line);

		if (line[0] == ';') {
            // lines starting with a semicolon (;) are comments, and
            // should be ignored
            free(parse);
            parse = NULL;
            continue;
        }

		// Check if the END:VCALENDAR has been hit. If it has, and there is still more file to be read,
        // then something has gone wrong.
        if (endCal) {
			errorMsg("\tMore lines after hitting END:VCALENDAR\n");
            cleanup(obj, parse, fin);
            return INV_CAL;
        }

        parse = strUpperCopy(line);

        //debugMsg("upper'd line = \"%s\"\n", parse);

		// Empty lines/lines containing just whitespace are NOT permitted
        // by the iCal specification.
        // (readFold function automatically trims whitespace)
        if (parse[0] == '\0') {
			errorMsg("\tLine read contained all whitespace\n");
            cleanup(obj, parse, fin);
            return INV_CAL;
        }

		// split the string into the property name and property description
		if ((name = strtok(parse, delim)) == NULL) {
			// The line is only delimiters, which obviously is not allowed
			debugMsg("\tLine contained only delimiters\n");
			cleanup(obj, parse, fin);
			return INV_CAL;
		}
		if ((descr = strtok(NULL, delim)) == NULL) {
			// The line has no property description, or doesn't contain any delimiters
			debugMsg("\tLine contains no property description\n");
			cleanup(obj, parse, fin);
			return INV_CAL;
		}

        // The first non-commented line must be BEGIN:VCALENDAR
        if (!beginCal && !(strcmp(name, "BEGIN") == 0 && strcmp(descr, "VCALENDAR") == 0)) {
			errorMsg("\tFirst non-comment line was not BEGIN:VCALENDAR\n");
            cleanup(obj, parse, fin);
            return INV_CAL;
        } else if (!beginCal) {
            beginCal = true;
            free(parse);
            continue;
        }
        

        // add properties, alarms, events, and other elements to the calendar
        if (strcmp(name, "VERSION") == 0) {
            if (version) {
				errorMsg("\tEncountered duplicate version\n");
                cleanup(obj, parse, fin);
                return DUP_VER;
            }

            //debugMsg("found VERSION line: \"%s\"\n", line);
            char *endptr;
            // +8 to start conversion after the 'VERSION:' part of the string
            (*obj)->version = strtof(line + 8, &endptr);

            if (strlen(line + 8) == 0 || line+8 == endptr || *endptr != '\0') {
                // VERSION property contains no data after the ':', or the data
                // could not be converted into a number
				errorMsg("\tVERSION property could not be coerced into an integer properly: \"%s\"\n", line);
                cleanup(obj, parse, fin);
                return INV_VER;
            }

            //debugMsg("set version to %f\n", (*obj)->version);
            version = true;
        } else if (strcmp(name, "PRODID") == 0) {
            if (prodID) {
				errorMsg("\tDuplicate PRODID\n");
                cleanup(obj, parse, fin);
                return DUP_PRODID;
            }

            // PRODID: contains no information
            if (strlen(line + 7) == 0) {
				errorMsg("\tPRODID empty\n");
                cleanup(obj, parse, fin);
                return INV_PRODID;
            }

            // +7 to only copy characters past 'PRODID:' part of the string
            //debugMsg("found PRODID line: \"%s\"\n", line);
            strcpy((*obj)->prodID, line + 7);
            //debugMsg("set product ID to\"%s\"\n", (*obj)->prodID);
            prodID = true;
        } else if (strcmp(name, "METHOD") == 0) {
            if (method) {
				errorMsg("\tDuplicate METHOD\n");
                cleanup(obj, parse, fin);
                return INV_CAL;
            }

            // METHOD: contains no information
            if (strlen(line + 7) == 0) {
				errorMsg("\tMETHOD empty\n");
                cleanup(obj, parse, fin);
                return INV_CAL;
            }

            //debugMsg("found METHOD line: \"%s\"\n", line);
            Property *methodProp;
            if ((error = initializeProperty(line, &methodProp)) != OK) {
                // something happened, and the property could not be created properly
				errorMsg("\tinitializeProperty() failed somehow with line \"%s\"\n", line);
                cleanup(obj, parse, fin);
                return INV_CAL;
            }

            insertBack((*obj)->properties, (void *)methodProp);
            method = true;
        } else if (strcmp(name, "END") == 0 && strcmp(descr, "VCALENDAR") == 0) {
            endCal = true;
        } else if (strcmp(name, "BEGIN") == 0 && strcmp(descr, "VCALENDAR") == 0) {
            // only 1 calendar allowed per file
			errorMsg("\tDuplicate BEGIN:VCALENDAR\n");
            cleanup(obj, parse, fin);
            return INV_CAL;
        } else if (strcmp(name, "BEGIN") == 0 && strcmp(descr, "VEVENT") == 0) {
            Event *event;
            if ((error = getEvent(fin, &event)) != OK) {
                // something happened, and the event could not be created properly
				errorMsg("\tgetEvent() failed somehow\n");
                cleanup(obj, parse, fin);
                return error;
            }
            foundEvent = true;

            insertBack((*obj)->events, (void *)event);
        } else if (strcmp(name, "BEGIN") == 0 && strcmp(descr, "VALARM") == 0) {
            // there can't be an alarm for an entire calendar
            errorMsg("found an alarm not in an event\n");
            cleanup(obj, parse, fin);
            return INV_ALARM;
        } else if (strcmp(name, "END") == 0 && (strcmp(descr, "VEVENT") == 0 || strcmp(descr, "VALARM") == 0)) {
            // a duplicated END tag was found
            errorMsg("Found a duplicated END tag: \"%s\"\n", line);
            cleanup(obj, parse, fin);
            return INV_CAL;
        } else {
            // All other BEGIN: clauses have been handled in their own 'else if' case.
            // If another one is hit, then it is an error.
            if (strcmp(name, "BEGIN") == 0) {
				errorMsg("\tFound illegal BEGIN: \"%s\"\n", line);
                cleanup(obj, parse, fin);
                return INV_CAL;
            }

            //debugMsg("found non-mandatory property: \"%s\"\n", line);
            Property *prop;
            if ((error = initializeProperty(line, &prop)) != OK) {
                // something happened, and the property could not be created properly
				errorMsg("\tinitializeProperty() failed somehow with line \"%s\"\n", line);
                cleanup(obj, parse, fin);
                return INV_CAL;
            }

            insertBack((*obj)->properties, (void *)prop);
        }

        free(parse);
        parse = NULL;
    }
    fclose(fin);

    // Calendars require a few mandatory elements. If one does not have
    // any of these properties/lines, it is invalid.
    if (!endCal || !foundEvent || !version || !prodID) {
		errorMsg("\tMissing required property: endCal=%d, foundEvent=%d, version=%d, prodID=%d\n", \
		         endCal, foundEvent, version, prodID);
        cleanup(obj, parse, NULL);
        return INV_CAL;
    }

    // the file has been parsed, mandatory properties have been found,
    // and the calendar is valid (or at least valid with respect to
    // the syntax of an iCalendar file)
	debugMsg("\t-----END createCalendar()-----\n");
    return OK;
}


/** Function to delete all calendar content and free all the memory.
 *@pre Calendar object exists, is not null, and has not been freed
 *@post Calendar object had been freed
 *@return none
 *@param obj - a pointer to a Calendar struct
**/
void deleteCalendar(Calendar* obj) {
    if (obj->events != NULL) {
		freeList(obj->events);
	}

	if (obj->properties != NULL) {
		freeList(obj->properties);
	}

    free(obj);
}


/** Function to create a string representation of a Calendar object.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a string representing the Calendar contents has been created
 *@return a string contaning a humanly readable representation of a Calendar object
 *@param obj - a pointer to a Calendar struct
**/
char* printCalendar(const Calendar* obj) {
    if (obj == NULL) {
        return NULL;
    }

    char *eventListStr = toString(obj->events);
    char *propertyListStr = toString(obj->properties);

	int size = strlen(eventListStr) + strlen(propertyListStr) + 1000;
	char *toReturn = malloc(size);

    // check for malloc failing
    if (toReturn == NULL) {
        return NULL;
    }

    // A neat little function I found that allows for string creation using printf
    // format specifiers. Makes stringing information together in a string like this
    // much easier than using strcat() repeatedly!
    snprintf(toReturn, size, "Start CALENDAR: {VERSION=%.2f, PRODID=%s, Start EVENTS={%s\n} End EVENTS, Start PROPERTIES={%s\n} End PROPERTIES}, End CALENDAR", \
             obj->version, obj->prodID, eventListStr, propertyListStr);

    free(eventListStr);
    free(propertyListStr);

    return realloc(toReturn, strlen(toReturn) + 1);
}


/** Function to "convert" the ICalErrorCode into a humanly redabale string.
 *@return a string containing a humanly readable representation of the error code by indexing into
          the descr array using the error code enum value as an index
 *@param err - an error code
**/
char* printError(ICalErrorCode err) {
    // FIXME this doesn't do what the description specificly says;
    // no array named 'descr' is found anywhere in the assignment outline,
    // so I can't exactly index it.
    char *toReturn = malloc(200);
    
    switch ((int)err) {
        case OK:
            strcpy(toReturn, "OK");
            break;

        case INV_FILE:
            strcpy(toReturn, "Invalid file");
            break;

        case INV_CAL:
            strcpy(toReturn, "Invalid calendar");
            break;

        case INV_VER:
            strcpy(toReturn, "Invalid version");
            break;

        case DUP_VER:
            strcpy(toReturn, "Duplicate version");
            break;

        case INV_PRODID:
            strcpy(toReturn, "Invalid product ID");
            break;

        case DUP_PRODID:
            strcpy(toReturn, "Duplicate product ID");
            break;

        case INV_EVENT:
            strcpy(toReturn, "Invalid event");
            break;

        case INV_DT:
            strcpy(toReturn, "Invalid date-time");
            break;

        case INV_ALARM:
            strcpy(toReturn, "Invalid alarm");
            break;

        case WRITE_ERROR:
            strcpy(toReturn, "Write error");
            break;

        case OTHER_ERROR:
            strcpy(toReturn, "Other error");
            break;

        default:
            sprintf(toReturn, "Unknown error: Found error with value %d", err);
            break;
    }

    return realloc(toReturn, strlen(toReturn) + 1);
}


/** Function to writing a Calendar object into a file in iCalendar format.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a file representing the
        Calendar contents in iCalendar format has been created
 *@return the error code indicating success or the error encountered when parsing the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode writeCalendar(char* fileName, const Calendar* obj) {
    FILE *fout;

	debugMsg("-----START writeCalendar()-----\n");

    if (fileName == NULL || obj == NULL) {
		errorMsg("\tEither fileName or obj is NULL. fileName == NULL: %d, obj == NULL: %d\n", \
		         fileName == NULL, obj == NULL);
        return WRITE_ERROR;
    }

    if (strcmp(fileName, "") == 0 || !endsWith(fileName, ".ics")) {
		errorMsg("\tfileName is empty or does not end with .ics extension. fileName=\"%s\"\n", fileName);
        return WRITE_ERROR;
    }

	debugMsg("\tfileName = \"%s\"\n", fileName);
    
    if ((fout = fopen(fileName, "w")) == NULL) {
		errorMsg("\tfile \"%s\" could not be opened for writing for some reason.\n", fileName);
        return WRITE_ERROR;
    }

    ICalErrorCode err;
    fprintf(fout, "BEGIN:VCALENDAR\r\n");
    fprintf(fout, "PRODID:%s\r\n", obj->prodID);
    fprintf(fout, "VERSION:%.1f\r\n", obj->version);

	debugMsg("\tWrote BEGIN:VCALENDAR, prodID, and version\n");
    
	if ((err = writeProperties(fout, obj->properties)) != OK) {
		errorMsg("\twriteProperties() failed somehow\n");
		fclose(fout);
        return WRITE_ERROR;
    }
    if ((err = writeEvents(fout, obj->events)) != OK) {
		errorMsg("\twriteEvents() failed somehow\n");
		fclose(fout);
        return WRITE_ERROR;
    }
    fprintf(fout, "END:VCALENDAR\r\n");

	fclose(fout);

	debugMsg("\tWrote END:VCALENDAR\n");
	debugMsg("\t-----END writeCalendar()-----\n");

    return OK;
}


/** Function to validating an existing a Calendar object
 *@pre Calendar object exists and is not null
 *@post Calendar has not been modified in any way
 *@return the error code indicating success or the error encountered when validating the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode validateCalendar(const Calendar* obj) {
	debugMsg("-----START validateCalendar()----\n");
	if (obj == NULL) {
		errorMsg("\tCalendar pointer is NULL\n");
		return INV_CAL;
	}

	// check for NULL Calendar members
	if (obj->prodID == NULL || obj->events == NULL || obj->properties == NULL) {
		errorMsg("\tEncountered NULL Calendar member. prodId == NULL: %d, events == NULL: %d, properties == NULL: %d\n", \
		         obj->prodID == NULL, obj->events == NULL, obj->properties == NULL);
		return INV_CAL;
	}

	// verify the version
	if (obj->version <= 0.0) {
		errorMsg("\tInvalid version: %f\n", obj->version);
		return INV_CAL;
	}

	// product Id can't be empty
	if ((obj->prodID)[0] == '\0') {
		errorMsg("\tprodID empty string\n");
		return INV_CAL;
	}

	// product Id can't be longer than 1000 characters (including '\0')
	bool terminator = false;
	for (int i = 0; i <= 999; i++) {
		if ((obj->prodID)[i] == '\0') {
			terminator = true;
			break;
		}
	}
	if (!terminator) {
		errorMsg("\tprodID did not have a '\\0' within the first 1000 characters\n");
		return INV_CAL;
	}

	// 'highestPriority' is necessary due to one of the last paragraphs of Module 2:
	//	"If the struct contains multiple errors, the error code should correspond to the highest level
	//	of error code that you encounter. For example, if the argument to validateCalendar contains:
	//	- an invalid Calendar property, and
	//	- an invalid Alarm component inside an Event component
	//	you must return INV_CAL, not INV_ALARM."
	ICalErrorCode err, highestPriority;
	char *printErr;

	// initialize to a dummy value of OK to symbolize no errors have been encountered
	highestPriority = OK;

	// verify events
	if ((err = validateEvents(obj->events)) != OK) {
		printErr = printError(err);
		debugMsg("\tvalidateEvents() returned an error: %s\n", printErr);
		free(printErr);
		highestPriority = err;
	}

	// verify calendar properties
	if ((err = validatePropertiesCal(obj->properties)) != OK) {
		printErr = printError(err);
		debugMsg("\tvalidatePropertiesCal() returned an error: %s\n", printErr);
		free(printErr);
		// determine the priority of the new error
		highestPriority = higherPriority(highestPriority, err);
	}

	// Return the highest priority error. This variable is initialized to OK, so if no errors were encountered
	// then it returns OK; indicative of a valid calendar with no errors.
	printErr = printError(err);
	notifyMsg("\tRETURN ERROR: %s\n", printErr);
	free(printErr);
    return highestPriority;
}


/** Function to converting a DateTime into a JSON string
 *@pre N/A
 *@post DateTime has not been modified in any way
 *@return A string in JSON format
 *@param prop - a DateTime struct
 **/
char* dtToJSON(DateTime prop) {
	debugMsg("-----START dtToJSON()-----\n");
	char *temp = printDate((void *)&prop);
	debugMsg("\tDT passed: \"%s\"\n", temp);
	free(temp);

	char *toReturn = malloc(1000);

	int written = snprintf(toReturn, 1000, "{\"date\":\"%s\",\"time\":\"%s\",\"isUTC\":%s}", prop.date, prop.time, \
	                       (prop.UTC) ? "true" : "false");

	notifyMsg("\tJSON created: \"%s\"\n", toReturn);

	return realloc(toReturn, written + 1);
}

/** Function to converting an Event into a JSON string
 *@pre Event is not NULL
 *@post Event has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to an Event struct
 **/
char* eventToJSON(const Event* event) {
	debugMsg("-----START eventToJSON()-----\n");
	char *toReturn;
	int written;

	if (event == NULL) {
		// In the case where an event is NULL, an empty JSON string is returned
		notifyMsg("\tEvent passed is NULL, returning \"{}\"\n");

		toReturn = malloc(3);
		strcpy(toReturn, "{}");
		written = 2;
	} else {
		char *temp = printEvent((void *)event);
		debugMsg("\tEvent passed: \"%s\"\n", temp);
		free(temp);

		char *startDT = dtToJSON(event->startDateTime);

		// Create a dummy property to find the "SUMMARY" property in 'event', if it exists
		Property *dummy = malloc(sizeof(Property));
		strcpy(dummy->propName, "SUMMARY");
		Property *summary = findElement(event->properties, propNamesEqual, dummy);
		free(dummy);

		// Allocate memory depending on whether a SUMMARY property needs to be written
		int size = (summary == NULL) ? 500 : strlen(summary->propDescr) + 500;
		toReturn = malloc(size);

		// Write the JSON in toReturn
		written = snprintf(toReturn, size, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\"}", \
		                   startDT, getLength(event->properties)+3, getLength(event->alarms), \
		                   // findElement returns NULL if the property could not be found in 'event',
		                   // in which case an empty string is written instead of the summary properties description
		                   (summary == NULL) ? "" : summary->propDescr);

		// NOTE: +3 is added to the length of the Event's proeprty list because
		// the required UID and the 2 required DateTimes count as properties
	}

	notifyMsg("\tJSON created: \"%s\"\n", toReturn);

	return realloc(toReturn, written + 1);
}

/** Function to converting an Event list into a JSON string
 *@pre Event list is not NULL
 *@post Event list has not been modified in any way
 *@return A string in JSON format
 *@param eventList - a pointer to an Event list
 **/
char* eventListToJSON(const List* eventList) {
	char *toReturn, *tempEvJSON;
	int currentLength;

	debugMsg("-----START eventListToJSON()-----\n");

	// Casting a List * into a List * to avoid a warning regarding non-const parameters
	if (eventList == NULL || getLength((List *)eventList) == 0) {
		toReturn = malloc(3);
		strcpy(toReturn, "[]");
		notifyMsg("\tEvent List empty or NULL, returning \"%s\"\n", toReturn);
		return toReturn;
	}

	char *temp = toString((List *)eventList);
	debugMsg("\tEvent list passed: \"%s\"\n", temp);
	free(temp);

	// Start by putting the initial bracket '[' in the JSON
	toReturn = malloc(2);
	strcpy(toReturn, "[");
	currentLength = 1;

	// Add all the event JSON's to 'toReturn'
	// Casting a List * into a List * to avoid a warning regarding non-const parameters
	ListIterator iter = createIterator((List *)eventList);
	Event *ev;
	while ((ev = (Event *)nextElement(&iter)) != NULL) {
		tempEvJSON = eventToJSON(ev);
		debugMsg("\tEvent JSON just created: \"%s\"\n", tempEvJSON);
		currentLength += strlen(tempEvJSON);
		toReturn = realloc(toReturn, currentLength);

		strcat(toReturn, tempEvJSON);
		strcat(toReturn, ",");
		currentLength += 1;

		free(tempEvJSON);
	}

	// There will always be a trailing comma at the very end of the string
	toReturn[currentLength-1] = ']';

	notifyMsg("\tJSON created: \"%s\"\n", toReturn);

	return toReturn;
}

/** Function to converting a Calendar into a JSON string
 *@pre Calendar is not NULL
 *@post Calendar has not been modified in any way
 *@return A string in JSON format
 *@param cal - a pointer to a Calendar struct
 **/
char* calendarToJSON(const Calendar* cal) {
	char *toReturn;
	int written;
	debugMsg("----START calendarToJSON()----\n");

	if (cal == NULL) {
		toReturn = malloc(3);
		strcpy(toReturn, "{}");
		notifyMsg("\tCalendar is null, returning \"%s\"\n", toReturn);
		return toReturn;
	}

	char *temp = printCalendar(cal);
	debugMsg("\tCalendar passed: \"%s\"\n", temp);
	free(temp);

	toReturn = malloc(2000);
	written = snprintf(toReturn, 2000, "{\"version\":%d,\"prodID\":\"%s\",\"numProps\":%d,\"numEvents\":%d}", \
	                   (int)cal->version, cal->prodID, getLength(cal->properties) + 2, getLength(cal->events));

	notifyMsg("\tJSON created: \"%s\"\n", toReturn);
	return realloc(toReturn, written + 1);
}

/** Function to converting a JSON string into a Calendar struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and partially initialized Calendar struct
 *@param str - a pointer to a string
 **/
Calendar* JSONtoCalendar(const char* str) {
	debugMsg("-----START JSONtoCalendar()-----\n");
	if (str == NULL) {
		notifyMsg("\tJSON string is NULL, returning NULL\n");
		return NULL;
	}

	debugMsg("\tJSON string passed: \"%s\"\n", str);
	Calendar *toReturn;
	if (initializeCalendar(&toReturn) != OK) {
		errorMsg("\tSomething bad happened in initializeCalendar(), returning NULL\n");
		return NULL;
	}

	// The JSON string 'str' is similar to the JSON string created by calendarToJSON(),
	// except it lacks the list info.
	if (sscanf(str, "{\"version\":%f,\"prodID\":\"%999[^\"]\"}", &(toReturn->version), toReturn->prodID) < 2) {
		errorMsg("\tUnable to parse the JSON for some reason. Returning NULL\n");
		return NULL;
	}

	debugMsg("\tSuccessfully created a Calendar object\n");
	debugMsg("\t-----END JSONtoCalendar()-----\n");
	return toReturn;
}

/** Function to converting a JSON string into an Event struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and partially initialized Event struct
 *@param str - a pointer to a string
 **/
Event* JSONtoEvent(const char* str) {
	debugMsg("-----START JSONtoEvent()-----\n");
	if (str == NULL) {
		notifyMsg("\tPassed string is NULL, returning NULL\n");
		return NULL;
	}

	debugMsg("\tJSON string passed: \"%s\"\n", str);

	Event *toReturn;
	if (initializeEvent(&toReturn) != OK) {
		errorMsg("\tSomething happened in initializeEvent(), returning NULL\n");
		return NULL;
	}

	// The JSON string 'str' contains only a "UID" field
	if (sscanf(str, "{\"UID\":\"%999[^\"]\"}", toReturn->UID) < 1) {
		errorMsg("\tCould not correctly parse the JSON string, returning NULL\n");
		return NULL;
	}

	char *temp = printEvent(toReturn);
	notifyMsg("\tSuccessfully parsed the JSON into an Event object: \"%s\"\n", temp);
	free(temp);
	return toReturn;
}

/** Function to adding an Event struct to an ixisting Calendar struct
 *@pre arguments are not NULL
 *@post The new event has been added to the calendar's events list
 *@return N/A
 *@param cal - a Calendar struct
 *@param toBeAdded - an Event struct
 **/
void addEvent(Calendar* cal, Event* toBeAdded) {
	if (cal == NULL || toBeAdded == NULL) {
		return;
	}

	// The event must be specifically added to the END of the events list
	insertBack(cal->events, toBeAdded);
}

// ************* List helper functions - MUST be implemented ***************

/*
 */
void deleteEvent(void* toBeDeleted) {
    if (!toBeDeleted) {
        return;
    }

    Event *ev = (Event *)toBeDeleted;

	if (ev->properties) {
		freeList(ev->properties);
	}

	if (ev->alarms) {
		freeList(ev->alarms);
	}

    free(ev);
}

/*
 */
int compareEvents(const void* first, const void* second) {
    Event *e1 = (Event *)first;
    Event *e2 = (Event *)second;

    return strcmp(e1->UID, e2->UID);
}

/*
 */
char* printEvent(void* toBePrinted) {
    if (!toBePrinted) {
        return NULL;
    }

    Event *ev = (Event *)toBePrinted;

    // DateTime's and Lists have their own print functions
    char *createStr = printDate(&(ev->creationDateTime));
    char *startStr = printDate(&(ev->startDateTime));
    char *propsStr = toString(ev->properties);
    char *alarmsStr = toString(ev->alarms);

    int length = strlen(createStr) + strlen(startStr) + strlen(propsStr) + strlen(alarmsStr) + 200;
    char *toReturn = malloc(length);

    snprintf(toReturn, length, "Start EVENT {EventUID: \"%s\", EventCreate: \"%s\", EventStart: \"%s\", EVENT_PROPERTIES: {%s\n} End EVENT_PROPERTIES, Start EVENT_ALARMS: {%s\n} End EVENT_ALARMS} End EVENT", \
             ev->UID, createStr, startStr, propsStr, alarmsStr);

    // Free dynamically allocated print strings
    free(createStr);
    free(startStr);
    free(propsStr);
    free(alarmsStr);

    return realloc(toReturn, strlen(toReturn) + 1);
}


/*
 */
void deleteAlarm(void* toBeDeleted) {
    if (!toBeDeleted) {
        return;
    }

    Alarm *al = (Alarm *)toBeDeleted;

	if (al->trigger) {
		free(al->trigger);
	}

	if (al->properties) {
		freeList(al->properties);
	}

    free(al);
}

/*
 * Compares the 'action' properties of two alarms.
 */
int compareAlarms(const void* first, const void* second) {
    Alarm *a1 = (Alarm *)first;
    Alarm *a2 = (Alarm *)second;

    return strcmp(a1->action, a2->action);
}

/*
 */
char* printAlarm(void* toBePrinted) {
    if (!toBePrinted) {
        return NULL;
    }

    Alarm *al = (Alarm *)toBePrinted;

    // Lists have their own print function
    char *props = toString(al->properties);

    int length = strlen(al->action) + strlen(al->trigger) + strlen(props) + 200;
    char *toReturn = malloc(length);

    snprintf(toReturn, length, "Start ALARM {AlarmAction: \"%s\", AlarmTrigger: \"%s\", Start ALARM_PROPERTIES: {%s\n} End ALARM_PROPERTIES} End ALARM", \
             al->action, al->trigger, props);

    // Free dynamically allocated print string
    free(props);

    return realloc(toReturn, strlen(toReturn) + 1);
}


/*
 */
void deleteProperty(void* toBeDeleted) {
    // Properties are alloc'd in one block and none of their
    // members needs to be freed in any special way, so no
    // type casting or calling other functions is necessary
    if (toBeDeleted) {
        free(toBeDeleted);
    }
}

/*
 * Compares the names of two properties, then the values if their names are the same.
 */
int compareProperties(const void* first, const void* second) {
    Property *p1 = (Property *)first;
    Property *p2 = (Property *)second;
	int toReturn;

    if ((toReturn = strcmp(p1->propName, p2->propName)) == 0) {
		toReturn = strcmp(p1->propDescr, p2->propDescr);
	}

	return toReturn;
}

/*
 */
char* printProperty(void* toBePrinted) {
    if (!toBePrinted) {
        return NULL;
    }

    Property *prop = (Property *)toBePrinted;

    int length = strlen(prop->propDescr) + 150;
    char *toReturn = malloc(length);

    snprintf(toReturn, length, "Start PROPERTY {PropName: \"%s\", PropDescr: \"%s\"} End PROPERTY", prop->propName, prop->propDescr);

    return realloc(toReturn, strlen(toReturn) + 1);
}


/*
 */
void deleteDate(void* toBeDeleted) {
    // DateTimes do not have to be dynamically allocated (no structures use DT pointers,
    // and DT's are always a set size), so I'm honestly not entirely sure why this function is even here.
    return;
}

/*
 * Compare dates, then times if the dates are the same, then UTC values
 * if the times are the same as well.
 */
int compareDates(const void* first, const void* second) {
    DateTime *dt1 = (DateTime *)first;
    DateTime *dt2 = (DateTime *)second;
    int cmp;

    // if dates are the same, then compare times instead
    if ((cmp = strcmp(dt1->date, dt2->date)) == 0) {
        // if times are also the same, then compare UTC instead
        if ((cmp = strcmp(dt1->time, dt2->time)) == 0) {
            return dt1->UTC - dt2->UTC;
        } // if times are not the same, then return the time comparison below
    } // if dates are not the same, then return the date comparison below

    return cmp;
}

/*
 */
char* printDate(void* toBePrinted) {
    if (!toBePrinted) {
        return NULL;
    }

    DateTime *dt = (DateTime *)toBePrinted;

    int length = 150;
    char *toReturn = malloc(length);

    snprintf(toReturn, length, "Start DATE_TIME {Date (YYYYMMDD): \"%s\", Time (HHMMSS): \"%s\", UTC?: %s} End DATE_TIME", \
             dt->date, dt->time, (dt->UTC) ? "Yes" : "No");

    return realloc(toReturn, strlen(toReturn) + 1);
}

