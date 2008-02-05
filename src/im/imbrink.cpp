/*  ENTROPY_BRINK
*
*   Created by Tristan Matthews, 2007.
*   Based on "entropy_brink.m" and "binar_ashley.m" by
*   John Ashley Burgoyne & Greg Eustace, McGill University  
*
*   Usage:
*    Topt => Optimal threshold
*    p = h / sum(h);                 % pmf (i.e., normalized histogram)
*    p_rep = repmat(p, G + 1, 1);
*    m_f = cumsum(g .* p).';         % first foreground moment
*    m_b = m_f(end) - m_f;         % first background moment
*    g = (0:G);                  % grayscale vector - column dimension
*    G = 255;                    % maximum grayscale value
*
*   References: Brink, A., and Pendock, N. 1996. Minimum cross-entropy
*   threshold selection. Pattern Recognition 29: 179-188. 
*
*/

#include "imext.h"
//#include <stdlib.h>
//#include <string.h>
//#include <math.h>

// IMLIB
#include <im.h>
#include <im_image.h>
#include <im_process_ana.h>
#include <im_process_pon.h>

// define some global constants

static unsigned short const MAX_GRAY = 256; // always used as a size variable, hence 255+1
//static unsigned short const TRUE = 1;
//static unsigned short const FALSE = 0;
static unsigned short const NON_CUMULATIVE = 0;
static const size_t VEC_DBL_SZ = sizeof(double) * MAX_GRAY;

// returns sum of a vector of unsigned longs
unsigned long vecSum(unsigned long *vec, const int vecSize)
{
	if (vecSize < 1)
		exit(EXIT_FAILURE);

	int i; 
	unsigned long result = 0;

	for (i = 0; i < vecSize; ++i)
		result += vec[i];
	
	return result;
}

int calcTopt(double m_f[MAX_GRAY], double m_b[MAX_GRAY], double vec[MAX_GRAY])
{
	int i; 
	double locMin;
	int isMinInit = FALSE;
	int Topt = 0;

	for (i = 0; i < MAX_GRAY; ++i)
	{
		if (m_f[i] != 0 && m_b[i] != 0)
		{
			if (!isMinInit || vec[i] < locMin)
			{
				isMinInit = TRUE;
				locMin = vec[i];	// gets a new minimum
				Topt = i;
			}
		}
	}
		
	return Topt + 1;	// DO I NEED TO ADD ONE
}

// computes the diagonal of the cumulative sum of an array Mat, stores result in diag
void diagCumSum(double diag[MAX_GRAY], const double mat[MAX_GRAY][MAX_GRAY])
{
	int i, j;
	double tmp[MAX_GRAY][MAX_GRAY];

	memcpy(tmp[0], mat[0], VEC_DBL_SZ);	 // copies first row of mat

	for (i = 1; i < MAX_GRAY; ++i)		// get cumulative sum
		for (j = 0; j < MAX_GRAY; ++j)
			tmp[i][j] = tmp[i-1][j] + mat[i][j];

	for (i = 0; i < MAX_GRAY; ++i)		// set to diagonal
		diag[i] = tmp[i][i];
}


void sumMinusDiagCumSum(double resultVec[MAX_GRAY], const double mat[MAX_GRAY][MAX_GRAY])
{
	// based on: tmpb5 = sum(tmpb4, 1).' - diag(cumsum(tmpb4, 1));   % sum columns, subtract diagonal of cumsum of tmp4 
	int i, j;
	double tmp[MAX_GRAY];

	memcpy(resultVec, mat[0], sizeof(double) * MAX_GRAY);	// copy first row in	

	for (i = 1; i < MAX_GRAY; ++i)		
		for (j = 0; j < MAX_GRAY; ++j)
			resultVec[j] += mat[i][j];	// sums of columns of mat

	diagCumSum(tmp, mat);		

	for (i = 0; i < MAX_GRAY; ++i)		
		resultVec[i] -= tmp[i];
}

