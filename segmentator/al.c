/*------------------------------------------------------------------------------
 *       Filename:  al.c
 *
 *       Usage:		al -r [configfile] // for activity recognition AR
 *       		al -p [configfile] // for activity prediction AP
 *       		al -d              // for activity discovery AD
 *       		If no configfile provided, then "ar.config" assumed.
 *
 *    Description:  Activity learning procedures common to AR, AP, and AD.
 *
 *        Version:  AL 1.0
 *        Created:  05/24/2013
 *        Authors:  Diane J. Cook, cook@eecs.wsu.edu
 *                  Larry Holder, holder@wsu.edu
 *   Organization:  Washington State University
 * ---------------------------------------------------------------------------*/

#include "al.h"

int main(int argc, char *argv[]) {
	char fname[MAXSTR];
	int i, j;

	if (argc > 1) {
		if (strcmp(argv[1], "-r") == 0)
			ALMode = AL_MODE_AR;
		else if (strcmp(argv[1], "-p") == 0)
			ALMode = AL_MODE_AP;
		else if (strcmp(argv[1], "-d") == 0)
			ALMode = AL_MODE_AD;
		else if (strcmp(argv[1], "-a") == 0)
			ALMode = AL_MODE_ANN;
		else if (strcmp(argv[1], "-f") == 0)
			ALMode = AL_MODE_DB;
		else if (strcmp(argv[1], "-l") == 0)
			ALMode = AL_MODE_RTP;
		else
			Error("Invalid argument. Options are [rpdafl]");
	} else
		Error("Missing argument. Options are -[rpdafl]");
	if (ANNOTATE == FALSE)
		printf("Reading the config file...");
	if (argc > 2)
		ReadConfig(argv[2]);
	else {
		strcpy(fname, "al.config");
		ReadConfig(fname);
	}
	if (ANNOTATE == FALSE)
		printf("done\n");
	if (ALMode == AL_MODE_AD) {
		AD();
		return (1);
	}
	NumDiscreteFeatures = 8 + (2 * NumSensors); // need to use 2 * NumSensors instead of NumDiscreteSensors since the sensors use indexing by all sensors, not just discrete ones
	if (WEARABLE == TRUE)
		NumDiscreteFeatures += (20 * NUMAXES) + (NUMAXES * (NUMAXES - 1));

	if (SamplingMode == SAMPLE) {
		printf("Using sampling features\n");
		// Determine the number of sample features to use:
		NumSampleFeatures = 34 * NumSensors;
		NumFeatures = NumDiscreteFeatures + NumSampleFeatures;
	} else {
		printf("Sampling features disabled\n");
		NumFeatures = NumDiscreteFeatures;
	}
	if (ALMode == AL_MODE_ANN) {
#ifdef GLOOX
		ANN_LabelData();
#endif
		return (1);
	}

	if (ALMode == AL_MODE_RTP) {
#ifdef GLOOX
		AP_RealtimePrediction();
#endif
		return (1);
	}

	if (ALMode == AL_MODE_DB) {
#ifdef SQL
		if (argc > 3)                    // Filename specified by user
		{
			sitename = (char *) malloc(MAXSTR * sizeof(char));
			if (sitename == NULL)
			MemoryError("main");
			strcpy(sitename, argv[3]);
		}
		else Error("Site must be specified");
		DB_Init();
		DB_LabelData();
#endif
		return (1);
	}
	if (argc > 3)                    // Filename specified by user
			{
		if (ANNOTATE == FALSE)
			printf("Reading user specified data file name...");
		filename = (char *) malloc(MAXSTR * sizeof(char));
		if (filename == NULL)
			MemoryError("main");
		strcpy(filename, argv[3]);
		if (ANNOTATE == FALSE)
			printf("done\n\n");
	}

	if ((ANNOTATE == FALSE) && (OutputLevel > 0))
		PrintParms();

	if (Mode == TRAIN) // TRAIN mode
	{
		printf("Entering training mode\n");

		// Compute the window probabilities
		if (OutputLevel > 0)
			printf("   Computing probabilities to determine window size...");

		// Compute activity probability distribution for alternative window sizes
		ComputeProbActWind();
		// Compute activity probability distribution for each sensor
		ComputeProbSenAct();

		if (OutputLevel > 0)
			printf("done\n");

		// Compute mutual information between the sensors
		if (Weight > 1) {
			if (OutputLevel > 0)
				printf("Computing mutual information between the sensors...");
			ComputeMI();
			if (OutputLevel > 0)
				printf("done\n");
		}

		// create directory to store the model files and the final config file
		if (mkdir(modelfname, S_IRWXU | S_IRWXG | S_IRWXO) > 0)
			Error("The model directory cannot be created");
		// Read data, learn and save the model
		if (ALMode == AL_MODE_AR)
			AR_TrainModel();
		else if (ALMode == AL_MODE_AP)
			AP_TrainModel();
		SaveParams();           // Save the program parameters to a file
	} else if (Mode == TEST) // TEST mode
	{
		if ((factwind == NULL) || (fsenact == NULL))
			MemoryError("factwind, fsenact need to be provided for testing");
		if (ANNOTATE == FALSE) {
			if (OutputLevel > 0)
				printf("Reading data...");
			if ((CMATRIX == TRUE) && (ALMode == AL_MODE_AR)) {
				cmatrix = (int **) calloc(NumActivities, sizeof(int *));
				if (cmatrix == NULL)
					MemoryError("main");
				for (i = 0; i < NumActivities; i++) {
					cmatrix[i] = (int *) calloc(NumActivities, sizeof(int));
					if (cmatrix[i] == NULL)
						MemoryError("main");
				}
			}
		}

		// Load the activity model and test the data
		if (ALMode == AL_MODE_AR)
			AR_TestModel();
		if (ALMode == AL_MODE_AP)
			AP_TestModel();

		if ((ANNOTATE == FALSE) && (CMATRIX == TRUE) && (ALMode == AL_MODE_AR)) // confusion matrix
				{
			printf("       ");
			for (i = 0; i < NumActivities; i++)
				printf(" %c%c%c[%d]", activitynames[i][0], activitynames[i][1],
						activitynames[i][2], i);
			printf("\n");
			for (i = 0; i < NumActivities; i++) {
				printf("%c%c%c[%d]:", activitynames[i][0], activitynames[i][1],
						activitynames[i][2], i);
				for (j = 0; j < NumActivities; j++) {
					if (cmatrix[i][j] < 100000) {
						printf(" ");
						if (cmatrix[i][j] < 10000) {
							printf(" ");
							if (cmatrix[i][j] < 1000) {
								printf(" ");
								if (cmatrix[i][j] < 100) {
									printf(" ");
									if (cmatrix[i][j] < 10)
										printf(" ");
								}
							}
						}
					}
					printf("%d ", cmatrix[i][j]);
				}
				printf("\n");
			}
		}
		if ((ANNOTATE == FALSE) && (NumData > 1)) // Report prediction performance
				{
			if (ALMode == AL_MODE_AR)
				printf("Activity recognition accuracy = %f\n",
						((float) NumRight / (float) NumData));
			if (ALMode == AL_MODE_AP) // Need to compute performance
			{
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
			}
		}
	} else if (Mode == PROMPT) // Mode = PROMPT, prompt for predicted activities
	{
		if ((factwind == NULL) || (fsenact == NULL))
			MemoryError("factwind, fsenact need to be provided for prompting");
		AP_TestModel();
	} else if (Mode == BOTH)  // Mode = BOTH, perform cross validation (k-fold for AR, sliding window or leave-day-out for AP)
	{
		if (OutputLevel > 0)
			printf("Entering cross-validation\n");

		// Compute the window probabilities
		if (OutputLevel > 1)
			printf("   Computing probabilities to determine window size...");

		// Compute activity probability distribution for alternative window sizes
		ComputeProbActWind();

		// Compute activity probability distribution for each sensor
		ComputeProbSenAct();

		if (OutputLevel > 1)
			printf("done\n");

		// Compute mutual information between the sensors
		if (Weight > 1) {
			if (OutputLevel > 1)
				printf("Computing mutual information between the sensors...");
			ComputeMI();
			if (OutputLevel > 1)
				printf("done\n");
		}

		if( ALMode == AL_MODE_AR) {
			printf("Entering AR %d-fold cross-validation\n", K);

			AR_ReadData();

			for (i = 0; i < K; i++) // Process one fold for n-fold cross validation
					{
				if (OutputLevel > 0)
					printf("fold %d\n", i + 1);
				AR_TrainKModel(i);
				AR_TestKModel(i);
			}
			printf("Accuracy is (%d %d) %f\n", NumRight, NumData,
					(float) NumRight / (float) NumData);
		} else if ( ALMode == AL_MODE_AP) {
			printf("Entering AP cross-validation mode\n");

			AP_ReadData();

			AllocateMatrices();

			if (APValMode == AP_VAL_MODE_SLIDING) { //sliding window, test-next-event mode
				printf("Entering sliding window validation mode\n");
				AP_SlidingWindowValidation();
			} else if (APValMode == AP_VAL_MODE_DAY_OUT) { //leave-a-day-out mode
				printf("Entering leave-a-day-out validation mode\n");
			} else //invalid/unknown mode
				printf("Invalid AP validation mode: %d\n", APValMode);

			FreeMatrices();
		} else
			printf("There is no cross-validation mode for AL mode %d\n", ALMode);
	} else if (Mode == TRACK) // TRACK mode
	{
		pmargin2 = PMARGIN * 6; // prune more aggressively

		// Compute the window probabilities
		if (OutputLevel > 1)
			printf("   Computing probabilities to determine window size...");

		// Compute activity probability distribution for alternative window sizes
		ComputeProbActWind();

		// Compute activity probability distribution for each sensor
		ComputeProbSenAct();

		if (OutputLevel > 1)
			printf("done\n");

		// Compute mutual information between the sensors
		if (Weight > 1) {
			if (OutputLevel > 1)
				printf("Computing mutual information between the sensors...");
			ComputeMI();
			if (OutputLevel > 1)
				printf("done\n");
		}

		// create directory to store the model files and the final config file
		if (mkdir(modelfname, S_IRWXU | S_IRWXG | S_IRWXO) > 0)
			Error("The model directory cannot be created");

		// Read data, learn and save the model
		if (OutputLevel > 0)
			printf("First round of training\n");
		AR_TrainModel();
		AR_AnnModel();      // Label Other_Activity data based on models
		if (OutputLevel > 0)
			printf("Retraining\n");
		AR_RetrainModel();  // Use self-training to refine activity models
		if (OutputLevel > 0)
			printf("Annotating from retrained model\n");
		AR_AnnModel();      // Label Other_Activity data based on refined models
		if (OutputLevel > 0)
			printf("Annotating from retrained model\n");
		AD();               // Discover patterns in the remainder of the data
		sprintf(filename, "%s.ann.annotated", trainname);
		if (OutputLevel > 0)
			printf("Retraining for discovered patterns\n");
		AR_RetrainModel();  // Model predefined and discovered activities
		if (OutputLevel > 0)
			printf("Annotating from combined model\n");
		AR_AnnModel();     // Label Other_Activity data based on combined models
		if (CLEANUP == TRUE) {
			sprintf(filename, "rm %s.ann", trainname);
			system(filename);
			sprintf(filename, "rm %s.ann.annotated", trainname);
			system(filename);
		}
	} else {
		printf("Mode %d not recognized:\n", Mode);
		exit(1);
	}
	FreeMemory();

	return (0);
}

