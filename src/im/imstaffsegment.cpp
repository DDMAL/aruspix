/////////////////////////////////////////////////////////////////////////////
// Name:        imstaffsegment.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "imstaffsegment.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/file.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"

#include "imstaffsegment.h"
#include "impage.h"

#include "app/axapp.h"

// WDR: class implementations

// hauteur de la zone de correlation pour le calcul de la courbure de staff
// correspond au decalage maximum
#define CORRELATION_HEIGHT 20

// space between every vertical position estimation
#define POSITION_STEP 30

// window size used for vertical position estimation
#define POSITION_WIN 90

enum
{ 
	VALUES_POSITIONS = 0,
	VALUES_LINE_P,
	VALUES_LINE_M
};

// probleme de link avec im_lib ???
#ifdef __WXMSW__
	extern "C" 
	{
		int __mb_cur_max;
		unsigned short* _pctype;
	}
#endif


imImage* imdebug = NULL;
int stepdebug = 0;

/*

// features count per window - used for memory allocation 
// must be changed if function is modified
#define FEATURES_COUNT 18



// 06
// 07 idem avec win 4 et overlap 2


// fonction d'extration de caracteristique par fenetres
void CalcWinFeatures(const imImage* image, float *features, int position_v, int height, int width )
{
	// empiric adjustment	
	height -= width;
	
	// 
	int line_width = width * image->width;

	// valeurs par defaut
	int i;
	for ( i = 0; i < FEATURES_COUNT; i++ )
		features[i] = 0.0;
	features[16] = 1.0; // euler
	features[17] = 0.0; // area

	int half_staff_height = STAFF_HEIGHT / 2;
	int pos[8]; // line positions ; 90 au lieu de 89 à cause de margins
	pos[0] = half_staff_height + height / 2 + height / 4 + position_v;
	pos[1] = half_staff_height + height / 2 + position_v;
	pos[2] = half_staff_height + height / 4 + position_v;
	pos[3] = half_staff_height + position_v;
	pos[4] = half_staff_height - height / 4 + position_v;
	pos[5] = half_staff_height - height / 2 + position_v;
	pos[6] = half_staff_height - height / 2 - height / 4 + position_v;
	pos[7] = half_staff_height - height + position_v;
	
	imbyte *imBuf = (imbyte*)image->data[0];
	
	int line_offset;
	int l;
	// completer les lignes
	for (l = 0; l < 8; l++)
	{
		if ( ( pos[l] < 0 ) || ( pos[l] >= STAFF_HEIGHT ) )
			continue;
		line_offset = pos[l];
		memset( imBuf + line_offset * image->width,
			1, image->width );
	}
	
	//if ( imdebug )
	//	imProcessInsert( imdebug, image, imdebug, stepdebug, 0 );

	//global
    imImage *imRegions = imImageCreate(image->width, image->height, IM_GRAY, IM_USHORT);
    int region_count = imAnalyzeFindRegions ( image, imRegions, 8, 1 );

	if (region_count == 0)
	{	
		imImageDestroy( imRegions );
		return;
	}

	// euler (+-)
	features[16] = 1.0 / (region_count + 1); // global
	
	imStats stats;
	imCalcImageStatistics( image, &stats );
	int tot = stats.positive - 5 * line_width;
	if ( tot > 0 ) 
		features[17] = (float)(tot) / (image->size); // global
	else
	{
		imImageDestroy( imRegions );	
		return;
	}

	// remove staff lines from area
	imushort *regionsBuf = (imushort*)imRegions->data[0];
	
	int m, n, o = 0;
	for (l = 0; l < 8; l++,o++)
	{
		float sum = 0.0;
		for (m =  -(height / 8); m <= 0; m++)
		{
			line_offset = pos[l] +  m;
			if ( ( line_offset < 0 ) || ( line_offset >= STAFF_HEIGHT ) )
				continue;
			for (n = 0; n < image->width; n++)
				if ( *(regionsBuf + line_offset * image->width + n) )
					sum++;
		}
		if ( sum > ( line_width / 2 ) )
		//if ( stats.positive && ( stats.positive > sum ) )
			features[o] =  (float)(sum - (line_width/2)) / (image->size - 5 * line_width); // 06
			//features[o] =  (float)(sum) / (stats.positive); // 05
			//features[o] =  (float)(sum - line_width) / (tot); // 04
		sum = 0.0;
		for (m =  0; m <= (height / 8); m++)
		{
			line_offset = pos[l] + m;
			if ( ( line_offset < 0 ) || ( line_offset >= STAFF_HEIGHT ) )
				continue;
			for (n = 0; n < image->width; n++)
				if ( *(regionsBuf + line_offset * image->width + n) )
					sum++;
		}
		o++;
		if ( sum > ( line_width / 2 ) )
		//if ( stats.positive && ( stats.positive > sum ) )
			(features[o]) =  (float)(sum - (line_width/2)) / (image->size - 5 * line_width); // 06
			//features[o] =  (float)(sum) / (stats.positive); // 05
			//features[o] =  (float)(sum - line_width) / (tot); // 04
	}	
	
	imImageDestroy( imRegions );
}

*/

