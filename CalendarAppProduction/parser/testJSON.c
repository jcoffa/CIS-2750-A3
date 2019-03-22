#include "CalendarParser.h"
#include "LinkedListAPI.h"
#include "Initialize.h"

int main() {
	printf("\n\n-----JSON TO STRUCT-----\n\n");

	char *print;
	char *json;

	char dtJ1[] = "{\"date\":\"20190507\",\"time\":\"121500\",\"isUTC\":false}";
	char dtJ2[] = "{\"date\":\"20170422\",\"time\":\"235959\",\"isUTC\":true}";
	char dtJ3[] = "{\"date\":\"20201225\",\"time\":\"191732\",\"isUTC\":false}";
	char dtJ4[] = "{\"date\":\"20180101\",\"time\":\"000000\",\"isUTC\":true}";

	DateTime dtFromJ1 = JSONtoDT(dtJ1);
	print = printDate(&dtFromJ1);
	json = dtToJSON(dtFromJ1);
	printf("DateTime: \"%s\"\n", print);
	printf("JSON:     \"%s\"\n", json);
	free(print);
	free(json);

	printf("------------------------------------------------------------\n");

	DateTime dtFromJ2 = JSONtoDT(dtJ2);
	json = dtToJSON(dtFromJ2);
	print = printDate(&dtFromJ2);
	printf("DateTime: \"%s\"\n", print);
	printf("JSON:     \"%s\"\n", json);
	free(print);
	free(json);

	printf("------------------------------------------------------------\n");

	DateTime dtFromJ3 = JSONtoDT(dtJ3);
	json = dtToJSON(dtFromJ3);
	print = printDate(&dtFromJ3);
	printf("DateTime: \"%s\"\n", print);
	printf("JSON:     \"%s\"\n", json);
	free(print);
	free(json);

	printf("------------------------------------------------------------\n");

	DateTime dtFromJ4 = JSONtoDT(dtJ4);
	json = dtToJSON(dtFromJ4);
	print = printDate(&dtFromJ4);
	printf("DateTime: \"%s\"\n", print);
	printf("JSON:     \"%s\"\n", json);
	free(print);
	free(json);

	printf("------------------------------------------------------------\n");


	Property *fromJ;
	List *props = initializeList(printProperty, deleteProperty, compareProperties);
	char propJ1[] = "{\"propName\":\"CALSCALE\",\"propDescr\":\"GREGORIAN\"}";
	char propJ2[] = "{\"propName\":\"CALSCALE\",\"propDescr\":\"VALUE=BIGBOIMCGEE;GREGORIAN\"}";
	char propJ3[] = "{\"propName\":\"ORGANIZER\",\"propDescr\":\"CN=\"James Earl Jones\";example@webmail.com\"}";

	fromJ = JSONtoProperty(propJ1);
	print = printProperty(fromJ);
	printf("Property retrieved from \"%s\": \"%s\"\n", propJ1, print);
	free(print);
	insertBack(props, fromJ);

	printf("------------------------------------------------------------\n");

	fromJ = JSONtoProperty(propJ2);
	print = printProperty(fromJ);
	printf("Property retrieved from \"%s\": \"%s\"\n", propJ2, print);
	free(print);
	insertBack(props, fromJ);

	printf("------------------------------------------------------------\n");

	fromJ = JSONtoProperty(propJ3);
	print = printProperty(fromJ);
	printf("Property retrieved from \"%s\": \"%s\"\n", propJ3, print);
	free(print);
	insertBack(props, fromJ);

	printf("------------------------------------------------------------\n");

	printf("\nPROPERTY LIST\n");
	json = propertyListToJSON(props);
	print = toString(props);
	printf("\nProperty List: \"%s\"\n", print);
	printf("\nJSON: \"%s\"\n", json);
	free(print);
	clearList(props);
	free(json);

	printf("------------------------------------------------------------\n");

	printf("\nALARMS\n");
	List *alarms = initializeList(printAlarm, deleteAlarm, compareAlarms);
	char alarmJ1[] = "{\"action\":\"DISPLAY\",\"trigger\":\"WHEN I TELL YOU TO WAKE TF UP\"}";
	char alarmJ2[] = "{\"action\":\"EMAIL\",\"trigger\":\"IT'S MEETING TIME\"}";

	Alarm *aFromJ = JSONtoAlarm(alarmJ1);
	print = printAlarm(aFromJ);
	printf("Alarm received from \"%s\": \"%s\"\n", alarmJ1, print);
	free(print);
	insertBack(alarms, aFromJ);

	printf("------------------------------------------------------------\n");

	aFromJ = JSONtoAlarm(alarmJ2);
	print = printAlarm(aFromJ);
	printf("Alarm received from \"%s\": \"%s\"\n", alarmJ2, print);
	free(print);
	insertBack(alarms, aFromJ);

	printf("------------------------------------------------------------\n");

	printf("\nALARM LIST\n");
	json = alarmListToJSON(alarms);
	print = toString(alarms);
	printf("\nAlarm list: \"%s\"\n", print);
	printf("\nJSON: \"%s\"\n", json);
	free(print);
	clearList(alarms);
	free(json);

	printf("------------------------------------------------------------\n");

	printf("\n\n-----STRUCT TO JSON-----\n\n");
	Property *propToJ1;
	initializeProperty("ORGANIZER;CN=John Doe:example@webmail.com", &propToJ1);
	json = propertyToJSON(propToJ1);
	print = printProperty(propToJ1);
	printf("Property: \"%s\"\n", print);
	printf("JSON:      \"%s\"\n\n", json);
	free(print);
	free(json);
	insertBack(props, propToJ1);

	printf("------------------------------------------------------------\n");

	Property *propToJ2;
	initializeProperty("CALSCALE:GREGORIAN", &propToJ2);
	json = propertyToJSON(propToJ2);
	print = printProperty(propToJ2);
	printf("Property: \"%s\"\n", print);
	printf("JSON:      \"%s\"\n\n", json);
	free(print);
	free(json);
	insertBack(props, propToJ2);

	printf("------------------------------------------------------------\n");

	Property *propToJ3;
	initializeProperty("THE_PROP_NAME:THE_PROPERTY_VALUE_IS_JUST_SLIGHTLY_LONGER_THAN_YOU_WOULD_OTHERWISE_ENCOUNTER", &propToJ3);
	json = propertyToJSON(propToJ3);
	print = printProperty(propToJ3);
	printf("Property: \"%s\"\n", print);
	printf("JSON:      \"%s\"\n\n", json);
	free(print);
	free(json);
	insertBack(props, propToJ3);

	printf("------------------------------------------------------------\n");

	json = propertyListToJSON(props);
	print = toString(props);
	printf("Property List: \"%s\"\n", print);
	printf("JSON: \"%s\"\n", json);
	free(print);
	free(json);

	printf("------------------------------------------------------------\n");

	Alarm *alarmToJ1;
	initializeAlarm(&alarmToJ1);
	strcpy(alarmToJ1->action, "DISPLAY");
	alarmToJ1->trigger = malloc(10);
	strcpy(alarmToJ1->trigger, "bigboiyay");
	freeList(alarmToJ1->properties);
	alarmToJ1->properties = props;
	json = alarmToJSON(alarmToJ1);
	print = printAlarm(alarmToJ1);
	printf("Alarm: \"%s\"\n", print);
	printf("JSON:  \"%s\"\n\n", json);
	free(json);
	free(print);
	insertBack(alarms, alarmToJ1);

	printf("------------------------------------------------------------\n");

	Alarm *alarmToJ2;
	initializeAlarm(&alarmToJ2);
	strcpy(alarmToJ2->action, "AUDIO");
	alarmToJ2->trigger = malloc(25);
	strcpy(alarmToJ2->trigger, "imma beat yo ass fool");
	json = alarmToJSON(alarmToJ2);
	print = printAlarm(alarmToJ2);
	printf("Alarm: \"%s\"\n", print);
	printf("JSON:  \"%s\"\n\n", json);
	free(json);
	free(print);
	insertBack(alarms, alarmToJ2);

	printf("------------------------------------------------------------\n");

	List *events = initializeList(printEvent, deleteEvent, compareEvents);

	Event *eventToJ1;
	initializeEvent(&eventToJ1);
	strcpy(eventToJ1->UID, "HOt-Dog-Hot-Dog-Hot-Diggity-Dog");
	eventToJ1->creationDateTime = dtFromJ1;
	eventToJ1->startDateTime = dtFromJ2;
	freeList(eventToJ1->alarms);
	eventToJ1->alarms = alarms;
	freeList(eventToJ1->properties);
	eventToJ1->properties = props;
	json = eventToJSON(eventToJ1);
	print = printEvent(eventToJ1);
	printf("Event: \"%s\"\n\n", print);
	printf("JSON:  \"%s\"\n\n", json);
	free(json);
	free(print);
	insertBack(events, eventToJ1);

	printf("------------------------------------------------------------\n");

	Event *eventToJ2;
	initializeEvent(&eventToJ2);
	strcpy(eventToJ2->UID, "Badda Bing, Badda Boom");
	eventToJ2->creationDateTime = dtFromJ3;
	eventToJ2->startDateTime = dtFromJ4;
	json = eventToJSON(eventToJ2);
	print = printEvent(eventToJ2);
	printf("Event: \"%s\"\n\n", print);
	printf("JSON:  \"%s\"\n\n", json);
	free(json);
	free(print);
	insertBack(events, eventToJ2);

	printf("------------------------------------------------------------\n");

	json = eventListToJSON(events);
	print = toString(events);
	printf("Event List: \"%s\"\n\n", print);
	printf("\nJSON: \"%s\"\n\n", json);
	free(json);
	free(print);


	printf("\n\n------------------------------------------------------------\n");
	printf("JSONtoEvent()\n\n");
	char testEventJ[] = "{\"startDT\":{\"date\":\"20190308\",\"time\":\"164500\",\"isUTC\":false},\"createDT\":{\"date\":\"20190308\",\"time\":\"164500\",\"isUTC\":false},\"UID\":\"NULL\",\"numProps\":3,\"numAlarms\":0,\"summary\":\"NULL\",\"properties\":[this isnt empty],\"alarms\":[this also isnt empty]}";
	Event *testEvent = JSONtoEvent(testEventJ);
	
	print = printEvent(testEvent);
	printf("JSON: \"%s\"\n", testEventJ);
	printf("\nEvent created: \"%s\"\n", print);
	free(print);
	deleteEvent(testEvent);
	
	return 0;
}
