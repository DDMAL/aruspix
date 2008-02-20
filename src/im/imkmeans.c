/*************************************************************
 *   kmeans image segmentation
 *
 *   Input:
 *          ima: a 1D buffer 
 *          imsize: its size 
 *          mask: the buffer for the mask, 
 *          k: the number of classes.  
 *   Output:
 *   	   mu: The k means values, returned by the function.
 *   
 *   old:   mu: vector of k means values
 *          mask: classification image mask
 *
 *   Created by Tristan Matthews, May 2007
 *
 *   Based on kmeans.m by:
 *   	Author: Jose Vicente Manjon Herrera
 *   	Email: jmanjon@fis.upv.es
 *    	Date: 27-08-2005
 *************************************************************/

/* Todo: -figure out what's up with mask calculation (although
 * 		the original gives the same result)
 * 	 -I think mu should be allocated by the caller
 * 	 -OPTIMIZE!!!
 */

#include "imkmeans.h"

//#include <im.h>
//#include <im_image.h>
//#include <im_convert.h>
//#include <im_process.h>
//#include <im_binfile.h>
//#include <im_counter.h>

#include <math.h>
//#include <string.h>
#include <stdlib.h>

#define NOT_FOUND -1

// finds minimum value in imbyte vector
imbyte vecMin(imbyte *im, const int imsize)
{
	int i;
	if (imsize < 1)
		exit(EXIT_FAILURE);	

	imbyte min = im[0];

	for (i = 1; i < imsize; i++)
	{
		if (im[i] < min)
			min = im[i];
	}
	return min;
}

// finds minimum value in double vector
double vecDblMin(double *vec, const int vecSize, int *index)
{
	*index = 0;
	int i;
	if (vecSize < 1)
		exit(EXIT_FAILURE);	

	double min = vec[0];

	for (i = 1; i < vecSize; i++)
	{
		if (vec[i] < min)
		{
			min = vec[i];
			*index = i;
		}	
	}
	return min;
}

// finds maximum value in imbyte vector
imbyte vecMax(imbyte *im, const int imsize)
{
	int i;
	if (imsize < 1)
		exit(EXIT_FAILURE);	

	imbyte max = im[0];

	for (i = 1; i < imsize; i++)
	{
		if (im[i] > max)
			max = im[i];
	}
	return max;
}

// finds index in an imbyte vector equal to a value
int vecFindEq(imbyte *im, const int imsize, const imbyte value)
{
	int i;
	if (imsize < 1)
		exit(EXIT_FAILURE);

	for (i = 0; i < imsize; ++i)
		if (im[i] == value) return i;

	return NOT_FOUND;	// found nothing
}

// finds index in a double vector equal to a value
/*int vecDblFindEq(double *im, const int imsize, const double value)
{
	int i;
	if (imsize < 1)
		exit(EXIT_FAILURE);

	for (i = 0; i < imsize; ++i)
		if (im[i] == value) return i;

	return NOT_FOUND;	// found nothing
}
*/

// adds a constant to an image vector of size imsize
void vecAddConst(imbyte *im, const int imsize, const imbyte value)
{
	int i;
	if (imsize < 1)
		exit(EXIT_FAILURE);

	for (i = 0; i < imsize; ++i)
		im[i] += value;
}

// Returns 1 if vectors are equal, else returns 0
int vecEq(const double *A, const double *B, const int imsize)
{
	int i;
	if (imsize < 1)
		exit(EXIT_FAILURE);

	for (i = 0; i < imsize; ++i)
		if (A[i] != B[i]) return 0;

	return 1;
}

