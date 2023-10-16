/*------------------------------------------------------------------------------
 *       Filename:  ar.c
 *
 *    Description:  Activity recognition on streaming sensor data.
 *
 *        Version:  AL 1.0
 *        Created:  07/01/2013
 *        Authors:  Diane J. Cook, cook@eecs.wsu.edu
 *                  Larry Holder, holder@wsu.edu
 *   Organization:  Washington State University
 * ---------------------------------------------------------------------------*/

#include "al.h"


/* Create a decision tree model for every node in the hierarchy. */
void AR_TrainModel()
{
   FILE *fpdata;
   int i, j;

   AR_ReadData();

   if (UNDERSAMPLE == TRUE)
      UnderSample();

   if (WRITEDATA == TRUE) // Write data points to a file in arff format
   {
      fpdata = fopen("ar.arff", "w");
      fprintf(fpdata, "@relation ar\n\n");
      for (i=0; i<NumDiscreteFeatures; i++)
         fprintf(fpdata, "@attribute %s numeric\n", gFeatureNames[i]);
      fprintf(fpdata, "@attribute class {");
      for (i=0; i<NumActivities; i++)
         if (i < (NumActivities-1))
            fprintf(fpdata, "%d,", i);
         else fprintf(fpdata, "%d}\n\n@data\n", i);

      for (i=0; i<NumData; i++)
      {
         for (j=0; j<NumDiscreteFeatures; j++)
            fprintf(fpdata, "%f,", data[i][j]);
         fprintf(fpdata, "%d\n", labels[i]);
      }
      fclose(fpdata);
   }

   if (OutputLevel > 0)
      printf("   Training the model\n");
   AR_DT();       // train the decision tree
   if (OutputLevel > 0)
      printf("   done\n");
}


/* Create a decision tree model for every node in the hierarchy. */
void AR_RetrainModel()
{
   NumData = 0;
   AR_ReadData();

   if (OutputLevel > 1)
      printf("   Retraining the model\n");
   AR_DT();       // train the decision tree
   if (OutputLevel > 1)
      printf("   done\n");
}


/* Create a decision tree model using data in the current fold. */
void AR_TrainKModel(int cvnum)
{
   if (OutputLevel > 1)
      printf("   Training the model\n");
   AR_KDT(cvnum);       // train the decision tree
   if (OutputLevel > 1)
      printf("   done\n");
}


/* Set number of features used for AR.
   See ComputeFeature procedure for explanation of these features. */
void AR_ReadData()
{
   SetFeatureNames();
   ReadData();
}


/* Randomly remove data points from largest classes in order to make the
   class distribution more even. */
void UnderSample()
{
   int i, j, k, total=0, *rcheck;
   float avg, premove;

   srand(time(NULL));
   printf("Before ");
   for (i=1; i<NumActivities; i++)   // Compute class sizes
   {
      total += classfreq[i];
      printf("%d ", classfreq[i]);
   }
   printf("\n");

   avg = (float) total / (float) NumActivities;
   rcheck = (int *) calloc(NumData, sizeof(int));

   for (i=1; i<NumActivities; i++)
   {
      premove = ((float) classfreq[i] - avg) / classfreq[i];
      for (j=0; j<NumData; j++)
         if ((labels[j] == i) &&
            ((float) (rand() % 1000) < ((int) (premove * 1000.0))))
         {
            rcheck[j] = -1;
            classfreq[i] -= 1;
         }
   }

   for (i=0; i<NumData; i++)
   {
      if (rcheck[i] == -1)
      {
         for (j=i; j<NumData-1; j++)
         {
            for (k=0; k<NumDiscreteFeatures; k++)
               data[j][k] = data[j+1][k];
            labels[j] = labels[j+1];
         }
         NumData--;
      }
   }
   printf("After ");
   for (i=1; i<NumActivities; i++)
      printf("%d ", classfreq[i]);
   printf("\n");

   free(rcheck);
}


