/*------------------------------------------------------------------------------
 *       Filename:  ap.c
 *
 *    Description:  Activity prediction on streaming sensor data.
 *
 *        Version:  AL 1.0 (based on AR v2.4)
 *        Created:  05/24/2013
 *         Author:  Diane J. Cook, cook@eecs.wsu.edu
 *                  Larry Holder, holder@wsu.edu
 *   Organization:  Washington State University
 * ---------------------------------------------------------------------------*/

#include "al.h"


/* Create a decision tree model for every node in the hierarchy. */
void AP_TrainModel()
{
   FILE *fpdata;
   char str[MAXSTR];
   int i, j;

   AP_ReadData();

   if (WRITEDATA == TRUE) // Write data points to a file in arff format
   {
      if (pactivity == -1)
         sprintf(str, "%s.arff", activitynames[other]);
      sprintf(str, "%s.arff", activitynames[pactivity]);
      fpdata = fopen(str, "w");
      fprintf(fpdata, "@relation ap\n\n");
      for (i=0; i<NumFeatures; i++)
         fprintf(fpdata, "@attribute %s numeric\n", gFeatureNames[i]);
      fprintf(fpdata, "@attribute class numeric\n\n");
      fprintf(fpdata, "@data\n");

      for (i=0; i<NumData; i++)
      {
         for (j=0; j<NumFeatures; j++)
            fprintf(fpdata, "%f,", data[i][j]);
         fprintf(fpdata, "%d\n", labels[i]);
      }
      fclose(fpdata);
   }

   AllocateMatrices();
   if (OutputLevel > 0)
      printf("   Training the model\n");
   AP_DT();       // train the decision tree
   if (OutputLevel > 0)
      printf("   done\n");
   FreeMatrices();
}


/* Insert additional features into the vector.
   2*NumSensors+8+NumActivities: sensor event timestamp
   2*NumSensors+9+NumActivities -
      2*NumSensors+9+NumActivities+2*MAXLAG: time stamp for each lag event */
void AP_ReadData()
{
   char alabel[MAXSTR];
   int i, j, offset, timediff;
   float ts, deltatime;

   // Other activity only set for AP, not AR, because calling FindActivity(A)
   // will add A to the list of global activities if not already there, and we
   // do not want that for AR.
   strcpy(alabel, "Other_Activity");
   other = FindActivity(alabel);

   //NumFeatures += NumActivities + 1 + (2*MAXLAG);
   NumDiscreteFeatures += 1 + (2*MAXLAG);
   NumFeatures += 1 + (2*MAXLAG);
   SetFeatureNames();

   ReadData();

   // Add timestamp and lag features
   timediff = (int) (lasttime - firsttime);
   deltatime = (float) timediff / (float) NumData;
   if (deltatime == 0.0)
      deltatime = MIN;
   //offset = 8 + (2*NumSensors) + NumActivities;
   offset = 8 + (2*NumSensors);
   for (i=0; i<NumData; i++)
   {
      ts = ((float) timestamp[i]) / deltatime;
      discreteData[i][offset] = ts;
      data[i][offset] = ts;
      for (j=1; j<=MAXLAG; j++)
      {
         if ((i-j) < 0) {
        	 discreteData[i][offset+j] = 0.0;
        	 data[i][offset+j] = 0.0;
         }
         else {
        	 discreteData[i][offset+j] = (float) timestamp[i-j] / deltatime;
        	 data[i][offset+j] = (float) timestamp[i-j] / deltatime;
         }
      }
      for (j=1; j<=MAXLAG; j++)
      {
         if ((i-j) < 0) {
        	 discreteData[i][offset+MAXLAG+j] = 0.0;
        	 data[i][offset+MAXLAG+j] = 0.0;
         }
         else {
        	 discreteData[i][offset+MAXLAG+j] = ((float) timestamp[i-j] / deltatime) * ts;
        	 data[i][offset+MAXLAG+j] = ((float) timestamp[i-j] / deltatime) * ts;
         }
      }
   }
}


void AP_DT()
{
   int i, numtrain, *training;
   TreeNodePtr dt = NULL;

   numtrain = NumData;
   training = (int *) calloc(numtrain, sizeof(int));
   for (i=0; i<numtrain; i++)
      training[i] = i;
   globalStdDev = StandardDeviation(training, numtrain);
   globalAbsDev = AbsoluteDeviation(training, numtrain);
   gAttributesTested = (int *) malloc(NumFeatures * sizeof(int));
   for (i = 0; i < NumFeatures; i++)
      gAttributesTested[i] = FALSE;

   printf("Building tree...\n");
   dt = AP_BuildTree(training, numtrain, NULL);

   printf("Installing models...\n");

   // If pruning, then prune, else install linear models
   if (dt != NULL)
   {
      if (AP_PRUNE == TRUE)
      {
         if (OutputLevel > 0)
            printf("Pruning...");
         AP_PruneTree(dt);
         if (OutputLevel > 0)
            printf("done\n");
      }
      else AP_InstallLinearModels(dt);
   }

	// If smoothed predictions, then install smoothed models
   if ((dt != NULL) && (AP_SMOOTH_PREDICTIONS))
   {
      printf("Smoothing models...\n");
      AP_InstallSmoothedModels(dt);
   }

	// Assign model numbers to leaves
   if (dt != NULL)
      AP_AssignModelNumbers(dt, 0);

   if (OutputLevel > 1)
   {
      printf("Final tree\n");
      AP_PrintDT(dt);
      printf("\n");
   }

   SaveDT(dt);
   FreeTree(dt);
   free(gAttributesTested);
   free(training);
}


