/**
 * Summary:
 *    Calculate
 *    returns:
 *         outVector[0] = sum;
 *         outVector[1] = ave;
 *         outVector[2] = adev;
 *         outVector[3] = var;
 *         outVector[4] = skew;
 *         outVector[5] = curt;
 *         outVector[6] = sdev;
 *         outVector[7...16] = 666666.666666; //contant number
 */

#include <stdio.h>
#include <stdlib.h>
#include "Maxfiles.h"
#include <MaxSLiCInterface.h>

int check(int vectorSize, float *outVector, float *expectedVector)
{
	int status = 0;
	float epsilon = 0.05;
	for (int i = 0; i < vectorSize * Vectors_vectorSize; i++) {
		if (fabs(outVector[i] - expectedVector[i]) > epsilon) {
			fprintf(stderr, "Error detected outVector[%f] != %f\n",
				outVector[i], expectedVector[i]);
			status = 1;
		}
	}
	return status;
}

void VectorsCPU(int32_t vectorSize, float *inVector, float *outVector, float sum)
{
    int j, n = 16;
    float ep=0.0, s, p, adev, var, skew, curt, sdev;
    float ave;
    s=0.0;

    for (j=0; j<n; j++) s += inVector[j];
    outVector[0] = s;
    ave=s/n;
    adev= 0.0;
    var= 0.0;
    skew= 0.0;
    curt=0.0;

    for (j=0; j<n; j++) {
        s=inVector[j]-(ave);
        adev += fabs(s);

        ep += s;
        var += (p=s*s);
        skew += (p *= s);
        curt += (p *= s);
    }


    adev /= n;
    var=(var-ep*ep/n)/(n-1);

    sdev=sqrt(var);
    skew /= (n*(var)*(sdev));
    curt=(curt)/(n*(var)*(var))-3.0;


    outVector[1] = ave;
    outVector[2] = adev;
    outVector[3] = var;
    outVector[4] = skew;
    outVector[5] = curt;
    outVector[6] = sdev;

	for (int i = 7; i < 16; i++) {
		outVector[i] = 666666.666666;
	}


}

int main()
{
	const int vectorSize = Vectors_vectorSize;
	const int streamSize = 16;

	size_t sizeBytes = vectorSize * streamSize * sizeof(uint32_t);
	float *inVector = malloc(sizeBytes);
	float *outVector = malloc(sizeBytes);
	float *expectedVector = malloc(sizeBytes);
    float sum = 0;
	for (int i = 0; i < vectorSize * streamSize; i++) {
		inVector[i] = (float)i;
		sum += (float)i;
	}


	VectorsCPU(streamSize, inVector, outVector, sum);

	printf("Running DFE.\n");
	Vectors(vectorSize, inVector, expectedVector);
	int status = check(streamSize, outVector, expectedVector);
	if (status)
		printf("Test failed.\n");
	else
		printf("Test passed OK!\n");
	return status;
}
