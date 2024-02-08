#include "al.h"

static int numPatterns = 0;
static PatternInfo *patInfo = NULL;
long int lroundf(float x);
double **distancematrix;                // *** added by ck 11/2/2011


// Keep track of sensors included in each pattern definition
void AddPatternInfo(Pattern *pattern, int patNumber)
{
   int i, nl;

   if (numPatterns == 0)
      patInfo = (PatternInfo *) malloc(sizeof(PatternInfo));
   else patInfo =
      (PatternInfo *) realloc(patInfo, (numPatterns+1) * sizeof(PatternInfo));

   if ((patNumber > numPatterns) || (patInfo == NULL))
      MemoryError("AddPatternInfo");
   patInfo[patNumber].meantime = pattern->meantime;
   nl = pattern->definition->numEvents;
   patInfo[patNumber].numLabels = nl;
   patInfo[patNumber].patLabels = (int *) malloc(nl * sizeof(int));
   patInfo[patNumber].numInstances = pattern->numInstances;
   if (patInfo[patNumber].patLabels == NULL)
      MemoryError("AddPatternInfo");
   for (i=0; i<nl; i++)
      patInfo[patNumber].patLabels[i] =
         events[pattern->definition->events[i]].label;
   numPatterns++;

   return;
}


// Remove the last pattern in the list.  This may be used if the last
// pattern does not achieve any compression.
void RemoveLastPattern()
{
   numPatterns--;

   return;
}

void writePattern(FILE *f,int label)
{
  char* buffer = adlabels[label];
  fprintf(f,buffer);
  buffer = " ";
  fprintf(f,buffer);
}

// Cluster similar patterns together to form a smaller number of more
// general patterns
void ClusterPatterns(FILE *fp)
{
   int i, j, k, merged;
   int *clusterassignments, nClusters;

   origLabels = numlabels - numPatterns;  // Index of last original label
   FlattenPatterns();

   FILE *fd;
   //FILE *fdArff;
   //char* arff = (char*) malloc(strlen(filename)+1);
   //strcpy (arff,filename);
   strcat(filename,"_.txt");
   //strcat(arff,".arff");
   printf("\nCreation of file %s\n", filename);
   fd=fopen(filename, "w");
   if(fd==NULL) {
	  perror("Errore in apertura del file");
	  exit(1);
   }
   //printf("Creation of arff file %s\n", arff);
   //fdArff=fopen(arff, "w");
   //if(fdArff==NULL) {
	//  perror("Errore in apertura del arff file");
	//  exit(1);
   //}
   

   if (OutputLevel > 0)
   {
      printf("\n");
      //fprintf(fdArff, "@relation dataset\n\n");
      //fprintf(fdArff, "@attribute trace string\n\n");
      //fprintf(fdArff, "@data\n");
      for (i=0; i<numPatterns; i++)
      {
         // printf("Pattern (%d): ", i);
	 printf("Pattern (%d) numInstances (%d): ", i,patInfo[i].numInstances);
	 //fprintf(fdArff, "'");
         for (j = 0; j < patInfo[i].numLabels; j++)
         {
            PrintLabel(patInfo[i].patLabels[j]);
            printf(" ");
	    writePattern(fd,patInfo[i].patLabels[j]);
	    //fprintf(fdArff,"%s",adlabels[patInfo[i].patLabels[j]]);
	    //if(j != patInfo[i].numLabels-1)
		//fprintf(fdArff, " ");
         }
	 //fprintf(fdArff, "'");
	 fprintf(fd, "%d", patInfo[i].numInstances);
         
	 printf("\n");
	 //fprintf(fdArff,"\n");
         fprintf(fd,"\n");
         
      }
   }
    
   fclose(fd);

   /**
   // learn the Markov model associated with every pattern
   if (OutputLevel > 0)
      printf("\nLearning Markov models...\n");
   LearnMarkovModel(fp);
    
   // compute the distance between every pair of patterns
   if (OutputLevel > 0)
      printf("computing distance between patterns...\n");
   ComputeDistanceMatrix();
    
   clusterassignments = (int *) malloc(numPatterns * sizeof(int));
   nClusters = HCluster(distancematrix, numPatterns, clusterassignments);

   for (i=0; i<numPatterns; i++)
      patInfo[i].clusternumber = clusterassignments[i];
    
   for (i=0; i<nClusters; i++)
   {
      printf("Cluster %d\n", i);
      for (j=0; j<numPatterns; j++)
      {
         if (patInfo[j].clusternumber == i)
         {
            printf("   [%d]: ", j);
            for (k=0; k<patInfo[j].numLabels; k++)
            {
               PrintLabel(patInfo[j].patLabels[k]);
               printf(" ");
            }
            printf("\n");
         }
      }
   }
    
   for (i=numPatterns-1; i>0; i--)   // Merge patterns if sufficiently similar
   {
      merged = 0;
      for (j=i-1; j>=0 && merged==0; j--)
      {
	 // Determine if patterns belong to the same cluster
         if (patInfo[i].clusternumber == patInfo[j].clusternumber)
         {
            MergePatterns(j, i);     // If similar merge patterns into cluster
            merged = 1;
         }
      }
   }
   **/
   return;
}