/* Generate a decision tree to map window features to an activity label. */
void AR_DT()
{
   int i, majorclass, *training, numtrain, small, numright;
   TreeNodePtr dt = NULL;

   // Allocate global working arrays for decision tree learning
   gFreq1 = (int *) calloc(NumActivities, sizeof(int));
   gFreq2 = (int *) calloc(NumActivities, sizeof(int));
   gValues = (float *) calloc(NumData, sizeof(float));

   numtrain = NumData;
   training = (int *) calloc(numtrain, sizeof(int));
   for (i=0; i<numtrain; i++)
      training[i] = i;

   majorclass = MajorityClass(training, numtrain, &small, &numright);
   if (numtrain > 0)
   {
      dt = AR_BuildTree(training, numtrain, majorclass, NULL);
      if (OutputLevel > 1)
      {
         printf("Unpruned tree\n");
	 AR_PrintDT(dt);
	 printf("\n");
      }
      if ((COLLAPSETREE == TRUE) && (dt != NULL))
      {
         if ((OutputLevel > 1) && (Mode != TRACK))
            printf("Collapsing...");
         CollapseTree(dt);
         if ((OutputLevel > 1) && (Mode != TRACK))
            printf("done\n");
	 if ((OutputLevel > 1) && (Mode != TRACK))
	 {
            printf("Collapsed tree\n");
	    AR_PrintDT(dt);
	    printf("\n");
         }
      }
      if ((AR_PRUNE == TRUE) && (dt != NULL))
      {
         if (OutputLevel > 0)
            printf("Pruning...");
         AR_PruneTree(dt);
         if (OutputLevel > 0)
	 {
            printf("done\n");
            printf("Pruned tree\n");
	    AR_PrintDT(dt);
	    printf("\n");
         }
      }
   }

   // save the model for the current node using the provided name
   SaveDT(dt);

   free(gFreq1);
   free(gFreq2);
   free(gValues);
   free(training);
   FreeTree(dt);
}


/* Generate a decision tree using the current fold
   to map window features to an activity label. */
void AR_KDT(int cvnum)
{
   int i, index, majorclass, *training, numtrain, small, numright;
   TreeNodePtr dt = NULL;

   // Allocate global working arrays for decision tree learning
   gFreq1 = (int *) calloc(NumActivities, sizeof(int));
   gFreq2 = (int *) calloc(NumActivities, sizeof(int));
   gValues = (float *) calloc (NumData, sizeof(float));

   numtrain = NumData;
   training = (int *) calloc(numtrain, sizeof(int));
   index = 0;
   for (i=0; i<numtrain; i++)
      if ((i % K) != cvnum)
         training[index++] = i;
   numtrain = index;

   majorclass = MajorityClass(training, numtrain, &small, &numright);
   if (numtrain > 0)
   {
      dt = AR_BuildTree(training, numtrain, majorclass, NULL);
      if ((COLLAPSETREE == TRUE) && (dt != NULL))
         CollapseTree(dt);
      if ((AR_PRUNE == TRUE) && (dt != NULL))
         AR_PruneTree(dt);
   }

   // save the model for the current node using the provided name
   SaveDT(dt);

   free(gFreq1);
   free(gFreq2);
   free(gValues);
   free(training);
   FreeTree(dt);
}


/* Recursively build decision tree. */
TreeNodePtr AR_BuildTree(int *training, int numtrain, int parentclass,
                         TreeNodePtr parent)
{
   int i, att, majorclass, newtrain, tmp, num, small, numright;
   float val;
   TreeNodePtr tree;

   tree = AllocateTreeNode();
   tree->parent = parent;
   tree->numChildren = 0;
   tree->numInstances = numtrain;    // Look at subset of data for this subtree
   if (TopN > 1)                     // adist is uniformly distributed
   {
      tree->adist = (float *) malloc(NumActivities * sizeof(float));
      if (tree->adist == NULL)
         MemoryError("AR_BuildTree");
      for (i=0; i<NumActivities; i++)
         tree->adist[i] = 0.0;
   }
   if (numtrain == 0)                // No data left, make this a leaf node
   {
      tree->instances = NULL;
      tree->Class = parentclass;
      tree->numRight = 0;
      tree->entropy = 0.0;
      if (TopN > 1)                  // adist is uniformly distributed
      {
	 val = (float) 1.0 / (float) NumActivities;
	 for (i=0; i<NumActivities; i++)
	    tree->adist[i] = val;
      }
   }
   else
   {
      tree->instances = (int *) malloc(numtrain * sizeof(int));
      for (i=0; i<numtrain; i++)
      {
         tree->instances[i] = training[i];
	 if (TopN > 1)
	    tree->adist[labels[training[i]]] += 1.0;
      }
      if (TopN > 1) // Normalize frequencies for distribution
         for (i=0; i<NumActivities; i++)
            tree->adist[i] /= (float) numtrain;
      majorclass = MajorityClass(training, numtrain, &small, &numright);
      tree->Class = majorclass;
      tree->numRight = numright;
      tree->entropy = OldEnt(training, numtrain);

      // If all of the data belongs to the same class or the number of data
      // points for a child is too small, make this a leaf node else split
      if  ((AllOneClass(training, numtrain) == FALSE) && (small == FALSE))
      {
         num = AR_SelectAttribute(tree, training, numtrain);
         if (num > 1)  // Split the data and make subtrees for each child
         {
            att = tree->attribute;
            tree->numChildren = 2;
            tree->children =
	       (TreeNodePtr *) calloc(tree->numChildren, sizeof(TreeNodePtr));
            val = tree->threshold;
            newtrain = CompactInst(training, numtrain, att, val);

            // Build left subtree
            tree->children[0] =
	       AR_BuildTree(training, newtrain, majorclass, tree);

            // swap instances on either side of threshold
            for (i=0; i<newtrain; i++)
            {
               numtrain--;
               tmp = training[i];
               training[i] = training[numtrain];
               training[numtrain] = tmp;
            }

            // Build right subtree
            tree->children[1] =
	       AR_BuildTree(training, numtrain, majorclass, tree);
         }
      }
   }
   return(tree);
}