void AP_PruneTree(TreeNodePtr tree)
{
   int leftParams = 0, rightParams = 0, numParametersModel;
   double rmseModel, rmseSubTree, adjustedRMSEModel, adjustedRMSESubTree;

   if (tree->numChildren > 0)
   {
      // Process internal node
      if (tree->children[0] != NULL)
         AP_PruneTree(tree->children[0]);
      if (tree->children[1] != NULL)
         AP_PruneTree(tree->children[1]);
      AP_BuildLinearModel(tree);

      // Evaluate model at this tree node
      rmseModel = AP_EvaluateModel(tree);
      numParametersModel = GetNumParametersModel(tree);
      adjustedRMSEModel =
         rmseModel * PruningFactor(tree->numInstances, numParametersModel + 1);

      // Evaluate subtree rooted at this tree node
      rmseSubTree = AP_EvaluateSubTree(tree);
      if (tree->children[0] != NULL)
         leftParams = tree->children[0]->numParameters;
      if (tree->children[1] != NULL)
         rightParams = tree->children[1]->numParameters;
      adjustedRMSESubTree = rmseSubTree *
              PruningFactor(tree->numInstances, (leftParams + rightParams + 1));

      // If tree node's model outperforms subtree
      // then make this tree node a leaf
      if ((adjustedRMSEModel <= adjustedRMSESubTree) ||
          (adjustedRMSEModel < (globalStdDev * 0.00001)))
      {
         FreeTree(tree->children[0]);
         FreeTree(tree->children[1]);
         free(tree->children);
         tree->children = NULL;
         tree->numChildren = 0;
         tree->numParameters = numParametersModel + 1;
         tree->rootMeanSquaredError = rmseModel;
      }
      else
      {
         tree->numParameters = (leftParams + rightParams + 1);
         tree->rootMeanSquaredError = rmseSubTree;
      }
   }
   else
   {
      // Process leaf node
      AP_BuildLinearModel(tree);
      tree->rootMeanSquaredError = AP_EvaluateModel(tree);
   }
}


double PruningFactor(int numInstances, int numParameters)
{
   double factor;

   if (numInstances <= numParameters)
      factor = 10.0;
   else factor = ((double) (numInstances + (2 * numParameters))) /
                 ((double) (numInstances - numParameters));

   return factor;
}


void AP_InstallLinearModels(TreeNodePtr tree)
{
   if (tree->numChildren > 0)
   {
      if (tree->children[0] != NULL)
         AP_InstallLinearModels(tree->children[0]);
      if (tree->children[1] != NULL)
         AP_InstallLinearModels(tree->children[1]);
   }
   AP_BuildLinearModel(tree);
   tree->rootMeanSquaredError = AP_EvaluateModel(tree);
}

/* Solve for X = (A+R)^-1 * B, where A = (1,data)^T * (1,data),
   R is the ridge regression correction (initially diagonal of RIDGE),
   B is (1,data)^T * Y, where Y is the vector of label values. Thus,
   X is vector of coefficients, one for each of the numIndices attributes
   in indices[] plus one for the intercept. If cannot solve for X, then
   keep adding ridge. */
void AP_BuildLinearModel(TreeNodePtr node)
{
   int i, j, k, att1, att2, nInsts, nInds, inst, success;
   float ridgeInc, ridgeBase;

   // Create matrices using reduced set of attributes
   // -- matrix A
   nInds = node->numIndices;
   nInsts = node->numInstances;
   for (i = 0; i < nInds; i++)
   {
      att1 = node->indices[i];
      for (j = 0; j < nInds; j++)
      {
         att2 = node->indices[j];
         gMatrixA[i][j] = 0.0;
         for (k = 0; k < nInsts; k++)
         {
            inst = node->instances[k];
            gMatrixA[i][j] += (data[inst][att1] * data[inst][att2]);
         }
      }
      gMatrixA[i][nInds] = 0.0;
      for (k = 0; k < nInsts; k++)
      {
         inst = node->instances[k];
         gMatrixA[i][nInds] += data[inst][att1];
      }
   }
   for (j = 0; j < nInds; j++)
   {
      att2 = node->indices[j];
      gMatrixA[nInds][j] = 0.0;
      for (k = 0; k < nInsts; k++)
      {
         inst = node->instances[k];
         gMatrixA[nInds][j] += data[inst][att2];
      }
   }
   gMatrixA[nInds][nInds] = (float) nInsts;
   // -- matrix B
   for (i = 0; i < nInds; i++)
   {
      att1 = node->indices[i];
      gMatrixB[i] = 0.0;
      for (k = 0; k < nInsts; k++)
      {
         inst = node->instances[k];
         gMatrixB[i] += (data[inst][att1] * labels[inst]);
      }
   }
   gMatrixB[nInds] = 0.0;
   for (k = 0; k < nInsts; k++)
   {
      inst = node->instances[k];
      gMatrixB[nInds] += (float) labels[inst];
   }
   // -- matrix X
   for (i = 0; i <= nInds; i++)
      gMatrixX[i] = 0.0;

   // Solve Ax=b (increasing ridge until success)
   ridgeInc = ridgeBase = RIDGE;
   success = FALSE;
   while ((success == FALSE) && (ridgeInc < RIDGE_MAX))
   {
      for (i = 0; i <= nInds; i++)
         gMatrixA[i][i] += ridgeInc;
      ridgeBase = 10.0 * ridgeBase;
      ridgeInc = 0.9 * ridgeBase;
      success = SolveMatrix(gMatrixA, gMatrixB, gMatrixX, (nInds+1));
   }

   // Install results in node
   for (i = 0; i < NumFeatures; i++)
      node->coefficients[i] = 0.0;
   node->numParameters = 0;
   for (i = 0; i < nInds; i++)
   {
      node->coefficients[node->indices[i]] = gMatrixX[i];
      if (gMatrixX[i] != 0.0)
         node->numParameters++;
   }
   node->intercept = gMatrixX[nInds];
}


/* Evaluate model stored in node on instances stored in node.
   Return root mean squared error. */
float AP_EvaluateModel(TreeNodePtr node)
{
   int i, numInsts;
   float prediction, error, sumSqrErr = 0.0, meanSqrErr;

   numInsts = node->numInstances;
   for (i = 0; i < numInsts; i++)
   {
      prediction = AP_ClassifyInstance(data[node->instances[i]], node);
      error = prediction - ((float) labels[node->instances[i]]);
      sumSqrErr += (error * error);
   }
   meanSqrErr = sumSqrErr / numInsts;

   return(sqrtf(meanSqrErr));
}


/* Evaluate instances stored in node using subtree rooted at node.
   Return root mean squared error. */
