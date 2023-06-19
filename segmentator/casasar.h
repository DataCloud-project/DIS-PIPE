#ifndef CASASAR_H
#define CASASAR_H

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
const string serverName = "192.168.2.2";
const string arChannel = "tag";
const string rawEventChannel = "rawevents";
const string arUserName = "server";
const string arPassword = "server";
const string arManager = "";
Casas *cbot = NULL;                            // Interface to CASAS middleware

void InitMonitorInterface();
void HandleRawEvent(map<string, string>);
void HandleAR(map<string, string>);
void PrintPacket(map<string, string>);

#endif // CASASAR_H
