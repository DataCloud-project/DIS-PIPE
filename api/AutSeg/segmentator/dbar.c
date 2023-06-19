#include "dbar.h"


void DB_Init()
{
   char alabel[MAXSTR];
   int i;

   // Set configuration string.
   conninfo = "dbname=casasdb";

   // Make a connection to the database.
   conn = PQconnectdb(conninfo);

   // Check to see that connection was made.
   if (PQstatus(conn) != CONNECTION_OK)
   {
      fprintf(stderr,
         "Connection to database failed: %s", PQerrorMessage(conn));
      exit(1);
   }

   if (TranslationFileName == NULL)
      TranslationFileName = (char *) malloc(MAXSTR * sizeof(char));
   if (TranslationFileName == NULL)
      MemoryError("DB_Init");
   sprintf(TranslationFileName, "%s%s/translate", PREFIX, sitename);
   ReadTranslations();
   if (OutputLevel > 1)
      PrintTranslations();
   SetFeatureNames();
   globaldt = LoadModel();
   tempdata = (float *) malloc(NumDiscreteFeatures * sizeof(float));
   if (tempdata == NULL)
      MemoryError("DB_Init");
   for (i=0; i<NumDiscreteFeatures; i++)
      tempdata[i] = 0;
   strcpy(alabel, "Other_Activity");
   other = FindActivity(alabel);
   strcpy(testdate, "");
}


