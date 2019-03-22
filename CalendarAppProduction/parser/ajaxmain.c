#include "ffiCalendar.h"

int main() {
	printf("fake Text:\n");
	char *f = fakeText(false, 1);
	printf("fakeText(false, 1): %s\n", f);
	free(f);

	f = fakeText(true, 3);
	printf("fakeText(true, 3): %s\n", f);
	free(f);

	f = fakeText(true, 6);
	printf("fakeText(true, 6): %s\n", f);
	free(f);

	f = fakeText(false, 4);
	printf("fakeText(false, 4): %s\n", f);
	free(f);

	printf("\nfake DT:\n");
	f = fakeDT();
	printf("fakeDT(): %s\n", f);
	free(f);

	f = fakeDT();
	printf("fakeDT(): %s\n", f);
	free(f);

	f = fakeDT();
	printf("fakeDT(): %s\n", f);
	free(f);

	printf("\nfake Alarm:\n");
	f = fakeAlarm();
	printf("fakeAlarm(): %s\n", f);
	free(f);

	f = fakeAlarm();
	printf("fakeAlarm(): %s\n", f);
	free(f);

	f = fakeAlarm();
	printf("fakeAlarm(): %s\n", f);
	free(f);

	printf("\nfake Event:\n");
	f = fakeEvent();
	printf("fakeEvent(): %s\n", f);
	free(f);

	f = fakeEvent();
	printf("fakeEvent(): %s\n", f);
	free(f);

	f = fakeEvent();
	printf("fakeEvent(): %s\n", f);
	free(f);

	printf("\nfake Calendar:\n");
	f = fakeCal();
	printf("fakeCal(): %s\n", f);
	free(f);

	f = fakeCal();
	printf("fakeCal(): %s\n", f);
	free(f);

	f = fakeCal();
	printf("fakeCal(): %s\n", f);
	free(f);

	return 0;
}