/* Return the class label that is most frequent for a set of data points. */
int MajorityClass(int *training, int num, int *small, int *numright)
{
   int i, max, maxfreq;

   for (i=0; i < NumActivities; i++)
      gFreq1[i] = 0;
   for (i=0; i<num; i++)
      gFreq1[labels[training[i]]] += 1;

   max = 0;
   maxfreq = gFreq1[0];

   for (i=1; i<NumActivities; i++)
   {
      if (gFreq1[i] > maxfreq)
      {
         max = i;
         maxfreq = gFreq1[i];
      }
   }
   *small = FALSE;
   if (num < (float) (2 * AR_MINNUMOBJ)) // Need >= AR_MINNUMOBJ for a leaf
      *small = TRUE;

   *numright = maxfreq;

   return(max);
}


/* Sets the attribute field of dtnode and the threshold
   according to the attribute having the best split potential.
   The split potential is measured as greatest gain ratio. */
int AR_SelectAttribute(TreeNodePtr dtnode, int *training, int numtrain)
{
   int att, bestAtt=0, validSplits = 0;
   float avgInfoGain = 0.0;
   float minResult = 0.0;
   float bestThreshold=0.0;
   AR_SplitInfo* splitInfoArray;

   splitInfoArray = (AR_SplitInfo*) malloc (NumDiscreteFeatures * sizeof(AR_SplitInfo));
   if (splitInfoArray == NULL)
      MemoryError("AR_SelectAttribute");
   for (att = 0; att < NumDiscreteFeatures; att++)
   {
      splitInfoArray[att].valid = FALSE;
      FindBestNumericSplit (dtnode, training, numtrain, att, splitInfoArray);
      if (splitInfoArray[att].valid == TRUE)
      {
         avgInfoGain += splitInfoArray[att].infoGain;
         validSplits++;
      }
   }
   if (validSplits > 0)
   {
      avgInfoGain = avgInfoGain / validSplits;
      for (att = 0; att < NumDiscreteFeatures; att++)
      {
         if ((splitInfoArray[att].valid == TRUE) &&
             (splitInfoArray[att].infoGain >= (avgInfoGain - 0.001)) &&
             (splitInfoArray[att].gainRatio > minResult))
         {
            bestAtt = att;
            bestThreshold = splitInfoArray[att].threshold;
            minResult = splitInfoArray[att].gainRatio;
         }
      }
   }
   free (splitInfoArray);
   if (minResult > 0.0)
   {
      dtnode->attribute = bestAtt;
      dtnode->threshold = C45SplitPoint (bestAtt, bestThreshold);
      if (OutputLevel > 1)
      {
         fprintf(stdout, "best %d threshold %f\n",
                 dtnode->attribute, dtnode->threshold);
         fflush(stdout);
      }
      return(2);
   }

   return(0);
}


/* Compare x with y to use in sort function, where x and y are indices into
   the data array, and we want to compare them along attribute gAtt. */