/* Read each line of the config file and set program parameters accordingly. */
void ReadConfig(char *name) {
	FILE *fp;
	char *cptr = NULL, *buffer = NULL, arg[MAXSTR];
	int i, j, pos, length, spos;

	fp = fopen(name, "r");
	if (fp == NULL) {
		printf("Unable to open the configuration file: %s\n", name);
		exit(1);
	}

	buffer = (char *) malloc(BUFFERSIZE * sizeof(char));
	if (buffer == NULL)
		MemoryError("ReadConfig");

	cptr = fgets(buffer, BUFFERSIZE, fp);  // Read next line
	while (cptr != NULL)                // Process each line of the config file
	{
		buffer = TrimStr(buffer);
		length = (int) strlen(buffer);
		sscanf(buffer, "%s", arg);
		if (strcmp(arg, "output") == 0)  // Specify output reporting level
			sscanf(buffer, "%s %d\n", arg, &OutputLevel);
		else if (strcmp(arg, "data") == 0)  // Data file name(s)
				{
			if (ALMode != AL_MODE_DB) {
				pos = strlen(arg) + 1;
				if (pos >= (int) strlen(buffer))
					Error("No data file specified");
				pos = ReadString(buffer, pos, arg, length);
				filename = (char *) malloc(MAXSTR * sizeof(char));
				if (filename == NULL)
					MemoryError("ReadConfig");
				sscanf(buffer, "%s %s\n", arg, filename);
				trainname = (char *) malloc(MAXSTR * sizeof(char));
				if (trainname == NULL)
					MemoryError("ReadConfig");
				strcpy(trainname, filename);
			}
		} else if (strcmp(arg, "mode") == 0)   // Train or test mode
			sscanf(buffer, "%s %d\n", arg, &Mode);
		else if (strcmp(arg, "topn") == 0)   // Report top n classes
				{
			sscanf(buffer, "%s %d\n", arg, &TopN);
			if (TopN <= 0)
				Error("TopN value must be greater than 0");
			pmargin2 = PMARGIN * 5;
		} else if (strcmp(arg, "numiterations") == 0)   // #discovery iterations
				{
			sscanf(buffer, "%s %d\n", arg, &NumIterations);
			if ((NumIterations == 0) || (NumIterations < -1))
				Error("Number of iterations must be -1 or >=1");
		} else if (strcmp(arg, "promptseconds") == 0)   // #seconds to prompt
				{
			sscanf(buffer, "%s %d\n", arg, &PromptSeconds);
			if (PromptSeconds <= 0)
				Error("Number of iterations must be greater than 0");
		} else if (strcmp(arg, "weight") == 0) // How to weight events in window
			sscanf(buffer, "%s %d\n", arg, &Weight);
		else if (strcmp(arg, "ignoreother") == 0) // Ignore OtherActivity
				{
			if (ALMode == AL_MODE_AD)
				printf("\nCannot ignore Other_Activity using AD component\n");
			else
				IgnoreOther = TRUE;
		} else if (strcmp(arg, "predictactivity") == 0) {
			pactivityname = (char *) malloc(MAXSTR * sizeof(char));
			sscanf(buffer, "%s %s\n", arg, pactivityname);
			if (strcmp(pactivityname, "-1") != 0)
				pactivity = FindActivity(pactivityname);
		} else if (strcmp(arg, "model") == 0)   // Specify model directory name
				{
			modelfname = (char *) malloc(MAXSTR * sizeof(char));
			sscanf(buffer, "%s %s\n", arg, modelfname);
		} else if (strcmp(arg, "site") == 0)   // Specify site name
				{
			sitename = (char *) malloc(MAXSTR * sizeof(char));
			sscanf(buffer, "%s %s\n", arg, sitename);
		} else if (strcmp(arg, "translate") == 0)   // Specify site name
				{
			TranslationFileName = (char *) malloc(MAXSTR * sizeof(char));
			sscanf(buffer, "%s %s\n", arg, TranslationFileName);
			Translate = TRUE;
			ReadTranslations();
		} else if (strcmp(arg, "sensor") == 0)  // List of sensor names
				{
			pos = strlen(arg) + 1;
			pos = ReadString(buffer, pos, arg, length);
			while (pos >= 0) {
				if (NumSensors == 0) {
					sensornames = (char **) malloc(sizeof(char *));
					sensortypes = (int *) malloc(sizeof(int));
					sensortimes = (int *) malloc(sizeof(int));
				} else {
					sensornames = (char **) realloc(sensornames,
							(NumSensors + 1) * sizeof(char *));
					sensortypes = (int *) realloc(sensortypes,
							(NumSensors + 1) * sizeof(int));
					sensortimes = (int *) realloc(sensortimes,
							(NumSensors + 1) * sizeof(int));
				}
				if ((sensornames == NULL) || (sensortypes == NULL)
						|| (sensortimes == NULL))
					MemoryError("ReadConfig");
				sensornames[NumSensors] = (char *) malloc(
				MAXSTR * sizeof(char));
				if (sensornames[NumSensors] == NULL)
					MemoryError("ReadConfig");
				strcpy(sensornames[NumSensors], arg);
				spos = strlen(arg);
				sensortypes[NumSensors] = MOTION;
				if (spos > 10) {
					if ((arg[spos - 11] == 'T') && (arg[spos - 10] == 'e')
							&& (arg[spos - 9] == 'm') && (arg[spos - 8] == 'p')
							&& (arg[spos - 7] == 'e') && (arg[spos - 6] == 'r')
							&& (arg[spos - 5] == 'a') && (arg[spos - 4] == 't')
							&& (arg[spos - 3] == 'u') && (arg[spos - 2] == 'r')
							&& (arg[spos - 1] == 'e'))
						sensortypes[NumSensors] = TEMPERATURE;
				}
				if (spos > 9) {
					if ((arg[spos - 10] == 'L') && (arg[spos - 9] == 'i')
							&& (arg[spos - 8] == 'g') && (arg[spos - 7] == 'h')
							&& (arg[spos - 6] == 't') && (arg[spos - 5] == 'L')
							&& (arg[spos - 4] == 'e') && (arg[spos - 3] == 'v')
							&& (arg[spos - 2] == 'e') && (arg[spos - 1] == 'l'))
						sensortypes[NumSensors] = LIGHTLEVEL;
				}
				if (spos > 6) {
					if ((arg[spos - 7] == 'C') && (arg[spos - 6] == 'a')
							&& (arg[spos - 5] == 'b') && (arg[spos - 4] == 'i')
							&& (arg[spos - 3] == 'n') && (arg[spos - 2] == 'e')
							&& (arg[spos - 1] == 't'))
						sensortypes[NumSensors] = CABINET;
				}
				if (spos > 4) {
					if ((arg[spos - 5] == 'L') && (arg[spos - 4] == 'i')
							&& (arg[spos - 3] == 'g') && (arg[spos - 2] == 'h')
							&& (arg[spos - 1] == 't'))
						sensortypes[NumSensors] = LIGHT;
				}
				if (spos > 3) {
					if ((arg[spos - 4] == 'D') && (arg[spos - 3] == 'o')
							&& (arg[spos - 2] == 'o') && (arg[spos - 1] == 'r'))
						sensortypes[NumSensors] = DOOR;
				}

				// Add to the number of discrete event sensors and their times:
				if (sensortypes[NumSensors] == MOTION
						|| sensortypes[NumSensors] == CABINET
						|| sensortypes[NumSensors] == LIGHT
						|| sensortypes[NumSensors] == DOOR) {
					sensortimes[NumSensors] = SECONDSINADAY;
					NumDiscreteSensors++;
				} else {
					sensortimes[NumSensors] = 0;
				}
				NumSensors++;
				pos = ReadString(buffer, pos, arg, length);
			}
		} else if (strcmp(arg, "activity") == 0)  // List of activity labels
				{
			pos = strlen(arg) + 1;
			pos = ReadString(buffer, pos, arg, length);
			while (pos >= 0) {
				if (NumActivities == 0) {
					activitynames = (char **) malloc(sizeof(char *));
					if (ALMode == AL_MODE_AR)
						classfreq = (int *) malloc(sizeof(int));
					atimes = (int *) malloc(sizeof(int));
				} else {
					activitynames = (char **) realloc(activitynames,
							(NumActivities + 1) * sizeof(char *));
					if (ALMode == AL_MODE_AR)
						classfreq = (int *) realloc(classfreq,
								(NumActivities + 1) * sizeof(int));
					atimes = (int *) realloc(atimes,
							(NumActivities + 1) * sizeof(int));
				}
				if ((activitynames == NULL)
						|| ((ALMode == AL_MODE_AR) && (classfreq == NULL))
						|| ((ALMode == AL_MODE_AP) && (atimes == NULL)))
					MemoryError("ReadConfig");
				activitynames[NumActivities] = (char *) malloc(
				MAXSTR * sizeof(char));
				if (activitynames[NumActivities] == NULL)
					MemoryError("ReadConfig");
				strcpy(activitynames[NumActivities], arg);
				if (ALMode == AL_MODE_AR)
					classfreq[NumActivities] = 0;
				atimes[NumActivities] = 0; // Initialize activity times to 0
				NumActivities++;
				pos = ReadString(buffer, pos, arg, length);
			}
		} else if (strcmp(arg, "fsenact") == 0) // Activity likelihood for sensors
				{
			i = 0;
			pos = strlen(arg) + 1;
			pos = ReadString(buffer, pos, arg, length);
			while (pos >= 0) {
				if (i == 0)
					fsenact = (int *) malloc(sizeof(int));
				else
					fsenact = (int *) realloc(fsenact, (i + 1) * sizeof(int));
				if (fsenact == NULL)
					MemoryError("ReadConfig");
				fsenact[i] = atoi(arg);
				i++;
				pos = ReadString(buffer, pos, arg, length);
			}
		} else if (strcmp(arg, "factwind") == 0) // Activity probs for window sizes
				{
			i = 0;
			pos = strlen(arg) + 1;
			pos = ReadString(buffer, pos, arg, length);
			while (pos >= 0) {
				if (i == 0)
					factwind = (int *) malloc(sizeof(int));
				else
					factwind = (int *) realloc(factwind, (i + 1) * sizeof(int));
				if (factwind == NULL)
					MemoryError("ReadConfig");
				factwind[i] = atoi(arg);
				i++;
				pos = ReadString(buffer, pos, arg, length);
			}
		} else if (strcmp(arg, "mi") == 0)  // Sensor mutual information
				{
			pos = strlen(arg) + 1;
			pos = ReadString(buffer, pos, arg, length);
			MI = (float **) malloc(NumSensors * sizeof(float *));
			if (MI == NULL)
				MemoryError("ReadConfig");
			for (i = 0; i < NumSensors; i++) {
				MI[i] = (float *) malloc(NumSensors * sizeof(float));
				if (MI[i] == NULL)
					MemoryError("ReadConfig");
				for (j = 0; j < NumSensors; j++) {
					MI[i][j] = (float) atof(arg);
					pos = ReadString(buffer, pos, arg, length);
				}
			}
		} else if (strcmp(arg, "sampling") == 0)   // Sampling mode
			sscanf(buffer, "%s %d\n", arg, &SamplingMode);
		else if (strcmp(arg, "prediction_validation_mode") == 0)   // Prediction cross-validation mode
			sscanf(buffer, "%s %d\n", arg, &APValMode);
		else if (strcmp(arg, "prediction_validation_window_size") == 0)   // Prediction cross-validation window size
			sscanf(buffer, "%s %d\n", arg, &SlidingWindowSize);
		else if (strcmp(arg, "prediction_validation_skip") == 0)   // Prediction cross-validation events to skip between window starts
			sscanf(buffer, "%s %d\n", arg, &SlidingWindowSkip);
		else
			printf("Unknown config parameter %s\n", arg);
		cptr = fgets(buffer, BUFFERSIZE, fp);  // Read next line
	}

	fclose(fp);
	free(buffer);
}

