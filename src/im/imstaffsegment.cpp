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

	
	// 11
	/*
	// marge verticale vers le bas contient aussi l'espace pour la ligne ( height / 4 ) et le decalage maximal (CORRELATION_HEIGHT)
 	// à optimiser, calcul pas necessaire à chaque passage ??? height est identique sur la portee ?? sur la page ??
	imImage *imMargins = imImageCreate( image->width, image->height + CORRELATION_HEIGHT + height / 4, 
        image->color_space, image->data_type );
    if ( !imMargins )
        return;
	imImageClear( imMargins );
	// debug1
	//imImage *writeMask = imImageCreate( image->width, image->height + CORRELATION_HEIGHT + height / 4, 
    //    IM_GRAY, image->data_type );
	//imbyte *writeMaskBuf = (imbyte*)writeMask->data[0];

    imProcessAddMargins( image, imMargins, 0, 0 );

    imRegions = imImageCreate(imMargins->width, imMargins->height, IM_GRAY, IM_USHORT);
    region_count = imAnalyzeFindRegions ( imMargins, imRegions, 8, 1 );

	// image de sous-fenetres
    imImage *imLine = imImageCreate( imMargins->width, height / 4 + 2, 
        imMargins->color_space, imMargins->data_type );
    if ( !imLine )
	{	
		imImageDestroy( imMargins );
		imImageDestroy( imRegions );
		return;
	}

	int *mask = (int*)malloc( imLine->height * sizeof( int ));
	//mfc8
	int m_size = 4;
	//mfc9
	//int m_size = 3;
	//mfc10
	//int m_size = 2;
	for (int y = 0; y < imLine->height; y++)
	{
		if ( y < imLine->height / 2)
			mask[y] = y / m_size + 1;
		else
			mask[y] = mask[imLine->height - 1 - y];
	}
	if ( imLine->height % 2 )
		mask[imLine->height / 2] = mask[imLine->height / 2 - 1];

	int margins_offset;
	int rows;
	imbyte *marginsBuf = (imbyte*)imMargins->data[0];
	imbyte *lineBuf = (imbyte*)imLine->data[0];
	// every line
	int zoom_height = 0;
	int sum = 0;
	for (int l = 0; l < 8; l++)
	{
		imImageClear( imLine );
		margins_offset = pos[l];
		line_offset = 0;
		rows = height / 4 + 2;
		if ( pos[l] < 0 ) // depassement de l'image
		{
			margins_offset = 0;
			line_offset -= pos[l];
			rows += pos[l]; // reduire le nombre de lignes à copier
			if ( rows < 0 ) // impossible si < 0;
				continue;
		}
		memcpy( lineBuf + line_offset * imMargins->width ,
			marginsBuf + margins_offset * imMargins->width,
			rows * imMargins->width );

		//int sum = 0;
		for (int y = 0; y < imLine->height; y++)
		{
			zoom_height += mask[y];
	 		for (int x = 0; x < imLine->width; x++)
			{
				int offset = y * imLine->width + x;
				sum += lineBuf[ offset ] * mask[y];

				// debug1
				//int val = lineBuf[ offset ];
				//lineBuf[ offset ] = val * mask[y];
			}
		}

		// debug1
		//memcpy( writeMaskBuf + margins_offset * imMargins->width, 
		//	lineBuf + line_offset * imMargins->width,
		//	rows * imMargins->width );

		// mfc4
		//features[l+6] = float(sum) / float(tot_area);
		// mfc5
		//features[l+6] = float(sum) / float(image->width * image->height);

		// **********
		//wxString filename = "D:/line";
		//filename << "." << l <<".tif";
		//int error;
	    //imFile *ifile = imFileNew( filename.c_str(), "TIFF", &error);
		//if (error == IM_ERR_NONE)
		//{
		//	imFileSetInfo( ifile, "RLE" ); // LZW Unisys par defaut
		//	error = imFileSaveImage( ifile, imLine );
		//	imFileClose(ifile);
		//}
		// ***********

	    //imImage *imLineRegions = imImageCreate(imLine->width, imLine->height, IM_GRAY, IM_USHORT);
		//region_count = imAnalyzeFindRegions ( imLine, imLineRegions, 8, 1 );

		// area
		//int* area = (int*)malloc(region_count*sizeof(int));
		//memset( area, 0, region_count*sizeof(int));
		//imAnalyzeMeasureArea( imLineRegions, area );
		//int sum = 0;
		//for (i = 0; i < region_count; i++)
		//	sum += area[i];
		//features[2*l+2] = (float)sum /(imLine->width * imLine->height);

		//imImageDestroy( imLineMargins );
		//imImageDestroy( imLineRegions );
		//free( area );


	}
	features[1] = (float)sum /(image->width * zoom_height); // 11

	// debug1
	//wxString writeMSTr;
	//writeMSTr = wxString::Format("%s%05d%s", "D:/out/test" , idxx , ".tif");
	//idxx++;
	//int error;
	//imFile *ifile = imFileNew( writeMSTr.c_str(), "TIFF", &error);
	//error = imFileSaveImage( ifile, writeMask );
	//imFileClose(ifile);
	//imImageDestroy( writeMask );
	
	free(mask);
	imImageDestroy( imMargins );
	imImageDestroy( imLine );
	imImageDestroy( imRegions );
	*/
}



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
    wxASSERT_MSG( m_imMap, wxT("MAP Image cannot be NULL") );
    int i;

    if ( !GetImagePlane( &m_imMain ) )
        return false;

    // margins
    m_imTmp = imImageCreate( m_imMain->width + 2, m_imMain->height + 2, m_imMain->color_space, m_imMain->data_type );
    if (!m_imTmp)
        return this->Terminate( ERR_MEMORY );
    imProcessAddMargins( m_imMain, m_imTmp, 1, 1);
    SwapImages( &m_imMain, &m_imTmp );


    // close
    m_imTmp = imImageClone( m_imMain );
    if (!m_imTmp)
        return this->Terminate( ERR_MEMORY );
    imProcessBinMorphClose( m_imMain, m_imTmp, 5, 1);
    SwapImages( &m_imMain, &m_imTmp );

    
    m_im = imImageCreate(m_imMain->width, m_imMain->height, IM_GRAY, IM_USHORT);
    int region_count = imAnalyzeFindRegions ( m_imMain, m_im, 8, 1);
    
    int* area = (int*)malloc( region_count * sizeof(int) );
    memset(area, 0, region_count * sizeof(int) );
    float* perim = (float*)malloc( region_count * sizeof(float) );
    memset(perim, 0, region_count * sizeof(float) );

    imAnalyzeMeasureArea( m_im, area );
    imAnalyzeMeasurePerimeter( m_im, perim );

    float c = 0;
    for (i = 0; i < region_count; i++ )
    {
        c += pow(perim[i],2) / (4 * AX_PI * area[i]) * (area[i] / m_im->width);
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
	if ( !GetImageFromPage( &m_im, page, y ) )
		return;

	wxString filename = *(wxString*)params[1];
	filename << "_" << staff << "." << segment <<".tif";

    if ( !Write( filename, &m_im ) )
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
	if ( !GetImageFromPage( &m_im, page, y ) )
		return;

	int *height = (int*)params[1];
	int *index = (int*)params[2];

    int x = 0;
    int width = POSITION_WIN;
	if ( m_im->width < width ) // force at least one corretation
		width = m_im->width;
    int step = POSITION_STEP;

    m_imTmp = imImageCreate( width, STAFF_HEIGHT, m_im->color_space, m_im->data_type);
    if ( !m_imTmp )
    {
        this->Terminate( ERR_MEMORY );
        return;
    }
    
    m_hist = new int[ m_imTmp->height ];

	m_lines1 = new int[ m_im->width];
	int lines = 0;
    //wxLogMessage("Segment %d - %d", staff, segment);
    while (1)
    {
        if ( x + width > m_im->width )
            break;

        imProcessCrop( m_im, m_imTmp, x, 0);

        imAnalyzeProjectionH( m_imTmp, m_hist );

		// pic de l'histogramme :bottom - top = epaisseur de portee;
		int i, top = 0;
		for (i = 0; i < STAFF_HEIGHT / 2; i++ )
			if ( m_hist[i] > width / 2 )
				break;
			else
				top = i;

		int bottom = STAFF_HEIGHT;
		for (i = STAFF_HEIGHT -1; i > STAFF_HEIGHT / 2; i-- )
			if ( m_hist[i] > width / 2 )
				break;
			else
				bottom = i;

		m_lines1[lines] = bottom - top;
		lines++;

        x += step;
    }

	height += (*index);
	*height = median( m_lines1, lines );
	(*index) += 1;
	//if ( height < 100 || height > 120)
	//	wxLogMessage("--- staff %d : height %d - mean max %d", staff, height, median( m_lines2, lines ) );

    delete[] m_lines1;
    m_lines1 = NULL;

    delete[] m_hist;
    m_hist = NULL;

    this->Terminate( ERR_NONE );
}

