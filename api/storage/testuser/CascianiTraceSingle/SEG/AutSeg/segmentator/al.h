/*------------------------------------------------------------------------------
 *       Filename:  al.h
 *
 *    Description:  Header file for activity learning (recognition & prediction)
 *
 *        Version:  AL 1.0
 *        Created:  05/24/2013
 *        Authors:  Diane J. Cook, cook@eecs.wsu.edu
 *                  Larry Holder, holder@wsu.edu
 *   Organization:  Washington State University
------------------------------------------------------------------------------*/

#ifndef AL_H
#define AL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include "sys/stat.h"

#define MAXSTR 80			// Maximum string size
#define BUFFERSIZE 50000
#define TRAIN 0
#define TEST 1
#define BOTH 2
#define TRACK 3
#define PROMPT 4
#define NONE 0
#define TIMEWEIGHT 1
#define MUTUALINFO 2
#define MUTUALINFO_TIME 3
#define NUMWINDOWS 4
#define MAXWINDOW 30
#define LN 2.30265441084134
#define MIN 0.0000000001
#define KAPPA 0.125
#define TRUE 1
#define FALSE 0
#define HOUR 0
#define SENSOR1 1
#define SENSOR2 2
#define SECONDS 3
#define EQ 0
#define LT 1
#define GE 2
#define MINVALUEDIFF 0.00001
#define WRITEDATA TRUE
#define PMARGIN 0.1
#define TREE 0
#define LEAF 1
#define SECONDSINADAY 86400
#define ANNOTATE FALSE
#define LIVE TRUE
#define AL_MODE_AR 0
#define AL_MODE_AP 1
#define AL_MODE_AD 2
#define AL_MODE_ANN 3
#define AL_MODE_DB 4
#define AL_MODE_RTP 5 //real-time prompting mode
#define NOSAMPLE 0 //sampling switch (for whether to compute sampling features)
#define SAMPLE 1
#define CHECKOTHER TRUE
#define LOW 0.02425
#define HIGH 0.97575
#define MOTION 0
#define DOOR 1
#define LIGHT 2
#define CABINET 3
#define TEMPERATURE 4
#define LIGHTLEVEL 5
#define K 3
#define CLEANUP FALSE
#define CLASSIFYTHRESHOLD 0.8
#define PREFIX "/net/files/home/cook/vis/"
#define WEARABLE FALSE
#define NUMAXES 6

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define absd(x) ((x < (double) 0.0) ? -x : x)

// Sampling-specific options:
#define ON 1
#define OFF 0
#define MSGON "ON" // constant strings for event messages
#define MSGOFF "OFF"
#define MSGOPEN "OPEN"
#define MSGCLOSED "CLOSE"

// AR-specific options
#define CMATRIX TRUE			// default=TRUE
#define UNDERSAMPLE FALSE		// default=TRUE
#define AR_MINNUMCLASS 8		// default=8
#define AR_MINNUMOBJ 2			// default=2
#define CF 0.25				// default=0.25
#define AR_PRUNE TRUE			// default=TRUE
#define COLLAPSETREE TRUE		// default=TRUE
#define USEMDLCORRECTION TRUE		// default=TRUE

// AP-specific options
#define AP_PRUNE TRUE			// default=TRUE
#define AP_REGRESSION_TREE FALSE	// default=FALSE (use model tree)
#define AP_SMOOTH_PREDICTIONS TRUE	// default=TRUE
#define AP_SMOOTHING_CONSTANT 15	// default=15
#define AP_MINNUMCLASS 2		// default=2
#define AP_MINNUMOBJ 4			// default=4
#define MAXLAG 12			// default=12
#define RIDGE 0.00000001		// default=0.00000001
#define RIDGE_MAX 100.0			// default=100.0
#define MINSTDDEVFRAC 0.05		// default=0.05
#define MINPURITY 0.000001		// default=0.000001
#define AP_VAL_MODE_SLIDING 0	// sliding window, test-next-event cross-validation mode
#define AP_VAL_MODE_DAY_OUT 1	// leave-a-day-out cross-validation mode

// AD-specific options
#define NEW 2                // First event in a pattern
#define PREDEFINED 3         // Event part of predefined activity, default is 3
#define BEFORE 0
#define AFTER 1
#define BEST 3               // Default is 3
#define TWEIGHT 0.01         // Default is 0.01
#define EXACT 0
#define INEXACT 1
#define MATCHTHRESHOLD 0.2   // Default is 0.1
       // Number of discover+compress iterations, -1 means go while compression
#define PATLABEL "Pat"
#define ALLOWOVERLAP 0       // Default is no overlap, 0
   // 0 = compute MDL, 1 = approximate by estimating size, 2 = ignore pat size