double *kmeans(imbyte *im, const int imsize, imbyte *mask, const int k)
{
	int i, j, counter, idx;

	imbyte *imcopy;
	if ((imcopy = (imbyte *) malloc(sizeof(imbyte) * imsize)) == NULL) 
		exit(EXIT_FAILURE);
	memcpy(imcopy, im, imsize);	// copies im to imcopy

	imbyte mi = vecMin(im, imsize); // deal with negative 
	vecAddConst(im, imsize, 1 - mi);     // and zero values
										
	// create image histogram
	imbyte maxPlusOne = vecMax(im, imsize) + 1;			
	imbyte *a;
	if ((a = (imbyte *) malloc(sizeof(imbyte) * maxPlusOne)) == NULL)
		exit(EXIT_FAILURE);
	memset(a, 0x0, sizeof(imbyte) * maxPlusOne);
	
	imbyte *h;
	if ((h = (imbyte *) malloc(sizeof(imbyte) * maxPlusOne)) == NULL)
		exit(EXIT_FAILURE);
	memset(h, 0x0, sizeof(imbyte) * maxPlusOne);

	imbyte *hc;
	if ((hc = (imbyte *) malloc(sizeof(imbyte) * maxPlusOne)) == NULL) 
		exit(EXIT_FAILURE);
	memset(hc, 0x0, sizeof(imbyte) * maxPlusOne);

	int lengthInd = 0;				
	for (i = 0; i < imsize; ++i)
	{
		if (im[i] > 0)
		{
			h[(int) im[i] - 1] += 1;	// - 1 due to zero based index		
			lengthInd++;	// number of non-zeros
		}
	}
	

	int *ind;
	if ((ind = (int *) malloc(sizeof(int) * lengthInd)) == NULL) 
		exit(EXIT_FAILURE);
	memset(ind, 0x0, sizeof(int) * lengthInd);
	
	double *c;
	if ((c = (double *) malloc(sizeof(double) * k)) == NULL)  
		exit(EXIT_FAILURE);
	memset(c, 0x0, sizeof(double) * k);

	for (i = 0, j = 0; i < maxPlusOne; ++i)		
	{
		if (h[i] != 0)
		{
			ind[j] = i;	// indices of non-zero values of h
			j++;		
		}
	}

	// initiate centroids
	double *mu;			
	if ((mu = (double *) malloc(sizeof(double) * k)) == NULL)
		exit(EXIT_FAILURE);
	for (i = 0; i < k; ++i)
		mu[i] = (i + 1.0) * maxPlusOne / (k + 1.0);

	double *oldmu;
	if ((oldmu = (double *) malloc(sizeof(double) * k)) == NULL)
		exit(EXIT_FAILURE);

	double cc;	
	// start process
	while (1)
	{
		memcpy(oldmu, mu, sizeof(double) * k);
		// current classification

		for (i = 0; i < lengthInd; ++i)
		{
			for (j = 0; j < k; ++j)
				c[j] = fabs((ind[i] + 1.0) - mu[j]);	// changed for zero-indexing	

			cc = vecDblMin(c, k, &idx); 	   // cc is the index
			//cc = vecDblFindEq(c, k, cc);  // of the min value of c	
			hc[ind[i]] = idx + 1;	// changed for zero-indexing	
		}

		// recalculation of means

		memset(a, 0x0, maxPlusOne);
		double leftSum, rightSum;
		for (i = 1; i <= k; ++i)
		{
			counter = 0;
			
			for (j = 0; j < maxPlusOne; ++j)		// a=find(hc==i);
			{
				if (hc[j] == i)
				{
					a[counter] = j + 1;	// adjusted for zero-indexing
					++counter;
				}
			}

			leftSum = rightSum = 0.0;

			for (j = 0; j < counter; ++j)		// mu(i)=sum(a.*h(a))/sum(h(a));
			{
				leftSum += a[j] * h[(int) a[j] - 1];	// adjusted for zero-indexing	
				rightSum += h[(int) a[j] - 1]; 		// adjusted for zero-indexing	
			}
			mu[i - 1] = leftSum / rightSum;			// adjusted for zero-indexing	
		}

		if (vecEq(mu, oldmu, k))
			break;
	}

	// calculate mask
	double dmin;
	for (i = 0; i < imsize; ++i)
	{
		memset(c, 0x0, k);
		for (j = 0; j < k; ++j)
			c[j] = fabs(imcopy[i] - mu[j]);
		
		dmin = vecDblMin(c, k, &idx);
		mask[i] = 1 + idx;
	}

	free(imcopy);	// deallocate space
	free(h);
	free(hc);
	free(ind);
	//	free(mu);	// can't return it if i free it
	free(oldmu);
	free(c);
	free(a);

	for (i = 0; i < k; i++)
		mu[i] = mu[i] + mi - 1;		// recover real range 

	return mu; 
}