int AR_compfn(const void *v1, const void *v2)
{
   int x = *((int *) v1);
   int y = *((int *) v2);

   if (data[x][gAtt] < data[y][gAtt])
      return(-1);
   if (data[x][gAtt] > data[y][gAtt])
      return(1);
   return(0);
}


/* Find best split for given numeric attribute and store split info in array. */
void FindBestNumericSplit(TreeNodePtr dtnode, int* training, int numtrain,
                          int att, AR_SplitInfo* splitInfoArray)
{
   int i, numThresholds, border, total1, total2;
   float bestThreshold=0.0;
   float e1, e2, defaultEnt, newEnt, bestSplitEnt=0.0, gain, bestGain = 0.0;

   splitInfoArray[att].valid = FALSE;

   // Calculate set of possible thresholds for this attribute
   gAtt = att; // used in qsort
   qsort(training, numtrain, sizeof(int), AR_compfn);

   // thresholds stored in gValues[]
   numThresholds = AR_ComputeThresholds(training, numtrain, att);
   if (numThresholds == 0)
      return;

   defaultEnt = dtnode->entropy;  // Entropy of the parent node
   for (i=0; i < NumActivities; i++)
   {
      gFreq1[i] = 0;
      gFreq2[i] = 0;
   }
   for (i=0; i<numtrain; i++)
      gFreq2[labels[training[i]]]++;
   border = 0;
   total1 = 0;
   total2 = numtrain;

   for (i = 0; i < numThresholds; i++)  // Test each threshold
   {
      // Update stats based on move to next threshold
      while ((border < numtrain) && (data[training[border]][att] <= gValues[i]))
      {
         gFreq1[labels[training[border]]]++;
         gFreq2[labels[training[border]]]--;
         total1++;
         total2--;
         border++;
      }
      e1 = Entropy(gFreq1, total1);
      e2 = Entropy(gFreq2, total2);
      newEnt = ((total1 / (float) numtrain) * e1) +
               ((total2 / (float) numtrain) * e2);
      gain = defaultEnt - newEnt;
      if (gain > bestGain)  // Find split that yields maximum gain
      {
         bestThreshold = gValues[i];
         bestGain = gain;
         bestSplitEnt = SplitEnt(total1, total2, numtrain);
      }
   }
   if (USEMDLCORRECTION == TRUE)
      bestGain = bestGain - (log2f((float) numThresholds) / ((float) numtrain));

   if (bestGain > 0.0)
   {
      splitInfoArray[att].valid = TRUE;
      splitInfoArray[att].threshold = bestThreshold;
      splitInfoArray[att].infoGain = bestGain;
      splitInfoArray[att].gainRatio = GainRatio(bestGain, bestSplitEnt);
   }
}


/* Returns the greatest value for the given attribute in all the data
   that is less than or equal to the given split point.  */
float C45SplitPoint(int att, float splitPoint)
{
   int i;
   float newSplitPoint = (- FLT_MAX);
   float tempValue;

   for (i = 0; i < NumData; i++)
   {
      tempValue = data[i][att];
      if ((tempValue > newSplitPoint) && (tempValue <= splitPoint))
         newSplitPoint = tempValue;
   }

   return(newSplitPoint);
}


/* Calculate the entropy of a set of data based on the distribution of class
   labels found in the data. */
float OldEnt(int *training, int numtrain)
{
   int i;
   float e;

   for (i=0; i < NumActivities; i++)
      gFreq1[i] = 0;
   for (i=0; i<numtrain; i++)
      gFreq1[labels[training[i]]] += 1;

   e = Entropy(gFreq1, numtrain);

   return(e);
}


/* Calculate the weighted entropy of a node based on the frequencies
   of the instances found in the children of this node. */
float SplitEnt(int total1, int total2, int numtrain)
{
   float e = 0.0, e1, e2;

   e1 = ((float) total1) / ((float) numtrain);
   if (e1 > 0.000001)
      e -= (e1 * log2f(e1));
   e2 = ((float) total2) / ((float) numtrain);
   if (e2 > 0.000001)
      e -= (e2 * log2f(e2));

   return(e);
}