// if segment == -1, the staff segments have been merged
void ImStaffSegment::CalcCorrelation(const int staff, const int segment, const int y, wxArrayPtrVoid params )
{
    // params 0: image de la page
	// params 1: nom de base (ajouter par exemple .x.tif pour sauver l'image)
	// params 2: hauteur de la portee

    imImage *page = (imImage*)params[0];    
	if ( !GetImageFromPage( &m_im, page, y ) )
		return;

    int x = 0;
    int width = POSITION_WIN;
	if ( m_im->width < width ) // force at least one corretation
		width = m_im->width;
    int step = POSITION_STEP;
    int dec_y, max;

    m_imTmp = imImageCreate( width, STAFF_HEIGHT, m_im->color_space, m_im->data_type);
    if ( !m_imTmp )
    {
        this->Terminate( ERR_MEMORY );
        return;
    }
    
    m_hist = new int[ m_imTmp->height ];

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
        if ( x + width > m_im->width )
            break;
        imProcessCrop( m_im, m_imTmp, x, 0);
        imAnalyzeProjectionH( m_imTmp, m_hist );
        corr(m_hist, mask, STAFF_HEIGHT, CORRELATION_HEIGHT, &dec_y, &max );
		//wxLogMessage("dec y = %d - max %d", dec_y , max);
		positions_tosave.Add( dec_y );
		//
		imAnalyzeRuns( m_imTmp, &peak_val, &median_val, 1 );
		//wxLogMessage("peak_val = %d, median_val = %d", peak_val , median_val );
		line_p_tosave.Add( peak_val );
		line_m_tosave.Add( median_val );		
        //
        x += step;
    }

    delete[] m_hist;
    m_hist = NULL;

	SetValues( &positions_tosave, VALUES_POSITIONS ); // conversion positions par POSITION_STEP -> position pour chaque px
	SetValues( &line_p_tosave, VALUES_LINE_P ); // idem
	SetValues( &line_m_tosave, VALUES_LINE_M ); // idem

	//// ***************provisoire - couvrir les lignes pour visualiser le resultat
    // lines
	/*wxString filename = *(wxString*)params[1];
	filename << "_" << staff << "." << segment <<".tif";

	ImageDestroy( &m_imTmp );
    m_imTmp = imImageClone( m_im );
    if ( !m_imTmp )
    {
        this->Terminate( ERR_MEMORY );
        return;
    }
	imImageClear( m_imTmp );

	imbyte *bufIm = (imbyte*)m_imTmp->data[0];

	int offset;
	int pos[5] = {35, 61, 87, 113, 139};
    for (int i = 0; i < m_imTmp->width; i++)
    {
		int dec = m_positions[i] *m_imTmp->width + i;
		for (int m = 0; m < STAFF_HEIGHT; m++)
		{
			if ( mask[m] == 1 )
			{
				offset = dec + m * m_imTmp->width;
				bufIm[ offset ] = 1;
			}
		}
    }

    //if ( !Write( filename, &m_imTmp ) )
    //    return;
    if ( !WriteMAP( filename, &m_im, &m_imTmp ,6) )
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
		filename << "." << staff << ".0.mfc"; 
			// force 0, but rewrite the mfc file( see below ). 
			// Ne marche que dans un sens... une autre solution serait de ne pas mettre le no de segment dans le fichier
			// et de simplement copier le fichier si identique (staves.0.0.mfc devient staves.0.mfc), mais cette solution
			// doit etre synchronizee avec output.mlf (ajouter une option merged pour les nom de fichiers .lab)
	else
		filename << "_" << staff << "." << segment <<".mfc";
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
	if ( !GetImageFromPage( &m_im, page, y ) )
		return;

    int x = 0;
    int width = *(int*)params[1];
	if ( m_im->width < width ) // force at least one correlation
		width = m_im->width;
    int step = width - *(int*)params[2];

    m_imTmp = imImageCreate( width, STAFF_HEIGHT, m_im->color_space, m_im->data_type);
    if ( !m_imTmp || !width )
    {
        this->Terminate( ERR_MEMORY );
        return;
    }
	
	// debug
	//imdebug = imImageClone( m_im );
	//stepdebug = 0;

	int size = FEATURES_COUNT * ( m_im->width / step );
	float *values = (float*)malloc( size * sizeof(float) );
	memset(values, 0, size * sizeof(float) );
	//wxLogMessage("step %d", step );
	int samples = 0;
    
	int height = *(int*)params[5];

    while (1)
    {
        if ( x + width > m_im->width )
            break;
			
        imProcessCrop( m_im, m_imTmp, x, 0);
		CalcWinFeatures( m_imTmp, values + ( samples * FEATURES_COUNT ), m_positions[ x ], height, m_line_p[x] ); // 01 et al.
		// CalcWinFeatures( m_imTmp, values + ( samples * FEATURES_COUNT ), m_positions[ x ], height, m_line_m[x] ); // 02
		samples++;

        x += step;
		
		//debug
		stepdebug = x;
    }
	//wxLogDebug("width %d",m_im->width );
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
	//filename << "." << staff << "." << segment <<".mfc";
	this->WriteMFC( filename, samples, step*100, FEATURES_COUNT, values );

	free( values );
    this->Terminate( ERR_NONE );
}

// WDR: handler implementations for ImStaffSegment