#define EVALUATE 1
#define MITHRESHOLD 0.01      // Default is 0.1
#define NUMCLUSTERS 12
#define CLUSTERRATIO 0.5
#define HC 1
#define CLUSTERTYPE HC


// Decision tree node
typedef struct TreeNode
{
   int numChildren;
   int attribute;
   int numInstances;
   int *instances;
   float threshold;
   struct TreeNode *parent;
   struct TreeNode **children;

   int Class;		// AR: activity class for this node
   int numRight;	// AR: #correct activity classifications at this node
   float entropy;	// AR: entropy at this node
   float *adist;        // AR: probability districution over class labels

   int *indices;	// AP: index of features to be used in regression
   int numIndices;	// AP: #indices
   float *coefficients;	// AP: this node's linear model feature coefficients
   float intercept;	// AP: this node's linear model intercept
   int numParameters;	// AP: #parameters used in this subtree
   int modelNumber;	// AP: #linear models used in this node (for printing)
   float rootMeanSquaredError;	// AP: model error
} TreeNode, *TreeNodePtr;

// AR structure to hold information about best split for each attribute
typedef struct
{
   int valid;
   float threshold;
   float infoGain;
   float gainRatio;
} AR_SplitInfo;

// AP structure to hold information about best split for each attribute
// Based on Weka's M5P YongSplitInfo
typedef struct
{
   int number;         // total instances
   int first;          // first instance index
   int last;           // last instance index
   int position;       // position of maximum impurity reduction
   double maxImpurity; // maximum impurity reduction
   double leftAve;     // left average class value
   double rightAve;    // right average class value
   int splitAttr;      // splitting attribute
   double splitValue;  // splitting value
} AP_SplitInfo;

// AP structure to maintain information about split impurity
// Based on Weka's M5P Impurity
typedef struct
{
   int n;		// total instances
   int attr;		// splitting attribute
   int nl;		// instances in left group
   int nr;		// instances in right group
   double sl;		// sum of left group
   double sr;		// sum of right group
   double s2l;		// squared sum of left group
   double s2r;		// squared sum of right group
   double sdl;		// standard deviation of left group
   double sdr;		// standard deviation of right group
   double vl;		// variance of left group
   double vr;		// variance of right group
   double sd;		// overall standard deviation
   double va;		// overall variance
   double impurity;	// impurity value;
   int order;		// order = 1, variance; order = 2, standard deviation;
   			// order = k, the k-th order root of the variance
} AP_Impurity;

typedef unsigned char BOOLEAN;

typedef struct
{
   int dow;
   int time;
   int sensorid;
   int sensorvalue;
   int label;
   int copy;
   int begin, end;
   int patsize;
} Event;

typedef struct _instance
{
   int numEvents;
   int *events;
   BOOLEAN used;
   struct _instance *parentInstance;
} Instance;

typedef struct _instance_list_node
{
   Instance *instance;
   struct _instance_list_node *next;
} InstanceListNode;

typedef struct
{
   InstanceListNode *head;
} InstanceList;

typedef struct
{
   int numEvents;
   int *events;
} Seq;

typedef struct
{
   Seq *definition;
   int numInstances;
   InstanceList *instances;
   float value;
   float meantime;
   float stddevtime;
} Pattern;

// PatternList Node: node in singly-linked list of sequence patterns
typedef struct _pattern_list_node
{
   Pattern *pattern;
   struct _pattern_list_node *next;
} PatternListNode;

// PatternList: singly-linked list of sequence patterns
typedef struct
{
   PatternListNode *head;
} PatternList;

typedef struct
{
   int numLabels;
   int *patLabels;
   int clusternumber;
   float meantime;
   double **transmat;               // HMM transition probabilities
   double *initprob;                // HMM initial probabilities
   double *counts;                  // Counts for bag of features
   int numInstances;
} PatternInfo;

// Sensor translation
struct Translation
{
   char name[MAXSTR];
   char trans1[MAXSTR];
   char trans2[MAXSTR];
};