/*

// features count per window - used for memory allocation 
// must be changed if function is modified
#define FEATURES_COUNT 10

// 03
// 08 idem avec win 4 et overlap 2
// 04
// 09 idem avec win 4 et overlap 2
// 05


// fonction d'extration de caracteristique par fenetres
void CalcWinFeatures(const imImage* image, float *features, int position_v, int height, int width )
{
	// empiric adjustment	
	height -= width;
	
	// 
	int line_width = width * image->width;

	// valeurs par defaut
	int i;
	for ( i = 0; i < FEATURES_COUNT; i++ )
		features[i] = 0.0;
	features[8] = 1.0; // euler
	features[9] = 0.0; // area

	int half_staff_height = STAFF_HEIGHT / 2;
	int pos[8]; // line positions ; 90 au lieu de 89 à cause de margins
	pos[0] = half_staff_height + height / 2 + height / 4 + position_v;
	pos[1] = half_staff_height + height / 2 + position_v;
	pos[2] = half_staff_height + height / 4 + position_v;
	pos[3] = half_staff_height + position_v;
	pos[4] = half_staff_height - height / 4 + position_v;
	pos[5] = half_staff_height - height / 2 + position_v;
	pos[6] = half_staff_height - height / 2 - height / 4 + position_v;
	pos[7] = half_staff_height - height + position_v;
	
	imbyte *imBuf = (imbyte*)image->data[0];
	
	int line_offset;
	int l;
	// completer les lignes
	for (l = 0; l < 8; l++)
	{
		if ( ( pos[l] < 0 ) || ( pos[l] >= STAFF_HEIGHT ) )
			continue;
		line_offset = pos[l];
		memset( imBuf + line_offset * image->width,
			1, image->width );
	}
	
	//if ( imdebug )
	//	imProcessInsert( imdebug, image, imdebug, stepdebug, 0 );

	//global
    imImage *imRegions = imImageCreate(image->width, image->height, IM_GRAY, IM_USHORT);
    int region_count = imAnalyzeFindRegions ( image, imRegions, 8, 1 );

	if (region_count == 0)
	{	
		imImageDestroy( imRegions );
		return;
	}

	// euler (+-)
	features[8] = 1.0 / (region_count + 1); // global
	
	imStats stats;
	imCalcImageStatistics( image, &stats );
	int tot = stats.positive - 5 * line_width;
	if ( tot > 0 ) 
		features[9] = (float)(tot) / (image->size); // global
	else
	{
		imImageDestroy( imRegions );	
		return;
	}

	// remove staff lines from area
	imushort *regionsBuf = (imushort*)imRegions->data[0];
	
	int m, n;
	for (l = 0; l < 8; l++)
	{
		float sum = 0.0;
		for (m =  -(height / 8); m <= (height / 8); m++)
		{
			line_offset = pos[l] + m;
			if ( ( line_offset < 0 ) || ( line_offset >= STAFF_HEIGHT ) )
				continue;
			for (n = 0; n < image->width; n++)
				if ( *(regionsBuf + line_offset * image->width + n) )
					sum++;
		}
		if ( sum > line_width ) // 03 04
		//if ( stats.positive && ( stats.positive > sum ) ) // 05
			features[l] =  (float)(sum - line_width) / (image->size - 5 * line_width); // 03
			//features[l] =  (float)(sum) / (stats.positive); // 05
			//features[l] =  (float)(sum - line_width) / (tot); // 04
	}	
	
	imImageDestroy( imRegions );
}



*/



// features count per window - used for memory allocation 
// must be changed if function is modified
#define FEATURES_COUNT 7

// 01
// 02
// 10 with biggest black without line removal
// 11 with masked area (as in origninal)

//#define STAFF_HEIGHT 180
//#define STAFF 100