/* Remove white space from the end of a line. */
char *TrimStr(char *str) {
	int i, length;

	length = (int) strlen(str);
	i = length - 1;
	while ((i > 0) && (isspace(str[i]) != 0))
		i--;
	str[i + 1] = '\0';
	length = (int) strlen(str);

	return (str);
}

/* Read the next string from a line buffer and update the current
 position in the buffer. */
int ReadString(char *str, int pos, char *arg, int length) {
	int i;

	i = pos;
	// Add to string until reach end of line or white space
	while ((i < length) && (isspace(str[i]) == 0)) {
		arg[i - pos] = str[i];
		i++;
	}
	arg[i - pos] = '\0';
	if (i > length)
		return (-1);
	else
		return (i + 1);
}

/* Print program parameter values. */
void PrintParms() {
	int i;

	printf("*****************************\n");
	printf("*** AL Program Parameters ***\n\n");
	switch (ALMode) {
		case AL_MODE_AR:
			printf("AL Mode: Activity Recognition\n");
			break;
		case AL_MODE_AP:
			printf("AL Mode: Activity Prediction\n");
			break;
		case AL_MODE_AD:
			printf("AL Mode: Activity Discovery\n");
			break;
		case AL_MODE_ANN:
			printf("AL Mode: Real-Time Recognition and Annotation\n");
			break;
		case AL_MODE_DB:
			printf("AL Mode: Database Recognition and Annotation\n");
			break;
		case AL_MODE_RTP:
			printf("AL Mode: Real-Time Activity Prediction\n");
			break;
		default:
			printf("AL Mode: Unknown\n");
			break;
	}
	switch (Mode) {
	case 0: {
		printf("Operating Mode: Training Mode\n");
		break;
	}
	case 1: {
		printf("Operating Mode: Testing Mode\n");
		break;
	}
	case 2: {
		printf("Operating Mode: Cross-Validation Mode\n");
		break;
	}
	case 3: {
		printf("Operating Mode: Tracking Mode\n");
		break;
	}
	case 4: {
		printf("Operating Mode: Prompting Mode\n");
		break;
	}
	default: {
		printf("Unknown Operating Mode\n");
		break;
	}
	}

	if (ALMode == AL_MODE_AP) {
		if (pactivity == -1)
			printf("Predict activity: ALL\n");
		else
			printf("Predict activity: %s\n", pactivityname);

		if (SamplingMode == SAMPLE)
			printf("Sampling Mode: Use Sample Features\n");
		else
			printf("Sampling Mode: No Sample Features\n");

		if (Mode == BOTH) {
			switch (APValMode) {
				case AP_VAL_MODE_SLIDING:
					printf("AP Validation Mode: Sliding Window\n");
					printf("Sliding Window Size: %d\n", SlidingWindowSize);
					printf("Sliding Window Skip Between Starts: %d\n", SlidingWindowSkip);
					break;
				case AP_VAL_MODE_DAY_OUT:
					printf("AP Validation Mode: Leave-a-Day-Out\n");
					break;
				default:
					printf("AP Validation Mode: Unknown\n");
					break;
			}
		}
	}
	if (Weight == 0)
		printf("Weight: No sensor event weighting\n");
	else if (Weight == 1)
		printf("Weight: Time sensor event weighting\n");
	else if (Weight == 2)
		printf("Weight: Mutual information sensor event weighting\n");
	else
		printf("Weight: Time and mutual information sensor event weighting\n");

	if (modelfname != NULL)
		printf("Model file: %s\n", modelfname);

	printf("Sensor names [%d]:", NumSensors);
	for (i = 0; i < NumSensors; i++)
		printf(" %s", sensornames[i]);
	printf("\n");

	if (Mode != TRAIN) {
		printf("Activity labels [%d]:", NumActivities);
		for (i = 0; i < NumActivities; i++)
			printf(" %s", activitynames[i]);
		printf("\n");
	}
	printf("*****************************\n\n");
}

/* Save the parameters for use during testing. */
void SaveParams() {
	FILE *fp;
	char str[MAXSTR];
	int i, j;

	sprintf(str, "%s/%s", modelfname, "model.config");
	fp = fopen(str, "w");
	if (fp == NULL) {
		printf("Unable to open %s for writing\n", str);
		exit(-1);
	}

	// sensor names
	fprintf(fp, "sensor ");
	for (i = 0; i < NumSensors; i++)
		fprintf(fp, "%s ", sensornames[i]);
	fprintf(fp, "\n");

	// activity names
	fprintf(fp, "activity ");
	for (i = 0; i < NumActivities; i++)
		fprintf(fp, "%s ", activitynames[i]);
	fprintf(fp, "\n");

	// fsenact
	fprintf(fp, "fsenact ");
	for (i = 0; i < NumSensors; i++)
		fprintf(fp, "%d ", fsenact[i]);
	fprintf(fp, "\n");

	// factwind
	fprintf(fp, "factwind ");
	for (i = 0; i < NumActivities; i++)
		fprintf(fp, "%d ", factwind[i]);
	fprintf(fp, "\n");

	// weight
	fprintf(fp, "weight %d\n", Weight);

	// mi
	if (Weight > 1) {
		fprintf(fp, "mi ");
		for (i = 0; i < NumSensors; i++)
			for (j = 0; j < NumSensors; j++)
				fprintf(fp, "%.6f ", MI[i][j]);
		fprintf(fp, "\n");
	}

	// model directory
	fprintf(fp, "model %s\n", modelfname);

	// test mode
	fprintf(fp, "mode 1\n");

	// ignore other activity
	if (IgnoreOther == TRUE)
		fprintf(fp, "ignoreother\n");

	// translate file
	if (TranslationFileName != NULL)
		fprintf(fp, "translate %s\n", TranslationFileName);

	// report top n classes
	if (TopN > 1)
		fprintf(fp, "topn %d\n", TopN);

	// predict activity
	if (ALMode == AL_MODE_AP) {
		if (pactivity == -1)
			fprintf(fp, "predictactivity -1\n");
		else
			fprintf(fp, "predictactivity %s\n", pactivityname);
	}

	fclose(fp);

	if (OutputLevel > 0)
		printf("Parameters saved to file.\n");
}

