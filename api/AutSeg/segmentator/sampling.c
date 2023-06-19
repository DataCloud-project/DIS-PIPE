/*------------------------------------------------------------------------------
 *       Filename:  sampling.c
 *
 *    Description:  Sample-based feature functionality for AL functions.
 *
 *        Version:  AL 2.0 (based on AR v2.4)
 *        Created:  10/31/2013
 *         Author:  Diane J. Cook, cook@eecs.wsu.edu
 *                  Larry Holder, holder@wsu.edu
 *                  Bryan Minor, bminor@wsu.edu
 *   Organization:  Washington State University
 * ---------------------------------------------------------------------------*/
#include "al.h"

/*
 * Initialize the state vector, the sample state vectors window, and arrays of statistical parameters
 * that are used frequently in the feature calculations (mean, median, etc).
 */
void InitializeSampleVectors() {
	int i, j;

	// Initialize the state vector to all OFF and the size of the number of sensors we have
	stateVector = (float *) malloc(NumSensors * sizeof(float));
	if (stateVector == NULL)
	   MemoryError("InitializeSampleVectors");

	for (i = 0; i < NumSensors; i++)
	   stateVector[i] = 0; //initialize all sensors to 0

	// Initialize the sampled state vectors window:
	sampleStateVectors = (float **)malloc(MaxSamplesToKeep * sizeof(float*));
	if (sampleStateVectors == NULL)
		MemoryError("InitializeSampleVectors");

	for (i = 0; i < MaxSamplesToKeep; i++) {
		sampleStateVectors[i] = (float *)malloc(NumSensors * sizeof(float));
		if (sampleStateVectors[i] == NULL)
			MemoryError("InitializeSampleVectors");

		for (j = 0; j < NumSensors; j++) {
			sampleStateVectors[i][j] = 0; //initialize all to 0
		}
	}

	// Initialize the statistical value arrays:
	samplingMax = (float *) malloc(NumSensors * sizeof(float));
	if (samplingMax == NULL)
		MemoryError("InitializeSampleVectors");

	samplingMin = (float *) malloc(NumSensors * sizeof(float));
	if (samplingMin == NULL)
		MemoryError("InitializeSampleVectors");

	samplingSum = (float *) malloc(NumSensors * sizeof(float));
	if (samplingSum == NULL)
		MemoryError("InitializeSampleVectors");

	samplingMean = (float *) malloc(NumSensors * sizeof(float));
	if (samplingMean == NULL)
		MemoryError("InitializeSampleVectors");

	samplingMedian = (float *) malloc(NumSensors * sizeof(float));
	if (samplingMedian == NULL)
		MemoryError("InitializeSampleVectors");

	samplingStdDev = (float *) malloc(NumSensors * sizeof(float));
	if (samplingStdDev == NULL)
		MemoryError("InitializeSampleVectors");

	for (i = 0; i < NumSensors; i++) {
		samplingSum[i] = 0;
		samplingMean[i] = 0;
		samplingMedian[i] = 0;
		samplingStdDev[i] = 0;
	}

	// Perform an initialization of the sortedValues arrays (though they will be reallocated later)
	// Note that this will be primarily indexed by sensor
	sortedValues = (float **) malloc(NumSensors * sizeof(float *));
	if (sortedValues == NULL)
		MemoryError("InitializeSampleVectors");
	for (i = 0; i < NumSensors; i++) {
		sortedValues[i] = (float *) malloc(MaxSamplesToKeep * sizeof(float));
		if (sortedValues[i] == NULL)
			MemoryError("InitializeSampleVectors");
	}
}

/*
 * Free memory used for sampling arrays.
 */