/* Calculate entropy of a set of values. */
float Entropy(int *freq, int total)
{
   int i;
   float sum = 0.0, p;

   for (i=0; i<NumActivities; i++)
   {
      if (total == 0)
         p = 0.0;
      else p = ((float) freq[i]) / ((float) total);
      if (p > 0.000001)
         sum -= p * log2f(p);
   }

   return(sum);
}


float GainRatio(float infoGain, float splitEnt)
{
   if (splitEnt == 0.0)
      return(0.0);
   else return(infoGain / splitEnt);
}


/* Stores thresholds in gValues array as determined from attribute attr in
   the first numTrain instances in training.  After determining the
   sorted sequence of unique values <v1,v2,...,vn> for attr, the n-1
   thresholds are <(v1+v2)/2,...,(vn-1+vn)/2>.  Returns the number of
   such thresholds. It is assumed that the training array has already
   been sorted according to the att attribute values. */
int AR_ComputeThresholds(int *training, int numtrain, int att)
{
   int i, n, v, minsplit;
   float v1, v2;

   minsplit = (int) (0.1 * (float) numtrain / (float) NumActivities);
   minsplit = max(min(minsplit, 25), AR_MINNUMOBJ);
   for (i=0; i<numtrain; i++)
      gValues[i] = data[training[i]][att]; // training assumed sorted by att
   v1 = gValues[0];
   n = 0;
   v = 1;
   while (v < numtrain)
   {
      v2 = gValues[v];
      if (((v1 + MINVALUEDIFF) < v2) && (v >= minsplit) &&
          ((numtrain - v) >= minsplit))
      {
         gValues[n] = (v1 + v2) / 2.0;  // threshold is midway between values

	 // if beyond precision, choose smaller value
         if (gValues[n] == v2) gValues[n] = v1;
            n++;
      }
      v1 = v2;
      v++;
   }

   return(n);
}


/* Return TRUE if all instances have the same class. */
int AllOneClass(int *training, int numtrain)
{
   int i, firstclass;

   firstclass = labels[training[0]];

   for (i=1; i<numtrain; i++)
      if (labels[training[i]] != firstclass)
         return(FALSE);
   return(TRUE);
}


/* Recursively print decision tree. */
void AR_PrintDT(TreeNodePtr tree)
{
   int size = TreeSize(tree);
   int leaves = NumLeaves(tree);
   printf("\nDecision Tree (size = %d, leaves = %d)\n", size, leaves);
   if (tree == NULL)
      printf("  Empty tree\n");
   else AR_PrintDT1(tree, 2);
}


/* Recursively print decision tree indented by tab spaces. */
void AR_PrintDT1(TreeNodePtr tree, int tab)
{
   int i;

   if (tree != NULL)
   {
      if (tree->numChildren == 0)
      {
         printf(" %s [%d %d]\n", activitynames[tree->Class], tree->numInstances, tree->numRight);
         if (TopN > 1)
	 {
            for (i=0; i<tab; i++)
               printf(" ");
	    printf("{");
            for (i=0; i<NumActivities; i++)
	    {
	       printf("%.2f", tree->adist[i]);
	       if (i < (NumActivities-1))
	          printf(" ");
	       else printf("}\n");
	    }
         }
      }
      else
      {
         printf("\n");
         for (i=0; i<tab; i++)
            printf(" ");
         printf("%s <= %f", gFeatureNames[tree->attribute], tree->threshold);
         AR_PrintDT1(tree->children[0], tab+2);
         for (i=0; i<tab; i++)
            printf(" ");
         printf("%s > %f", gFeatureNames[tree->attribute], tree->threshold);
         AR_PrintDT1(tree->children[1], tab+2);
      }
   }
}