// fonction d'extration de caracteristique par fenetres
void CalcWinFeatures(const imImage* image, float *features, int position_v, int height, int width )
{
	
	if ( width > ( STAFF / 8 ) )
	{
		//wxLogDebug( "Staff width larger than half staff space" );
		width = STAFF / 8;
	}
	
	
	// empiric adjustment		
	height -= width;

	// valeurs par defaut
	int i;
	for ( i = 0; i < FEATURES_COUNT; i++ )
		features[i] = 0.0;
	features[0] = 1.0; // euler
	features[1] = 0.0; // area
	features[2] = 0.5; // centroids
	features[3] = 0.5;
	features[4] = 0.0; // bigest black
	features[5] = 1.0; // smallest white
	features[6] = 0.5; // bigest black cy

	int half_staff_height = STAFF_HEIGHT / 2;
	int pos[8]; // line positions ; 90 au lieu de 89 à cause de margins
	pos[0] = half_staff_height + height / 2 + height / 4 + position_v;
	pos[1] = half_staff_height + height / 2 + position_v;
	pos[2] = half_staff_height + height / 4 + position_v;
	pos[3] = half_staff_height + position_v;
	pos[4] = half_staff_height - height / 4 + position_v;
	pos[5] = half_staff_height - height / 2 + position_v;
	pos[6] = half_staff_height - height / 2 - height / 4 + position_v;
	pos[7] = half_staff_height - height + position_v;
	
	imbyte *imBuf = (imbyte*)image->data[0];
	
	int line_offset;
	int l;
	// completer les lignes
	for (l = 0; l < 8; l++)
	{
		if ( ( pos[l] < 0 ) || ( pos[l] >= STAFF_HEIGHT ) )
			continue;
		line_offset = pos[l];
		memset( imBuf + line_offset * image->width,
			1, image->width );
	}
	
	//if ( imdebug )
	//	imProcessInsert( imdebug, image, imdebug, stepdebug, 0 );

	//global
    imImage *imRegions = imImageCreate(image->width, image->height, IM_GRAY, IM_USHORT);
    int region_count = imAnalyzeFindRegions ( image, imRegions, 8, 1 );

	if (region_count == 0)
	{	
		imImageDestroy( imRegions );
		return;
	}

	// euler (+-)
	features[0] = 1.0 / (region_count + 1); // global

	// centroid
	float *cx = (float*)malloc(region_count*sizeof(float));
	memset( cx, 0, region_count*sizeof(float));
	float *cy = (float*)malloc(region_count*sizeof(float));
	memset( cy, 0, region_count*sizeof(float));
	imAnalyzeMeasureCentroid( imRegions, NULL, region_count, cx, cy );

	// remove staff lines from area
	imushort *regionsBuf = (imushort*)imRegions->data[0];
	
	if ( (width % 2) == 0 )
		width++;
	int m;

	for (l = 0; l < 8; l++)
	{
		for (m = -(width / 2); m <= (width / 2); m++)
		{
			line_offset = pos[l] + m;
			if ( ( line_offset < 0 ) || ( line_offset >= STAFF_HEIGHT ) )
				continue;
			memset( regionsBuf + line_offset * image->width,
				0, sizeof( imushort ) * image->width );
		}
	}	
	
	// area (without lines)
	int* area = (int*)malloc(region_count*sizeof(int));
	memset( area, 0, region_count*sizeof(int));
	imAnalyzeMeasureArea( imRegions, area );
	
	int tot_area = 0;
	int max_area = 0;
	float max_black_cy = 0.5;
	for (i = 0; i < region_count; i++)
	{
		tot_area += area[i];
		if ( area[i] > max_area )
		{
			max_area = area[i];
			max_black_cy = cy[i];
		}
	}
	features[1] = (float)tot_area /(image->width * image->height);
	// plus grand noir
	features[4] = float(max_area) /(image->width * image->height); // 01
	
	// intersafflines cy
	max_black_cy -= position_v;
	int interline = height / 4;
	max_black_cy += interline - (half_staff_height % interline) + interline / 2;
	max_black_cy = (int)max_black_cy % interline;
	//printf("interligne %d, max_black_cy %f\n", interline, max_black_cy);
	features[6] = max_black_cy / interline;

	// centroid
	float sum_cx = 0, sum_cy = 0;
	for (i = 0; i < region_count; i++)
	{
		sum_cx += cx[i] * area[i];
		sum_cy += cy[i] * area[i];
	}
	if (tot_area)
	{
		features[2] = ((sum_cy / tot_area) - 1 - position_v) / image->height;
		features[3] = ((sum_cx / tot_area) + 1) / image->width;
	}
	free(cy);
	free(cx);
	free(area);
	
	// get biggest black
    region_count = imAnalyzeFindRegions ( image, imRegions, 8, 1 );

	if (region_count != 0)
	{
		int* forground = (int*)malloc(region_count*sizeof(int));
		memset( forground, 0, region_count*sizeof(int));
		imAnalyzeMeasureArea( imRegions, forground );

		int max_forground = 0;
		for (i = 0; i < region_count; i++)
		{
			if ( forground[i] > max_forground )
				max_forground = forground[i];
		}
		free(forground);
		// plus grand noir
		//features[4] = float(max_forground) /(image->width * image->height); // 10
	}


	// get smallest white
	imImage *negate = imImageClone( image );
	imProcessNegative( image, negate );
    region_count = imAnalyzeFindRegions ( negate, imRegions, 8, 1 );

	if (region_count != 0)
	{	
		int* background = (int*)malloc(region_count*sizeof(int));
		memset( background, 0, region_count*sizeof(int));
		imAnalyzeMeasureArea( imRegions, background );

		int min_background = image->width * image->height;
		for (i = 0; i < region_count; i++)
		{
			if ( background[i] < min_background )
				min_background = background[i];
		}
		free(background);
		// smallest white
		features[5] = float(min_background) / (image->width * image->height);
	}

	imImageDestroy( imRegions );
	imImageDestroy( negate );
}