/* Find the index of a specified activity from the activity list. */
int FindActivity(char *name) {
	int i;

	for (i = 0; i < NumActivities; i++)
		if (strcmp(name, activitynames[i]) == 0)
			return (i);

	if (Mode == TEST) {
		printf("Encountered new untrained activity %s\n", name);
		exit(1);
	}

	if (NumActivities == 0) {
		activitynames = (char **) malloc(sizeof(char *));
		if (activitynames == NULL)
			MemoryError("FindActivity");
		actwind = (int **) malloc(sizeof(int *));
		if (actwind == NULL)
			MemoryError("FindActivity");
		if (ALMode == AL_MODE_AR) {
			classfreq = (int *) malloc(sizeof(int));
			if (classfreq == NULL)
				MemoryError("FindActivity");
		}
		atimes = (int *) malloc(sizeof(int));
		if (atimes == NULL)
			MemoryError("FindActivity");
	} else {
		activitynames = (char **) realloc(activitynames,
				(NumActivities + 1) * sizeof(char *));
		if (activitynames == NULL)
			MemoryError("FindActivity");
		actwind = (int **) realloc(actwind,
				(NumActivities + 1) * sizeof(int *));
		if (actwind == NULL)
			MemoryError("FindActivity");
		if (ALMode == AL_MODE_AR) {
			classfreq = (int *) realloc(classfreq,
					(NumActivities + 1) * sizeof(int));
			if (classfreq == NULL)
				MemoryError("FindActivity");
		}
		atimes = (int *) realloc(atimes, (NumActivities + 1) * sizeof(int));
		if (atimes == NULL)
			MemoryError("FindActivity");
	}
	activitynames[NumActivities] = (char *) malloc(MAXSTR * sizeof(char));
	if (activitynames[NumActivities] == NULL)
		MemoryError("FindActivity");
	strcpy(activitynames[NumActivities], name);

	actwind[NumActivities] = (int *) calloc(NUMWINDOWS, sizeof(int));
	if (actwind[NumActivities] == NULL)
		MemoryError("FindActivity");

	if (ALMode == AL_MODE_AR)
		classfreq[NumActivities] = 0;
	atimes[NumActivities] = 0;  // Initialize activity times to 0

	NumActivities++;

	return (NumActivities - 1);
}

/* Find the index of a specified sensor from the sensor list. */
int FindSensor(char *name) {
	int i;

	for (i = 0; i < NumSensors; i++)
		if (strcmp(name, sensornames[i]) == 0)
			return (i);
	if (LIVE == FALSE) {
		printf("Could not find the sensor %s\n", name);
		exit(1);
	} else
		return (-1);
}

/* Compute the probability distribution over activity labels for alternative
 window sizes.  Determine the most likely window size given an activity.   */
void ComputeProbActWind() {
	FILE *fp;
	char *cptr = NULL;
	char buffer[BUFFERSIZE], sdate[MAXSTR], stime[MAXSTR], sensorid[MAXSTR];
	char newsensorid[MAXSTR], sensorstatus[MAXSTR], alabel[MAXSTR];
	char prevlabel[MAXSTR];
	int cnt, i, j, anum, max;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Unable to open data file %s\n", filename);
		exit(1);
	}

	cptr = fgets(buffer, BUFFERSIZE, fp);
	sscanf(buffer, "%s %s %s %s %s %s", sdate, stime, sensorid, newsensorid,
			sensorstatus, alabel);

	strcpy(prevlabel, alabel);            // Keep track of previous activity
	cnt = 0;
	while (cptr != NULL)  // Process each sensor event in the file
	{
		sscanf(buffer, "%s %s %s %s %s %s", sdate, stime, sensorid, newsensorid,
				sensorstatus, alabel);

		anum = FindActivity(alabel);
		if ((IgnoreOther == FALSE) || (strcmp(alabel, "Other_Activity") != 0)) {
			// Store activity probability distribution for
			// four alternative window sizes: 5, 10, 20, and 30.
			// Determine ideal window size by looking at actual length of
			// activities in sample data.
			if (strcmp(prevlabel, alabel) != 0) {
				anum = FindActivity(prevlabel);
				if (cnt <= 5)
					actwind[anum][0]++;
				else if (cnt > 5 && cnt <= 10)
					actwind[anum][1]++;
				else if (cnt > 10 && cnt <= 20)
					actwind[anum][2]++;
				else if (cnt > 20)
					actwind[anum][3]++;
				cnt = 0;
				strcpy(prevlabel, alabel);
			} else
				cnt++;
		}

		cptr = fgets(buffer, BUFFERSIZE, fp);
	}
	fclose(fp);

	// Identify the window size with the greatest probability for each activity
	factwind = (int *) calloc(NumActivities, sizeof(int));
	if (factwind == NULL)
		MemoryError("ComputeProbActWind");
	for (i = 0; i < NumActivities; i++) {
		max = 0;
		for (j = 0; j < NUMWINDOWS; j++) {
			if (actwind[i][j] > max) {
				factwind[i] = j;
				max = actwind[i][j];
			}
		}
	}
}

/* Compute the probability distribution of the activities given the sensor and
 identify the most likely activity given the sensor */
void ComputeProbSenAct() {
	FILE *fp;
	char *cptr = NULL;
	char buffer[BUFFERSIZE], sdate[MAXSTR], stime[MAXSTR], sensorid[MAXSTR];
	char newsensorid[MAXSTR], sensorstatus[MAXSTR], alabel[MAXSTR];
	int i, j, snum, anum, max;

	senact = (int **) malloc(NumSensors * sizeof(int *));
	if (senact == NULL)
		MemoryError("ComputeProbSenAct");
	for (i = 0; i < NumSensors; i++) {
		senact[i] = (int *) calloc(NumActivities, sizeof(int));
		if (senact[i] == NULL) {
			fprintf(stderr, "Memory error in ComputeProbSenAct\n");
			exit(1);
		}
	}

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Unable to open %s\n", filename);
		exit(1);
	}

	cptr = fgets(buffer, BUFFERSIZE, fp);
	while (cptr != NULL)  // Process each sensor event in the file
	{
		sscanf(buffer, "%s %s %s %s %s %s", sdate, stime, sensorid, newsensorid,
				sensorstatus, alabel);
		if (strcmp(alabel, "none") != 0) {
			snum = FindSensor(newsensorid);
			anum = FindActivity(alabel);
			senact[snum][anum] += 1;
		}
		cptr = fgets(buffer, BUFFERSIZE, fp);
	}
	fclose(fp);

	// Identify most likely activity associated with each sensor
	fsenact = (int *) calloc(NumSensors, sizeof(int));
	if (fsenact == NULL)
		MemoryError("ComputeProbSenAct");
	for (i = 0; i < NumSensors; i++) {
		max = 0;
		for (j = 0; j < NumActivities; j++) {
			if (senact[i][j] > max) {
				fsenact[i] = j;
				max = senact[i][j];
			}
		}
	}
}

/* Compute the mutual information between sensors by determining the
 probability of co-occurence of every pair of sensors */
void ComputeMI() {
	FILE *fp;
	char *cptr = NULL;
	char buffer[BUFFERSIZE], sdate[MAXSTR], stime[MAXSTR], sensorid[MAXSTR];
	char newsensorid[MAXSTR], sensorstatus[MAXSTR], alabel[MAXSTR];
	int i, j, presen, cursen;
	float total;

	MI = (float **) malloc(NumSensors * sizeof(float *));
	if (MI == NULL)
		MemoryError("ComputeMI");
	for (i = 0; i < NumSensors; i++) {
		MI[i] = (float *) calloc(NumSensors, sizeof(float));
		if (MI[i] == NULL)
			MemoryError("ComputeMI");
	}

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Unable to open %s\n", filename);
		exit(1);
	}

	presen = 0;
	cursen = 0;

	cptr = fgets(buffer, BUFFERSIZE, fp);
	while (cptr != NULL)  // Process each sensor event in the file
	{
		sscanf(buffer, "%s %s %s %s %s %s", sdate, stime, sensorid, newsensorid,
				sensorstatus, alabel);
		cursen = FindSensor(newsensorid);

		// Update the MI count based on the temporal proximity of presen
		// (the previous sensor) to cursen (the current sensor)
		MI[presen][cursen]++;
		presen = cursen;
		cptr = fgets(buffer, BUFFERSIZE, fp);
	}
	fclose(fp);

	for (i = 0; i < NumSensors; i++) // Normalize MI values for each pair of sensors
			{
		total = 0;
		for (j = 0; j < NumSensors; j++)
			total += MI[i][j];
		if (total != 0) {
			for (j = 0; j < NumSensors; j++)
				MI[i][j] /= total;
			if (MI[i][j] == 0.0)
				MI[i][j] = MIN;
		}
	}
}