/* Test the decision tree model for every node in the hierarchy. */
void AR_TestModel()
{
   FILE *fp;
   char *cptr=NULL;
   char buffer[BUFFERSIZE], date[MAXSTR], stime[MAXSTR], sensorid[MAXSTR];
   char newsensorid[MAXSTR], sensorstatus[MAXSTR], alabel[MAXSTR];
   char prevdate[MAXSTR], trsensorid[MAXSTR], trnewsensorid[MAXSTR];
   int i, cval=0, end, snum, snum2, anum, other, small;
   float *tempdata;
   TreeNodePtr dt;

   dt = LoadModel();    // load the regular model
   tempdata = (float *) malloc(NumDiscreteFeatures * sizeof(float));
   if (tempdata == NULL)
      MemoryError("AR_TestModel");

   if (ANNOTATE == TRUE)
   {
      strcpy(alabel, "Other_Activity");
      other = FindActivity(alabel);
   }

   strcpy(date, "");
   if ((OutputLevel > 0) && (Mode != TEST))
      printf("   Reading file %s...", filename);
   fp = fopen(filename, "r");
   if (fp == NULL)
      MemoryError("AR_TestModel");

   cptr = fgets(buffer, BUFFERSIZE, fp);
   while (cptr != NULL)  // Process each sensor event in the file
   {
      strcpy(prevdate, date);
      if (ANNOTATE == TRUE)
         sscanf(buffer, "%s %s %s %s %s",
                   date, stime, sensorid, newsensorid, sensorstatus);
      else sscanf(buffer, "%s %s %s %s %s %s",
                     date, stime, sensorid, newsensorid, sensorstatus, alabel);
      if (strcmp(prevdate, date) != 0) // new day
         prevdays += SECONDSINADAY;
      currenttime = ComputeSeconds(stime);
      if (Translate == TRUE)
      {
         snum = FindTranslation(sensorid);
	 strcpy(trsensorid, Translations[snum].trans1);
         snum = FindSensor(trsensorid);
      }
      else snum = FindSensor(sensorid);
      if ((ANNOTATE == TRUE) && (snum == -1))
         cval = other;
      else
      {
	                                               // last time sensor fired
         sensortimes[snum] = currenttime + prevdays;
         if (Translate == TRUE)
         {
            snum2 = FindTranslation(newsensorid);
	    strcpy(trnewsensorid, Translations[snum2].trans2);
            snum = FindSensor(trnewsensorid);
         }
         else snum = FindSensor(newsensorid);
         if ((ANNOTATE == TRUE) && (snum == -1))
            cval = other;
         else
         {
	                                               // last time sensor fired
            sensortimes[snum] = currenttime + prevdays;

            if (ANNOTATE == FALSE)
               anum = FindActivity(alabel);

            // initialize the temp data
            for (i=0; i<NumDiscreteFeatures; i++)
               tempdata[i] = 0;

            // Compute a feature vector for the sensor event
            if ((ANNOTATE == TRUE) || (IgnoreOther == FALSE) ||
               (strcmp(alabel, "Other_Activity") != 0))
	    {
	       if (Translate == TRUE)
                  end =
		     ComputeFeature(stime, trsensorid, trnewsensorid, tempdata);
               else end =
	            ComputeFeature(stime, sensorid, newsensorid, tempdata);
	    }
            else end = 0;
            if (end == 1)        // End of window reached, add feature vector
            {
               if (NumData == 0)
               {
                  labels = (int *) malloc(sizeof(int));
                  if (labels == NULL)
                     MemoryError("AR_TestModel");
               }
               else
               {
                  labels = (int *) realloc(labels, (NumData+1) * sizeof(int));
                  if (labels == NULL)
                     MemoryError("AR_TestModel");
               }

               if (ANNOTATE == FALSE)
               {
                  labels[NumData] = anum;
                  classfreq[labels[NumData]] += 1;
               }
               NumData++;

               small = FALSE;

	       // Decision tree value
               cval = DTClassify(tempdata, dt, AR_MINNUMCLASS, &small);
	       // If empty leaf label Other_Activity
               if ((small == TRUE) && (ANNOTATE == TRUE))
                  cval = other;
               if (ANNOTATE == TRUE)
                  printf("%s %s %s %s %s %s\n",
                            date, stime, sensorid, newsensorid,
                            sensorstatus, activitynames[cval]);
               else
               {
                  if (CMATRIX == TRUE)
                     cmatrix[labels[NumData-1]][cval] += 1;
                  if (cval == labels[NumData-1])
                     NumRight++;
               }
            }
         }
      }
      cptr = fgets(buffer, BUFFERSIZE, fp);
   }

   fclose(fp);
   free(tempdata);
}