/*

// features count per window - used for memory allocation 
// must be changed if function is modified
#define FEATURES_COUNT 8
#define FEATURE_LINES 7

// 01
// 02
// 10 with biggest black without line removal
// 11 with masked area (as in origninal)

//#define STAFF_HEIGHT 180
//#define STAFF 100

// fonction d'extration de caracteristique par fenetres
void CalcWinFeatures(const imImage* image, float *features, int position_v, int height, int width )
{
	
	if ( width > ( STAFF / 8 ) )
	{
		//wxLogDebug( "Staff width larger than half staff space" );
		width = STAFF / 8;
	}
	
	
	// empiric adjustment		
	height -= width;

	// valeurs par defaut
	int i;
	for ( i = 0; i < FEATURES_COUNT; i++ )
		features[i] = 0.0;

	int half_staff_height = STAFF_HEIGHT / 2;
	int line_step = height / 8;
	int pos[FEATURE_LINES]; // line positions ; 90 au lieu de 89 à cause de margins
	pos[0] = half_staff_height - height / 2 - height / 4 + position_v;
	pos[1] = half_staff_height - height / 2 + position_v;
	pos[2] = half_staff_height - height / 4 + position_v;
	pos[3] = half_staff_height + position_v;	
	pos[4] = half_staff_height + height / 4 + position_v;
	pos[5] = half_staff_height + height / 2 + position_v;
	pos[6] = half_staff_height + height / 2 + height / 4 + position_v;
	//pos[7] = half_staff_height - height + position_v;
	
	//pos[0] = half_staff_height - (6 * line_step) + position_v;
	//pos[1] = half_staff_height - (5 * line_step) + position_v;
	//pos[2] = half_staff_height - (4 * line_step) + position_v;
	//pos[3] = half_staff_height - (3 * line_step) + position_v;
	//pos[4] = half_staff_height - (2 * line_step) + position_v;
	//pos[5] = half_staff_height - (1 * line_step) + position_v;
	//pos[6] = half_staff_height + position_v;	
	//pos[7] = half_staff_height + (1 * line_step) + position_v;
	//pos[8] = half_staff_height + (2 * line_step) + position_v;
	//pos[9] = half_staff_height + (3 * line_step) + position_v;
	//pos[10] = half_staff_height + (4 * line_step) + position_v;
	//pos[11] = half_staff_height + (5 * line_step) + position_v;
	//pos[12] = half_staff_height + (6 * line_step) + position_v;
		
	imbyte *imBuf = (imbyte*)image->data[0];	
		
	int line_top, line_bottom, line_height;
	int j, l, sum;
	imStats istats;
	// completer les lignes
	for (l = 0; l < FEATURE_LINES + 1; l++)
	{
		if ((l < FEATURE_LINES) && ( ( pos[l] < 0 ) || ( pos[l] >= STAFF_HEIGHT ) ) )
			continue;
		line_top = (l == 0) ? 0 : pos[l - 1];
		line_bottom = (l >= FEATURE_LINES) ? image->height - 1 : pos[l];
		line_height = line_bottom - line_top;
		sum = 0;
		for ( j = line_top * image->width; j < line_bottom * image->width; j++)
			sum += imBuf[j];
		
		//wxLogMessage("%d, %d", sum, line_height * image->width);
		features[l] = float(sum) / (line_height * image->width);
		//wxLogMessage( "%d %d", line_bottom, line_top );
		//imImage *imRegion = imImageCreate(image->width, line_bottom - line_top, IM_BINARY, IM_BYTE);
		//imProcessCrop( image, imRegion, line_top, 0 );
		//imCalcImageStatistics( imRegion, &istats );
		//features[l] = 1 - istats.mean;
		//imImageDestroy( imRegion );
	}
}


/*

// features count per window - used for memory allocation 
// must be changed if function is modified
#define FEATURES_COUNT 5

// 01
// 02
// 10 with biggest black without line removal
// 11 with masked area (as in origninal)

//#define STAFF_HEIGHT 180
//#define STAFF 100

// fonction d'extration de caracteristique par fenetres
void CalcWinFeatures(const imImage* image, float *features, int position_v, int height, int width )
{
	
	if ( width > ( STAFF / 8 ) )
	{
		//wxLogDebug( "Staff width larger than half staff space" );
		width = STAFF / 8;
	}
	
	
	// empiric adjustment		
	height -= width;

	// valeurs par defaut
	int i;
	for ( i = 0; i < FEATURES_COUNT; i++ )
		features[i] = 0.0;
	features[0] = 1.0; // euler
	features[1] = 0.0; // area
	features[2] = 0.5; // x centroid
	features[3] = 0.0; // bigest black
	features[4] = 1.0; // smallest white

	int half_staff_height = STAFF_HEIGHT / 2;
	int pos[8]; // line positions ; 90 au lieu de 89 à cause de margins
	pos[0] = half_staff_height + height / 2 + height / 4 + position_v;
	pos[1] = half_staff_height + height / 2 + position_v;
	pos[2] = half_staff_height + height / 4 + position_v;
	pos[3] = half_staff_height + position_v;
	pos[4] = half_staff_height - height / 4 + position_v;
	pos[5] = half_staff_height - height / 2 + position_v;
	pos[6] = half_staff_height - height / 2 - height / 4 + position_v;
	pos[7] = half_staff_height - height + position_v;
	
	imbyte *imBuf = (imbyte*)image->data[0];
	
	int line_offset;
	int l;
	// completer les lignes
	for (l = 0; l < 8; l++)
	{
		if ( ( pos[l] < 0 ) || ( pos[l] >= STAFF_HEIGHT ) )
			continue;
		line_offset = pos[l];
		memset( imBuf + line_offset * image->width,
			1, image->width );
	}
	
	//if ( imdebug )
	//	imProcessInsert( imdebug, image, imdebug, stepdebug, 0 );

	//global
    imImage *imRegions = imImageCreate(image->width, image->height, IM_GRAY, IM_USHORT);
    int region_count = imAnalyzeFindRegions ( image, imRegions, 8, 1 );

	if (region_count == 0)
	{	
		imImageDestroy( imRegions );
		return;
	}

	// euler (+-)
	features[0] = 1.0 / (region_count + 1); // global

	// centroid
	float *cx = (float*)malloc(region_count*sizeof(float));
	memset( cx, 0, region_count*sizeof(float));
	float *cy = (float*)malloc(region_count*sizeof(float));
	memset( cy, 0, region_count*sizeof(float));
	imAnalyzeMeasureCentroid( imRegions, NULL, region_count, cx, cy );

	// remove staff lines from area
	imushort *regionsBuf = (imushort*)imRegions->data[0];
	
	if ( (width % 2) == 0 )
		width++;
	int m;

	for (l = 0; l < 8; l++)
	{
		for (m = -(width / 2); m <= (width / 2); m++)
		{
			line_offset = pos[l] + m;
			if ( ( line_offset < 0 ) || ( line_offset >= STAFF_HEIGHT ) )
				continue;
			memset( regionsBuf + line_offset * image->width,
				0, sizeof( imushort ) * image->width );
		}
	}	
	
	// area (without lines)
	int* area = (int*)malloc(region_count*sizeof(int));
	memset( area, 0, region_count*sizeof(int));
	imAnalyzeMeasureArea( imRegions, area );
	
	int tot_area = 0;
	int max_area = 0;
	float max_black_cy = 0.5;
	for (i = 0; i < region_count; i++)
	{
		tot_area += area[i];
		if ( area[i] > max_area )
		{
			max_area = area[i];
			max_black_cy = cy[i];
		}
	}
	features[1] = (float)tot_area /(image->width * image->height);
	// plus grand noir
	features[3] = float(max_area) /(image->width * image->height); // 01

	// centroid
	float sum_cx = 0, sum_cy = 0;
	for (i = 0; i < region_count; i++)
	{
		sum_cx += cx[i] * area[i];
		sum_cy += cy[i] * area[i];
	}
	if (tot_area)
	{
		features[2] = ((sum_cx / tot_area) + 1) / image->width;
	}
	free(cy);
	free(cx);
	free(area);
	
	// get biggest black
    region_count = imAnalyzeFindRegions ( image, imRegions, 8, 1 );


	// get smallest white
	imImage *negate = imImageClone( image );
	imProcessNegative( image, negate );
    region_count = imAnalyzeFindRegions ( negate, imRegions, 8, 1 );

	if (region_count != 0)
	{	
		int* background = (int*)malloc(region_count*sizeof(int));
		memset( background, 0, region_count*sizeof(int));
		imAnalyzeMeasureArea( imRegions, background );

		int min_background = image->width * image->height;
		for (i = 0; i < region_count; i++)
		{
			if ( background[i] < min_background )
				min_background = background[i];
		}
		free(background);
		// smallest white
		features[4] = float(min_background) / (image->width * image->height);
	}

	imImageDestroy( imRegions );
	imImageDestroy( negate );
}


*/