float AP_EvaluateSubTree(TreeNodePtr node)
{
   int i, numInsts;
   float prediction, error, sumSqrErr = 0.0, meanSqrErr;

   numInsts = node->numInstances;
   for (i = 0; i < numInsts; i++)
   {
      prediction = AP_DTClassify (data[node->instances[i]], node);
      error = prediction - ((float) labels[node->instances[i]]);
      sumSqrErr += (error * error);
   }
   meanSqrErr = sumSqrErr / numInsts;

   return(sqrtf(meanSqrErr));
}


void AP_InstallSmoothedModels(TreeNodePtr tree)
{
   int i, numInsts;
   TreeNodePtr current = tree;

   if (tree->numChildren > 0)
   {
      if (tree->children[0] != NULL)
         AP_InstallSmoothedModels(tree->children[0]);
      if (tree->children[1] != NULL)
         AP_InstallSmoothedModels(tree->children[1]);
   }
   else
   {
      // Smooth leaf node model coefficients based on ancestors' models
      while (current->parent != NULL)
      {
         numInsts = current->numInstances;
         for (i = 0; i < NumFeatures; i++)
            tree->coefficients[i] = ((tree->coefficients[i] * numInsts) /
	                             (numInsts + AP_SMOOTHING_CONSTANT));
         tree->intercept = ((tree->intercept * numInsts) /
	                    (numInsts + AP_SMOOTHING_CONSTANT));

         for (i=0; i<NumFeatures; i++)
            tree->coefficients[i] +=
	       ((AP_SMOOTHING_CONSTANT * current->parent->coefficients[i]) /
	        (numInsts + AP_SMOOTHING_CONSTANT));
         tree->intercept +=
	    ((AP_SMOOTHING_CONSTANT * current->parent->intercept) /
	     (numInsts + AP_SMOOTHING_CONSTANT));
         current = current->parent;
      }
      // Update number of model parameters
      tree->numParameters = 0;
      for (i = 0; i < NumFeatures; i++)
      if (tree->coefficients[i] != 0.0)
         tree->numParameters++;
   }
}


/* Assign models numbers to leaf nodes, left to right, starting at 1.
   Internal nodes have a model number of 0. */
int AP_AssignModelNumbers(TreeNodePtr tree, int number)
{
   if (tree->numChildren > 0)
   {
      tree->modelNumber = 0;
      if (tree->children[0] != NULL)
         number = AP_AssignModelNumbers(tree->children[0], number);
      if (tree->children[1] != NULL)
         number = AP_AssignModelNumbers(tree->children[1], number);
      }
      else
      {
         number++;
         tree->modelNumber = number;
      }

   return(number);
}

/* Returns number of non-zero coefficients in node's model. */
int GetNumParametersModel(TreeNodePtr node)
{
   int i, numParameters = 0;

   if (node->coefficients != NULL)
   {
      for (i = 0; i < NumFeatures; i++)
         if (node->coefficients[i] != 0.0)
            numParameters++;
   }

   return(numParameters);
}


TreeNodePtr AP_BuildTree(int *training, int numtrain, TreeNodePtr parent)
{
   int i, tmp, newtrain, att, attrCount, goodSplit;
   float stddev, val;
   TreeNodePtr tree;

   tree = AllocateTreeNode();
   tree->parent = parent;
   tree->numInstances = numtrain;
   tree->instances = (int *) malloc(numtrain * sizeof(int));
   if (tree->instances == NULL)
      MemoryError("AP_BuildTree");
   for (i=0; i<numtrain; i++)
      tree->instances[i] = training[i];
   tree->coefficients = (float *) malloc(NumFeatures * sizeof(float));
   if (tree->coefficients == NULL)
      MemoryError("AP_BuildTree");
   stddev = StandardDeviation(training, numtrain);
   tree->numChildren = 0;
   tree->numParameters = 1;
   if ((numtrain >= AP_MINNUMOBJ) && (stddev >= (globalStdDev * MINSTDDEVFRAC)))
   {
      // Try to split
      goodSplit = AP_SelectAttribute(tree, training, numtrain);
      if (goodSplit)
      {
         att = tree->attribute;
         val = tree->threshold;
         tree->numChildren = 2;
         tree->children =
	    (TreeNodePtr *) calloc(tree->numChildren, sizeof(TreeNodePtr));
         newtrain = CompactInst(training, numtrain, att, val);

         // Build left subtree
         tree->children[0] = AP_BuildTree(training, newtrain, tree);

         // swap instances on either side of threshold
         for (i=0; i<newtrain; i++)
         {
            numtrain--;
            tmp = training[i];
            training[i] = training[numtrain];
            training[numtrain] = tmp;
         }

         // Build right subtree
         tree->children[1] = AP_BuildTree(training, numtrain, tree);

         // If model tree, then collect attributes tested in subtrees
         if (!AP_REGRESSION_TREE)
         {
            for (i = 0; i < NumFeatures; i++)
               gAttributesTested[i] = FALSE;
            SetAttributesTested(tree, gAttributesTested);
            attrCount = 0;
            for (i = 0; i < NumFeatures; i++)
            {
               if (gAttributesTested[i] == TRUE)
                  attrCount++;
            }
            tree->numIndices = attrCount;
            if (attrCount > 0)
            {
               tree->indices = (int *) malloc(attrCount * sizeof(int));
               if (tree->indices == NULL)
                  MemoryError("AP_BuildTree");
            }
            attrCount = 0;
            for (i = 0; i < NumFeatures; i++)
            {
               if (gAttributesTested[i] == TRUE)
               {
                  tree->indices[attrCount] = i;
                  attrCount++;
               }
            }
         }
      }
   }

   return(tree);
}


/* Compare x with y to use in sort function, where x and y are indices into
   the data array, and we want to compare them along attribute gAtt. */
int AP_compfn(const void *v1, const void *v2)
{
   int x = *((int *) v1);
   int y = *((int *) v2);

   if (data[x][gAtt] < data[y][gAtt])
      return(-1);
   if (data[x][gAtt] > data[y][gAtt])
      return(1);
   return(0);
}


/* If a good split attribute can be found, then set attribute and threshold
   within dtnode, and return TRUE. Otherwise, return FALSE. */
