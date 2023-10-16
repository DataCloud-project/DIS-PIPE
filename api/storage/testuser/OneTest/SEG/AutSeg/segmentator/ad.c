#include "al.h"

//------------------------------------------------------------------------------
// Written by Diane J. Cook
// Washington State University
// February 27, 2012
//------------------------------------------------------------------------------


// Main AD function.  AD reads an input file containing time-ordered
// sensor data.  Sensor event sequences are discovered and evaluated using
// the MDL principle and the best sequences each iteration are used
// to compress the dataset.  The final set of patterns are clustered
// and the original dataset is output with additional tags added to
// indicate the pattern instances in the data.
void AD()
{
   FILE *inputfile, *annotatefile;
   int i, j, compressed;
   PatternList *patternList = NULL;

   AD_Init(&inputfile, &annotatefile); // Initialize parameters
   AD_ReadData(inputfile);                         // Process and store data
   sim = (int **) malloc(numlabels * sizeof(int *));
   if (sim == NULL)
      MemoryError("AD");
   for (i=0; i<numlabels; i++)  // Initialize mutual information value array
   {
      sim[i] = (int *) malloc(numlabels * sizeof(int));
      if (sim[i] == NULL)
         MemoryError("AD");
      for (j=0; j<numlabels; j++)
         sim[i][j] = 0;
   }

   if (OutputLevel > 0)
   {
      printf("%d events and %d labels in data set\n", nevents, numlabels);
      printf("\nDiscovering patterns...");         // Discover patterns
   }
   origLabels = numlabels;

   // Multiple discovery iterations are performed.  In each iteration the
   // best sequence pattern (according to MDL value) is found and used to
   // compress the original dataset.
   for (iteration=0; iteration<NumIterations || NumIterations==-1; iteration++)
   {
      if (EVALUATE == 0)     // Data size can include or ignore pattern sizes
         datasize = DataSize();
      else datasize = nevents;
      if (NumIterations != 1)
         printf("\n------- Iteration %d, %d events, %d size -------\n",
	        iteration, nevents, datasize);
      patternList = DiscoverSequences();   // Find and evaluate patterns

      if (patternList->head == NULL)
      {
         printf("No sequences found.\n\n");
         break;
      }
      else if ((patternList->head->pattern->numInstances == 1) ||
               (patternList->head->pattern->definition->numEvents == 1))
      {
	 printf("\nNo more compression can be achieved.\t");
	 break;
      }
      else
      {
         printf("\nBest discovered sequence\n\n");
	 PrintPattern(patternList->head->pattern);

	 // Best patterns are stored for later clustering and annotation
	 AddPatternInfo(patternList->head->pattern, iteration);

         // Compress dataset using best pattern
         compressed = CompressData(patternList);
	 if (compressed == 0)       // Done if pattern did not compress dataset
	 {
	    printf("\nNo more compression can be achieved.\n");
	    break;
         }
         FreePatternList(patternList);
         patternList = NULL;
      }

      if (OutputLevel > 1)
      {
         printf("\nCompressed list of %d events\n", nevents);
         for (i=0; i<nevents; i++)
            PrintEvent(events[i]);
      }
   }
   printf("Done.\n");

   printf("\nClustering patterns...");       // Cluster best patterns together
   ClusterPatterns(inputfile);
   printf("Done.\n");

   if (OutputLevel > 1)
   {
      printf("\nFinal list of %d events\n", nevents);
      for (i=0; i<nevents; i++)
         PrintEvent(events[i]);
   }

   printf("\nAnnotating data...");           // Tag data with patterns
   AnnotateData(inputfile, annotatefile);
   printf("Done.\n");
   fclose(inputfile);
   fclose(annotatefile);
}