/* Test the decision tree model for every node in the hierarchy. */
void AR_AnnModel()
{
   FILE *fp, *fpann;
   char *cptr=NULL;
   char buffer[BUFFERSIZE], date[MAXSTR], stime[MAXSTR], sensorid[MAXSTR];
   char newsensorid[MAXSTR], sensorstatus[MAXSTR], alabel[MAXSTR];
   char prevdate[MAXSTR];
   int i, cval=0, end, snum, other, small;
   float *tempdata;
   TreeNodePtr dt;

   NumData = 0;
   dt = LoadModel();    // load the regular model
   tempdata = (float *) malloc(NumDiscreteFeatures * sizeof(float));
   if (tempdata == NULL)
      MemoryError("AR_AnnModel");

   strcpy(date, "");
   fp = fopen(trainname, "r");
   sprintf(filename, "%s.ann", trainname);
   fpann = fopen(filename, "w");
   if ((fp == NULL) || (fpann == NULL))
      MemoryError("AR_AnnModel");

   if (labels != NULL)
      free(labels);
   labels = NULL;
   strcpy(alabel, "Other_Activity");
   other = FindActivity(alabel);
   cptr = fgets(buffer, BUFFERSIZE, fp);
   while (cptr != NULL)  // Process each sensor event in the file
   {
      strcpy(prevdate, date);
      sscanf(buffer, "%s %s %s %s %s %s",
         date, stime, sensorid, newsensorid, sensorstatus, alabel);
      if (strcmp(prevdate, date) != 0) // new day
         prevdays += SECONDSINADAY;
      currenttime = ComputeSeconds(stime);
      snum = FindSensor(sensorid);
      if (snum == -1)
         cval = other;
      else
      {
	                                              // last time sensor fired
         sensortimes[snum] = currenttime + prevdays;
         snum = FindSensor(newsensorid);
         if (snum == -1)
            cval = other;
         else
         {
            // initialize the temp data
            for (i=0; i<NumDiscreteFeatures; i++)
               tempdata[i] = 0;

            // Compute a feature vector for the sensor event
            end = ComputeFeature(stime, sensorid, newsensorid, tempdata);
            if (end == 1)        // End of window reached, add feature vector
            {
               if (NumData == 0)
               {
                  labels = (int *) malloc(sizeof(int));
                  if (labels == NULL)
                     MemoryError("AR_AnnModel");
               }
               else
               {
                  labels = (int *) realloc(labels, (NumData+1) * sizeof(int));
                  if (labels == NULL)
                     MemoryError("AR_AnnModel");
               }
               NumData++;

               small = FALSE;

	       // Decision tree value
               cval = DTClassify(tempdata, dt, AR_MINNUMCLASS, &small);

               if (small == TRUE)
                  cval = other;
	       if (strcmp(alabel, "Other_Activity") == 0)  // add AR label
                  fprintf(fpann, "%s %s %s %s %s %s\n",
                          date, stime, sensorid, newsensorid,
                          sensorstatus, activitynames[cval]);
	       else fprintf(fpann, "%s %s %s %s %s %s\n",
                            date, stime, sensorid, newsensorid,
                            sensorstatus, alabel);
            }
         }
      }
      cptr = fgets(buffer, BUFFERSIZE, fp);
   }

   fclose(fp);
   fclose(fpann);
   free(tempdata);
   if (labels != NULL)
      free(labels);
   labels = NULL;
}

/* Test the decision tree model using current fold
   for every node in the hierarchy. */
void AR_TestKModel(int cvnum)
{
   int i, cval=0, small;
   TreeNodePtr dt;

   dt = LoadModel();    // load the regular model

   for (i=0; i<NumData; i++)
      if ((i % K) == cvnum)
      {
         small = FALSE;

	 // Decision tree value
         cval = DTClassify(data[i], dt, AR_MINNUMCLASS, &small);
	 // If empty leaf label Other_Activity
         if (cval == labels[i])
            NumRight++;
      }
}


/* Prune decision tree based on estimated error.
   Works only for binary numeric splits.
   if E(leaf) <= (E(tree)+PMARGIN) then change subtree into leaf. */
void AR_PruneTree(TreeNodePtr tree)
{
   float errorsLeaf, errorsTree;

   if (tree->numChildren > 0)
   {
      AR_PruneTree(tree->children[0]);
      AR_PruneTree(tree->children[1]);
      errorsLeaf = GetEstimatedErrorsForDistribution(tree);
      errorsTree = GetEstimatedErrors(tree);
      if (errorsLeaf <= (errorsTree + pmargin2))
      {
         // Node better off as leaf; remove children
         FreeTree(tree->children[0]);
         FreeTree(tree->children[1]);
         free(tree->children);
         tree->children = NULL;
         tree->numChildren = 0;
      }
   }
}


