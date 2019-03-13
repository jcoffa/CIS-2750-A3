/************************************
 *  Name: Joseph Coffa              *
 *  Student #: 1007320              *
 *  Due Date: March 20, 2019        *
 *                                  *
 *  Assignment 3, CIS*2750          *
 *  AJAXstub.c                      *
 ************************************/

#include "AJAXstub.h"

void seedrand() {
	static bool didSeed = false;

	if (!didSeed) {
		srand(time(NULL));
		didSeed = true;
	}
}

void wordgen(char str[], int length) {
	seedrand();

	int i;
	for (i = 0; i < length; i++) {
		str[i] = (rand() % 26) + 'a';
	}
	str[i] = '\0';
}

char *fakeText(bool spaces, int numWords) {
	char *toReturn = malloc(1000);
	char temp[100];
	if (numWords <= 0) {
		strcpy(toReturn, "Number of words was <= 0");
		return toReturn;
	}

	strcpy(toReturn, "");

	for (int i = 0; i < numWords; i++) {
		wordgen(temp, (rand() % 8) + 3);
		strcat(toReturn, temp);
		if (spaces) {
			strcat(toReturn, " ");
		}
	}

	return toReturn;
}

int randdate() {
	seedrand();
	int toReturn = 201900; // year is 2019 always

	toReturn += rand() % 12; // month
	toReturn *= 100;
	toReturn += rand() % 30; // day

	return toReturn;
}

int randtime() {
	seedrand();
	int toReturn = 0;

	toReturn += rand() % 24; // hour
	toReturn *= 100;
	toReturn += rand() % 60; // minute
	toReturn *= 100;
	toReturn += rand() % 60; // second

	return toReturn;
}

char *fakeDT() {
	seedrand();
	char *toReturn = malloc(200);

	int written = snprintf(toReturn, 200, "{\"date\":\"%08d\",\"time\":\"%06d\",\"isUTC\":%s}", \
			               randdate(), randtime(), (rand() % 2) ? "true" : "false");

	return realloc(toReturn, written+1);
}

char *fakeAlarm() {
	seedrand();
	char *toReturn = malloc(2000);

	char *fakeAct = fakeText(true, 2);
	char *fakeTrig = fakeText(false, 1);

	int written = snprintf(toReturn, 2000, "{\"action\":\"%s\",\"trigger\":\"%s\",\"numProps\":%d}",\
		                   fakeAct, fakeTrig, rand() % 10);

	free(fakeAct);
	free(fakeTrig);

	return realloc(toReturn, written+1);
}

char *fakeEvent() {
	seedrand();
	char *toReturn = malloc(2000);

	char *fakeSummary = fakeText(true, 10);

	char *fakeStart = fakeDT();

	int written = snprintf(toReturn, 1000, "{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\"}", \
	              fakeStart, rand() % 10, rand() % 8, fakeSummary);

	free(fakeSummary);
	free(fakeStart);

	return realloc(toReturn, written+1);
}

char *fakeCal() {
	seedrand();
	char *toReturn = malloc(2000);

	char *fakeProdid = fakeText(false, 4);

	int written = snprintf(toReturn, 2000, "{\"version\":2,\"prodID\":\"%s\",\"numProps\":%d,\"numEvents\":%d}",\
			               fakeProdid, rand() % 10, rand() % 8);

	free(fakeProdid);

	return realloc(toReturn, written+1);

}