int AP_SelectAttribute(TreeNodePtr dtnode, int *training, int numtrain)
{
   int att, goodSplit;
   AP_SplitInfo currentSplit, bestSplit;

   AP_SplitInfo_Init(&currentSplit, numtrain, -1);
   AP_SplitInfo_Init(&bestSplit, numtrain, -1);

   for (att = 0; att < NumFeatures; att++)
   {
      // sort instances based on att
      gAtt = att; // used in qsort
      qsort(training, numtrain, sizeof(int), AP_compfn);

      // update current split info
      AP_SplitInfo_Update(&currentSplit, training, numtrain, att);

      if (currentSplit.maxImpurity > (bestSplit.maxImpurity + MINPURITY))
         AP_SplitInfo_Copy(&currentSplit, &bestSplit);
   }
   if ((bestSplit.splitAttr < 0) || (bestSplit.position < 1) ||
       (bestSplit.position >= numtrain))
      goodSplit = FALSE;
   else
   {
      dtnode->attribute = bestSplit.splitAttr;
      dtnode->threshold = bestSplit.splitValue;
      goodSplit = TRUE;
   }

   return(goodSplit);
}


void AP_SplitInfo_Init(AP_SplitInfo* splitInfo, int numtrain, int attr)
{
   splitInfo->number = numtrain;
   splitInfo->first = 0;
   splitInfo->last = numtrain-1;
   splitInfo->position = -1;
   splitInfo->maxImpurity = -1.e20;
   splitInfo->splitAttr = attr;	// attr=-1 invalid split
   splitInfo->splitValue = 0.0;
}


/* Copy splitInfo1 data into splitInfo2. */
void AP_SplitInfo_Copy(AP_SplitInfo* splitInfo1, AP_SplitInfo* splitInfo2)
{
   splitInfo2->number = splitInfo1->number;
   splitInfo2->first = splitInfo1->first;
   splitInfo2->last = splitInfo1->last;
   splitInfo2->position = splitInfo1->position;
   splitInfo2->maxImpurity = splitInfo1->maxImpurity;
   splitInfo2->splitAttr = splitInfo1->splitAttr;
   splitInfo2->splitValue = splitInfo1->splitValue;
}

void AP_SplitInfo_Prin(AP_SplitInfo* splitInfo)
{
   printf("AP_SplitInfo (%s<=%f): num=%d, first=%d, last=%d, pos=%d, maxImp=%f, left=%f, right=%f\n\n",
      gFeatureNames[splitInfo->splitAttr], splitInfo->splitValue,
      splitInfo->number, splitInfo->first, splitInfo->last, splitInfo->position,
      splitInfo->maxImpurity, splitInfo->leftAve, splitInfo->rightAve);
}


void AP_SplitInfo_Update(AP_SplitInfo* splitInfo, int* training, int numtrain,
                         int attr)
{
   int i, len, partition;
   float val1, val2;
   AP_Impurity imp;

   AP_SplitInfo_Init(splitInfo, numtrain, attr);

   if (splitInfo->number < 4)
      return;

   if (numtrain < 5)
      len = 1;
   else len = numtrain / 5;
   splitInfo->position = 0;
   partition = len - 1;
   AP_Impurity_Init(&imp, partition, attr, training, numtrain, 5);

   for (i = len; i <= numtrain-len-2; i++)
   {
      AP_Impurity_Increment (&imp, labels[training[i]]);

      val1 = data[training[i]][attr];
      val2 = data[training[i+1]][attr];
      if (val1 != val2)
      {
         if (imp.impurity > splitInfo->maxImpurity)
         {
            splitInfo->maxImpurity = imp.impurity;
            splitInfo->splitValue = (val1 + val2) * 0.5;
            splitInfo->leftAve = imp.sl / imp.nl;
            splitInfo->rightAve = imp.sr / imp.nr;
            splitInfo->position = i;
         }
      }
   }
}


void AP_Impurity_Init(AP_Impurity* imp, int partition, int attr, int* training,
                      int numtrain, int k)
{
   int i;
   double value, sum = 0.0, sqrSum = 0.0;

   // compute stats for left partition
   for (i = 0; i <= partition; i++)
   {
      value = labels[training[i]];
      sum += value;
      sqrSum += value*value;
   }
   imp->nl  = partition + 1;
   imp->sl  = sum;
   imp->s2l = sqrSum;

   // compute stats for right partition
   for (i = partition+1; i < numtrain; i++)
   {
      value = labels[training[i]];
      sum += value;
      sqrSum += value*value;
   }
   imp->nr  = numtrain - partition - 1;
   imp->sr  = sum - imp->sl;
   imp->s2r = sqrSum - imp->s2l;

   // compute stats for all instances
   imp->n = numtrain;
   if (numtrain > 1)
   {
      imp->va = fabs((sqrSum - (sum * sum / numtrain)) / numtrain);
      imp->sd = sqrt(imp->va);
   }
   else
   {
      imp->va = 0.0;
      imp->sd = 0.0;
   }

   imp->order = k;
   imp->attr = attr;
   AP_Impurity_Compute(imp);
}


/* Update impurity by moving class value from right to left partition. */
void AP_Impurity_Increment (AP_Impurity* imp, float value)
{
   imp->nl += 1;
   imp->nr -= 1;
   imp->sl += value;
   imp->sr -= value;
   imp->s2l += value*value;
   imp->s2r -= value*value;
   AP_Impurity_Compute(imp);
}


void AP_Impurity_Compute(AP_Impurity* imp)
{
   double p, y = 0.0, yl = 0.0, yr = 0.0;

   if (imp->nl <= 0)
   {
      imp->vl = 0.0;
      imp->sdl = 0.0;
   }
   else
   {
      imp->vl = ((imp->nl * imp->s2l) - (imp->sl * imp->sl)) /
                 (imp->nl * imp->nl);
      imp->vl = fabs (imp->vl);
      imp->sdl = sqrt (imp->vl);
   }

   if (imp->nr <= 0)
   {
      imp->vr = 0.0;
      imp->sdr = 0.0;
   }
   else
   {
      imp->vr = ((imp->nr * imp->s2r) - (imp->sr * imp->sr)) /
                 (imp->nr * imp->nr);
      imp->vr = fabs (imp->vr);
      imp->sdr = sqrt (imp->vr);
   }

   if (imp->order == 1)
   {
      y = imp->va;
      yl = imp->vl;
      yr = imp->vr;
   }
   else
   {
      p = 1.0 / imp->order;
      y = pow (imp->va, p);
      yl = pow (imp->vl, p);
      yr = pow (imp->vr, p);
   }

   if ((imp->nl <= 0) || (imp->nr <= 0))
      imp->impurity = 0.0;
   else imp->impurity = y - (((double)imp->nl / (double)imp->n) * yl) -
                            (((double)imp->nr / (double)imp->n) * yr);
}