/* Read the data from the input files.  Compute a feature vector for
 each window-length sequence of sensor events. Be sure to define NumFeatures
 before calling this procedure.
 If sampling features are desired, these features are also computed while reading the file.
 Note that this procedure computes additional features for AP, and
 even more features for AP are computed after calling this procedure
 (back in AP_ReadData). */
void ReadData() {
	FILE *fp;
	char *cptr = NULL;
	char buffer[BUFFERSIZE], date[MAXSTR], stime[MAXSTR], sensorid[MAXSTR];
	char newsensorid[MAXSTR], sensorstatus[MAXSTR], alabel[MAXSTR];
	char prevdate[MAXSTR];
	int i, end, snum, anum;
	float *tempdata;
	float *tempdataSamples = NULL;
	int timeSinceStart;

	tempdata = (float *) malloc(NumDiscreteFeatures * sizeof(float));
	if (tempdata == NULL)
		MemoryError("ReadData");

	// If the sampling mode is on, initialize the sampling arrays
	if (SamplingMode == SAMPLE
			&& (ALMode == AL_MODE_AP || ALMode == AL_MODE_RTP)) {
		InitializeSampleVectors();

		tempdataSamples = (float *) malloc(NumSampleFeatures * sizeof(float));
		if (tempdataSamples == NULL)
			MemoryError("ReadData");
		for (i = 0; i < NumSampleFeatures; i++)
			tempdataSamples[i] = 0;
	}

	printf("NumDiscreteSensors: %d\n", NumDiscreteSensors);
	printf("NumSensors: %d\n", NumSensors);
	printf("NumDiscreteFeatures: %d\n", NumDiscreteFeatures);
	printf("NumSampleFeatures: %d\n", NumSampleFeatures);
	printf("NumFeatures: %d\n", NumFeatures);

	strcpy(date, "");
	if ((OutputLevel > 0) && (Mode != TEST))
		printf("   Reading file %s...", filename);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Unable to open %s", filename);
		exit(1);
	}

	cptr = fgets(buffer, BUFFERSIZE, fp);
	while (cptr != NULL)  // Process each sensor event in the file
	{
//	   printf("%s\n", buffer);
		strcpy(prevdate, date);
		sscanf(buffer, "%s %s %s %s %s %s", date, stime, sensorid, newsensorid,
				sensorstatus, alabel);
		if ((strcmp(prevdate, date) != 0) && (NumData > 0)) // new day
			prevdays += SECONDSINADAY;    // #days (in seconds) since beginning
		currenttime = ComputeSeconds(stime); // #seconds begin current day
		snum = FindSensor(newsensorid);
		sensortimes[snum] = currenttime + prevdays;   // last time sensor fired
		snum = FindSensor(sensorid);
		sensortimes[snum] = currenttime + prevdays;   // last time sensor fired
		anum = FindActivity(alabel);                  // current activity

		if (ALMode == AL_MODE_AP) {
			// last time activity started
			if (anum == other)  // Ignore times for other
				atimes[anum] = -1;
			else
				atimes[anum] = currenttime + prevdays;
		}

		// initialize the temp data
		for (i = 0; i < NumDiscreteFeatures; i++)
			tempdata[i] = 0;

		// Compute a feature vector for the sensor event
		if ((ALMode == AL_MODE_AP) || (IgnoreOther == FALSE)
				|| (strcmp(alabel, "Other_Activity") != 0)) {
			if (SamplingMode == SAMPLE
					&& (ALMode == AL_MODE_AP || ALMode == AL_MODE_RTP)) {
				// Save the time of the first event received (for sampling purposes, different than firsttime above):
				if (firstEventSamplingFlag == FALSE) {
					firstTimeSampling = ConvertTime(date, stime);
					firstEventSamplingFlag = TRUE;
				}

				// Compute the time since the start, and then compute the sampling features and the discrete event features together, only acting when both have reached their windows
				timeSinceStart = ConvertTime(date, stime) - firstTimeSampling;
				if (sensortypes[snum] != LIGHTLEVEL
						&& sensortypes[snum] != TEMPERATURE) // if it's not a sampling-only sensor
					end = ComputeFeature(stime, sensorid, newsensorid, tempdata)
							&& ComputeSampleFeatures(timeSinceStart, sensorid,
									newsensorid, sensorstatus, tempdataSamples);
				else
					end = ComputeSampleFeatures(timeSinceStart, sensorid,
							newsensorid, sensorstatus, tempdataSamples); // only compute the sample features
			} else if (sensortypes[snum] != LIGHTLEVEL
					&& sensortypes[snum] != TEMPERATURE) // if it's not a sampling-only sensor
				end = ComputeFeature(stime, sensorid, newsensorid, tempdata); //otherwise, just compute the discrete event features
		} else
			end = 0;
		if (end == 1)            // End of window reached, add feature vector
				{
			// Store discrete event data for this event:
			if (NumData == 0) {
				data = (float **) malloc(sizeof(float *));
				if (data == NULL)
					MemoryError("ReadData");
				discreteData = (float **) malloc(sizeof(float *));
				if (discreteData == NULL)
					MemoryError("ReadData");
				actnums = (int *) malloc(sizeof(int));
				if (actnums == NULL)
					MemoryError("ReadData");
				labels = (int *) malloc(sizeof(int));
				if (labels == NULL)
					MemoryError("ReadData");
				if (ALMode == AL_MODE_AP) {
					firsttime = ConvertTime(date, stime);
					lasttime = firsttime;
					datetime = (char **) malloc(sizeof(char *));
					if (datetime == NULL)
						MemoryError("ReadData");
					timestamp = (int *) malloc(sizeof(int));
					if (timestamp == NULL)
						MemoryError("ReadData");
				}

				if (SamplingMode == SAMPLE
						&& (ALMode == AL_MODE_AP || ALMode == AL_MODE_RTP)) {
					samplingData = (float **) malloc(sizeof(float *));
					if (samplingData == NULL)
						MemoryError("ReadData");
				}
			} else {
				data = (float **) realloc(data,
						(NumData + 1) * sizeof(float *));
				if (data == NULL)
					MemoryError("ReadData");
				discreteData = (float **) realloc(discreteData,
						(NumData + 1) * sizeof(float *));
				if (discreteData == NULL)
					MemoryError("ReadData");
				actnums = (int *) realloc(actnums, (NumData + 1) * sizeof(int));
				if (actnums == NULL)
					MemoryError("ReadData");
				labels = (int *) realloc(labels, (NumData + 1) * sizeof(int));
				if (labels == NULL)
					MemoryError("ReadData");
				if (ALMode == AL_MODE_AP) {
					lasttime = ConvertTime(date, stime);
					datetime = (char **) realloc(datetime,
							(NumData + 1) * sizeof(char *));
					if (datetime == NULL)
						MemoryError("ReadData");
					timestamp = (int *) realloc(timestamp,
							(NumData + 1) * sizeof(int));
					if (timestamp == NULL)
						MemoryError("ReadData");
				}

				if (SamplingMode == SAMPLE
						&& (ALMode == AL_MODE_AP || ALMode == AL_MODE_RTP)) {
					samplingData = (float **) realloc(samplingData,
							(NumData + 1) * sizeof(float *));
					if (samplingData == NULL)
						MemoryError("ReadData");
				}
			}
			discreteData[NumData] = (float *) malloc(
					NumDiscreteFeatures * sizeof(float));
			if (discreteData[NumData] == NULL)
				MemoryError("ReadData");
			if (ALMode == AL_MODE_AR) {
				labels[NumData] = FindActivity(alabel);
				classfreq[labels[NumData]] += 1;
			}
			if (ALMode == AL_MODE_AP) {
				// Calculate time in seconds from beginning of dataset to here
				timestamp[NumData] = lasttime - firsttime;

				// If the current actvity is the predicted activity, update previous events to show the time to the next occurence of the activity (which is the current time)
				// This can be done by subtracting the timestamps of those events from the current time (to find the difference)
				if (anum == pactivity) // update prediction seconds
					for (i = aindex; i < NumData; i++)
						labels[i] = timestamp[NumData] - timestamp[i];
				labels[NumData] = 0;
				datetime[NumData] = (char *) malloc(MAXSTR * sizeof(char));
				sprintf(datetime[NumData], "%s %c%c%c%c%c%c%c%c", date,
						stime[0], stime[1], stime[2], stime[3], stime[4],
						stime[5], stime[6], stime[7]);
			}
			for (i = 0; i < NumDiscreteFeatures; i++)   // Store data point
				discreteData[NumData][i] = tempdata[i];

			if (SamplingMode == SAMPLE
					&& (ALMode == AL_MODE_AP || ALMode == AL_MODE_RTP)) {
				samplingData[NumData] = (float *) malloc(
						NumSampleFeatures * sizeof(float));
				if (samplingData[NumData] == NULL)
					MemoryError("ReadData");
				for (i = 0; i < NumSampleFeatures; i++)   // Store data point
					samplingData[NumData][i] = tempdataSamples[i];
			}
			// Now fill the entire data vector, which could include both discrete-event and sampling features:
			data[NumData] = (float *) malloc(NumFeatures * sizeof(float));
			if (data[NumData] == NULL)
				MemoryError("ReadData");

			if (SamplingMode == SAMPLE
					&& (ALMode == AL_MODE_AP || ALMode == AL_MODE_RTP)) {
				for (i = 0; i < NumDiscreteFeatures; i++) {
					data[NumData][i] = discreteData[NumData][i];
				}
				for (i = 0; i < NumSampleFeatures; i++) {
					data[NumData][i + NumDiscreteFeatures] =
							samplingData[NumData][i];
				}
			} else {
				for (i = 0; i < NumDiscreteFeatures; i++)
					data[NumData][i] = discreteData[NumData][i];
			}

			actnums[NumData] = anum;

			NumData++;
		}

		if (ALMode == AL_MODE_AP) {
			if (anum == pactivity)
				aindex = NumData;
		}
		cptr = fgets(buffer, BUFFERSIZE, fp);

	}
	fclose(fp);

	if (ALMode == AL_MODE_AP)
		for (i = aindex; i < NumData; i++)
			labels[i] = SECONDSINADAY;

	if ((OutputLevel > 0) && (Mode != TEST)) {
		fprintf(stdout, "done\n");
		fflush(stdout);
	}
	if (ANNOTATE == FALSE)
		printf("   Read %d examples\n", NumData);
	if (tempdata != NULL)
		free(tempdata);
	if (tempdataSamples != NULL)
		free(tempdataSamples);
	FreeSampleVectors();
	if (NumData == 0)
		Error("Data set is empty");
}