// The DiscoverSequences function first computes the number of occurrences
// of each sensor type.  Candidate sensor sequence patterns are incrementally
// expanded by adding a prefix or suffix for each instance of the pattern.
// Patterns are evaluated using the MDL value.
PatternList *DiscoverSequences()
{
   PatternList *parentPatList = NULL;
   PatternList *childPatList = NULL;
   PatternList *extendedPatList = NULL;
   PatternList *discoveredPatList = NULL;
   PatternListNode *parentPatListNode = NULL;
   PatternListNode *extendedPatListNode = NULL;
   Pattern *parentPattern = NULL;
   Pattern *extendedPattern = NULL;

   // Seed patterns with individual events
   parentPatList = GetInitialPatterns();
   discoveredPatList = AllocatePatternList();

   while (parentPatList->head != NULL)    // Continue until no more expansions
   {
      parentPatListNode = parentPatList->head;
      childPatList = AllocatePatternList();

      // extend each sequence in the parent list
      while (parentPatListNode != NULL)
      {
	 parentPattern = parentPatListNode->pattern;
	 parentPatListNode->pattern = NULL;
	 if (OutputLevel > 2)
	 {
	    printf("Considering ");
	    PrintPattern(parentPattern);
	    printf("\n");
	 }
	 
	 extendedPatList = ExtendPattern(parentPattern);
	 extendedPatListNode = extendedPatList->head;
	 while (extendedPatListNode != NULL)
	 {
	    extendedPattern = extendedPatListNode->pattern;
	    extendedPatListNode->pattern = NULL;
	    // Evaluate each extension and add to child list
	    EvaluatePattern(extendedPattern);
	                          // Prune patterns with only one instance
	    if (extendedPattern->numInstances == 1)
            {
	       FreePattern(extendedPattern);
               extendedPattern = NULL;
            }
	                          // Prune patterns with value less than parent
	    else if (extendedPattern->value < parentPattern->value)
            {
	       FreePattern(extendedPattern);
               extendedPattern = NULL;
            }
	    else PatternListInsert(extendedPattern, childPatList, BEST);
	    extendedPatListNode = extendedPatListNode->next;
	 }
	 FreePatternList(extendedPatList);
         extendedPatList = NULL;

	 // Add parent sequence to final discovered list
	 PatternListInsert(parentPattern, discoveredPatList, BEST);
         parentPatListNode = parentPatListNode->next;
      }
      FreePatternList(parentPatList);
      parentPatList = NULL;
      parentPatList = childPatList;
   }

   if (OutputLevel > 2)
      printf("Sequence queue empty.\n");

   // Insert remaining sequences from parent list to discovered list
   parentPatListNode = parentPatList->head;
   while (parentPatListNode != NULL)
   {
      parentPattern = parentPatListNode->pattern;
      parentPatListNode->pattern = NULL;
      PatternListInsert(parentPattern, discoveredPatList, BEST);
      parentPatListNode = parentPatListNode->next;
   }
   FreePatternList(parentPatList);
   parentPatList = NULL;

   return(discoveredPatList);
}


// Create an initial list of pattern candidates that are one sensor event
// in length.
PatternList *GetInitialPatterns()
{
   int i, j, *used = NULL, numInitialPatterns, curlabel;
   PatternList *initialPatterns = NULL;
   Seq *s = NULL;
   Pattern *pattern = NULL;
   Instance *instance = NULL;

   used = (int *) malloc(numlabels * sizeof(int));
   if (used == NULL)
      MemoryError("GetInitialPatterns");
   for (i=0; i<numlabels; i++)
      used[i] = FALSE;
   numInitialPatterns = 0;
   initialPatterns = AllocatePatternList();

   for (i=0; i<nevents; i++)         // Consider each individual sensor event
   {
      if ((events[i].copy != PREDEFINED) && (used[events[i].label] == FALSE))
      {
         used[events[i].label] = TRUE;

	 s = AllocateSequence(1);    // Create one-element sequence definition
	 s->events[0] = i;
         pattern = AllocatePattern();
	 pattern->definition = s;
	 pattern->instances = AllocateInstanceList();

	 for (j=0; j<nevents; j++)   // Find instances of the candidate pattern
	 {
	    // Do not include events that are part of a predefined activity
	    if (events[i].copy != PREDEFINED)
	    {
	       curlabel = events[i].label;
	       if (events[j].label == curlabel)  // This is an instance
	       {
	          instance = AllocateInstance(1);
	          instance->events[0] = j;
	          InstanceListInsert(instance, pattern->instances);
	          pattern->numInstances++;
	          if (iteration == 0)   // Record co-occurrence for MI values
	          {
		     if ((i == 0) || (i == (nevents-1)))
	                sim[curlabel][curlabel] += 1;
	             else sim[curlabel][curlabel] += 2;
		     if (i > 0)
		        sim[curlabel][events[i-1].label]++;
		     if (i < (nevents-1))
		        sim[curlabel][events[i+1].label]++;
	          }
	       }
	    }
	 }
	 if (pattern->numInstances > 1)  // Only use events with >1 instance
	 {
            EvaluatePattern(pattern);
	    // Add to initial sequences
	    PatternListInsert(pattern, initialPatterns, 0);
	    numInitialPatterns++;
         }
      }
   }

   if (iteration == 0)     // Compute mutual information scores for event names
      AD_ComputeMI();

   if (OutputLevel > 1)
   {
      printf("%d initial sequences\n", numInitialPatterns);
      if (OutputLevel > 1)
         PrintPatternList(initialPatterns);
   }

   return(initialPatterns);
}


// Initialize parameters and data structures
void AD_Init(FILE **inputfile, FILE **annotatefile)
{
   char afilename[MAXSTR];

   numlabels = 0;
   origLabels = 0;
   adlabels = NULL;
   *inputfile = fopen(filename, "r");
   if (*inputfile == NULL)
   {
      printf("Error reading data file %s\n", filename);
      exit(1);
   }
   sprintf(afilename, "%s.%s", filename, "annotated");
   *annotatefile = fopen(afilename, "w");
   if (*annotatefile == NULL)
   {
      printf("Error reading data file %s\n", afilename);
      exit(1);
   }
   nevents = 0;

   return;
}