/* Return estimated errors treating given tree as a leaf node. */
float GetEstimatedErrorsForDistribution(TreeNodePtr tree)
{
   float errors = 0.0;

   if (tree->numInstances > 0)
   {
      errors = (float) (tree->numInstances - tree->numRight);
      errors += AddErrors (tree->numInstances, errors, CF);
   }

   return(errors);
}


float GetEstimatedErrors(TreeNodePtr tree)
{
   int i;
   float errors = 0.0;

   if (tree->numChildren == 0)
   {
      errors = GetEstimatedErrorsForDistribution (tree);
   }
   else
   {
      for (i = 0; i < tree->numChildren; i++)
      errors += GetEstimatedErrors (tree->children[i]);
   }

   return(errors);
}


/* Compute estimated additional errors based on normal approximation to binomial
   distribution with given number examples and actual errors. This is based on
   the approach in Weka's J48 classifier. Assumes numExamples > 0. */
float AddErrors(int numExamples, float errors, float confidenceFactor)
{
   float addErrors, examples = (float) numExamples;

   if ((confidenceFactor > 0.5) || (errors == examples))
      return(0.0);

   if (errors == 0.0)
   {
      addErrors =
         (float) (examples * (1.0 - powf(confidenceFactor, 1.0 / examples)));
      return(addErrors);
   }

   // Get z-score corresponding to CF
   double z = ltqnorm(1 - CF);

   // Compute upper limit of confidence interval
   double f = (errors + 0.5) / examples;
   double r = (f + (z * z) / (2 * examples) +
              z * sqrt((f / examples) - (f * f / examples) +
	                                (z * z / (4 * examples * examples)))) /
              (1 + (z * z) / examples);

   addErrors = (float) ((r * examples) - errors);

   return(addErrors);
}


/* Lower tail quantile for standard normal distribution function.
  
   This function returns an approximation of the inverse cumulative
   standard normal distribution function.  I.e., given P, it returns
   an approximation to the X satisfying P = Pr{Z <= X} where Z is a
   random variable from the standard normal distribution.
  
   The algorithm uses a minimax approximation by rational functions
   and the result has a relative error whose absolute value is less
   than 1.15e-9.
  
   Author:      Peter John Acklam
   Time-stamp:  2002-06-09 18:45:44 +0200
   E-mail:      jacklam@math.uio.no
   WWW URL:     http://www.math.uio.no/~jacklam
  
   C implementation adapted from Peter's Perl version.  */

/* Coefficients in rational approximations. */
static const double a[] =
{
   -3.969683028665376e+01,
   2.209460984245205e+02,
   -2.759285104469687e+02,
   1.383577518672690e+02,
   -3.066479806614716e+01,
   2.506628277459239e+00
};

static const double b[] =
{
   -5.447609879822406e+01,
   1.615858368580409e+02,
   -1.556989798598866e+02,
   6.680131188771972e+01,
   -1.328068155288572e+01
};

static const double c[] =
{
   -7.784894002430293e-03,
   -3.223964580411365e-01,
   -2.400758277161838e+00,
   -2.549732539343734e+00,
   4.374664141464968e+00,
   2.938163982698783e+00
};

static const double d[] =
{
   7.784695709041462e-03,
   3.224671290700398e-01,
   2.445134137142996e+00,
   3.754408661907416e+00
};


double ltqnorm(double p)
{
   double q, r;

   if (p < 0 || p > 1)
      return(0.0);
   else if (p == 0)
      return(-HUGE_VAL); // minus infinity
   else if (p == 1)
      return(HUGE_VAL); // infinity
   else if (p < LOW)
   {
      /* Rational approximation for lower region */
      q = sqrt(-2*log(p));
      return((((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
             ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1));
   }
   else if (p > HIGH)
   {
      /* Rational approximation for upper region */
      q  = sqrt(-2*log(1-p));
      return(-(((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
              ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1));
   }
   else
   {
      /* Rational approximation for central region */
      q = p - 0.5;
      r = q*q;
      return((((((a[0]*r+a[1])*r+a[2])*r+a[3])*r+a[4])*r+a[5])*q /
             (((((b[0]*r+b[1])*r+b[2])*r+b[3])*r+b[4])*r+1));
   }
}