void DB_LabelData()
{
   // Variables associated with the DB query
   FILE *ftimestamp;
   char *query=NULL, *alabel=NULL, *laststamp=NULL, *lastdate=NULL;
   char *lasttime=NULL, *tstbdname=NULL, *activityname=NULL;
   char *sensorid=NULL, *eventid=NULL, *datasetid=NULL, **labelids=NULL;
   int nFields, small;
   PGresult *ores, *ires;

   // AR variables
   char prevdate[MAXSTR], date[MAXSTR], stime[MAXSTR], tsname[MAXSTR];
   int i, j, end, snum, snum1, snum2, cval;
   float *tempdata;

   // Allocate memory for the query string.
   query = (char *) malloc(300 * sizeof(char));
   laststamp = (char *) malloc(100 * sizeof(char));
   lastdate = (char *) malloc(100 * sizeof(char));
   lasttime = (char *) malloc(100 * sizeof(char));
   tstbdname = (char *) malloc(MAXSTR * sizeof(char));
   sensorid = (char *) malloc(MAXSTR * sizeof(char));
   datasetid = (char *) malloc(MAXSTR * sizeof(char));
   eventid = (char *) malloc(MAXSTR * sizeof(char));
   activityname = (char *) malloc(MAXSTR * sizeof(char));
   alabel = (char *) malloc(MAXSTR * sizeof(char));

   // copy the apartment name
   strcpy(tstbdname, sitename);

   // copy the last time stamp;
   sprintf(tsname, "%s%s/ts", PREFIX, sitename);

   ftimestamp = fopen(tsname, "r");
   if (ftimestamp == NULL)
   {
      printf("Time stamp file %s did not open\n", tsname);
      exit(0);
   }
   fscanf(ftimestamp, "%s %s", lastdate, lasttime);
   sprintf(laststamp, "%s %s", lastdate, lasttime);
   fclose(ftimestamp);

   labelids = (char **) malloc(NumActivities * sizeof(char *));
   // Read the label table and obtain the label ids for all the activities
   for (i=0; i<NumActivities; i++)
   {
      // create the query string to obtain the label id
      sprintf(query,
         "SELECT labelid FROM label WHERE name='%s';", activitynames[i]);
      // Execute the query
      ores = PQexec(conn, query);
      if (ores == NULL)
      {
         printf("Query: %s did not execute\n", query);
         PQfinish(conn);
         exit(1);
      }

      // copy the labelid associated with a activityname
      labelids[i] = (char *) malloc(MAXSTR * sizeof(char));
      if (PQntuples(ores) > 0)
         strcpy(labelids[i], PQgetvalue(ores, 0, 0));
      PQclear(ores);
   }

   // Read the dataset id associated with this experiment and dataset
   sprintf(query, "SELECT data_set_id from data_set WHERE experid=8 and name='2011-07-19-%s'", tstbdname);
   ores = PQexec(conn, query);
   if (ores == NULL)
   {
      printf("Query : %s did not execute\n", query);
      PQfinish(conn);
      exit(1);
   }
   else
   {
      if (PQntuples(ores) > 0)
         strcpy(datasetid, PQgetvalue(ores, 0, 0));
      PQclear(ores);
   }

   // Check to see if the database has been updated
   sprintf(query, "SELECT stamp, location, eventid FROM all_events WHERE tbname='%s' and stamp>'%s' and type in ('Control4-Motion','Control4-MotionArea','Control4-Door');", tstbdname, laststamp);// Removed 'Control4-Light'
   ores = PQexec(conn, query);

   tempdata = (float *) malloc(NumDiscreteFeatures * sizeof(float));
   if (tempdata == NULL)
   {
      fprintf(stderr, "Memory error\n");
      exit(1);
   }

   if (ores == NULL)
      printf("Query: %s did not execute\n", query);
   else
   {
      strcpy(alabel, "Other_Activity");
      other = FindActivity(alabel);

      nFields = PQnfields(ores);
      // Print out the rows
      for (i=0; i<PQntuples(ores); i++)
      {
         // Copy the contents of the new record into the local variables
         strcpy(laststamp, PQgetvalue(ores, i, 0));
         strcpy(sensorid, PQgetvalue(ores, i, 1));
         strcpy(eventid, PQgetvalue(ores, i, 2));

         // Parse the time stamp to extract date, hour, minutes and seconds
         sscanf(laststamp, "%s %s", date, stime);
         strcpy(prevdate, date);
         if (strcmp(prevdate, date) != 0) // new day
            prevdays += SECONDSINADAY;
         currenttime = ComputeSeconds(stime);
         snum = FindTranslation(sensorid);
         snum1 = FindSensor(Translations[snum].trans1);
         snum2 = FindSensor(Translations[snum].trans2);

         for (j=0; j<NumDiscreteFeatures; j++) // initialize the temp data
            tempdata[j] = 0;

         if ((snum1 != -1) && (snum2 != -1))
         {
            // last time sensor fired
            sensortimes[snum1] = currenttime + prevdays;
            sensortimes[snum2] = currenttime + prevdays;
            end = ComputeFeature(stime, Translations[snum].trans1,
	             Translations[snum].trans2, tempdata);
	    small = FALSE;
            cval = DTClassify(tempdata, globaldt, AR_MINNUMCLASS, &small);
	    if (small == TRUE)
	       cval = other;
         }
         else cval = other; 
         
         strcpy(activityname, labelids[(int) cval]);
   
         // last time activity occurred
         atimes[cval] = currenttime + prevdays;
            // Create the query to insert the tag/labelid into the database
         sprintf(query, "INSERT INTO tag(eventid, labelid, created_by, inserted_when, data_set_id, value, start_version) VALUES(%s,%s,'Bosch Baseline',NOW(),%s,NULL,1);\n", eventid, activityname, datasetid);
         printf("%s\n", query);
         // Execute the insert query
         //ires = PQexec(conn, query);
         printf("%s %s %d (%s) %s\n", laststamp, sensorid, cval, activitynames[cval], activityname);
      }
      if (PQntuples(ores) == 0)
         printf("Query: %s did not execute\n", query);
      
      // PQclear PGresult when it is no longer needed to avoid memory leaks
      PQclear(ores);
   }

   // Close the connection to the database and cleanup.
   PQfinish(conn);
   ftimestamp = fopen(tsname, "w");
   fprintf(ftimestamp, "%s\n", laststamp);
   fclose(ftimestamp);
   
   // free all memory
   free(laststamp);
   free(lasttime);
   free(lastdate);
   free(tstbdname);
   free(sensorid);
   free(eventid);
   free(tempdata);
   FreeMemory();
}
