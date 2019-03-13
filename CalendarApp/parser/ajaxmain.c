#include "AJAXstub.h"

int main() {
	printf("fake test:\n");
	char *f = fakeText(false, 1);
	printf("%s\n", f);
	free(f);

	f = fakeText(true, 3);
	printf("%s\n", f);
	free(f);

	f = fakeText(true, 6);
	printf("%s\n", f);
	free(f);

	printf("\nfake DT:\n");
	f = fakeDT();
	printf("%s\n", f);
	free(f);

	f = fakeDT();
	printf("%s\n", f);
	free(f);

	f = fakeDT();
	printf("%s\n", f);
	free(f);

	printf("\nfake Alarm:\n");
	f = fakeAlarm();
	printf("%s\n", f);
	free(f);

	f = fakeAlarm();
	printf("%s\n", f);
	free(f);

	f = fakeAlarm();
	printf("%s\n", f);
	free(f);

	printf("\nfake Event:\n");
	f = fakeEvent();
	printf("%s\n", f);
	free(f);

	f = fakeEvent();
	printf("%s\n", f);
	free(f);

	f = fakeEvent();
	printf("%s\n", f);
	free(f);

	printf("\nfake Calendar:\n");
	f = fakeCal();
	printf("%s\n", f);
	free(f);

	f = fakeCal();
	printf("%s\n", f);
	free(f);

	f = fakeCal();
	printf("%s\n", f);
	free(f);

	return 0;
}