// Global variables (AR, AP, AD)
extern char *filename;
extern char *trainname;
extern char *modelfname;
extern char *sitename;
extern char **activitynames;
extern char **gFeatureNames;
extern char **sensornames;
extern char testprevdate[MAXSTR];
extern char testdate[MAXSTR];
extern int Mode;
extern int TopN;
extern int Weight;
extern int OutputLevel;
extern int IgnoreOther;
extern int *sensortypes;
extern int NumActivities;
extern int NumData;
extern int NumDiscreteSensors; // the number of discrete event sensors
extern int NumSensors; // the total number of sensors (including sampling-only sensors)
extern int windata[MAXWINDOW][4];
extern int wincnt;;
extern int PrevActivity;
extern int CurActivity;
extern int *labels;
extern int NumRight;
extern int totalfreq;
extern int *sensortimes;
extern int currenttime;
extern int prevdays;
extern int NumDiscreteFeatures; // number of discrete-event-sensor features
extern int NumFeatures; // total number of features (including sampling features if used)
extern int ALMode;
extern int Translate;
extern float **discreteData; // discrete-sensor-based feature data
extern float **data; // both discrete-sensor-based and sampling-based feature data
extern int *actnums; // activity id values for each sensor event
extern float *tempdata;
extern float **MI;
extern float pmargin2;
extern struct Translation *Translations;
extern TreeNodePtr globaldt;

// Sampling variables; many initialized in globals.c:
extern int SamplingMode;
extern int SampleInterval;
extern float **sampleStateVectors;
extern int NumSamples;
extern float *stateVector;
extern int firstTimeSampling;
extern int firstEventSamplingFlag;
extern int MaxSampleLag;
extern int MaxSamplesToKeep;
extern int numBins;
extern int SampleWindowCount;
extern int NumSampleFeatures;
extern float *tempdataSamples;
extern float **samplingData;
extern int NumSampleData;

// Statistical values:
extern float *samplingMax;
extern float *samplingMin;
extern float *samplingSum;
extern float *samplingMean;
extern float *samplingMedian;
extern float *samplingStdDev;
extern float **sortedValues;

// Dynamic window variables (AR and AP); initialized in globals.c
extern int **actwind;
extern int *factwind;
extern int **senact;
extern int *fsenact;

// Dominant sensor ids for previous three windows (AR and AP)
// initialized in globals.c
extern int numwin;
extern int dominant;
extern int prevwin1;
extern int prevwin2;

extern float Val[9];
extern float Dev[9];

// AR-specific global variables; initialized in globals.c
extern int *classfreq;
extern int **cmatrix;
extern int *gFreq1;
extern int *gFreq2;
extern float *gValues;
extern int gAtt;

// AP-specific global variables; initialized in globals.c
extern float globalStdDev;
extern float globalAbsDev;
extern int *gAttributesTested;
extern char *pactivityname;
extern char **datetime;
extern int *atimes;
extern int aindex;
extern int pactivity;
extern int other;
extern int *timestamp;
extern int PromptSeconds;
extern time_t firsttime;
extern time_t lasttime;
extern float maeErr;
extern float mapeErr;
extern float msdErr;
extern float mseErr;
extern float rmseErr;
extern float nrmseRangeErr;
extern float nrmseMeanErr;
extern float corrCoeff;
extern float **gMatrixA;
extern float *gMatrixB;
extern float *gMatrixX;
extern float **gMatrixLU;
extern float *gMatrixD;
extern int APValMode; // AP cross-validation mode
extern int SlidingWindowSize; // sliding window size for cross-validation
extern int SlidingWindowSkip; // number of events to skip between starts of subsequent windows (regardless of window size)

// AD-specific global variables; initialized in globals.c
extern char **adlabels;
extern char *TranslationFileName;
extern int nevents;
extern int numlabels;
extern int datasize;
extern int origLabels;
extern int iteration;
extern int **sim;
extern int numsim;
extern int NumTranslations;
extern int NumIterations;
extern float **mi;
extern Event *events;

// Global prototypes (AR, AP, AD)
void ReadConfig(char *name);  	// Read the configuration file
void FreeMemory();
void PrintParms();		// Print the program parameter values
void SaveParams();            	// Save the training parameters
// Compute the activity probability distributions for alternative sensor windows
void ComputeProbActWind();
// Compute the activity probability distribution for each sensor
void ComputeProbSenAct();
void ComputeMI();		// Compute mutual information for sensor pairs
// Read data from individual files and update current window parameters
void ReadData();
void SetFeatureNames();
void CollapseTree(TreeNodePtr tree);
void FreeTree(TreeNodePtr tree);
void SaveDT(TreeNodePtr tree);
void WriteDT(FILE *fpmodel, TreeNodePtr tree);
void MemoryError(const char *context);
void Error(const char *context);
char *TrimStr(char *buffer);	// Trim white space at the end of the string
int ReadString(char *str, int pos, char *arg, int length);
int FindActivity(char *name);	// Find the index of an activity from a list
int FindSensor(char *name);   	// Find the index of a sensor from a list
int ComputeFeature(char *time, char *sensorid1, char *sensorid2,
                   float *tempdata);
