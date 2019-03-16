#include "CalendarParser.h"
#include "LinkedListAPI.h"

int main() {
	char *print;

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

	fromJ = JSONtoProperty(propJ2);
	print = printProperty(fromJ);
	printf("Property retrieved from \"%s\": \"%s\"\n", propJ2, print);
	free(print);
	insertBack(props, fromJ);

	fromJ = JSONtoProperty(propJ3);
	print = printProperty(fromJ);
	printf("Property retrieved from \"%s\": \"%s\"\n", propJ3, print);
	free(print);
	insertBack(props, fromJ);

	printf("\nPROPERTY LIST\n");
	char *json = propertyListToJSON(props);
	print = toString(props);
	printf("\nProperty List: \"%s\"\n", print);
	printf("\nJSON: \"%s\"\n", json);
	free(print);
	freeList(props);
	free(json);

	printf("\nALARMS\n");
	List *alarms = initializeList(printAlarm, deleteAlarm, compareAlarms);
	char alarmJ1[] = "{\"action\":\"DISPLAY\",\"trigger\":\"WHEN I TELL YOU TO WAKE TF UP\"}";
	char alarmJ2[] = "{\"action\":\"EMAIL\",\"trigger\":\"IT'S MEETING TIME\"}";

	Alarm *aFromJ = JSONtoAlarm(alarmJ1);
	print = printAlarm(aFromJ);
	printf("Alarm received from \"%s\": \"%s\"\n", alarmJ1, print);
	free(print);
	insertBack(alarms, aFromJ);

	aFromJ = JSONtoAlarm(alarmJ2);
	print = printAlarm(aFromJ);
	printf("Alarm received from \"%s\": \"%s\"\n", alarmJ2, print);
	free(print);
	insertBack(alarms, aFromJ);

	printf("\nALARM LIST\n");
	json = alarmListToJSON(alarms);
	print = toString(alarms);
	printf("\nAlarm list: \"%s\"\n", print);
	printf("\nJSON: \"%s\"\n", json);
	free(print);
	freeList(alarms);
	free(json);
	
	return 0;
}