//----------------------------------------------------------------------------
// ImStaffSegment
//----------------------------------------------------------------------------

ImStaffSegment::ImStaffSegment(  ) :
    ImOperator( )
{
    m_x1 = 0;
    m_x2 = 100;
}

ImStaffSegment::~ImStaffSegment()
{
    m_x1 = 0;
}


bool ImStaffSegment::Load( TiXmlNode *node )
{
    TiXmlElement *current = node->ToElement();
    if ( !current )
        return false;

    if ( current->Attribute("x1"))
        m_x1 = atoi(current->Attribute("x1"));
    if ( current->Attribute("x2"))
        m_x2 = atoi(current->Attribute("x2"));


    if ( current->Attribute("positions"))
	{
		wxArrayInt saved;
		wxStringTokenizer tkz( current->Attribute("positions"), wxT(";"));
		while ( tkz.HasMoreTokens() )
			saved.Add( atoi( tkz.GetNextToken().c_str() ) );
		SetValues( &saved, VALUES_POSITIONS );
	}
	
    if ( current->Attribute("line_p"))
	{
		wxArrayInt saved;
		wxStringTokenizer tkz( current->Attribute("line_p"), wxT(";"));
		while ( tkz.HasMoreTokens() )
			saved.Add( atoi( tkz.GetNextToken().c_str() ) );
		SetValues( &saved, VALUES_LINE_P );
	}
	
    if ( current->Attribute("line_m"))
	{
		wxArrayInt saved;
		wxStringTokenizer tkz( current->Attribute("line_m"), wxT(";"));
		while ( tkz.HasMoreTokens() )
			saved.Add( atoi( tkz.GetNextToken().c_str() ) );
		SetValues( &saved, VALUES_LINE_M );
	}
	

    // positions
	/*   
	TiXmlElement *elem = NULL;
	for( node = current->FirstChild( "position" ); node; node = node->NextSibling( "position" ) )
    {
        elem = node->ToElement();
        if (!elem) return false;
		if ( elem->Attribute("position"))
			m_positions.Add( atoi(elem->Attribute("position")) );
    }*/

    return true;
}

bool ImStaffSegment::Save( TiXmlNode *node )
{
    TiXmlElement *current = node->ToElement();
    if ( !current )
        return false;

    wxString tmp;

    tmp = wxString::Format("%d", m_x1 );
    current->SetAttribute( "x1", tmp.c_str() );

    tmp = wxString::Format("%d", m_x2 );
    current->SetAttribute( "x2", tmp.c_str() );

	wxArrayInt positions_tosave = this->GetValuesToSave( VALUES_POSITIONS );
	wxArrayInt line_p_tosave = this->GetValuesToSave( VALUES_LINE_P );
	wxArrayInt line_m_tosave = this->GetValuesToSave( VALUES_LINE_M );

	if ( (int)positions_tosave.GetCount() > 0 )
	{
		tmp = "";
		for( int i = 0; i < (int)positions_tosave.GetCount() ; i++)
	        tmp << positions_tosave[i] << ";";
		current->SetAttribute( "positions", tmp.c_str() );
    }
	
	if ( (int)line_p_tosave.GetCount() > 0 )
	{
		tmp = "";
		for( int i = 0; i < (int)line_p_tosave.GetCount() ; i++)
	        tmp << line_p_tosave[i] << ";";
		current->SetAttribute( "line_p", tmp.c_str() );
    }
	
	if ( (int)line_m_tosave.GetCount() > 0 )
	{
		tmp = "";
		for( int i = 0; i < (int)line_m_tosave.GetCount() ; i++)
	        tmp << line_m_tosave[i] << ";";
		current->SetAttribute( "line_m", tmp.c_str() );
    }
	
    return true;
}

void ImStaffSegment::SetValues( wxArrayInt *saved_values, int type )
{
	wxArrayInt *values = NULL;
	if ( type == VALUES_POSITIONS )
		values = &m_positions;
	else if ( type == VALUES_LINE_P )
		values = &m_line_p;
	else if ( type == VALUES_LINE_M )
		values = &m_line_m;
	else
		return;

	int val_index = 0;
	int val_index_max = (int) saved_values->GetCount() - 1;

	int w = m_x2 - m_x1;
	values->Clear();

	if ( w < 1 )
		return;

	values->Alloc( w );
	for ( int i = 0; i < w ; i++ )
	{
		val_index = min( (i - POSITION_STEP ) / (POSITION_STEP), val_index_max );
		val_index = max( 0, val_index );
		values->Add( (*saved_values)[val_index] );
	}
}