int HCluster(double **distance, int npoints, int *clusterassignments)
{
   int i, j, x, y, sizetree, **tree, *sizenodes, size, numclusters;

   tree = (int **) malloc(npoints * sizeof(int *));
   sizenodes = (int *) malloc(npoints * sizeof(int));
   sizetree = npoints;

   for (i=0; i<npoints; i++)
   {
      tree[i] = (int *) malloc(npoints * sizeof(int));
      sizenodes[i] = 1;
      tree[i][0] = i;
   }

   numclusters = (int) ((float) numPatterns * CLUSTERRATIO);
   //numclusters = 3;

   while (sizetree > numclusters)
   {
      PickClosestNodes(&x, &y, tree, sizetree, sizenodes);
      size = sizenodes[x] + sizenodes[y];
      
      for (i=sizenodes[x]; i<size; i++)
         tree[x][i] = tree[y][i-sizenodes[x]];
      sizenodes[x] = size;

      for (i=y; i<sizetree-1; i++)        // Move the rest of the clusters down
      {
         sizenodes[i] = sizenodes[i+1];
         for (j=0; j<sizenodes[i]; j++)
	    tree[i][j] = tree[i+1][j];
      }
      sizetree--;
   }

   for (i=0; i<sizetree; i++)
      for (j=0; j<sizenodes[i]; j++)
         clusterassignments[tree[i][j]] = i;

   return(sizetree);
}


void PickClosestNodes(int *x, int *y, int **tree, int sizetree, int *sizenodes)
{
   int i, j, k, l, count, min1=0, min2=0;
   float total, min=-1.0;

   for (i=0; i<sizetree; i++)
      for (j=i+1; j<sizetree; j++)
      {
         total = 0;
	 count = 0;
	 for (k=0; k<sizenodes[i]; k++)
	    for (l=0; l<sizenodes[j]; l++)
	    {
	       total += fabs(distancematrix[tree[i][k]][tree[j][l]]);
	       count++;
	    }
         total /= (float) count;
	 if ((total < min) || (min == -1.0))
	 {
	    min = total;
	    min1 = i;
	    min2 = j;
	 }
      }

   *x = min1;
   *y = min2;
}


// Merge two patterns together into one pattern definition
void MergePatterns(int x, int y)
{
   int i, temp;

   if (x == y)
      return;

   if (x > y)   // Swap x and y
   {
      temp = y;
      y = x;
      x = temp;
   }
   if (OutputLevel > 1)
      printf("Replacing pattern %d with pattern %d, origLabels %d\n",
         y, x, origLabels);
   // Replace the higher number pattern label with the
   // lower number pattern label.  Keep the rest of the original
   // information the same.
   for (i=0; i<nevents; i++)
      if (events[i].label == (y + origLabels))
          events[i].label = x + origLabels;
   return;
}


