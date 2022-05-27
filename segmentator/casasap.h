#ifndef CASASAP_H
#define CASASAP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <float.h>
#include "sys/stat.h"
#include "al.h"
#include "casas.h"

using namespace std;

// CASAS middleware parameters
const string name = "AP_PROMPTER"; // the name of this application that will show up in XMPP messages
const string mobileName = "mobile"; // the "device" attribute of the mobile to send the prompts to
int cid = 0; // the CID value
int reminderID = 0; // the ID for the reminders sent; for now a constant
const string serverName = "192.168.2.2";
const string rawEventChannel = "rawevents";
const string controlChannel = "control";
const string apUserName = "server";
const string apPassword = "server";
const string apManager = "";
Casas *cbotap = NULL;                            // Interface to CASAS middleware
int secondsPromptThreshold = 60*15; // threshold below which the number of seconds until an activity must go before prompting

char eventDate[MAXSTR];
float *fullTempData = NULL;

void InitMonitorInterfaceAP();
void HandleRawEventAP(map<string, string>);
void PrintPacket(map<string, string>);
void PublishCommand(string);
void PublishPrompt(int, char *, time_t);
time_t ConvertStamptoTime(string);

#endif // CASASAP_H