wxArrayInt ImStaffSegment::GetValuesToSave( int type )
{
	wxArrayInt tosave;
	
	wxArrayInt *values = NULL;
	if ( type == VALUES_POSITIONS )
		values = &m_positions;
	else if ( type == VALUES_LINE_P )
		values = &m_line_p;
	else if ( type == VALUES_LINE_M )
		values = &m_line_m;
	else
		return tosave;

	int w = m_x2 - m_x1;
	if ( w < 1 )
		return tosave;
    int x = POSITION_STEP;
    int width = POSITION_WIN;
	if ( w < width ) // force at least one corretation
		width = w;
    int step = POSITION_STEP;

    while (1)
    {
        if ( x + width - POSITION_STEP > w )
            break;
		
		if ( x > (int)values->GetCount() - 1 )
			break;

		tosave.Add( (*values)[x] );
        x += step;
    }
	return tosave;
}


bool ImStaffSegment::GetImageFromPage( _imImage **image, _imImage *page, int y )
{
	wxASSERT_MSG( !(*image), "Image pointer must be NULL");

    *image = imImageCreate( m_x2 - m_x1, STAFF_HEIGHT, page->color_space, page->data_type);   
	if ( !*image )
		return this->Terminate( ERR_MEMORY );
        
    imProcessCrop( page, *image, m_x1, y);

	return true;
}

bool ImStaffSegment::AnalyzeSegment()
{
    wxASSERT_MSG( m_opImMap, wxT("MAP Image cannot be NULL") );
    int i;

    if ( !GetImagePlane( &m_opImMain ) )
        return false;

    // margins
    m_opImTmp1 = imImageCreate( m_opImMain->width + 2, m_opImMain->height + 2, m_opImMain->color_space, m_opImMain->data_type );
    if (!m_opImTmp1)
        return this->Terminate( ERR_MEMORY );
    imProcessAddMargins( m_opImMain, m_opImTmp1, 1, 1);
    SwapImages( &m_opImMain, &m_opImTmp1 );


    // close
    m_opImTmp1 = imImageClone( m_opImMain );
    if (!m_opImTmp1)
        return this->Terminate( ERR_MEMORY );
    imProcessBinMorphClose( m_opImMain, m_opImTmp1, 5, 1);
    SwapImages( &m_opImMain, &m_opImTmp1 );

    
    m_opIm = imImageCreate(m_opImMain->width, m_opImMain->height, IM_GRAY, IM_USHORT);
    int region_count = imAnalyzeFindRegions ( m_opImMain, m_opIm, 8, 1);
    
    int* area = (int*)malloc( region_count * sizeof(int) );
    memset(area, 0, region_count * sizeof(int) );
    float* perim = (float*)malloc( region_count * sizeof(float) );
    memset(perim, 0, region_count * sizeof(float) );

    imAnalyzeMeasureArea( m_opIm, area );
    imAnalyzeMeasurePerimeter( m_opIm, perim );

    float c = 0;
    for (i = 0; i < region_count; i++ )
    {
        c += pow(perim[i],2) / (4 * AX_PI * area[i]) * (area[i] / m_opIm->width);
    }
    //a /= median( area, region_count);
    //p /= medianf( perim, region_count);
    //wxLogMessage("compactness %f", c / region_count  );
    this->m_compactness = c;

    free( area );
    free( perim );

    return this->Terminate( ERR_NONE );
}


bool ImStaffSegment::WriteMFC( wxString filename, int samplesCount, int period, int sampleSize, float *samples )
{
	wxInt32 int32;
	wxInt16 int16;

	if ( sizeof( float ) != 4 )
	{
		wxLogError(_("Invalid size of float. Must be 4") );
		return false;
	}

	//printf(filename.c_str());
	wxFileOutputStream file( filename );
	if ( !file.IsOk() )
		return false;

	int32 = wxINT32_SWAP_ON_LE ( samplesCount );
	file.Write( &int32, 4 );
	int32 = wxINT32_SWAP_ON_LE ( period );
	file.Write( &int32, 4 );
	int16 = wxINT16_SWAP_ON_LE ( sampleSize * sizeof( float ) );
	file.Write( &int16, 2 );
	int16 = wxINT16_SWAP_ON_LE ( 9 );
	file.Write( &int16, 2 );

	int i = 1;
	wxInt32 swap = wxINT32_SWAP_ON_LE ( i );
	if ( i == swap )
		file.Write( samples, samplesCount * sampleSize * sizeof( float ) );
	else
	{
		for ( i = 0; i < samplesCount * sampleSize; i++ )
		{
			memcpy( &swap, samples + i, 4 );
			int32 = wxINT32_SWAP_ON_LE ( swap );
			file.Write( &int32, 4 );
		}
	}

	file.Close();
	return true;

}


// calcule le masque de portee pour le correlation
// height = hauteur de la portee
// mask a une taille de STAFF_HEIGHT
// modifier les valeur dans pos si STAFF_HEIGHT change !!!!
void ImStaffSegment::CalcMask( int height, int mask[] ) 
{
    int line[5] = {1,1,1,1,1};
	int pos[5] = {39, 64, 89, 114, 139}; // centre des lignes avec height = 100 px 
										 // 100 px de hauteur correspond à 104 px avec l'epaisseur des lignes
	if ( height != 104 ) // adapter les position si different de 100
	{
		height -= 4; // supprimer l'epaisseur de ligne;
		pos[0] = pos[2] - height / 2;
		pos[1] = pos[2] - height / 4;
		pos[3] = pos[2] + height / 4;
		pos[4] = pos[2] + height / 2;
	}

    memset(mask, 0, STAFF_HEIGHT * sizeof(int) );

	for (int p = 0; p < 5; p++)
	{
		memcpy(mask + pos[p] - 2, line, 5 * sizeof(int) );
    }
}