/* Compute the feature vector for each window-size sequence of sensor events.
 fields
 0: time of the last sensor event in window (hour)
 1: time of the last sensor event in window (seconds)
 2: window size in time duration
 3: time since last sensor event
 4: dominant sensor for previous window
 5: dominant sensor two windows back
 6: last sensor event in window
 7: last location in window
 8 - NumSensors+7: counts for each sensor
 NumSensors+8 - 2*NumSensors+7: time since sensor last fired (<= SECSINDAY)
 AP only: 2*NumSensors+8 ... 2*NumSensors+8+NumActivities:
 time since activity last occurred */
int ComputeFeature(char *time, char *sensorid1, char *sensorid2,
		float *tempdata) {
	int i, id, wsize, duration, max = 0, *scount, diff, time1, time2;
	int lastlocation = -1;

	windata[wincnt][HOUR] = ComputeSeconds(time);
	windata[wincnt][SENSOR1] = FindSensor(sensorid1);
	windata[wincnt][SENSOR2] = FindSensor(sensorid2);
	windata[wincnt][SECONDS] = currenttime;

	if (wincnt < (MAXWINDOW - 1))  // Not reached end of window
			{
		wincnt++;  // Keep track of number of events in current window
		return (0);
	} else                      // Reached end of window, create feature vector
	{
		wsize = GetWindow(sensorid1);             // Determine the window size

		scount = (int *) malloc(NumSensors * sizeof(int));
		if (scount == NULL)
			MemoryError("ComputeFeature");
		for (i = 0; i < NumSensors; i++)
			scount[i] = 0;

		// Determine the dominant sensor for this window
		for (i = (MAXWINDOW - 1); i >= (MAXWINDOW - wsize); i--) {
			scount[windata[i][SENSOR1]]++;
			scount[windata[i][SENSOR2]]++;
			id = windata[i][SENSOR1];
			if ((lastlocation == -1) && (sensortypes[id] == MOTION))
				lastlocation = id;
		}
		if ((numwin % MAXWINDOW) == 0) {
			prevwin2 = prevwin1;
			prevwin1 = dominant;
			dominant = 0;
			for (i = 0; i < NumSensors; i++)
				if (scount[i] > max) {
					max = scount[i];
					dominant = i;
				}
		}
		free(scount);

		// Attribute 0..1: time of last sensor event in window
		tempdata[0] = windata[MAXWINDOW - 1][HOUR] / 3600;   // hour of day
		tempdata[1] = (float) windata[MAXWINDOW - 1][HOUR];  // seconds of day

		// Attribute 2: time duration of window in seconds
		time1 = windata[MAXWINDOW - 1][HOUR];        // most recent sensor event
		time2 = windata[MAXWINDOW - wsize][HOUR]; // first sensor event in window
		if (time1 < time2)
			duration = time1 + (86400 - time2);
		else
			duration = time1 - time2;
		tempdata[2] = duration;                               // window duration

		// Attribute 3: time since last sensor event
		time2 = windata[MAXWINDOW - 2][HOUR];
		if (time1 < time2)
			duration = time1 + (86400 - time2);
		else
			duration = time1 - time2;
		tempdata[3] = duration;

		// Attributes 4..5: dominant sensors from previous windows
		tempdata[4] = prevwin1;
		tempdata[5] = prevwin2;

		// Attribute 6: last sensor id in window
		tempdata[6] = FindSensor(sensorid1);

		// Attribute 7: last location in window
		tempdata[7] = lastlocation;

		// Attributes 8..(NumSensors+7)
		// Weight each sensor event within the window
		// according to the selected weight scheme (none, time, MI, or time+MI)
		for (i = (MAXWINDOW - 1); i >= (MAXWINDOW - wsize); i--) {
			if (Weight == NONE)                       // Increment sensor count
			{
				tempdata[windata[i][SENSOR1] + 8]++;
				tempdata[windata[i][SENSOR2] + 8]++;
			}

			else if (Weight == TIMEWEIGHT) {
				tempdata[windata[i][SENSOR1] + 8] += (float) exp(
						-KAPPA
								* fabs(
										windata[MAXWINDOW - 1][SECONDS]
												- windata[i][SECONDS]));
				tempdata[windata[i][SENSOR2] + 8] += (float) exp(
						-KAPPA
								* fabs(
										windata[MAXWINDOW - 1][SECONDS]
												- windata[i][SECONDS]));
			}

			else if (Weight == MUTUALINFO) {
				tempdata[windata[i][SENSOR1] + 8] +=
						MI[windata[MAXWINDOW - 1][SENSOR1]][windata[wincnt][SENSOR1]];
				tempdata[windata[i][SENSOR2] + 8] +=
						MI[windata[MAXWINDOW - 1][SENSOR2]][windata[wincnt][SENSOR2]];
			}

			else if (Weight == MUTUALINFO_TIME) {
				tempdata[windata[i][SENSOR1] + 8] +=
						(float) exp(
								-KAPPA
										* fabs(
												windata[MAXWINDOW - 1][SECONDS]
														- windata[i][SECONDS]))
								* MI[windata[MAXWINDOW - 1][SENSOR1]][windata[wincnt][SENSOR1]];
				tempdata[windata[i][SENSOR2] + 8] +=
						(float) exp(
								-KAPPA
										* fabs(
												windata[MAXWINDOW - 1][SECONDS]
														- windata[i][SECONDS]))
								* MI[windata[MAXWINDOW - 1][SENSOR2]][windata[wincnt][SENSOR2]];
			}

			// Normalize the sensor counts to not be affected by window size
			if ((Weight != TIMEWEIGHT) && (Weight != MUTUALINFO_TIME))
				tempdata[windata[i][SENSOR1] + 8] /= (float) (wsize * 2);
		}

		// Attributes NumSensors+8..(2*NumSensors)+7
		// time since each sensor fired
		int j = 0;
		for (i = 0; i < NumSensors; i++) {
			// Only save if it's a discrete sensor:
//			if (sensortypes[i] == MOTION || sensortypes[i] == CABINET
//					|| sensortypes[i] == LIGHT || sensortypes[i] == DOOR) {
				diff = (currenttime + prevdays) - sensortimes[i];
				if (diff < 0)
					diff += SECONDSINADAY;
				if (diff > SECONDSINADAY)
					diff = SECONDSINADAY;
				tempdata[8 + NumSensors + j] = (float) diff;
				j++;
//			}
		}

		/*if (ALMode == AL_MODE_AP)
		 {
		 // time since activity last occurred
		 for (i=0; i<NumActivities; i++)
		 {
		 diff = (currenttime + prevdays) - atimes[i];
		 if (i == other)
		 tempdata[8+(2*NumSensors)+i] = 0.0;
		 else tempdata[8+(2*NumSensors)+i] = (float) diff;
		 }
		 }*/

		for (i = 0; i < MAXWINDOW - 1; i++) {
			windata[i][HOUR] = windata[i + 1][HOUR];
			windata[i][SENSOR1] = windata[i + 1][SENSOR1];
			windata[i][SENSOR2] = windata[i + 1][SENSOR2];
			windata[i][SECONDS] = windata[i + 1][SECONDS];
		}
		numwin++;
		return (1);
	}

	return (0);
}

