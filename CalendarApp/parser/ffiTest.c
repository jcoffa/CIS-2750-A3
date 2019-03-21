#include "ffiCalendar.h"

int main() {
	char caljson[] = "{\"version\":2,\"prodID\":\"-//Joseph Coffa/CIS*2750 iCalendar File Manager V1.0//EN\",\"numProps\":2,\"numEvents\":1,\"properties\":[],\"events\":[]}";
	char eventjson[] = "{\"startDT\":{\"date\":\"19990308\",\"time\":\"164500\",\"isUTC\":false},\"createDT\":{\"date\":\"20190320\",\"time\":\"155920\",\"isUTC\":false},\"UID\":\"NULL\",\"numProps\":4,\"numAlarms\":0,\"summary\":\"birth\",\"properties\":[],\"alarms\":[]}";

	char *returned = writeCalFromJSON("testOut.ics", caljson, eventjson);

	printf("\n\n\"%s\"\n", returned);

	return 0;
}