// Replace pattern labels with pattern definition sequence
void FlattenPatterns()
{
   int i, j, nl;
   BOOLEAN flat;

   for (i=0; i<numPatterns; i++)
   {
      flat = FALSE;
      while (flat == FALSE)
      {
	 nl = patInfo[i].numLabels;
	 flat = TRUE;
	 for (j=0; j<nl; j++)
	 {
	    if (patInfo[i].patLabels[j] > (origLabels-1))
	    {
	       Splice(i, j, patInfo[i].patLabels[j] - origLabels);
	       flat = FALSE;
	    }
	 }
      }
   }

   return;
}


// Replace pattern label with the definition for the corresponding pattern
void Splice(int old, int pos, int sub)
{
   int i, nl, subnl;
   PatternInfo subpat;

   subpat = patInfo[sub];
   subnl = subpat.numLabels;
   nl = patInfo[old].numLabels;
   patInfo[old].numLabels += subnl - 1;
   if (subnl > 1)    // No need to expand if replacing with pattern of size one
      patInfo[old].patLabels =
         (int *) realloc(patInfo[old].patLabels,
	                 patInfo[old].numLabels * sizeof(int));
   if (patInfo[old].patLabels == NULL)
      MemoryError("Splice");

   for (i=nl-1; i>pos; i--)                 // Shift remainder of pattern
      patInfo[old].patLabels[i+subnl-1] = patInfo[old].patLabels[i];
   for (i=0; i<subnl; i++)                  // Insert substitute pattern
      patInfo[old].patLabels[pos+i] = subpat.patLabels[i];

   return;
}


// learn a first order Markov model for every pattern
void LearnMarkovModel(FILE *fp)
{
   char *cptr = NULL, buffer[256], alabel[MAXSTR], *pat=NULL, c;
   char date[MAXSTR], time[MAXSTR], sensorid[MAXSTR], sensorvalue[MAXSTR];
   int i, j, k, numevents=0, num = 0, patnum=0, *totals, prev=-1, sid, tval;

   totals = (int *) malloc(sizeof(int) * numPatterns);
   for (i=0; i<numPatterns; i++)
   {
      // initialize the aprior probability vector and
      // transition matrix for the first order Markov model
      patInfo[i].initprob =
         (double *) malloc(origLabels * sizeof(double));
      patInfo[i].transmat =
         (double **) malloc(origLabels * sizeof(double *));
      patInfo[i].counts =
         (double *) malloc((origLabels + 4) * sizeof(double));

      for (j=0; j<origLabels+4; j++)
         patInfo[i].counts[j] = 0;

      for (j=0; j<origLabels; j++)
      {
         patInfo[i].initprob[j] = 0;
         patInfo[i].transmat[j] =
	    (double *) malloc(origLabels * sizeof(double));
         for (k=0; k<origLabels; k++)
            patInfo[i].transmat[j][k] = 0;
      }
      totals[i] = 0;
   }

   rewind(fp);     // Process the original file with sensor events
   cptr = fgets(buffer, 256, fp);
   numevents++;    // Line number in the file
   while (cptr != NULL)
   {
      sscanf(buffer, "%s %s %s %s %s",
             date, time, sensorid, sensorvalue, alabel);
      if (num < nevents)
      {
         pat = strstr(adlabels[events[num].label], "Pat_");
      }
      if ((pat != NULL) && (num < nevents))
      {
	 if (numevents <= events[num].end)
	 {
            sscanf(pat, "%c%c%c%c%d", &c, &c, &c, &c, &patnum);
	    totals[patnum]++;
	    sid = MapSensors(sensorid);
	    patInfo[patnum].counts[sid] += 1.0;
	    tval = ComputeHour(time);       // factor time into matrix
	    tval /= 6;
	    patInfo[patnum].counts[origLabels+tval] += 1.0;
	    if (prev == -1)                     // compute initial probability
	    {
               patInfo[patnum].initprob[sid] += 1.0;
	       prev = sid;
	    }
	    else if (num != 0)            // compute the transition probability
               patInfo[patnum].transmat[prev][sid] += 1.0;
	 }
	 if (numevents >= events[num].end)
	 {
	    num++;
	    prev = -1;
	 }
      }
      else num++;    // advance to next event
      numevents++;   // advance to next line of the file

      cptr = fgets(buffer, 256, fp);
   }

   for (i=0; i<numPatterns; i++)
      for (j=0; j<origLabels; j++)
      {
	 if (totals[i] == 0)
	 {
            patInfo[i].initprob[j] = 0.0;
	    patInfo[i].counts[j] = 0.0;
	 }
         else
	 {
	    patInfo[i].initprob[j] /= (double) totals[i];
	    patInfo[i].counts[j] /= (double) totals[i];
         }
      }

   for (i=0; i<numPatterns; i++)           // Add time features
      for (j=origLabels; j<origLabels+4; j++)
	 if (totals[i] == 0)
	    patInfo[i].counts[j] = 0.0;
         else patInfo[i].counts[j] /= (double) totals[i];
      
   for (i=0; i<numPatterns; i++)
      for (j=0; j<origLabels; j++)
         for (k=0; k<origLabels; k++)
	 {
	    if (totals[i] == 0)
               patInfo[i].transmat[j][k] = 0.0;
            else patInfo[i].transmat[j][k] /= (double) totals[i];
         }
}