int ComputeYear(char *dstr);	// Compute year from date
int ComputeDow(char *date);
int ComputeMonth(char *dstr); 	// Compute month from date
int ComputeDay(char *dstr);   	// Compute day from date
int ComputeHour(char *time);	// Compute hour of day based on 24 hour clock
int ComputeMinute(char *time);	// Compute minute of the hour
int ComputeSecond(char *time);	// Compute seconds of the minute
int ComputeSeconds(char *time);	// Compute seconds past midnight
int GetWindow(char *name);	// Compute best window size for the given sensor
int TreeSize(TreeNodePtr tree);
int NumLeaves(TreeNodePtr tree);
int GetTrainingErrors(TreeNodePtr tree);
int NumInClass(TreeNodePtr tree, int majorclass, int type);
int FindClass(int *training, int numtrain, int att, float value);
int CompactInst(int *training, int numtrain, int att, float val);
int DTClassify(float *data, TreeNodePtr tree, int minNumClass, int *small);
time_t ConvertTime(char *dstr, char *tstr);
TreeNodePtr AllocateTreeNode();
TreeNodePtr LoadModel();
TreeNodePtr ReadDT(FILE *fpmodel, TreeNodePtr parent);

// Sampling prototypes
void InitializeSampleVectors();
void FreeSampleVectors();
void UpdateStateVector(char *sensorid1, char *sensorid2, char *sensorStatus);
int ComputeSampleFeatures(int timeSinceStart, char *sensorid1, char *sensorid2, char *sensorStatus, float *tempdataSamples);
int GetSamplingWindow(char *sensorName);
int floatcomp(const void* a, const void* b);
float Percentile(float percent, int N, float *sortedValues);

// AR-specific prototypes
void AR_TrainModel();
void AR_TrainKModel(int cvnum);
void AR_TestModel();
void AR_TestKModel(int cvnum);
void AR_AnnModel();
void AR_RetrainModel();
void AR_DT();
void AR_KDT(int cvnum);
void AR_ReadData();
void AR_PrintDT(TreeNodePtr tree);
void AR_PrintDT1(TreeNodePtr tree, int tab);
void UnderSample();
void AR_PruneTree(TreeNodePtr tree);
void FindBestNumericSplit(TreeNodePtr dtnode, int* training, int numtrain,
                          int att, AR_SplitInfo* splitInfoArray);
int MajorityClass(int *training, int num, int *small, int *numright);
int AR_SelectAttribute(TreeNodePtr dtnode, int *training, int numtrain);
int AR_ComputeThresholds(int *training, int numtrain, int att);
int AllOneClass(int *training, int numtrain);
float GetEstimatedErrors(TreeNodePtr tree);
float AddErrors(int numExamples, float errors, float confidenceFactor);
float GetEstimatedErrorsForDistribution(TreeNodePtr tree);
float C45SplitPoint(int att, float splitPoint);
float Entropy(int *freq, int total);
float OldEnt(int *training, int numtrain);
float SplitEnt(int total1, int total2, int numtrain);
float GainRatio(float infoGain, float splitEnt);
double ltqnorm(double p);
TreeNodePtr AR_BuildTree(int *training, int numtrain, int parentclass,
                         TreeNodePtr parent);

// AP-specific prototypes
void AP_TrainModel();
void AP_TestModel();
void AP_DT();
void AP_ReadData();
void AP_SplitInfo_Init(AP_SplitInfo* splitInfo, int numtrain, int attr);
void AP_SplitInfo_Copy(AP_SplitInfo* splitInfo1, AP_SplitInfo* splitInfo2);
void AP_SplitInfo_Update(AP_SplitInfo* splitInfo, int* training,
                         int numtrain, int attr);
void AP_SplitInfo_Print(AP_SplitInfo*);
void AP_Impurity_Init(AP_Impurity* imp, int partition, int attr, int* training,
                      int numtrain, int k);