void FreeSampleVectors() {
	int i;

	if (stateVector != NULL)
		free(stateVector);

	if (sampleStateVectors != NULL) {
		for (i = 0; i < MaxSamplesToKeep; i++)
			free(sampleStateVectors[i]);
		free(sampleStateVectors);
	}

	if (samplingMax != NULL)
		free(samplingMax);

	if (samplingMin != NULL)
		free(samplingMin);

	if (samplingSum != NULL)
		free(samplingSum);

	if (samplingMean != NULL)
		free(samplingMean);

	if (samplingMedian != NULL)
		free(samplingMedian);

	if (samplingStdDev != NULL)
		free(samplingStdDev);

	if (sortedValues != NULL) {
		for (i = 0; i < NumSensors; i++)
			free(sortedValues[i]);
		free(sortedValues);
	}
}

/* Update the state vector based on a sensor event and the sensor status */
void UpdateStateVector(char *sensorid1, char *sensorid2, char *sensorStatus) {
	int sensor1 = FindSensor(sensorid1);
	int sensor2 = FindSensor(sensorid2);

	// Determine the state of the sensor based on the sensor type and message:
	switch(sensortypes[sensor1]) {
	case MOTION:
	case LIGHT:
	case DOOR:
		if (strcmp(sensorStatus, MSGON) == 0)
			stateVector[sensor1] = ON;
		else if (strcmp(sensorStatus, MSGOFF) == 0)
			stateVector[sensor1] = OFF;
		else if (strcmp(sensorStatus, MSGOPEN) == 0)
			stateVector[sensor1] = ON;
		else if (strcmp(sensorStatus, MSGCLOSED) == 0)
			stateVector[sensor1] = OFF;
		else
			if (OutputLevel > 1)
				printf("Invalid message for sensor type %d: %s - Ignoring\n", sensortypes[sensor1], sensorStatus);
		break;
	case TEMPERATURE:
	case LIGHTLEVEL:
	case CABINET:
		stateVector[sensor1] = (float) atof(sensorStatus); // convert the string to a float (if string isn't a number, will be 0.0)
		break;
	default:
		if (OutputLevel > 1)
			printf("Unknown sensor type: %d - Ignoring\n", sensortypes[sensor1]);
		break;
	}

	switch(sensortypes[sensor2]) {
	case MOTION:
	case LIGHT:
	case DOOR:
		if (strcmp(sensorStatus, MSGON) == 0)
			stateVector[sensor2] = ON;
		else if (strcmp(sensorStatus, MSGOFF) == 0)
			stateVector[sensor2] = OFF;
		else if (strcmp(sensorStatus, MSGOPEN) == 0)
			stateVector[sensor2] = ON;
		else if (strcmp(sensorStatus, MSGCLOSED) == 0)
			stateVector[sensor2] = OFF;
		else
			if (OutputLevel > 1)
				printf("Invalid message for sensor type %d: %s - Ignoring\n", sensortypes[sensor1], sensorStatus);
		break;
	case TEMPERATURE:
	case LIGHTLEVEL:
	case CABINET:
		stateVector[sensor2] = (float) atof(sensorStatus); // convert the string to a float (if string isn't a number, will be 0.0)
		break;
	default:
		if (OutputLevel > 1)
			printf("Unknown sensor type: %d - Ignoring\n", sensortypes[sensor2]);
		break;
	}
}

/*
 * Function to compute sampling features (including computing the sample vectors).
 */