void AP_Impurity_Print(AP_Impurity* imp)
{
   printf("Impurity for %s = %f (order=%d):\n",
          gFeatureNames[imp->attr], imp->impurity, imp->order);
   printf("  n=%d, sd=%f, va=%f\n", imp->n, imp->sd, imp->va);
   printf("  nl=%d, sl=%f, s2l=%f, sdl=%f, vl=%f\n",
          imp->nl, imp->sl, imp->s2l, imp->sdl, imp->vl);
   printf("  nr=%d, sr=%f, s2r=%f, sdr=%f, vr=%f\n\n",
          imp->nr, imp->sr, imp->s2r, imp->sdr, imp->vr);
}


void SetAttributesTested(TreeNodePtr tree, int *gAttributesTested)
{
   if (tree->numChildren > 0)
   {
      if (tree->children[0] != NULL)
         SetAttributesTested (tree->children[0], gAttributesTested);
      if (tree->children[1] != NULL)
         SetAttributesTested (tree->children[1], gAttributesTested);
      gAttributesTested[tree->attribute] = TRUE;
   }
}


/* Compute standard deviation of class values. */
float StandardDeviation(int *training, int numtrain)
{
   int i;
   float x, sum=0.0, sqsum=0.0, variance;

   if (numtrain < 2)
	   return 0.0;

   for (i=0; i<numtrain; i++)
   {
      x = labels[training[i]];
      sum += x;
      sqsum += (x * x);
   }
   variance = (sqsum - (sum * sum / (float) numtrain)) / (float) numtrain;
   return(sqrtf(variance));
}


/* Compute absolute deviation of class values. */
float AbsoluteDeviation(int *training, int numtrain)
{
   int i;
   float sum=0.0, avg=0.0, absdev=0.0;

   if (numtrain < 2)
	   return 0.0;

   for (i=0; i<numtrain; i++)
	   sum += labels[training[i]];
   avg = sum / (float) numtrain;

   sum = 0.0;
   for (i=0; i<numtrain; i++)
	   sum += fabs(labels[training[i]] - avg);

   absdev = sum / (float) numtrain;
   return(absdev);
}


/* Solve Ax=b using LU decomposition. Return TRUE if successful.
   I.e., compute upper-triangular matrix L, lower-triangular matrix U,
   and column matrix D such that LU=A and LD=b; then LUx=LD and Ux=D,
   which can be solved using back substitution. If any L[i][i]=0.0,
   then A is singular and no solution exists (return FALSE). */
int SolveMatrix(float** matrixA, float* matrixB, float* matrixX, int n)
{
   int i, j, k;
   float sum;

   // Compute matrix LU
   for (i = 0; i < n; i++)
      gMatrixLU[i][0] = matrixA[i][0];
   if (gMatrixLU[0][0] == 0.0)
      return FALSE;
   for (j = 1; j < n; j++)
      gMatrixLU[0][j] = matrixA[0][j] / gMatrixLU[0][0];
   for (j = 1; j < (n-1); j++)
   {
      for (i = j; i < n; i++)
      {
         sum = 0.0;
         for (k = 0; k < j; k++)
            sum += (gMatrixLU[i][k] * gMatrixLU[k][j]);
         gMatrixLU[i][j] = matrixA[i][j] - sum;
      }
      if (gMatrixLU[j][j] == 0.0)
         return FALSE;
      for (k = j+1; k < n; k++)
      {
         sum = 0.0;
         for (i = 0; i < j; i++)
            sum += (gMatrixLU[j][i] * gMatrixLU[i][k]);
         gMatrixLU[j][k] = (matrixA[j][k] - sum) / gMatrixLU[j][j];
      }
   }
   sum = 0.0;
   for (k = 0; k < (n-1); k++)
      sum += (gMatrixLU[n-1][k] * gMatrixLU[k][n-1]);
   gMatrixLU[n-1][n-1] = matrixA[n-1][n-1] - sum;
   if (gMatrixLU[n-1][n-1] == 0.0)
      return FALSE;

   // Compute matrix D
   gMatrixD[0] = matrixB[0] / gMatrixLU[0][0];
   for (i = 1; i < n; i++)
   {
      sum = 0.0;
      for (j = 0; j < i; j++)
         sum += (gMatrixLU[i][j] * gMatrixD[j]);
      gMatrixD[i] = (matrixB[i] - sum) / gMatrixLU[i][i];
   }

   // Compute solution matrix X
   matrixX[n-1] = gMatrixD[n-1];
   for (i = (n-2); i >= 0; i--)
   {
      sum = 0.0;
      for (j = i+1; j < n; j++)
         sum += (gMatrixLU[i][j] * matrixX[j]);
      matrixX[i] = gMatrixD[i] - sum;
      }

   return(TRUE);
}


/* Recursively print decision tree. */
void AP_PrintDT(TreeNodePtr tree)
{
   int size = TreeSize (tree);
   int leaves = NumLeaves (tree);

   printf("\nDecision Tree (size = %d, leaves = %d)\n", size, leaves);
   if (tree == NULL)
      printf("  Empty tree\n");
   else
   {
      AP_PrintDT1(tree, 2);
      AP_PrintModels(tree);
   }
   printf("\n");
}


/* Recursively print decision tree indented by tab spaces. */
void AP_PrintDT1(TreeNodePtr tree, int tab)
{
   int j;

   if (tree != NULL)
   {
      if (tree->numChildren == 0)
      {
         printf(" LM%d ", tree->modelNumber);
         if (globalStdDev > 0.0)
         {
            printf("(%d/%.3f%%)\n",
	           tree->numInstances,
		   (100.0 * tree->rootMeanSquaredError / globalStdDev));
         }
	 else printf("(%d)\n", tree->numInstances);
      }
      else
      {
         printf("\n");
         for (j=0; j<tab; j++)
            printf(" ");
         printf("%s <= %f :", gFeatureNames[tree->attribute], tree->threshold);
         AP_PrintDT1(tree->children[0], tab+2);
         for (j=0; j<tab; j++)
         printf(" ");
         printf("%s > %f :", gFeatureNames[tree->attribute], tree->threshold);
         AP_PrintDT1(tree->children[1], tab+2);
      }
   }
}