// Read the sensor data
// Data line is formatted as date, time, sensor id1, sensor id2, sensor value
// Additional optional columns include activity name and "begin" or "end"
void AD_ReadData(FILE *fp)
{
   char *cptr = NULL, buffer[256], alabel[MAXSTR];
   char date[MAXSTR], time[MAXSTR], sensorid[MAXSTR], sensorvalue[MAXSTR];
   char sensorid2[MAXSTR];
   int dow, sid, svalue, hour;

   cptr = fgets(buffer, 256, fp);       // Read first line of dataset
   while (cptr != NULL)
   {
      sscanf(buffer, "%s %s %s %s %s %s",
         date, time, sensorid, sensorid2, sensorvalue, alabel);

                                       // Process event
      dow = ComputeDow(date);
      hour = ComputeHour(time);

      sid = MapSensors(sensorid);      // Map sensor name to integer value
      if (strcmp(sensorvalue, "OFF") == 0)
         svalue = 0;
      else svalue = 1;

      if (nevents == 0)                // Add event to array
         events = (Event *) malloc(sizeof(Event));
      else events = (Event *) realloc(events, (nevents + 1) * sizeof(Event));
      if (events == NULL)
         MemoryError("ReadData");
      events[nevents].dow = dow;
      events[nevents].time = hour;
      events[nevents].sensorid = sid;
      events[nevents].sensorvalue = svalue;
      events[nevents].label = sid;      // The label is the sensor id
      events[nevents].patsize = 1;      // Initial patterns are size 1
      if ((strcmp(alabel, "none") == 0) ||
          (strcmp(alabel, "Other_Activity") == 0))
         events[nevents].copy = TRUE;
      else events[nevents].copy = PREDEFINED;
           
      events[nevents].begin = events[nevents].end = nevents+1;
      nevents++;

      cptr = fgets(buffer, 256, fp);    // Get next line of dataset
   }
}


// Map sensors to unique numbers
// Integer representation improves simplicity and efficiency
int MapSensors(char *sistr)
{
   int i;

   for (i=0; i<numlabels; i++) // If label in table, return corresponding index
      if (strcmp(adlabels[i], sistr) == 0)
         return(i);

   if (numlabels == 0)         // Did not find sensor label, add a new mapping
      adlabels = (char **) malloc(sizeof(char *));
   else adlabels = (char **) realloc(adlabels, (numlabels+1) * sizeof(char *));
   adlabels[numlabels] = (char *) malloc(MAXSTR * sizeof(char));
   strcpy(adlabels[numlabels], sistr);
   numlabels++;

   return(numlabels - 1);
}


// Determine size of entire dataset.  Pattern labels are estimated as size
// of the pattern definition.
int DataSize()
{
   int i, total=0;

   for (i=0; i<nevents; i++)
      total += events[i].patsize;

   return(total);
}


// Compute mutual information between sensor event labels (identifiers).
// MI(I,J) is defined as
// sum_{i in I} sum_{j in J} p(i,j) log(p(i,j) / p(i) * p(j))
// probabilities are estimated based on events occurring next to each
// other sequentially in the dataset.
void AD_ComputeMI()
{
   int i, j;

   mi = (float **) malloc(numlabels * sizeof(float *));
   for (i=0; i<numlabels; i++)
   {
      mi[i] = (float *) malloc(numlabels * sizeof(float));
      for (j=0; j<numlabels; j++)
      {
	 if (sim[i][i] == 0)
	    mi[i][j] = 0;
         else mi[i][j] =
	      (float) (sim[i][j] + sim[j][i]) / (float) (2 * sim[i][i]);
      }
   }

   for (i=0; i<numlabels; i++)
   {
      for (j=0; j<numlabels; j++)
      {
         if (mi[i][j] > MITHRESHOLD)
	    sim[i][j] = 1;
         else sim[i][j] = 0;
      }
   }
   numsim = numlabels;
}


// Annotate the sensor data.  Pattern instances are indicated by
// labeling sensor events as the begin or end of the corresponding
// pattern occurrence.
void AnnotateData(FILE *fp, FILE *out)
{
   char *cptr = NULL, buffer[256], alabel[MAXSTR], *pat=NULL;
   char date[MAXSTR], time[MAXSTR], sensorid[MAXSTR], sensorvalue[MAXSTR];
   char sensorid2[MAXSTR];
   int numevents=0, num = 0;
    
   rewind(fp);
   numevents++;
   cptr = fgets(buffer, 256, fp);
   while (cptr != NULL)             // Process next line from input file
   {
      sscanf(buffer, "%s %s %s %s %s %s",
	 date, time, sensorid, sensorid2, sensorvalue, alabel);
      if (num < nevents)
         pat = strstr(adlabels[events[num].label], "Pat_");
      if ((pat != NULL) && (num < nevents))
      {
         if (numevents <= events[num].end)
            fprintf(out, "%s\t%s\t%s\t%s\t%s\t%s\n",
	      date, time, sensorid, sensorid2,
	      sensorvalue, adlabels[events[num].label]);
         if (numevents >= events[num].end)
            num++;
      }
      else
      {
         fprintf(out, "%s\t%s\t%s\t%s\t%s\t%s\n",
	    date, time, sensorid, sensorid2, sensorvalue, alabel);
         num++;
      }                          
      numevents++;
      cptr = fgets(buffer, 256, fp);
   }
}
