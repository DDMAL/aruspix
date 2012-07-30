/////////////////////////////////////////////////////////////////////////////
// Name:        muswindow.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musrc.h"

//#define NBInt	40

static int factor = 100;
static long factor2;
//static int nbint = NBInt;
static int	*pCC = NULL;

/** static members **/
MusPoint MusRC::point_[];
MusPoint MusRC::bcoord[];
//MusPoint MusRC::point_[4];
//MusPoint MusRC::bcoord[2*(BEZIER_NB_POINTS+1)];

/** DEPRECATED
int MusRC::CC(int n, int i) 
{
	register int j;
	register int a;

	a = 1; 
	for (j = i + 1; j <= n; j++) 
		a *= j; 
	for (j = 1; j <= n - i; j++) 
		a /= j; 
	return a; 
} 


long MusRC::BBlend(int i, int n, long u) 
{
	register int j;
    long	b;

	b = pCC[i]; 
	for (j = 1; j <= i; j++) 
		b *= u;
	for (j = 1; j <= (n - i); j++) 
		b *= (factor - u); 
	for (j = 3; j <= n; j++) 
		b /= factor; 
	return b; 
}

int MusRC::InitBezier (register int n)
{
	register int i;

	if (pCC == NULL)
	{	pCC = (int *)malloc((n+1) * sizeof(int));

		if (pCC == NULL)
			return 0;
		for (i = 0; i <= n; i++)
			pCC[i] = CC (n,i);
		factor2 = (long)factor * (long)factor;
	}
	return 1;
}


void MusRC::Bezier(long *x, long *y, long u, int n ) 
{
	int        i;
    long       b;

	*x = 0; 
	*y = 0; 
	for (i = 0; i <= n; i++)
	{	b = BBlend(i, n, u); 
		*x += point_[i].x * b; 
		*y += point_[i].y * b; 
	} 
    *x /= factor2;
    *y /= factor2;

}


void MusRC::calcBez ( MusPoint *ptcoord, int _nbint )
{
	int          i;
   long       x, y;

	if (!InitBezier(3))
		return;

//	for (i = 0; i < _nbint; i++)
	for (i = 0; i <= _nbint; i++)
	{
		Bezier(&x, &y, (long)(i * 100) / (long)_nbint, 3);
		ptcoord->x = (int)x;
		ptcoord->y = (int)y;
		ptcoord++;
	}
	
}
end deprecated */
 
void MusRC::pntswap (MusPoint *x1, MusPoint *x2)
{	MusPoint a;
	a = *x1;
	*x1 = *x2;
	*x2 = a;
}