void SetFeatureNames() {
	int i, offset;

	gFeatureNames = (char **) malloc(NumFeatures * sizeof(char *));
	if (gFeatureNames == NULL)
		MemoryError("SetFeatureNames");
	for (i = 0; i < NumFeatures; i++) {
		gFeatureNames[i] = (char *) malloc(MAXSTR * sizeof(char));
		if (gFeatureNames[i] == NULL)
			MemoryError("SetFeatureNames");
	}
	strcpy(gFeatureNames[0], "lastSensorEventHours");
	strcpy(gFeatureNames[1], "lastSensorEventSeconds");
	strcpy(gFeatureNames[2], "windowDuration");
	strcpy(gFeatureNames[3], "timeSinceLastSensorEvent");
	strcpy(gFeatureNames[4], "prevDominantSensor1");
	strcpy(gFeatureNames[5], "prevDominantSensor2");
	strcpy(gFeatureNames[6], "lastSensorID");
	strcpy(gFeatureNames[7], "lastLocation");
	offset = 8;
	for (i = 0; i < NumSensors; i++)
		sprintf(gFeatureNames[i + offset], "sensorCount-%s", sensornames[i]);
	offset += NumSensors;
	for (i = 0; i < NumSensors; i++)
		sprintf(gFeatureNames[i + offset], "sensorElTime-%s", sensornames[i]);

	if (ALMode == AL_MODE_AP || ALMode == AL_MODE_RTP) {
		offset += NumSensors;
		/*for (i = 0; i < NumActivities; i++)
		 sprintf(gFeatureNames[i+offset],
		 "timeSinceLastActivity-%s", activitynames[i]);
		 offset += NumActivities;*/
		strcpy(gFeatureNames[offset], "timeStamp");
		offset++;
		for (i = 0; i < MAXLAG; i++)
			sprintf(gFeatureNames[i + offset], "lag-%d", (i + 1));
		offset += MAXLAG;
		for (i = 0; i < MAXLAG; i++)
			sprintf(gFeatureNames[i + offset], "tslag-%d", (i + 1));

		if (SamplingMode == SAMPLE) {
			offset = NumDiscreteFeatures;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-maximum-value-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-minimum-value-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-sum-%d", (i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-mean-%d", (i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-mean-abs-dev-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-med-abs-dev-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-std-dev-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-coeff-var-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-num-zero-cross-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-25th-per-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset],
						"sampling-sq-sum-25th-per-%d", (i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-50th-per-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset],
						"sampling-sq-sum-50th-per-%d", (i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-75th-per-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset],
						"sampling-sq-sum-75th-per-%d", (i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset],
						"sampling-interquart-range-%d", (i + 1));
			offset += NumSensors;
			for (int j = 0; j < numBins; j++) {
				for (i = 0; i < NumSensors; i++)
					sprintf(gFeatureNames[i + offset],
							"sampling-count-bin-%d-%d", (j + 1), (i + 1));
				offset += NumSensors;
			}
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-skewness-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-kurtosis-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-sig-energy-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-log-sig-energy-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-sig-power-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-peak-to-peak-%d",
						(i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset],
						"sampling-avg-time-btwn-peaks-%d", (i + 1));
			offset += NumSensors;
			for (i = 0; i < NumSensors; i++)
				sprintf(gFeatureNames[i + offset], "sampling-num-peaks-%d",
						(i + 1));
			offset += NumSensors;
		}
	}
}

/* Compute the hour of the day (based on 24 hour clock). */
int ComputeHour(char *time) {
	char temp[3];

	temp[0] = time[0];
	temp[1] = time[1];
	temp[2] = '\0';

	return (atoi(temp));
}

/* Compute the number of seconds past midnight on the current day. */
int ComputeSeconds(char *time) {
	char hour[3], minute[3], second[3];

	hour[0] = time[0];
	hour[1] = time[1];
	hour[2] = '\0';
	minute[0] = time[3];
	minute[1] = time[4];
	minute[2] = '\0';
	second[0] = time[6];
	second[1] = time[7];
	second[2] = '\0';

	return ((atoi(hour) * 3600) + (atoi(minute) * 60) + atoi(second));
}

/* Compute the year. */
int ComputeYear(char *dstr) {
	char date[MAXSTR];

	date[0] = dstr[0];
	date[1] = dstr[1];
	date[2] = dstr[2];
	date[3] = dstr[3];
	date[4] = '\0';
	return (atoi(date));
}

/* Compute the day of the week. */
int ComputeDow(char *date) {
	int dow, year, month, day;
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

	year = ComputeYear(date);
	month = ComputeMonth(date);
	day = ComputeDay(date);
	year -= month < 3;
	// dow value starts with 0 = Sunday
	dow = (year + (year / 4) - (year / 100) + (year / 400) + t[month - 1] + day)
			% 7;
	return (dow);
}

/* Compute the month. */
int ComputeMonth(char *dstr) {
	char date[MAXSTR];

	date[0] = dstr[5];
	date[1] = dstr[6];
	date[2] = '\0';
	return (atoi(date));
}

/* Compute the day of the month. */
int ComputeDay(char *dstr) {
	char date[MAXSTR];

	date[0] = dstr[8];
	date[1] = dstr[9];
	date[2] = '\0';
	return (atoi(date));
}

/* Compute the minute of the hour. */
int ComputeMinute(char *time) {
	char temp[3];

	temp[0] = time[3];
	temp[1] = time[4];
	temp[2] = '\0';

	return (atoi(temp));
}

/* Compute the seconds of the minute. */
int ComputeSecond(char *time) {
	char temp[3];

	temp[0] = time[6];
	temp[1] = time[7];
	temp[2] = '\0';

	return (atoi(temp));
}

time_t ConvertTime(char *dstr, char *tstr) {
	struct tm t;
	time_t time;

	t.tm_year = ComputeYear(dstr);
	t.tm_mon = ComputeMonth(dstr);
	t.tm_mday = ComputeDay(dstr);
	t.tm_hour = ComputeHour(tstr);
	t.tm_min = ComputeMinute(tstr);
	t.tm_sec = ComputeSecond(tstr);
	t.tm_isdst = -1;
	time = mktime(&t);

	return (time);
}

/* Compute the window size for a given sensor. */
int GetWindow(char *name) {
	int win;

	win = factwind[fsenact[FindSensor(name)]];

	if (win == 0)
		return (5);
	else if (win == 1)
		return (10);
	else if (win == 2)
		return (20);
	else
		return (30);
}

/* Return total number of nodes in the tree. */
int TreeSize(TreeNodePtr tree) {
	int i, subtreeSize = 0;

	if (tree == NULL)
		return (0);

	if (tree->numChildren > 0) {
		for (i = 0; i < tree->numChildren; i++)
			subtreeSize += TreeSize(tree->children[i]);
	}
	return (subtreeSize + 1);
}

/* Return number of leaves in the tree. */
int NumLeaves(TreeNodePtr tree) {
	int i, leaves = 0;

	if (tree == NULL)
		return (0);

	if (tree->numChildren > 0) {
		for (i = 0; i < tree->numChildren; i++)
			leaves += NumLeaves(tree->children[i]);
	} else
		leaves = 1;
	return (leaves);
}

/* Turn tree into a leaf if the errors of its children exceed the errors
 as a leaf. */
void CollapseTree(TreeNodePtr tree) {
	int i, errorsOfTree, errorsOfSubtree;

	if (tree->numChildren > 0) {
		errorsOfTree = tree->numInstances - tree->numRight;
		errorsOfSubtree = GetTrainingErrors(tree);
		if (errorsOfSubtree >= (errorsOfTree - 0.001)) {
			for (i = 0; i < tree->numChildren; i++)
				FreeTree(tree->children[i]);
			free(tree->children);
			tree->numChildren = 0;
		} else {
			for (i = 0; i < tree->numChildren; i++)
				CollapseTree(tree->children[i]);
		}
	}
}

/* Return total number of errors made at the leaves of the tree. */
int GetTrainingErrors(TreeNodePtr tree) {
	int i, errors = 0;

	if (tree->numChildren > 0)
		for (i = 0; i < tree->numChildren; i++)
			errors += GetTrainingErrors(tree->children[i]);
	else
		errors = tree->numInstances - tree->numRight;

	return (errors);
}

/* Determine the number of data points represented by a particular subtree
 that would be categorized correctly using majorclass or the class value
 indicated at the leaf node. */
int NumInClass(TreeNodePtr tree, int majorclass, int type) {
	int i, num = 0;

	if (tree->numChildren == 0)   // Leaf node
			{
		for (i = 0; i < tree->numInstances; i++)
			if (type == LEAF)    // Accuracy if subtree collapsed into leaf node
			{
				if (labels[tree->instances[i]] == majorclass)
					num++;
			} else                   // Accuracy if subtree kept
			{
				if (labels[tree->instances[i]] == tree->Class)
					num++;
			}
	} else {
		for (i = 0; i < tree->numChildren; i++)
			num += NumInClass(tree->children[i], majorclass, type);
	}

	return (num);
}

/* Generate the set of class labels that correspond to a set of data points. */
int FindClass(int *training, int numtrain, int att, float value) {
	int i;

	for (i = 0; i < numtrain; i++)
		if (data[training[i]][att] == value)
			return (labels[training[i]]);

	return (0);
}

/* Moves all instances in training having attribute attr equal to (or
 less than or equal to if numeric) value val to the left of the training array
 and returns the number of such instances. */
int CompactInst(int *training, int numtrain, int att, float val) {
	int left, right, tmp;

	left = 0;
	right = numtrain - 1;
	while (left < right) {
		while ((left < numtrain) && (data[training[left]][att] <= val))
			left++;
		while ((right > 0) && (data[training[right]][att] > val))
			right--;
		if (left < right) {
			tmp = training[left];
			training[left] = training[right];
			training[right] = tmp;
		}
	}

	return (left);
}

/* Allocate memory for a node in the decision tree. */
TreeNodePtr AllocateTreeNode() {
	TreeNodePtr node;

	node = (TreeNodePtr) malloc(sizeof(TreeNode));
	node->attribute = 0;
	node->threshold = 0.0;
	node->numChildren = 0;
	node->children = NULL;
	node->parent = NULL;
	node->Class = 0;
	node->numInstances = 0;
	node->instances = NULL;
	node->adist = NULL;
	node->numRight = 0;
	node->entropy = 0.0;
	node->indices = NULL;
	node->numIndices = 0;
	node->numParameters = 0;
	node->modelNumber = 0;
	node->coefficients = NULL;
	node->intercept = 0.0;
	node->rootMeanSquaredError = 0.0;

	return (node);
}

/* Free memory for a decision tree. */
void FreeTree(TreeNodePtr tree) {
	int i;

	if (tree == NULL)
		return;

	if (tree->numChildren > 0) {
		for (i = 0; i < tree->numChildren; i++)
			FreeTree(tree->children[i]);
		free(tree->children);
	}
	if (tree->numInstances > 0)
		free(tree->instances);
	if (tree->adist != NULL)
		free(tree->adist);
	if (tree->numIndices > 0)
		free(tree->indices);
	if (tree->coefficients != NULL)
		free(tree->coefficients);
	free(tree);
}