int ComputeSampleFeatures(int timeSinceStart, char *sensorid1, char *sensorid2, char *sensorStatus, float *tempdataSamples) {
	int end = 0;
	int samplingWSize;
	int i, j;
	float max, min, sum, percentile;
	int offset = 0;
	float *binLimits = NULL; //store the limits (breakpoints) of the bins
	int *binCounts = NULL; // counts of values in each bin
	int numPeaks, timeLastPeak;

	binLimits = (float *) malloc((numBins + 1) * sizeof(float));
	if (binLimits == NULL)
		MemoryError("ComputeSampleFeatures");

	binCounts = (int *) malloc(numBins * sizeof(float));
	if (binCounts == NULL)
		MemoryError("ComputeSampleFeatures");

	// Loop through and create the sampled state vectors since the last one that was created
	// Do so in a way that keeps only the last MaxSamplesToKeep samples in the window
	while(NumSamples*SampleInterval <= timeSinceStart) {
		// If the next sample falls on the current time, update the state to the current event before saving
		if(NumSamples*SampleInterval == timeSinceStart)
			UpdateStateVector(sensorid1, sensorid2, sensorStatus);

		// TODO: If the current event is between samples, should it update the last sample to include changes from itself?
		// (e.g. if M001 ON happens between samples, should it change M001 to on in the vector, even though it does not fall on a sample point?)

		// Increment the count of items in the window until we fill it up
		// If it is full, we want to shift down old samples to make room for the new one
		// (Newest is always the last index)
		if (SampleWindowCount < MaxSamplesToKeep) {
			SampleWindowCount++; //increment the count if it's less than the max to keep
			if (SampleWindowCount == MaxSamplesToKeep)
				end = 1; //we have reached the full window
		} else {
			// Drop the oldest (lowest-index) sample vector and shift the rest down:
			for (int i = 0; i < MaxSamplesToKeep - 1; i++) {
				for (int j = 0; j < NumSensors; j++)
					sampleStateVectors[i][j] = sampleStateVectors[i+1][j];
			}
		}

		for (int i = 0; i < NumSensors; i++) {
			sampleStateVectors[SampleWindowCount - 1][i] = stateVector[i];
		}

		NumSamples++;
	}

	// Update the state vector now for future samples
	UpdateStateVector(sensorid1, sensorid2, sensorStatus);

	if (SampleWindowCount >= MaxSamplesToKeep)
		end = 1; //signify that the window is full

	// Compute the sampling features if we have a full window:
	if (end == 1) {
		samplingWSize = GetSamplingWindow(sensorid1);

		// Attributes 0...NumSensors-1: maximum value for each sensor during the window
		for (i = 0; i < NumSensors; i++) {
			max = INT_MIN; //initialize to minimum int value possible
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				if (sampleStateVectors[j][i] > max) {
					max = sampleStateVectors[j][i];
				}
			}
			samplingMax[i] = max;
			tempdataSamples[i] = max;
		}

		// Attributes NumSensors...2*NumSensors-1: minimum value for each sensor during the window
		offset = NumSensors;
		for (i = 0; i < NumSensors; i++) {
			min = INT_MAX; //initialize to maximum int value possible
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				if (sampleStateVectors[j][i] < min)
					min = sampleStateVectors[j][i];
			}
			samplingMin[i] = min;
			tempdataSamples[offset + i] = min;
		}

		// Attributes 2*NumSensors...3*NumSensors-1: sum of values for each sensor during the window
		offset = 2*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			samplingSum[i] = 0; //init the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				samplingSum[i] += sampleStateVectors[j][i];
			}
			tempdataSamples[offset + i] = samplingSum[i];
		}

		// Attributes 3*NumSensors...4*NumSensors-1: mean value of each sensor during the window
		offset = 3*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			samplingMean[i] = samplingSum[i]/(float) samplingWSize; //previous features were sum, so divide that by number of samples
			tempdataSamples[offset + i] = samplingMean[i];
		}

		// Sort the values for each sensor into non-decreasing order
		// Compute the median value of each sensor during the window:
		for (i = 0; i < NumSensors; i++) {
			// Reallocate the sortedValues array to be the length of our sampling size:
			sortedValues[i] = (float *) realloc(sortedValues[i], samplingWSize * sizeof(float *));
			if (sortedValues[i] == NULL)
				MemoryError("ComputeSampleFeatures");

			// Now save the values for this sensor from the desired window size into the new array
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sortedValues[i][j] = sampleStateVectors[j][i];
			}

			// Now sort the values:
			// TODO: Find a faster method than just using qsort?
			qsort(sortedValues[i], samplingWSize, sizeof(float), floatcomp);

			// Find the median value
			if (samplingWSize % 2) // if the window size is odd
				samplingMedian[i] = sortedValues[i][(int) floor(((float) samplingWSize) / 2)];
			else //if the window size is even
				samplingMedian[i] = (sortedValues[i][samplingWSize/2 - 1] + sortedValues[i][samplingWSize/2])/2;
		}

		// Attributes 4*NumSensors...5*NumSensors-1: mean absolute deviation of each sensor during the window
		offset = 4*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			sum = 0; //init the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sum += fabsf(sampleStateVectors[j][i] - samplingMean[i]);
			}
			tempdataSamples[offset + i] = sum / (float) samplingWSize;
		}

		// Attributes 5*NumSensors...6*NumSensors-1: median absolute deviation of each sensor during the window
		offset = 5*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			sum = 0; //init the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sum += fabsf(sampleStateVectors[j][i] - samplingMedian[i]);
			}
			tempdataSamples[offset + i] = sum / (float) samplingWSize;
		}

		// Attributes 6*NumSensors...7*NumSensors-1: standard deviation of each sensor during the window
		offset = 6*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			sum = 0; //init the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sum += powf(sampleStateVectors[j][i] - samplingMean[i], 2);
			}
			samplingStdDev[i] = sqrtf(sum / (float) samplingWSize);
			tempdataSamples[offset + i] = samplingStdDev[i];
		}

		// Attributes 7*NumSensors...8*NumSensors-1: coefficient of variation for each sensor during the window
		// TODO: Deal with negative-valued sensors here?
		// TODO: Also, deal with zero-mean values (divide by zero)?
		offset = 7*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			if (samplingMean[i] != 0)
				tempdataSamples[offset + i] = samplingStdDev[i] / samplingMean[i];
			else
				tempdataSamples[offset + i] = 0; //set to zero if would get a nan
		}

		// Attributes 8*NumSensors...9*NumSensors-1: number of zero-crossings (crossing the median) for each sensor during the window
		offset = 8*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			sum = 0; //init the sum
			for (j = (MaxSamplesToKeep -2); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				if (((sampleStateVectors[j][i] < samplingMedian[i]) && (sampleStateVectors[j+1][i] > samplingMedian[i])) //if the first value is less than the median and second is greater
						|| ((sampleStateVectors[j][i] > samplingMedian[i]) && (sampleStateVectors[j+1][i] < samplingMedian[i]))) //or the first is greater and the second less
					sum++; //add to the number of zero-crossings
			}
			tempdataSamples[offset + i] = sum;
		}

		// TODO: Possibly revisit the percentiles to get better values and make sure they're correct? (interpolation, etc)

		// Attributes 9*NumSensors...10*NumSensors-1: 25th percentile for each sensor during the window
		// Attributes 10*NumSensors...11*NumSensors-1: square sum of the 25th percentile for each sensor during the window
		// Attributes 11*NumSensors...12*NumSensors-1: 50th percentile for each sensor during the window
		// Attributes 12*NumSensors...13*NumSensors-1: square sum of the 50th percentile for each sensor during the window
		// Attributes 13*NumSensors...14*NumSensors-1: 75th percentile for each sensor during the window
		// Attributes 14*NumSensors...15*NumSensors-1: square sum of the 75th percentile for each sensor during the window
		offset = 9*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			// Find the 25th percentile
			percentile = Percentile(25.0,samplingWSize, sortedValues[i]);
			tempdataSamples[offset + i] = percentile;

			// Compute the square sum for the 25th percentile:
			sum = 0;
			for (j = 0; j <= samplingWSize; j++) {
				if (sortedValues[i][j] < percentile)
					sum += powf(sortedValues[i][j], 2);
			}
			tempdataSamples[offset + NumSensors + i] = sum;

			// Find the 50th percentile
			percentile = Percentile(50.0,samplingWSize, sortedValues[i]);
			tempdataSamples[offset + 2*NumSensors + i] = percentile;

			// Compute the square sum for the 50th percentile:
			sum = 0;
			for (j = 0; j <= samplingWSize; j++) {
				if (sortedValues[i][j] < percentile)
					sum += powf(sortedValues[i][j], 2);
			}
			tempdataSamples[offset + 3*NumSensors + i] = sum;

			// Find the 75th percentile
			percentile = Percentile(75.0,samplingWSize, sortedValues[i]);
			tempdataSamples[offset + 4*NumSensors + i] = percentile;

			// Compute the square sum for the 75th percentile:
			sum = 0;
			for (j = 0; j <= samplingWSize; j++) {
				if (sortedValues[i][j] < percentile)
					sum += powf(sortedValues[i][j], 2);
			}
			tempdataSamples[offset + 5*NumSensors + i] = sum;
		}

		// Attributes 15*NumSensors...16*NumSensors-1: interquartile range for each sensor during the window
		offset = 15*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			tempdataSamples[offset + i] = tempdataSamples[13*NumSensors + i] - tempdataSamples[9*NumSensors + i];
		}

		// We will use 10 bins for now (in the future we will probably want this to be more dynamic?)  The bins equaly divide the value range of the sensor's values
		// Attributes 16*NumSensors...26*NumSensors-1: number of values in the bins for each sensor during the window
		// First NumSensor values are the first bin, second NumSensor values the second, etc
		offset = 16*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			// Compute the breakpoints of each bin (lower limit of the first bin will be the minimum value):
			binLimits[0] = samplingMin[i] - 0.1; //subtract a little from the lower limit so it will be > than the value
			for (j = 1; j <= numBins; j++) {
				binLimits[j] = ((float) j) * (samplingMax[i] - samplingMin[i]) / ((float) numBins) + samplingMin[i];
				binCounts[j-1] = 0;
			}

			// Accumulate the counts for the bins:
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				for (int k = 0; k < numBins; k++) {
					if ((sampleStateVectors[j][i] > binLimits[k]) && (sampleStateVectors[j][i] <= binLimits[k+1]))
						binCounts[k] += 1;
				}
			}

			// Save the counts as features:
			for (j = 0; j < numBins; j++)
				tempdataSamples[offset + (j*NumSensors) + i] = binCounts[j];
		}

		// Attributes 26*NumSensors...27*NumSensors-1: skewness of each sensor during the window
		offset = 26*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			// Compute the numerator:
			sum = 0; //init the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sum += powf(sampleStateVectors[j][i] - samplingMean[i], 3);
			}
			tempdataSamples[offset + i] = sum / (float) samplingWSize;

			// Compute the denominator:
			sum = 0; //reinit the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sum += powf(sampleStateVectors[j][i] - samplingMean[i], 2);
			}
			sum = powf(sum / (float)samplingWSize, 1.5);

			if (sum != 0)
				tempdataSamples[offset + i] = tempdataSamples[offset + i] / sum;
			else
				tempdataSamples[offset + i] = 0; //set to zero if would have had a divide by zero error
		}

		// Attributes 27*NumSensors...28*NumSensors-1: kurtosis of each sensor during the window
		offset = 27*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			// Compute the numerator:
			sum = 0; //init the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sum += powf(sampleStateVectors[j][i] - samplingMean[i], 4);
			}
			tempdataSamples[offset + i] = sum / (float) samplingWSize;

			// Compute the denominator:
			sum = 0; //reinit the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sum += powf(sampleStateVectors[j][i] - samplingMean[i], 2);
			}
			sum = powf(sum / (float)samplingWSize, 3);

			if (sum != 0)
				tempdataSamples[offset + i] = tempdataSamples[offset + i] / sum - 3;
			else
				tempdataSamples[offset + i] = 0; //set to zero if would have had a divide by zero error
		}

		// Attributes 28*NumSensors...29*NumSensors-1: signal energy of each sensor during the window
		offset = 28*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			sum = 0; //init the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sum += powf(sampleStateVectors[j][i], 2);
			}
			tempdataSamples[offset + i] = sum;
		}

		// Attributes 29*NumSensors...30*NumSensors-1: log signal energy of each sensor during the window
		offset = 29*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			sum = 0; //init the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				if (sampleStateVectors[j][i] != 0)
					sum += log10f(powf(sampleStateVectors[j][i], 2)); // only add if it won't give a nan log
			}
			tempdataSamples[offset + i] = sum;
		}

		// Attributes 30*NumSensors...31*NumSensors-1: signal power of each sensor during the window
		offset = 30*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			sum = 0; //init the sum
			for (j = (MaxSamplesToKeep -1); j >= (MaxSamplesToKeep - samplingWSize); j--) {
				sum += powf(sampleStateVectors[j][i], 2);
			}
			tempdataSamples[offset + i] = sum / (float)samplingWSize;
		}

		// Attributes 31*NumSensors...32*NumSensors-1: peak-to-peak amplitude for each sensor during the window
		offset = 31*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			tempdataSamples[offset + i] = samplingMax[i] - samplingMin[i];
		}

		// Attributes 32*NumSensors...33*NumSensors-1: average time between peaks of each sensor during the window
		// Attributes 33*NumSensors...34*NumSensors-1: number of peaks for each sensor during the window
		// We will currently define a peak as a point where the value before and the value after are less than the value
		offset = 32*NumSensors;
		for (i = 0; i < NumSensors; i++) {
			sum = 0; //init the sum (for time between peaks)
			numPeaks = 0; //init the count for the number of peaks
			timeLastPeak = -1; //init the time of the last peak
			// Loop from the second to last point down to the second point of the window (as end points can't be peaks):
			for (j = (MaxSamplesToKeep -2); j > (MaxSamplesToKeep - samplingWSize); j--) {
				if (sampleStateVectors[j][i] > sampleStateVectors[j-1][i] && sampleStateVectors[j][i] > sampleStateVectors[j+1][i]) {
					if (numPeaks > 0) {
						sum += timeLastPeak - j;
					}
					timeLastPeak = j;
					numPeaks++;
				}
			}
			if (numPeaks > 1)
				tempdataSamples[offset + i] = sum / (numPeaks - 1); //compute average time between peaks
			else
				tempdataSamples[offset + i] = 0; // set it to zero if we don't have more than one peak to find times from
			tempdataSamples[offset + NumSensors + i] = numPeaks;
		}
	}

	if (binLimits != NULL)
		free(binLimits);

	if (binCounts != NULL)
		free(binCounts);

	return end;
}

