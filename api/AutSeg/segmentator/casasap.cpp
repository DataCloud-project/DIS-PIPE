#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include "casas.h"
#include "connection.h"
#include "casasap.h"

//Process CASAS sensor events and form a prediction for the desired activity
void AP_RealtimePrediction()
{
	char alabel[MAXSTR];

	// Perform prompting setup steps:
	// Setup the translations file
	if (TranslationFileName != NULL)
	{
		ReadTranslations();
		if (OutputLevel > 1)
			PrintTranslations();
	}

	if ((factwind == NULL) || (fsenact == NULL))
		MemoryError("factwind, fsenact need to be provided for prompting");

	// Other activity only set for AP, not AR, because calling FindActivity(A)
	// will add A to the list of global activities if not already there, and we
	// do not want that for AR.
	strcpy(alabel, "Other_Activity");
	other = FindActivity(alabel);
	NumDiscreteFeatures += 1 + (2*MAXLAG);
	NumFeatures += 1 + (2*MAXLAG);
	SetFeatureNames();

	// Allocate the tempdata array:
	tempdata = (float *) malloc(NumDiscreteFeatures * sizeof(float));
	if (tempdata == NULL)
		MemoryError("AP_RealtimePrediction");

	// If the sampling mode is on, initialize the state vector array
	if (SamplingMode == SAMPLE) {
		InitializeSampleVectors();
		tempdataSamples = (float *) malloc(NumSampleFeatures * sizeof(float));
		if (tempdataSamples == NULL)
			MemoryError("AP_RealtimePrediction");
	}

	// Allocate the overall data array (used for combining discrete and sample data:
	fullTempData = (float *) malloc(NumFeatures * sizeof(float));
	if (fullTempData == NULL)
		MemoryError("AP_RealtimePrediction");

	// Allocate the timestamp array
	// We will only keep the current and previous MAXLAG timestamp values
	timestamp = (int *) malloc((MAXLAG + 1) * sizeof(int));
	if (timestamp == NULL)
		MemoryError("AP_Realtime_Prediction");

	// Set up the date to be blank for starters
	strcpy(eventDate, "");

	// Set up the model:
	// TODO: Allow more than one activity model?
	globaldt = LoadModel();
    if (globaldt != NULL) // Assign model numbers to leaves
       AP_AssignModelNumbers(globaldt, 0);

    printf("About to init interface");
    // Initialize and connect to CASAS middleware
    InitMonitorInterfaceAP();
    cbotap->connect();

    FreeMemory();
}

// Set up a connection with the CASAS middleware
void InitMonitorInterfaceAP()
{
   string userName = apUserName + "@" + serverName + "/onlyone";

   // Create a new CASAS object for AR
   cbotap = new Casas("AP", userName, apPassword);

   // Subscribe to rawevents channel
   cbotap->addSubscribeChannel(rawEventChannel);
   cbotap->setChanMsgCallback(rawEventChannel, HandleRawEventAP);

   // Publish to the control channel:
   cbotap->addPublishChannel(controlChannel);
}