// functors for ImStaffSegment

void ImStaffSegment::PrintLabel(const int staff, const int segment, const int y, wxArrayPtrVoid params )
{
    //ImStaffSegment *staff = (ImStaffSegment*)params;
    /*int *val = (int*)params[0];
    void **p = (void**)malloc( 10 * sizeof(void*) );
    Proc( 1, p );
    wxLogMessage("p %d", *(int*)(p[0]) );
    free(p);
    wxLogMessage("count %d - x1 %d, x2 %d, y %d", count, m_x1, m_x2, y );
	*/
}


// if segment == -1, the staff segments have been merged
void ImStaffSegment::SaveImage(const int staff, const int segment, const int y, wxArrayPtrVoid params )
{
    // param 0: image de la page
	// params 1: nom de base (ajouter par exemple .x.tif pour sauver l'image)

    imImage *page = (imImage*)params[0];    
	if ( !GetImageFromPage( &m_opIm, page, y ) )
		return;

	wxString filename = *(wxString*)params[1];
	filename << "_" << staff << "." << segment <<".tif";

    if ( !Write( filename, &m_opIm ) )
        return;

    this->Terminate( ERR_NONE );
}


// if segment == -1, the staff segments have been merged
void ImStaffSegment::CalcStaffHeight(const int staff, const int segment, const int y, wxArrayPtrVoid params )
{
    // param 0: image de la page
	// param 1: pointer to int[] = height ( output )
	// param 2: current segment index int height array

    imImage *page = (imImage*)params[0];    
	if ( !GetImageFromPage( &m_opIm, page, y ) )
		return;

	int *height = (int*)params[1];
	int *index = (int*)params[2];

    int x = 0;
    int width = POSITION_WIN;
	if ( m_opIm->width < width ) // force at least one corretation
		width = m_opIm->width;
    int step = POSITION_STEP;

    m_opImTmp1 = imImageCreate( width, STAFF_HEIGHT, m_opIm->color_space, m_opIm->data_type);
    if ( !m_opImTmp1 )
    {
        this->Terminate( ERR_MEMORY );
        return;
    }
    
    m_opHist = new int[ m_opImTmp1->height ];

	m_opLines1 = new int[ m_opIm->width];
	int lines = 0;
    //wxLogMessage("Segment %d - %d", staff, segment);
    while (1)
    {
        if ( x + width > m_opIm->width )
            break;

        imProcessCrop( m_opIm, m_opImTmp1, x, 0);

        imAnalyzeProjectionH( m_opImTmp1, m_opHist );

		// pic de l'histogramme :bottom - top = epaisseur de portee;
		int i, top = 0;
		for (i = 0; i < STAFF_HEIGHT / 2; i++ )
			if ( m_opHist[i] > width / 2 )
				break;
			else
				top = i;

		int bottom = STAFF_HEIGHT;
		for (i = STAFF_HEIGHT -1; i > STAFF_HEIGHT / 2; i-- )
			if ( m_opHist[i] > width / 2 )
				break;
			else
				bottom = i;

		m_opLines1[lines] = bottom - top;
		lines++;

        x += step;
    }

	height += (*index);
	*height = median( m_opLines1, lines );
	(*index) += 1;
	//if ( height < 100 || height > 120)
	//	wxLogMessage("--- staff %d : height %d - mean max %d", staff, height, median( m_opLines2, lines ) );

    delete[] m_opLines1;
    m_opLines1 = NULL;

    delete[] m_opHist;
    m_opHist = NULL;

    this->Terminate( ERR_NONE );
}

// if segment == -1, the staff segments have been merged
void ImStaffSegment::CalcCorrelation(const int staff, const int segment, const int y, wxArrayPtrVoid params )
{
    // params 0: image de la page
	// params 1: nom de base (ajouter par exemple .x.tif pour sauver l'image)
	// params 2: hauteur de la portee

    imImage *page = (imImage*)params[0];    
	if ( !GetImageFromPage( &m_opIm, page, y ) )
		return;

    int x = 0;
    int width = POSITION_WIN;
	if ( m_opIm->width < width ) // force at least one corretation
		width = m_opIm->width;
    int step = POSITION_STEP;
    int dec_y, max;

    m_opImTmp1 = imImageCreate( width, STAFF_HEIGHT, m_opIm->color_space, m_opIm->data_type);
    if ( !m_opImTmp1 )
    {
        this->Terminate( ERR_MEMORY );
        return;
    }
    
    m_opHist = new int[ m_opImTmp1->height ];

    int mask[STAFF_HEIGHT];
	int height = *(int*)params[2];
    CalcMask( height, mask );
	
	int peak_val, median_val;

	wxArrayInt positions_tosave; // positions à conserver dans le fichier xml - pas tous les px mais 1 par POSITION_STEP
	wxArrayInt line_p_tosave; // line width (peak in run lengths)
	wxArrayInt line_m_tosave; // line width (median in run lengths)
    //wxLogMessage("Segment %d - %d", staff, segment);
    while (1)
    {
        if ( x + width > m_opIm->width )
            break;
        imProcessCrop( m_opIm, m_opImTmp1, x, 0);
        imAnalyzeProjectionH( m_opImTmp1, m_opHist );
        corr(m_opHist, mask, STAFF_HEIGHT, CORRELATION_HEIGHT, &dec_y, &max );
		//wxLogMessage("dec y = %d - max %d", dec_y , max);
		positions_tosave.Add( dec_y );
		//
		imAnalyzeRuns( m_opImTmp1, &peak_val, &median_val, 1 );
		//wxLogMessage("peak_val = %d, median_val = %d", peak_val , median_val );
		line_p_tosave.Add( peak_val );
		line_m_tosave.Add( median_val );		
        //
        x += step;
    }

    delete[] m_opHist;
    m_opHist = NULL;

	SetValues( &positions_tosave, VALUES_POSITIONS ); // conversion positions par POSITION_STEP -> position pour chaque px
	SetValues( &line_p_tosave, VALUES_LINE_P ); // idem
	SetValues( &line_m_tosave, VALUES_LINE_M ); // idem

	//// ***************provisoire - couvrir les lignes pour visualiser le resultat
    // lines
	/*wxString filename = *(wxString*)params[1];
	filename << "_" << staff << "." << segment <<".tif";

	ImageDestroy( &m_opImTmp1 );
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
    {
        this->Terminate( ERR_MEMORY );
        return;
    }
	imImageClear( m_opImTmp1 );

	imbyte *bufIm = (imbyte*)m_opImTmp1->data[0];

	int offset;
	int pos[5] = {35, 61, 87, 113, 139};
    for (int i = 0; i < m_opImTmp1->width; i++)
    {
		int dec = m_positions[i] *m_opImTmp1->width + i;
		for (int m = 0; m < STAFF_HEIGHT; m++)
		{
			if ( mask[m] == 1 )
			{
				offset = dec + m * m_opImTmp1->width;
				bufIm[ offset ] = 1;
			}
		}
    }

    //if ( !Write( filename, &m_opImTmp1 ) )
    //    return;
    if ( !WriteMAP( filename, &m_opIm, &m_opImTmp1 ,6) )
        return;
	*/
	/************/

    this->Terminate( ERR_NONE );
}