/*
 * Function to get the sampling window size to use for the given sensor.
 * TODO: Actually implement storing and retrieving/using window sizes per sensor
 * For now, simply returns the number of stored samples
 */
int GetSamplingWindow(char *sensorName) {
	return MaxSamplesToKeep;
}

/*
 * Comparator function for the median qsort.  Simply provides negative if first value
 * is less than the second, 0 if equal, and positive if greater (float sort).
 */
int floatcomp(const void* a, const void* b) {
	if (*(const float*)a < *(const float*)b)
		return -1;
	return *(const float*)a > *(const float*)b;
}

/*
 * Function to compute the percentile value for the given percentage and sorted array.
 * (see second definition at http://en.wikipedia.org/wiki/Percentile)
 * Inputs:
 * percent: the percetage to use for the percentile
 * N: the number of samples in the array
 * sortedValues: the values of the array, sorted from least to greatest
 */
float Percentile(float percent, int N, float *sortedValues) {
	float rank;

	// Compute the rank of the percentile:
	rank = percent*(float)N/100-0.5; //get the rank (starting with zero)
	// Check if the rank is outside the first or last values:
	if (rank < 0)
		rank = 0;
	else if (rank > N - 1)
		rank = N - 1;

	// Now, check if the rank is exactly on an index or not
	// If not, we will need to interpolate the value:
	if (floorf(rank) == rank)
		return sortedValues[(int) rank];
	else {
		return sortedValues[(int) floorf(rank)] + ((float)N*percent/100-(floorf(rank)+0.5))*(sortedValues[(int) ceilf(rank)]-sortedValues[(int) floorf(rank)]);
	}
}