// compute the distance between two patterns
// Calculate KL divergence of MM
// KL(P,Q) = sum over i of P(i) * ln(P(i) / Q(i))
// To make symmetric, compute KL(P,Q) * KL(Q,P)
// Multiply KL divergence with bag of sensors difference
double ComputePatDistance(int patx, int paty)
{
   int i, j;
   double distance = 0.0, v1, v2;

   for (i=0; i<origLabels; i++)          // KL divergence for init prob
   {
      if (patInfo[patx].initprob[i] == 0.0)
         v1 = MIN;
      else v1 = patInfo[patx].initprob[i];
      if (patInfo[paty].initprob[i] == 0.0)
         v2 = MIN;
      else v2 = patInfo[paty].initprob[i];
      distance += fabs((v1 * log(v1 / v2)) * (v2 * log(v2 / v1)));
   }

   for (i=0; i<origLabels; i++)          // KL divergence for transition prob
   {
      for (j=0; j<origLabels; j++)
      {
         if (patInfo[patx].transmat[i][j] == 0.0)
	    v1 = MIN;
         else v1 = patInfo[patx].transmat[i][j];
         if (patInfo[paty].transmat[i][j] == 0.0)
	    v2 = MIN;
         else v2 = patInfo[paty].transmat[i][j];
         distance += fabs((v1 * log(v1 / v2)) * (v2 * log(v2 / v1)));
      }
   }

   for (i=0; i<origLabels+4; i++)        // Bag of sensors difference
      distance += fabs(patInfo[patx].counts[i] - patInfo[paty].counts[i]);

   return(distance);
}


// compute the distance matrix between every pattern
void ComputeDistanceMatrix()
{
   int i, j;
    
   // initialize the affinity matrix.
   distancematrix = (double **) malloc(numPatterns * sizeof(double *));
   for (i=0; i<numPatterns; i++)
      distancematrix[i] = (double *) malloc(numPatterns * sizeof(double));
 
   for (i=0; i<numPatterns; i++)
      for (j=0; j<numPatterns; j++)
         distancematrix[i][j] = 0;
    
   // compute each element of the affinity matrix
   for (i=0; i<numPatterns; i++)
      for (j=i; j<numPatterns; j++)
         distancematrix[i][j] = distancematrix[j][i] =
	    ComputePatDistance(i, j);
    
   if (OutputLevel > 2)
   {
      printf("%dx%d distance matrix between the patterns:\n   ",
             numPatterns, numPatterns);
      for (i=0; i<numPatterns; i++)
      {
	 printf("%d ", i);
         for (j=0; j<numPatterns; j++)
            printf("%.3f ", distancematrix[i][j]);
         printf("\n   ");
      }
      printf("\n");
   }
}