void AP_PrintModels(TreeNodePtr tree)
{
   int i;

   if (tree != NULL)
   {
      if (tree->numChildren > 0)
      {
         AP_PrintModels(tree->children[0]);
         AP_PrintModels(tree->children[1]);
      }
      else
      {
         if (tree->modelNumber > 0)
         {
            printf("LM%d: class =\n", tree->modelNumber);
            for (i = 0; i < NumFeatures; i++)
               if (tree->coefficients[i] != 0.0)
                  printf("        %f * %s +\n",
		         tree->coefficients[i], gFeatureNames[i]);
            printf("        %f\n\n", tree->intercept);
         }
      }
   }
}


void AllocateMatrices()
{
   int i;

   // Matrix A
   gMatrixA = (float **) malloc ((NumFeatures+1) * sizeof (float *));
   if (gMatrixA == NULL)
      MemoryError("AllocateMatrices");
   for (i = 0; i <= NumFeatures; i++)
   {
      gMatrixA[i] = (float *) malloc ((NumFeatures+1) * sizeof (float));
      if (gMatrixA[i] == NULL)
         MemoryError("AllocateMatrices");
   }
   // Matrix B
   gMatrixB = (float *) malloc ((NumFeatures+1) * sizeof (float));
   if (gMatrixB == NULL)
      MemoryError("AllocateMatrices");
   // Matrix X
   gMatrixX = (float *) malloc ((NumFeatures+1) * sizeof (float));
   if (gMatrixX == NULL)
      MemoryError("AllocateMatrices");
   // Matrix LU
   gMatrixLU = (float **) malloc ((NumFeatures+1) * sizeof (float *));
   if (gMatrixLU == NULL)
      MemoryError("AllocateMatrices");
   for (i = 0; i <= NumFeatures; i++)
   {
      gMatrixLU[i] = (float *) malloc ((NumFeatures+1) * sizeof (float));
      if (gMatrixLU[i] == NULL)
         MemoryError("AllocateMatrices");
   }
   // Matrix D
   gMatrixD = (float *) malloc ((NumFeatures+1) * sizeof (float));
   if (gMatrixD == NULL)
      MemoryError("AllocateMatrices");
}


void FreeMatrices()
{
   int i;

   if (gMatrixA != NULL)
   {
      for (i = 0; i <= NumFeatures; i++)
         free(gMatrixA[i]);
      free(gMatrixA);
   }
   free(gMatrixB);
   free(gMatrixX);
   if (gMatrixLU != NULL)
   {
      for (i=0; i<=NumFeatures; i++)
         free(gMatrixLU[i]);
      free(gMatrixLU);
   }
   free(gMatrixD);
}


/* Filter instance down decision tree to lead node, then predict class value. */
float AP_DTClassify(float *instance, TreeNodePtr tree)
{
   while (tree->numChildren > 0)
   {
      if (instance[tree->attribute] <= tree->threshold)
         tree = tree->children[0];
      else tree = tree->children[1];
   }

   return(AP_ClassifyInstance(instance, tree));
}


/* Use regression model stored at node to predict class of given instance. */
float AP_ClassifyInstance(float* instance, TreeNodePtr node)
{
   int i;
   float result = 0.0;

   for (i = 0; i < NumFeatures; i++) {
      result += (node->coefficients[i] * instance[i]);
   }
   result += node->intercept;

   return(result);
}