void AP_Impurity_Increment(AP_Impurity* imp, float value);
void AP_Impurity_Compute(AP_Impurity* imp);
void AP_Impurity_Print(AP_Impurity* imp);
void AP_PrintDT(TreeNodePtr tree);
void AP_PrintDT1(TreeNodePtr tree, int tab);
void AP_PrintModels(TreeNodePtr tree);
void AP_PruneTree(TreeNodePtr tree);
void AP_InstallLinearModels(TreeNodePtr tree);
void AP_InstallSmoothedModels(TreeNodePtr tree);
void AP_BuildLinearModel(TreeNodePtr node);
void AP_SlidingWindowValidation();
void SetAttributesTested(TreeNodePtr tree, int *gAttributesTested);
void AllocateMatrices();
void FreeMatrices();
int AP_SelectAttribute(TreeNodePtr dtnode, int *training, int numtrain);
int AP_AssignModelNumbers(TreeNodePtr tree, int number);
int GetNumParametersModel(TreeNodePtr node);
int SolveMatrix(float** matrixA, float* matrixB, float* matrixX, int n);
float StandardDeviation(int *training, int numtrain);
float AbsoluteDeviation(int *training, int numtrain);
float AP_EvaluateModel(TreeNodePtr node);
float AP_EvaluateSubTree(TreeNodePtr node);
float AP_DTClassify(float *instance, TreeNodePtr tree);
float AP_ClassifyInstance(float* instance, TreeNodePtr node);
double PruningFactor(int numInstances, int numParameters);
TreeNodePtr AP_BuildTree(int *training, int numtrain, TreeNodePtr parent);

// AD-specific prototypes
void AD();
void AD_ReadData(FILE *fp);
void InstanceListInsert(Instance *instance, InstanceList *instanceList);
void PatternListInsert(Pattern *pattern, PatternList *patList, int max);
void FreePattern(Pattern *pattern);
void FreePatternListNode(PatternListNode *patListNode);
void FreePatternList(PatternList *patList);
void FreeInstanceList(InstanceList *instanceList);
void FreeSequence(Seq *seq);
void PrintLabel(int label);
void PrintEvent(Event event);
void PrintPattern(Pattern *pattern);
void PrintSequence(Seq *seq);
void PrintPatternList(PatternList *patList);
void PrintInstance(Instance *instance);
void PrintInstanceList(Pattern *pattern);
void AddInstancesToPattern(Pattern *pattern, Instance *subInstance,
   InstanceList *instanceList, int index);
void EvaluatePattern(Pattern *pattern);
void StoreLabel(char *patLabel);
void AD_Init(FILE **inputfile, FILE **annotatefile);
void AD_ComputeMI();
void AddPatternInfo(Pattern *pattern, int patNumber);
void ClusterPatterns(FILE *fp);
void AnnotateData(FILE *fp, FILE *out);
void RemoveLastPattern();
void FlattenPatterns();
void Splice(int old, int pos, int sub);
void MergePatterns(int x, int y);
void AddProbs(Pattern *pattern);
void LearnMarkovModel(FILE *fp);
void ComputeDistanceMatrix();
void PickClosestNodes(int *x, int *y, int **tree, int sizetree, int *sizenodes);
int CompressData(PatternList *patternList);
int Exactmatch(Seq *def1, Seq *def2);
int Match(Seq *def1, Seq *def2, int similarity);
int ComputeDistance(Seq *def1, Seq *def2);
int minlist(int *list, int num);
int MarkEvents(InstanceListNode *instanceListNode);
int SizePattern(Seq *definition);
int DataSize();
int MapSensors(char *sistr);
int ExactMatch(Seq *def1, Seq *def2);
int HCluster(double **distance, int npoints, int *clusterassignments);
int *SpectralCluster(double **distance, int npoints, int *ncluster);
int *Kmeans(double **points, int numpoints, int dimension, int numcentroid);
int *Randperm(int n);
int *SimpleCluster(double **points, int numpoints, int dimension,
   int *numcentroid);
double ComputePatDistance(int patx, int paty);
BOOLEAN MemberOfPatternList(Pattern *pattern, PatternList *patternList);
BOOLEAN InstanceListOverlap(Instance *instance, InstanceList *instanceList);
BOOLEAN InstanceOverlap(Instance *instance1, Instance *instance2);
BOOLEAN Similar(int label1, int label2);
Pattern *AllocatePattern();
PatternList *DiscoverSequences();
PatternList *GetInitialPatterns();
PatternListNode *AllocatePatternListNode(Pattern *pattern);
PatternList *AllocatePatternList(void);
PatternList *ExtendPattern(Pattern *parentPattern);
Pattern *CreatePatternFromInstance(Instance *instance);
InstanceList *AllocateInstanceList(void);
InstanceList *ExtendInstances(InstanceList *instanceList);
Instance *AllocateInstance(int numevents);
Instance *CreateExtendedInstance(Instance *instance, BOOLEAN where);
Seq *AllocateSequence(int numevents);
Seq *InstanceToSequence(Instance *instance);

// CASASAR-specific prototypes
void ANN_LabelData();
void ReadTranslations();
void PrintTranslations();
void InitMonitorInterface();
int FindTranslation(char *name);

// CASASAP-specific (real-time prompting) prototypes
void AP_RealtimePrediction();

// DBAR-specific prototypes
void DB_Init();
void DB_LabelData();

#endif // AL_H