int imProcessBrinkThreshold(const imImage* image, imImage* dest, bool white_is_255 )
{
	imImage *src = imImageDuplicate( image );
     
	if ( !white_is_255 )
		imProcessNegative( src, src );
	
	int i, j;
	int Topt = 0;

	double p[MAX_GRAY];				// pmf (i.e. normalized histogram)
	unsigned long histo[MAX_GRAY];			// from imlib: "Histogram is always 256 positions long"

	double m_f[MAX_GRAY];			// first foreground moment
	double m_b[MAX_GRAY];			// first background moment

	double tmp0[MAX_GRAY][MAX_GRAY];			
	double tmp1[MAX_GRAY][MAX_GRAY];			
	double tmp3[MAX_GRAY][MAX_GRAY];			
	double tmp4[MAX_GRAY][MAX_GRAY];			

	double tmpVec1[MAX_GRAY];
	double tmpVec2[MAX_GRAY];

	imCalcGrayHistogram(src, histo, NON_CUMULATIVE);  	// gray histogram computed

	double invHistSum = 1.0 / vecSum(histo, MAX_GRAY);	// inverse of the sum

	for (i = 0; i < MAX_GRAY; ++i)
		p[i] = histo[i] * invHistSum;		// equivalent to dividing by the sum, but faster!


	m_f[0] = 0.0;
	for (i = 1; i < MAX_GRAY; ++i)			// m_f = cumsum(g .* p).'; 
		m_f[i] = i * p[i] + m_f[i - 1];

	memcpy(m_b, m_f, VEC_DBL_SZ);		// m_b = m_f(end) - m_f; 

	for (i = 0; i < MAX_GRAY; ++i)
		m_b[i] = m_f[MAX_GRAY - 1] - m_b[i];		

	/****************** END OF BINAR_ASHLEY PORTION, START OF ENTROPY_BRINK********************************/

	for (i = 0; i < MAX_GRAY; ++i)		// tmp0 = m_f_rep ./ g_rep;
	{
		for (j = 0; j < MAX_GRAY; ++j)
		{
			tmp0[i][j] = m_f[j] / i;

			if ((m_f[j] == 0) || (i == 0)) 
			{
				tmp1[i][j] = 0.0;				// replace inf or NaN values with 0
				tmp3[i][j] = 0.0;
			}
			else
			{
				tmp1[i][j] = log(tmp0[i][j]);
				tmp3[i][j] = log(1.0 / tmp0[i][j]);;
			}
			tmp4[i][j] = p[i] * (m_f[j] * tmp1[i][j] + i * tmp3[i][j]);
		}
	}

	diagCumSum(tmpVec1, tmp4);	// tmpVec is now the diagonal of the cumulative sum of tmp4

	// same operation but for background moment, NOTE: tmp1 through tmp4 get overwritten
	for (i = 0; i < MAX_GRAY; ++i)		
	{
		for (j = 0; j < MAX_GRAY; ++j)
		{
			tmp0[i][j] = m_b[j] / i;	// tmpb0 = m_b_rep ./ g_rep;

			if ((m_b[j] == 0) || (i == 0)) 
			{
				tmp1[i][j] = 0.0;				// replace inf or NaN values with 0
				tmp3[i][j] = 0.0;
			}
			else
			{
				tmp1[i][j] = log(tmp0[i][j]);
				tmp3[i][j] = log(1.0 / tmp0[i][j]);;
			}
			tmp4[i][j] = p[i] * (m_b[j] * tmp1[i][j] + i * tmp3[i][j]);
		}
	}

	sumMinusDiagCumSum(tmpVec2, tmp4);		// sum columns, subtract diagonal of cumsum of tmp4 

	for (int i = 0; i < MAX_GRAY; ++i)
		tmpVec1[i] += tmpVec2[i];

	Topt = calcTopt(m_f, m_b, tmpVec1);		// DO I NEED TO ADD ONE?

	imProcessThreshold(src, dest, Topt, TRUE);
	imProcessBitwiseNot(dest, dest);		// HACK ALERT: HAVE TO FLIP BITS

	return Topt;
}