// TEST 2 - 3


// if segment == -1, the staff segments have been merged
void ImStaffSegment::CalcFeatures(const int staff, const int segment, const int y, wxArrayPtrVoid params )
{
    // params 0: image de la page
	// params 1: taille de la fenetre
	// params 2: taille de la zone de recouvrement
	// params 3: nom de base du fichier de sortie (ajout de .x.mfc)
	// params 4: fichier contenant la liste des fichiers mfc
	// params 5: taille de la portee

	if ( m_progressDlg && m_progressDlg->GetCancel() )
	{
        this->Terminate( ERR_NONE ); // do not print cancel message since it doesn't return a value
		return;
	}

	
	wxString filename = *(wxString*)params[3];
	if ( segment == -1 ) // merged segment : change name to avoid confusion
		filename << "." << staff << ".0." << MFC; 
			// force 0, but rewrite the mfc file( see below ). 
			// Ne marche que dans un sens... une autre solution serait de ne pas mettre le no de segment dans le fichier
			// et de simplement copier le fichier si identique (staves.0.0.mfc devient staves.0.mfc), mais cette solution
			// doit etre synchronizee avec output.mlf (ajouter une option merged pour les nom de fichiers .lab)
	else
		filename << "_" << staff << "." << segment << "." << MFC;
	wxString mfc_name = filename;
	mfc_name.Replace( "\\/", "/" );
	wxFile *file = (wxFile*)params[4];
	file->Write( mfc_name );
	file->Write( "\n" );
	// check if file already exists
	//wxLogMessage("ImStaffSegment::CalcFeatures 01");
	//if ( wxFileExists( filename ) && ( segment != -1 ) )
	//	return;

	//wxLogMessage("Staff segment %d.%d", staff , segment );
    
	imImage *page = (imImage*)params[0];       
	if ( !GetImageFromPage( &m_opIm, page, y ) )
		return;

    int x = 0;
    int width = *(int*)params[1];
	if ( m_opIm->width < width ) // force at least one correlation
		width = m_opIm->width;
    int step = width - *(int*)params[2];

    m_opImTmp1 = imImageCreate( width, STAFF_HEIGHT, m_opIm->color_space, m_opIm->data_type);
    if ( !m_opImTmp1 || !width )
    {
        this->Terminate( ERR_MEMORY );
        return;
    }
	
	// debug
	//imdebug = imImageClone( m_opIm );
	//stepdebug = 0;

	int size = FEATURES_COUNT * ( m_opIm->width / step );
	float *values = (float*)malloc( size * sizeof(float) );
	memset(values, 0, size * sizeof(float) );
	//wxLogMessage("step %d", step );
	int samples = 0;
    
	int height = *(int*)params[5];

    while (1)
    {
        if ( x + width > m_opIm->width )
            break;
			
        imProcessCrop( m_opIm, m_opImTmp1, x, 0);
		CalcWinFeatures( m_opImTmp1, values + ( samples * FEATURES_COUNT ), m_positions[ x ], height, m_line_p[x] ); // 01 et al.
		// CalcWinFeatures( m_opImTmp1, values + ( samples * FEATURES_COUNT ), m_positions[ x ], height, m_line_m[x] ); // 02
		samples++;

        x += step;
		
		//debug
		stepdebug = x;
    }
	//wxLogDebug("width %d",m_opIm->width );
	//wxLogDebug("pos_index %d",pos_index  );
	//wxLogDebug("samples %d",samples  );
	//wxLogDebug("size %d",size /  FEATURES_COUNT);
	
	// debug
	//wxString namedebug = filename + ".tif";
	//int error;
	//imFile *ifile = imFileNew( namedebug.c_str(), "TIFF", &error);
	//imFileSaveImage( ifile, imdebug );
	//imFileClose(ifile);
	//imImageDestroy( imdebug );

	//wxString filename = *(wxString*)params[3];
	//filename << "." << staff << "." << segment << "." << MFC;
	this->WriteMFC( filename, samples, step*100, FEATURES_COUNT, values );

	free( values );
    this->Terminate( ERR_NONE );
}

// WDR: handler implementations for ImStaffSegment
