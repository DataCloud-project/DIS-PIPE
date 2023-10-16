/*------------------------------------------------------------------------------
 *       Filename:  globals.c
 *
 *    Description:  Global variable initialization.
 *
 *        Version:  AL 1.0
 *        Created:  05/24/2013
 *        Authors:  Diane J. Cook, cook@eecs.wsu.edu
 *                  Larry Holder, holder@wsu.edu
 *   Organization:  Washington State University
------------------------------------------------------------------------------*/

#include "al.h"

// Global variables (AR, AP, AD)
char *filename = NULL;     // name of the input data file
char *trainname = NULL;    // name of the training data file
char *modelfname = NULL;   // filename for storing / loading the activity model
char *sitename = NULL;
char **sensornames = NULL; // sensor names
char **activitynames = NULL; // activity names
char **gFeatureNames = NULL;	// feature/attribute names
char *TranslationFileName = NULL; // file containing transated names
char testdate[MAXSTR];
char testprevdate[MAXSTR];
int Mode = TRAIN;          // 0=train, 1=test, 2=both (cross-validation)
int TopN = 1;              // Report top n classes
int Weight = NONE;         // 0=none, 1=time, 2=mutual info, 3=time+mutual info
int Translate = FALSE;
int OutputLevel = 1;       // Ranges from 0 to 2
int IgnoreOther = FALSE;   // TRUE=ignore OtherActivity
int NumActivities = 0;     // total number of activities
int NumData = 0;           // total number of discrete event data entries
int NumDiscreteSensors = 0; // total number of discrete-event sensors
int NumSensors = 0;	   // total number of sensors
int windata[MAXWINDOW][4];
int wincnt = 0;
int PrevActivity = 0;
int CurActivity = 0;
int *labels = NULL;
int NumRight = 0;
int totalfreq = 0;
int *sensortimes = NULL;  // time since sensor last fired
int currenttime = 0;
int prevdays = 0;
int *sensortypes = NULL;
int NumDiscreteFeatures = 0;	  // discrete-event feature vector size
int NumFeatures = 0; // total number of features (discrete-event and sampling, if included)
int ALMode = AL_MODE_AR;  // Mode for activity learner:
                          // recognition, discovery, or prediction
int NumTranslations = 0;
float **discreteData = NULL; // feature vector matrix derived from discrete-event sensor data
float **data = NULL;	  // feature vector matrix derived from all sensor data
int *actnums = NULL; // activity id for each event
float *tempdata = NULL;
float **MI = NULL;	  // mutual information for sensor pairs
float pmargin2 = 0.0;
Translation *Translations = NULL;
TreeNodePtr globaldt = NULL;

// Sampling variables:
int SamplingMode = NOSAMPLE; // 0=no sampling features, 1=sampling features
int SampleInterval = 1; // the interval between samples, in seconds
float **sampleStateVectors = NULL; // Array of arrays of state vectors at each sample instance
int NumSamples = 0; // number of sampled state vectors stored
float *stateVector = NULL; // Array of the state vector to be used at a single point in time
int firstTimeSampling = 0; // Time since the first received/read sensor event (different than the normal firsttime calculation)
int firstEventSamplingFlag = FALSE; // flag set to true when first event is received (used for sampling)
int MaxSampleLag = 5; // number of seconds' worth of sample vectors to store for real-time prediction
int MaxSamplesToKeep = (int) floor(((double) MaxSampleLag) / ((double) SampleInterval)); // the number of previous samples to keep for a realtime system
int numBins = 10; // the number of bins used for binning features - for now set to 10 but may make dynamic later
int SampleWindowCount = 0; // number of samples stored in the realtime sampling window
int NumSampleFeatures = 0; // number of sampling-based features to use
int NumSampleData = 0;           // total number of sampling data entries
float **samplingData = NULL;	  // feature vector matrix derived from sampling sensor data
float *tempdataSamples = NULL;
float *samplingMax = NULL;
float *samplingMin = NULL;
float *samplingSum = NULL;
float *samplingMean = NULL;
float *samplingMedian = NULL;
float *samplingStdDev = NULL;
float **sortedValues = NULL; //used to store sorted sample values for each sensor


// Dynamic window variables (AR and AP)
int **actwind = NULL;
int *factwind = NULL;
int **senact = NULL;
int *fsenact = NULL;

// Dominant sensor ids for previous three windows (AR and AP)
int numwin = 0;
int dominant = 0;
int prevwin1 = 0;
int prevwin2 = 0;

float Val[9] = {  0,  0.001, 0.005, 0.01, 0.05, 0.10, 0.20, 0.40, 1.00};
float Dev[9] = {4.0,  3.09,  2.58,  2.33, 1.65, 1.28, 0.84, 0.25, 0.00};

// AR-specific global variables
int *classfreq = NULL;     // number of data points with the activity label
int **cmatrix = NULL;
int *gFreq1 = NULL;
int *gFreq2 = NULL;
int gAtt = 0;
float *gValues = NULL;

// AP-specific global variables
char *pactivityname = NULL;
char **datetime = NULL;
int *gAttributesTested = NULL;
int *atimes = NULL;
int aindex = 0;
int pactivity = -1;       // default to predicting all activities except other
int other = -1;
int *timestamp = NULL;
int PromptSeconds = 300;  // default prompt 300 seconds in advance
float maeErr = 0.0;
float mapeErr = 0.0;
float msdErr = 0.0;
float mseErr = 0.0;
float rmseErr = 0.0;
float nrmseRangeErr = 0.0;
float nrmseMeanErr = 0.0;
float corrCoeff = 0.0; // pearson's correlation coefficient
float **gMatrixA = NULL;
float *gMatrixB = NULL;
float *gMatrixX = NULL;
float **gMatrixLU = NULL;
float *gMatrixD = NULL;
float globalStdDev = 0.0;
float globalAbsDev = 0.0;
time_t firsttime;	// time of first event
time_t lasttime;		// time of last event
int APValMode = AP_VAL_MODE_SLIDING; //default AP validation mode of sliding window
int SlidingWindowSize = 1000; //default sliding window size of 1000 for test-on-last validation
int SlidingWindowSkip = 1000; //default number of events to skip between window starts (each start will be 1000 events after the previous window's start)

// AD-specific global variables
char **adlabels = NULL;
int nevents = 0;
int numlabels = 0;
int datasize = 0;
int origLabels = 0;
int iteration = 0;
int **sim = NULL;
int numsim = 0;
int NumIterations = 1;
float **mi = NULL;
Event *events = NULL;
