#include <iostream>
#include <string>
#include <map>
#include "casas.h"
#include "connection.h"
#include "casasar.h"


// Process CASAS sensor events and tag them with activity label
void ANN_LabelData()
{
   char alabel[MAXSTR];
   int i;

   if (TranslationFileName != NULL)
   {
      ReadTranslations();
      if (OutputLevel > 1)
         PrintTranslations();
   }
   SetFeatureNames();
   globaldt = LoadModel();
   tempdata = (float *) malloc(NumDiscreteFeatures * sizeof(float));
   if (tempdata == NULL)
      MemoryError("ANN_LabelData");
   for (i=0; i<NumDiscreteFeatures; i++)
      tempdata[i] = 0;
   strcpy(alabel, "Other_Activity"); 
   other = FindActivity(alabel);
   strcpy(testdate, "");
   // Initialize and connect to CASAS middleware
   InitMonitorInterface();
   cbot->connect();

   FreeMemory();
}


// Set up a connection with the CASAS middleware
void InitMonitorInterface()
{
   string userName = arUserName + "@" + serverName + "/onlyone";

   // Create a new CASAS object for AR
   cbot = new Casas("AR", userName, arPassword);

   // Subscribe to rawevents channel
   cbot->addSubscribeChannel(rawEventChannel);
   cbot->setChanMsgCallback(rawEventChannel, HandleRawEvent);

   // Publish to the AR channel so that AR can post events
   cbot->addPublishChannel(arChannel);

   // Subscribe to the AR channel so that the AR events can be printed
   // This is for testing; usually a separate agent would handle AR events
   cbot->addSubscribeChannel(arChannel);
   cbot->setChanMsgCallback(arChannel, HandleAR);
}


void HandleRawEvent(map<string, string> packet)
{
   int temp;
   int activity;
   char value[MAXSTR];
   char eventDate[MAXSTR];
   char eventTime[MAXSTR];
   char sensorId1[MAXSTR];
   char sensorId2[MAXSTR];
   int transIndex;
   int sensorIndex;
   int prevdays=0;
   int small;

   if (OutputLevel > 0)
   {
      cout << "AR received rawevent message:\n";
      PrintPacket(packet);
   }
   // Build event feature vector
   strcpy(value, packet["stamp"].c_str());
   sscanf(value, "%s %s", eventDate, eventTime);
   strcpy(value, packet["location"].c_str());
   sscanf(value, "%s", sensorId1);
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

   strcpy(testprevdate, testdate);
   if (strcmp(testprevdate, testdate) != 0) // new day
      prevdays += SECONDSINADAY;
   currenttime = ComputeSeconds(eventTime);
   if (transIndex == -1 && TranslationFileName != NULL)
   {
      activity = other;
      printf("tag with Other_Activity (did not recognize sensor)\n");
      cbot->tagEvent(packet, "AR", "AR", serverName, activitynames[activity], "", arChannel);
   }
   else
   {
      sensorIndex = FindSensor(sensorId1);
      sensortimes[sensorIndex] = currenttime + prevdays;
      sensorIndex = FindSensor(sensorId2);
      sensortimes[sensorIndex] = currenttime + prevdays;
      if (strcmp(sensorId1, "Ignore") != 0)
         temp = ComputeFeature(eventTime, sensorId1, sensorId2, tempdata);
      else temp = 0;
      if (temp == 1) // sufficient prior data to make a good classification
      {
		small = FALSE;
		activity = DTClassify(tempdata, globaldt, AR_MINNUMCLASS, &small);
		if (small == TRUE)
			activity = other;
		printf("tag with %s\n", activitynames[activity]);
		cbot->tagEvent(packet, "AR", "AR", serverName, activitynames[activity], "", arChannel);
      }
      else
      {
		activity = other;
		if (strcmp(sensorId1, "Ignore") != 0)
			printf("tag with Other_Activity (need to fill buffer)\n");
		else printf("tag with Other_Activity (ignoring sensor)\n");
			cbot->tagEvent(packet, "AR", "AR", serverName, activitynames[activity], "", arChannel);
      }
   }
}


void HandleAR(map<string, string> packet)
{
	if (OutputLevel > 0)
	{
		cout << "AR message received:\n";
		PrintPacket (packet);
	}
}


void PrintPacket(map<string, string> packet)
{
   map<string, string>::iterator it;
   for (it = packet.begin(); it != packet.end(); it++)
   {
      cout << "  " << it->first << " : " << it->second << endl;
   }
}