void HandleRawEventAP(map<string, string> packet)
{
	char value[MAXSTR];
	char eventTime[MAXSTR];
	char sensorId1[MAXSTR];
	char sensorId2[MAXSTR];
	char sensorStatus[MAXSTR];
	int transIndex;
	int snum1, snum2;
	char prevEventDate[MAXSTR];
	int i, j, end, offset, timediff;
	float ts, deltatime;
	float secondsToActivity;
	int timeSinceStart;

	if (OutputLevel > 0)
	{
		cout << "AR received rawevent message:\n";
		PrintPacket(packet);
	}

	// Save the previous date:
	strcpy(prevEventDate, eventDate);

	// Get the sesnor event information:
	strcpy(value, packet["stamp"].c_str());
	sscanf(value, "%s %s", eventDate, eventTime);
	strcpy(value, packet["location"].c_str());
	sscanf(value, "%s", sensorId1);
	strcpy(value, packet["message"].c_str());
	sscanf(value, "%s", sensorStatus);

	// Look for the sensor translation:
	transIndex = FindTranslation(sensorId1);
	if (transIndex < 0)
	{
		// No translation found, just copy sensorid to sensorid2
		strcpy(sensorId2, sensorId1);
	}
	else
	{
		strcpy(sensorId1, Translations[transIndex].trans1);
		strcpy(sensorId2, Translations[transIndex].trans2);
	}

	// Try to get the sensor numbers:
    snum1 = FindSensor(sensorId1);
    snum2 = FindSensor(sensorId2);

    // Check if the location is a recognized sensor:
    if (snum1 == -1 || snum2 == -1) {
    	printf("Did not recognize sensor: %s\n", sensorId1);
    } else {
    	printf("Sensor found - processing\n");

    	// Add to the number of previous days if we're on a new day:
    	if ((strcmp(prevEventDate, eventDate) !=0) && (NumData >0))
    		prevdays += SECONDSINADAY; //days (in seconds) since the beginning

    	// Compute the current time and set the sensor last time fired values:
    	currenttime = ComputeSeconds(eventTime);
        sensortimes[snum1] = currenttime + prevdays;   // last time sensor fired
        sensortimes[snum2] = currenttime + prevdays;   // last time sensor fired

        // Compute the feature vector for the event:
        if (strcmp(sensorId1, "Ignore") != 0) {
			if (SamplingMode == SAMPLE) {
				// Save the time of the first event received (for sampling purposes, different than firsttime above):
				if (firstEventSamplingFlag == FALSE) {
					firstTimeSampling = ConvertTime(eventDate, eventTime);
					firstEventSamplingFlag = TRUE;
				}

				// Compute the time since the start, and then compute the sampling features
				timeSinceStart = ConvertTime(eventDate, eventTime) - firstTimeSampling;
				if(sensortypes[snum1] != LIGHTLEVEL && sensortypes[snum1] != TEMPERATURE && sensortypes[snum2] != LIGHTLEVEL && sensortypes[snum2] != TEMPERATURE) // if it's not a sampling-only sensor
					end = ComputeFeature(eventTime, sensorId1, sensorId2, tempdata) && ComputeSampleFeatures(timeSinceStart, sensorId1, sensorId2, sensorStatus, tempdataSamples);
				else
					end = ComputeSampleFeatures(timeSinceStart, sensorId1, sensorId2, sensorStatus, tempdataSamples); // only compute the sample features
			} else if(sensortypes[snum1] != LIGHTLEVEL && sensortypes[snum1] != TEMPERATURE && sensortypes[snum2] != LIGHTLEVEL && sensortypes[snum2] != TEMPERATURE) // if it's not a sampling-only sensor
				end = ComputeFeature(eventTime, sensorId1, sensorId2, tempdata); //otherwise, just compute the discrete event features
		}
		else end = 0;

        // If we have sufficient data, begin to process:
        if (end == 1) {
        	printf("Event window full - processing features\n");
        	// Save the normal discrete event and sampling features:
            if (SamplingMode == SAMPLE) {
           	 for (i=0; i < NumDiscreteFeatures; i++) {
           		 fullTempData[i] = tempdata[i];
           	 }
           	 for (i = 0; i < NumSampleFeatures; i++) {
           		fullTempData[i + NumDiscreteFeatures] = tempdataSamples[i];
           	 }
            } else {
           	 for (i=0; i < NumDiscreteFeatures; i++)
           		fullTempData[i] = tempdataSamples[i];
            }

        	if (NumData == 0) //if it's the first data point save the time as the first time
        		firsttime = ConvertTime(eventDate, eventTime);

        	// Save the current event time as the last time
        	lasttime = ConvertTime(eventDate, eventTime);

        	// Calculate the time in seconds from the beginning of the dataset to here
        	timestamp[MAXLAG] = lasttime - firsttime;

        	// Compute the deltatime value:
        	timediff = (int) (lasttime - firsttime);
        	if (NumData > 0) // if we won't divide by zero, process normally
        		deltatime = (float) timediff / (float) NumData;
        	else
        		deltatime = MIN;
        	if (deltatime == 0.0) //handle if deltatime is zero
        		deltatime = MIN;

        	// Now compute the normalized and scaled timestamp values:
        	offset = 8 + (2*NumSensors);
        	ts = ((float) timestamp[MAXLAG]) / deltatime;
        	tempdata[offset] = ts;
        	fullTempData[offset] = ts;

        	for (j=1; j <= MAXLAG; j++) {
        		if ((NumData - j) < 0) { // if we're trying to look back further than we have data
        			tempdata[offset + j] = 0.0; //set the timestamp values to 0
        			fullTempData[offset + j] = 0.0;
        			tempdata[offset + MAXLAG + j] = 0.0;
        			fullTempData[offset + MAXLAG + j] = 0.0;
        		} else { //otherwise, get the timestamp from j events ago
        			tempdata[offset + j] = (float) timestamp[MAXLAG - j] / deltatime;
        			fullTempData[offset + j] = (float) timestamp[MAXLAG - j] / deltatime;
        			tempdata[offset + MAXLAG + j] = ((float) timestamp[MAXLAG - j] / deltatime) * ts;
        			fullTempData[offset + MAXLAG + j] = ((float) timestamp[MAXLAG - j] / deltatime) * ts;
        		}
        	}

        	// Perform the prediction of the time (in seconds) until the next prompt:
        	secondsToActivity = AP_DTClassify(fullTempData, globaldt);
        	printf("Activity %s in %f seconds\n", activitynames[pactivity], secondsToActivity);

        	// Prompt only if the number of seconds until the activty has passed the threshold:
        	if (secondsToActivity <= secondsPromptThreshold) {
        		printf("Below the threshold of %d seconds - prompting\n", secondsPromptThreshold);

				// Compute the epoch timestamp of the activity start time:
				int activityStartTime = atoi(packet["epoch"].c_str()) + secondsToActivity;
				time_t predictedActivityTime = activityStartTime;

				// Publish a prompt for this activity:
				// TODO: Probably need to send multiple prompts at specified intervals, until we see that the activity has occurred or we've reached a specified number of prompts
				// We will need to consider how to define a single predicted activity (which will have related prompts and the same reminderID), as well as
				// a way to define when the activity has been completed...
				char message[MAXSTR];
				sprintf(message, "Activity %s", activitynames[pactivity]);
				PublishPrompt(1, message , predictedActivityTime);
        	}

        	// Drop the first element in the MAXLAG window of timestamps and shift all other elements back by one:
        	for (i = 0; i < MAXLAG; i++) {
        		timestamp[i] = timestamp[i+1];
        	}

        	NumData++; //increment the data count

        } else
        	printf("Events window not yet full - no features computed\n");
    }
}

void PublishPrompt(int reminderID, char *promptMessage, time_t predictedActivityTime) {
	// Send a prompt to the desired location with information about the activity and its predicted time:
	// The reminderID should be the same for all prompts related to the same predicted activity instance

	// Build the value to send:
	stringstream valueStream;
	valueStream << "<reminderID>" << reminderID << "</reminderID>";
	valueStream << "<promptMessage>" << promptMessage << "</promptMessage>";
	valueStream << "<predictedActivityTime>" << predictedActivityTime << "</predictedActivityTime>";
	PublishCommand(valueStream.str());
}

void PublishCommand(string value) {

	// Build the message:
	stringstream messageStream;
	messageStream << "<control><by>" << name << "</by>";
	messageStream << "<command>prompt</command>";
	messageStream << "<value>" << value << "</value>";
	messageStream << "<serial></serial>";
	messageStream << "<location>" << mobileName << "</location>";
	messageStream << "<replyto></replyto>";
	messageStream << "<cid>" << cid << "</cid>";
	messageStream << "</control>";

	// Publish the message
    cbotap->publish(messageStream.str(), controlChannel);
}