void AP_TestModel()
{
   FILE *fpmodel;
   char str[MAXSTR];
   int i, j, numa=0, *pactivities=NULL;
   float* predictions; //array of predicted values
   float prediction; //single predicted value (used for multiple-activity prompting)
   float error;
   float predictedSum, actualSum; //sum values for use in correlation coefficient
   float predictedMean, actualMean; //mean values for use in correlation coefficient
   float Combined, Predicted, Actual; //values for different components of the correlation coefficient
   float actualMax, actualMin; //maximium and minimum actual values for computing the normalized RMSE
   TreeNodePtr dt, *dts=NULL;

   AP_ReadData();
   if (pactivity == -1) // Prompt for all activities for which there are models
   {
      for (i=0; i<NumActivities; i++)
      {
	 sprintf(str, "%s/%s", modelfname, activitynames[i]);
         fpmodel = fopen(str, "r");
	 if (fpmodel != NULL)
	 {
	    if (numa == 0)
	    {
	       pactivities = (int *) malloc(sizeof(int));
	       dts = (TreeNodePtr *) malloc(sizeof(TreeNodePtr));
	    }
	    else
	    {
	       pactivities =
	          (int *) realloc(pactivities, (numa+1) * sizeof(int));
	       dts =
	          (TreeNodePtr *) realloc(dts, (numa+1) * sizeof(TreeNodePtr));
	    }
	    pactivities[numa] = i;
	    dts[numa] = ReadDT(fpmodel, NULL);
	    fclose(fpmodel);
	    numa++;
	 }
      }
      for (i=0; i<NumData; i++)
      {
	 for (j=0; j<numa; j++)
	 {
				prediction = AP_DTClassify(data[i], dts[j]);
            if ((prediction >= 0.0) && (prediction < (float) PromptSeconds))
               printf("%s in %f minutes\n",
	          activitynames[pactivities[j]], prediction);
         }
      }
   }
   else
   {
      dt = LoadModel();
      if (dt != NULL) // Assign model numbers to leaves
         AP_AssignModelNumbers(dt, 0);

      // Allocate prediction array:
      predictions = (float *) malloc(NumData * sizeof(float));
      if (predictions == NULL)
    	  MemoryError("AP_TestModel");

      if (Mode != PROMPT)
      {
         maeErr = 0.0;   	 // Compute mean absolute error (MAE)
         mapeErr = 0.0;      // Compute mean absolute percentage error (MAPE)
         msdErr = 0.0;		 // Compute mean signed difference (MSD)
         mseErr = 0.0;		 // Compute mean squared error (MSE)
         rmseErr = 0.0; 	 // Compute root mean squared error (RMSE)
         actualMax = INT_MIN; // Used to store the maximum actual value
         actualMin = INT_MAX; // Used to store the minimum actual value
         nrmseRangeErr = 0.0; // Compute RMSE normalized by the range of actual values
         nrmseMeanErr = 0.0; // Compute RMSE normalized by the mean of actual values
         predictedSum = 0.0; // Sum of predicted values (used for predicted mean in corr coefficient)
         actualSum = 0.0;	 // Sum of actual values (used for actual mean in corr coefficient)
         Combined = 0.0; 	 // Numerator term for coefficient
         Predicted = 0.0;	 // Predicted denominator term for coefficient
         Actual = 0.0;		 // Actual denominator term for coefficient
      }
      for (i=0; i<NumData; i++)
      {
    	  predictions[i] = AP_DTClassify(data[i], dt);
         if (Mode == PROMPT)
         {
            if ((predictions[i] > 0.0) && (predictions[i] < (float) PromptSeconds))
               printf("%s in %f minutes\n",
	          activitynames[pactivity], predictions[i]);
         }
         else
         {
            error = predictions[i] - ((float) labels[i]);
            //printf("%d: Predicted: %.2f Actual: %.2f Error: %.2f\n", i, predictions[i], (float) labels[i], error );
            maeErr += fabsf(error);
            if (labels[i] != 0)
               mapeErr += fabsf(error / (float) labels[i]);
            msdErr += error;
            mseErr += error * error;

            // Compute sums to use in means for correlation coefficient:
            predictedSum += predictions[i];
            actualSum +=  (float) labels[i];

            // Compute the minimum and maximum actual values:
            if ((float) labels[i] > actualMax)
            	actualMax = (float) labels[i];
            if ((float) labels[i] < actualMin)
            	actualMin = (float) labels[i];
         }
      }

//      printf("Maximum actual value: %.2f\n", actualMax);
//      printf("Minimum actual value: %.2f\n", actualMin);

      // Compute Pearson's Correlation Coefficient if not prompting:
      if (Mode != PROMPT) {
    	  predictedMean = predictedSum / (float) NumData;
    	  actualMean = actualSum / (float) NumData;


//    	  printf("timestamp,activity_id,sensor,label,predicted,error\n");
    	  for (i=0; i < NumData; i++) {
    		  Combined += (predictions[i]-predictedMean) * ((float) labels[i] - actualMean);
    		  Predicted += powf(predictions[i] - predictedMean, 2);
    		  Actual += powf((float) labels[i] - actualMean, 2);

//    		  printf("%d, %d, %.0f, %d, %.1f, %.1f\n", timestamp[i], actnums[i], discreteData[i][6], labels[i], predictions[i], predictions[i] - (float) labels[i]);
    	  }

    	  corrCoeff = Combined / (sqrtf(Predicted) * sqrtf(Actual));
      }

      // Compute the other error values:
      // TODO: Move these up into the above if statement?
      maeErr = maeErr / (float) NumData;
      mapeErr = (mapeErr / (float) NumData);
      msdErr = (msdErr / (float) NumData);
      mseErr = (mseErr / (float) NumData);
      if (mseErr > 0.0)
    	  rmseErr = sqrtf(mseErr);
      else rmseErr = NAN;
      nrmseRangeErr = rmseErr/(actualMax - actualMin);
      nrmseMeanErr = rmseErr/actualMean;

      if (predictions != NULL)
    	  free(predictions);
   }
}