/* Return class found at leaf of tree by following path according to the
 attribute values of the instance. */
int DTClassify(float *data, TreeNodePtr tree, int minNumClass, int *small) {
	int i, j, best, att = 0, *used = NULL;
	float max;

	while (tree->numChildren > 0) {
		att = tree->attribute;
		if (data[att] <= tree->threshold)
			tree = tree->children[0];
		else
			tree = tree->children[1];
	}

	if (tree->numInstances < minNumClass)
		*small = TRUE;

	if (CHECKOTHER == TRUE)
		if (((float) tree->numRight / (float) tree->numInstances)
				<= CLASSIFYTHRESHOLD)
			*small = TRUE;

	if (TopN > 1) {
		used = (int *) malloc(NumActivities * sizeof(int));
		if (used == NULL)
			MemoryError("DTClassify");
		for (i = 0; i < NumActivities; i++)
			used[i] = FALSE;
		printf("Top %d: ", TopN);
		for (i = 0; i < TopN; i++) {
			best = 0;
			max = 0.0;
			for (j = 0; j < NumActivities; j++)
				if ((used[j] == FALSE) && (tree->adist[j] > max)) {
					best = j;
					max = tree->adist[j];
				}
			printf("%s (%.2f) ", activitynames[best], tree->adist[best]);
			used[best] = TRUE;
		}
		printf("\n");
		free(used);
	}

	return (tree->Class);
}

/* Save the decision tree to a file. */
void SaveDT(TreeNodePtr dt) {
	FILE *fpmodel;
	char str[MAXSTR];

	if (Mode != BOTH)
		printf("   Saving the model...");
	sprintf(str, "%s/model", modelfname);
	fpmodel = fopen(str, "w");
	if (fpmodel == NULL) {
		printf("File %s not writeable\n", str);
		exit(1);
	}
	WriteDT(fpmodel, dt);
	fclose(fpmodel);
	if (Mode != BOTH)
		printf("done\n");
}

/* Write the parameters describing a subtree in the decision tree to a file. */
void WriteDT(FILE *fpmodel, TreeNodePtr dt) {
	int i, att;

	if (dt != NULL) {
		if (Mode == TRACK)
			fprintf(fpmodel, "%d %d %d\n", dt->numChildren, dt->numInstances,
					dt->numRight);
		else
			fprintf(fpmodel, "%d %d\n", dt->numChildren, dt->numInstances);
		if (dt->numChildren == 0) {
			if (ALMode == AL_MODE_AP) {
				for (i = 0; i < NumFeatures; i++)
					fprintf(fpmodel, "%f ", dt->coefficients[i]);
				fprintf(fpmodel, "%f\n", dt->intercept);
			} else if (TopN > 1)
				for (i = 0; i < NumActivities; i++)
					fprintf(fpmodel, "%f ", dt->adist[i]);
			else
				fprintf(fpmodel, "%d %d %d\n", dt->Class, dt->numInstances,
						dt->numRight);
		} else {
			att = dt->attribute;
			fprintf(fpmodel, "%d %f\n", att, dt->threshold);
			for (i = 0; i < dt->numChildren; i++)
				WriteDT(fpmodel, dt->children[i]);
		}
	}
}

/* Read decision tree information from a file and store it as a
 decision tree. */
TreeNodePtr ReadDT(FILE *fpmodel, TreeNodePtr parent) {
	int i;
	TreeNodePtr dt;

	dt = AllocateTreeNode();
	dt->parent = parent;
	dt->numRight = 0;
	if (Mode == TRACK)
		fscanf(fpmodel, "%d %d %d\n", &(dt->numChildren), &(dt->numInstances),
				&(dt->numRight));
	else
		fscanf(fpmodel, "%d %d\n", &(dt->numChildren), &(dt->numInstances));
	if (dt->numChildren == 0) {
		if (ALMode == AL_MODE_AP || ALMode == AL_MODE_RTP) {
			dt->coefficients = (float *) malloc(NumFeatures * sizeof(float));
			for (i = 0; i < NumFeatures; i++)
				fscanf(fpmodel, "%f ", &(dt->coefficients[i]));
			fscanf(fpmodel, "%f\n", &(dt->intercept));
		} else if (TopN > 1) {
			dt->adist = (float *) malloc(NumActivities * sizeof(float));
			for (i = 0; i < NumActivities; i++)
				fscanf(fpmodel, "%f ", &(dt->adist[i]));
		} else
			fscanf(fpmodel, "%d %d %d\n", &(dt->Class), &(dt->numInstances),
					&(dt->numRight));
	} else {
		fscanf(fpmodel, "%d %f\n", &(dt->attribute), &(dt->threshold));
		dt->children = (TreeNodePtr *) calloc(dt->numChildren,
				sizeof(TreeNodePtr));
		for (i = 0; i < dt->numChildren; i++)
			dt->children[i] = ReadDT(fpmodel, dt);
	}

	return (dt);
}

/* Read the decision tree. */
TreeNodePtr LoadModel() {
	FILE *fpmodel;
	char str[MAXSTR];
	TreeNodePtr dt;

	sprintf(str, "%s/model", modelfname);
	if ((ANNOTATE == FALSE) && (Mode != BOTH))
		printf("Reading model from file %s...", str);
	fpmodel = fopen(str, "r");
	if (fpmodel == NULL) {
		printf("File %s not readable\n", str);
		exit(1);
	}
	dt = ReadDT(fpmodel, NULL);
	fclose(fpmodel);
	if ((ANNOTATE == FALSE) && (Mode != BOTH))
		printf("done\n");

	return (dt);
}

/* Free allocated memory. */
void FreeMemory() {
	int i;

	if (filename != NULL)
		free(filename);
	if (modelfname != NULL)
		free(modelfname);
	if (sitename != NULL)
		free(sitename);
	if (NumSensors > 0) {
		for (i = 0; i < NumSensors; i++)
			if (sensornames[i] != NULL)
				free(sensornames[i]);
		if (sensornames != NULL)
			free(sensornames);
		if (sensortimes != NULL)
			free(sensortimes);
	}
	if (senact != NULL) {
		for (i = 0; i < NumSensors; i++)
			if (senact[i] != NULL)
				free(senact[i]);
		if (senact != NULL)
			free(senact);
	}
	if (actwind != NULL)
		free(actwind);
	if (factwind != NULL)
		free(factwind);
	if (fsenact != NULL)
		free(fsenact);
	if (MI != NULL) {
		for (i = 0; i < NumSensors; i++)
			free(MI[i]);
		free(MI);
	}
	if (labels != NULL)
		free(labels);
	labels = NULL;
	if (datetime != NULL) {
		for (i = 0; i < NumData; i++)
			free(datetime[i]);
		free(datetime);
	}
	if (timestamp != NULL)
		free(timestamp);
	if (activitynames != NULL) {
		for (i = 0; i < NumActivities; i++)
			if (activitynames[i] != NULL)
				free(activitynames[i]);
		free(activitynames);
		free(atimes);
	}
	if (gFeatureNames != NULL) {
		for (i = 0; i < NumFeatures; i++)
			if (gFeatureNames[i] != NULL)
				free(gFeatureNames[i]);
		free(gFeatureNames);
	}
	if (classfreq != NULL)
		free(classfreq);
	if (cmatrix != NULL) {
		for (i = 0; i < NumActivities; i++)
			free(cmatrix[i]);
		free(cmatrix);
	}
}

// Indicate there was trouble allocating memory for a data structure
void MemoryError(const char *context) {
	printf("ERROR: Out of memory in %s.\n", context);
	exit(1);
}

// Indicate there was an error in the specification of parameters
void Error(const char *context) {
	printf("ERROR: %s.\n", context);
	exit(1);
}

/* Read translations from file. */
void ReadTranslations() {
	FILE *fp;
	char *cptr, buffer[BUFFERSIZE];
	char tname[MAXSTR], trans1[MAXSTR], trans2[MAXSTR];

	fp = fopen(TranslationFileName, "r");
	if (fp == NULL) {
		printf("Unable to open translation file %s\n", TranslationFileName);
		exit(1);
	}
	cptr = fgets(buffer, BUFFERSIZE, fp);
	while (cptr != NULL) // Process each translation pair in the file
	{
		sscanf(buffer, "%s %s %s", tname, trans1, trans2);
		if (NumTranslations == 0)
			Translations = (Translation *) malloc(sizeof(Translation));
		else
			Translations = (Translation *) realloc(Translations,
					(NumTranslations + 1) * sizeof(Translation));
		if (Translations == NULL)
			MemoryError("ReadTranslations");
		strcpy(Translations[NumTranslations].name, tname);
		strcpy(Translations[NumTranslations].trans1, trans1);
		strcpy(Translations[NumTranslations].trans2, trans2);
		NumTranslations++;

		cptr = fgets(buffer, BUFFERSIZE, fp);
	}
}

void PrintTranslations() {
	int i;

	printf("Translations (%d):\n", NumTranslations);
	for (i = 0; i < NumTranslations; i++)
		printf("  %s -> %s %s\n", Translations[i].name, Translations[i].trans1,
				Translations[i].trans2);
}

/* Return index into Translation array for given name.
 Return -1 if no such translation found. */
int FindTranslation(char *name) {
	int i;

	for (i = 0; i < NumTranslations; i++) {
		if (strcmp(name, Translations[i].name) == 0)
			return (i);
	}
	return (-1);
}