// Function to perform sliding window validation on the data
void AP_SlidingWindowValidation() {
	int numSlidingWindows = 0;
	int i, j, totalNumWindows, windowStartIndex, windowStartTime;
	int *trainingIndices;
	float **originalData; // array of arrays to hold original feature data
	TreeNodePtr dt = NULL;

	// Result tracking:
	float *predictions; // store the predictions
	float *actuals; // store the actual (label) values
	float error;
	float predictedSum, actualSum; //sum values for use in correlation coefficient
	float predictedMean, actualMean; //mean values for use in correlation coefficient
	float Combined, Predicted, Actual; //values for different components of the correlation coefficient
	float actualMax, actualMin; //maximium and minimum actual values for computing the normalized RMSE

	// Allocate the array to hold the original feature data and copy it over:
	originalData = (float **) malloc(NumData*sizeof(float *));
	if (originalData == NULL)
		MemoryError("AP_SlidingWindowValidation");

	for (i = 0; i < NumData; i++) {
		originalData[i] = (float *) malloc(NumFeatures * sizeof(float));
		if (originalData[i] == NULL)
			MemoryError("AP_SlidingWindowValidation");
		for (j = 0; j < NumFeatures; j++)
			originalData[i][j] = data[i][j];
	}

	// Initialize the training indices (indices of which values to use for training from the feature data) and list of tested attributed:
	trainingIndices = (int *) calloc(SlidingWindowSize, sizeof(int));
	gAttributesTested = (int *) malloc(NumFeatures * sizeof(int));

	// Compute the number of windows we will have:
	totalNumWindows = (NumData - SlidingWindowSize)/SlidingWindowSkip;
	printf("Will run %d windows\n", totalNumWindows);

	// Initialize the result-tracking values:
	predictions = (float *) malloc(totalNumWindows * sizeof(float));
    if (predictions == NULL)
  	  MemoryError("AP_SlidingWindowValidation");
	actuals = (float *) malloc(totalNumWindows * sizeof(float));
    if (actuals == NULL)
  	  MemoryError("AP_SlidingWindowValidation");
    maeErr = 0.0;   	 // Compute mean absolute error (MAE)
    mapeErr = 0.0;      // Compute mean absolute percentage error (MAPE)
    msdErr = 0.0;		 // Compute mean signed difference (MSD)
    mseErr = 0.0;		 // Compute mean squared error (MSE)
    rmseErr = 0.0; 	 // Compute root mean squared error (RMSE)
    actualMax = INT_MIN; // Used to store the maximum actual value
    actualMin = INT_MAX; // Used to store the minimum actual value
    nrmseRangeErr = 0.0; // Compute RMSE normalized by the range of actual values
    nrmseMeanErr = 0.0; // Compute RMSE normalized by the mean of actual values
    predictedSum = 0.0; // Sum of predicted values (used for predicted mean in corr coefficient)
    actualSum = 0.0;	 // Sum of actual values (used for actual mean in corr coefficient)
    Combined = 0.0; 	 // Numerator term for coefficient
    Predicted = 0.0;	 // Predicted denominator term for coefficient
    Actual = 0.0;		 // Actual denominator term for coefficient

	// Loop through, starting with the first event, and work on windows of the specified size.
	// Skip a number of events between window starts determined by the skip parameter.
	for (numSlidingWindows = 0; numSlidingWindows*SlidingWindowSkip + SlidingWindowSize < NumData; numSlidingWindows++) {
		windowStartIndex = numSlidingWindows*SlidingWindowSkip;

		// We want to treat the window as a self-contained unit.  As such, we will need to change
		// the feature vectors in the window to act as if they are a self-contained data file.
		// Thus, the first MAXWINDOW-1 events of the window are treated as the "buffering"
		// data and not used.  Further, we cannot use events before the MaxSampleLag has been
		// reached from the start of the dataset (if using sampling).
		// After these two conditions are met, we can begin to use feature vectors.  However,
		// there are some features that need to be adjusted to account for the lack of previous
		// window data.  This includes removing the previous window dominant sensors for the first
		// two sub-windows, as well as adjusting the "time since sensor last fired" values for all events
		// (since the window doesn't know about previous windows' sensor firings).
		// It would probably be best to use a function to do this so we can reuse it for leave-day-out, etc
		windowStartTime = timestamp[windowStartIndex]; // get the timestamp of the first event in the window

		// TODO: Implement this

		// Set the indices of the feature data that we want to use for training with this window:
		for (i = 0; i < SlidingWindowSize; i++) {
			trainingIndices[i] = windowStartIndex + i;
		}
		globalStdDev = StandardDeviation(trainingIndices, SlidingWindowSize);
		globalAbsDev = AbsoluteDeviation(trainingIndices, SlidingWindowSize);
		for (i = 0; i < NumFeatures; i++)
		  gAttributesTested[i] = FALSE;

		// Create the tree
		printf("Building tree %d of %d\n", numSlidingWindows, totalNumWindows);
		dt = AP_BuildTree(trainingIndices, SlidingWindowSize, NULL);

		// If pruning, then prune, else install linear models
		if (dt != NULL)
		{
		  if (AP_PRUNE == TRUE)
		  {
			 if (OutputLevel > 0)
				printf("Pruning...");
			 AP_PruneTree(dt);
			 if (OutputLevel > 0)
				printf("done\n");
		  }
		  else AP_InstallLinearModels(dt);
		}

		// If smoothed predictions, then install smoothed models
		if ((dt != NULL) && (AP_SMOOTH_PREDICTIONS))
		{
			printf("Smoothing models...\n");
			AP_InstallSmoothedModels(dt);
		}

		// Assign model numbers to leaves
		if (dt != NULL)
			AP_AssignModelNumbers(dt, 0);


		// Now classify the next point:
		predictions[numSlidingWindows] = AP_DTClassify(data[windowStartIndex + SlidingWindowSize], dt);
		actuals[numSlidingWindows] = (float) labels[windowStartIndex + SlidingWindowSize];

		// Compute error and parts of performance measures:
        error = predictions[numSlidingWindows] - actuals[numSlidingWindows];
        printf("%d: Predicted: %.2f Actual: %.2f Error: %.2f\n", numSlidingWindows, predictions[numSlidingWindows], actuals[numSlidingWindows], error );
        maeErr += fabsf(error);
        if (actuals[numSlidingWindows] != 0)
           mapeErr += fabsf(error / actuals[numSlidingWindows]);
        msdErr += error;
        mseErr += error * error;
        // Compute sums to use in means for correlation coefficient:
        predictedSum += predictions[numSlidingWindows];
        actualSum +=  actuals[numSlidingWindows];
        // Compute the minimum and maximum actual values:
        if (actuals[numSlidingWindows] > actualMax)
        	actualMax = actuals[numSlidingWindows];
        if (actuals[numSlidingWindows] < actualMin)
        	actualMin = actuals[numSlidingWindows];


		// Restore the previous-window features of the first two feature windows' worth of data
		// TODO: Implement this
	}

    // Compute Pearson's Correlation Coefficient:
	predictedMean = predictedSum / (float) totalNumWindows;
	actualMean = actualSum / (float) totalNumWindows;

	for (i=0; i < totalNumWindows; i++) {
	  Combined += (predictions[i]-predictedMean) * ((float) actuals[i] - actualMean);
	  Predicted += powf(predictions[i] - predictedMean, 2);
	  Actual += powf((float) actuals[i] - actualMean, 2);
	}

	corrCoeff = Combined / (sqrtf(Predicted) * sqrtf(Actual));

    // Compute the other error values:
    maeErr = maeErr / (float) totalNumWindows;
    mapeErr = (mapeErr / (float) totalNumWindows);
    msdErr = (msdErr / (float) totalNumWindows);
    mseErr = (mseErr / (float) totalNumWindows);
    if (mseErr > 0.0)
  	  rmseErr = sqrtf(mseErr);
    else rmseErr = NAN;
    nrmseRangeErr = rmseErr/(actualMax - actualMin);
    nrmseMeanErr = rmseErr/actualMean;

    // Show the results:
	printf("Activity prediction performance:\n");
	printf("   mean absolute error = %f\n",
			maeErr);
	printf("   mean absolute percentage error = %f\n",
			mapeErr);
	printf("   mean signed difference error = %f\n",
			msdErr);
	printf("   mean square error = %f\n",
			mseErr);
	printf("   root mean standard error = %f\n",
			rmseErr);
	printf("   range-normalized root mean standard error = %f\n",
			nrmseRangeErr);
	printf("   mean-normalized root mean standard error = %f\n",
			nrmseMeanErr);
	printf("   Pearson's correlation coefficient = %f\n",
			corrCoeff);


    // Clean up memory:
	if (originalData != NULL) {
		for (i = 0; i < NumData; i++)
			if (originalData[i] != NULL)
				free(originalData[i]);
		free(originalData);
	}
	if (trainingIndices != NULL)
		free(trainingIndices);
	if (predictions != NULL)
		free(predictions);
	if (actuals != NULL)
		free(actuals);
	FreeTree(dt);

}
