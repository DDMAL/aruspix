/////////////////////////////////////////////////////////////////////////////
// Name:        imstaff.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/file.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"

#include "imstaff.h"
#include "imstaffsegment.h"

#include "math.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfStaffSegments );


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


//imImage* imdebug = NULL;
//int stepdebug = 0;

// features count per window - used for memory allocation 
// must be changed if function is modified
#define FEATURES_COUNT 7
#define LYRIC_FEATURES_COUNT 16

// Feature extration fonction on one window
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
	imAnalyzeMeasureArea( imRegions, area, 1 );
	
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
		imAnalyzeMeasureArea( imRegions, forground , 1);

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
		imAnalyzeMeasureArea( imRegions, background, 1 );

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
Breaks the image into a 4x4 grid and calculates the forground pixel density in each cell.
	image: a slice of a cropped lyric image
	pixel_density: a 16 dimensional vector 
 */
bool CalcLyricWinFeatures(const imImage* image, float *pixel_density )
{
	int height = image->height;
	int width = image->width;
	imbyte *buffer = (imbyte*)image->data[0];
	imbyte *tmp = (imbyte*)malloc( 4 * height * 4 * width );

	int i, j, k, m;

	// Scale original image by a factor of 4 on both axis
	for ( i = 0; i < height; i++ ){
		for ( j = 0; j < width; j++){
			for ( k = 0; k < 4; k++ ){
				for ( m = 0; m < 4; m++ ){
					tmp[ ( 4 * i + k ) * 4 * width + j * 4 + m  ] = buffer[ i * width + j ];
				}
			}
		}
	}
	
	height *= 4;
	width *= 4;
	
	// Pixel count for each of the 16 cells
	int pixel_count[4][4];
	for ( i = 0; i < 4; i++ )
		for ( j = 0; j < 4; j++ )
			pixel_count[i][j] = 0;
	
	int y1 = height / 4;		// y-coordinates that split the image 4 times horizontally
	int y2 = 2 * y1;
	int y3 = 3 * y1;

	int x1 = width / 4;		// x-coordinates that split the image 4 times vertically
	int x2 = 2 * x1;
	int x3 = 3 * x1;
	
	// Count the number of pixels in each cell
	for ( i = 0; i < width; i ++ ){
		for ( j = 0; j < height; j++ ){
			int index = i * width + j;				
			if ( j < y1 && i < x1 ){										// [0,0]
				if ( tmp[index] != 0 ) pixel_count[0][0]++;
			} else if ( ( j >= y1 && j < y2 ) && i < x1 ){					// [1,0]	
				if ( tmp[index] != 0 ) pixel_count[1][0]++;
			} else if ( ( j >= y2 && j < y3 ) && i < x1 ){					// [2,0]
				if ( tmp[index] != 0 ) pixel_count[2][0]++;			
			} else if ( j >= y3 && i < x1 ){								// [3,0]
				if ( tmp[index] != 0 ) pixel_count[3][0]++;
			} else if ( j < y1 && ( i >= x1 && i < x2 ) ){					// [0,1]
				if ( tmp[index] != 0 ) pixel_count[0][1]++;
			} else if ( ( j >= y1 && j < y2 ) && ( i >= x1 && i < x2 ) ){	// [1,1]
				if ( tmp[index] != 0 ) pixel_count[1][1]++;
			} else if ( ( j >= y2 && j < y3 ) && ( i >= x1 && i < x2 ) ){	// [2,1]
				if ( tmp[index] != 0 ) pixel_count[2][1]++;			
			} else if ( j >= y3 && ( i >= x1 && i < x2 ) ){					// [3,1]
				if ( tmp[index] != 0 ) pixel_count[3][1]++;
			} else if ( j < y1 && ( i >= x2 && i < x3 ) ){					// [0,2]
				if ( tmp[index] != 0 ) pixel_count[0][2]++;
			} else if ( ( j >= y1 && j < y2 ) && ( i >= x2 && i < x3 ) ){	// [1,2]
				if ( tmp[index] != 0 ) pixel_count[1][2]++;
			} else if ( ( j >= y2 && j < y3 ) && ( i >= x2 && i < x3 ) ){	// [2,2]
				if ( tmp[index] != 0 ) pixel_count[2][2]++;			
			} else if ( j >= y3 && ( i >= x2 && i < x3 ) ){					// [3,2]
				if ( tmp[index] != 0 ) pixel_count[3][2]++;
			} else if ( j < y1 && i >= x3 ){								// [0,3]
				if ( tmp[index] != 0 ) pixel_count[0][3]++;
			} else if ( ( j >= y1 && j < y2 ) && i >= x3 ){					// [1,3]
				if ( tmp[index] != 0 ) pixel_count[1][3]++;
			} else if ( ( j >= y2 && j < y3 ) && i >= x3 ){					// [2,3]
				if ( tmp[index] != 0 ) pixel_count[2][3]++;			
			} else if ( j >= y3 && i >= x3 ){								// [3,3]
				if ( tmp[index] != 0 ) pixel_count[3][3]++;
			} 
		}
	}
	free( tmp );
	
	float pixel_sum = 0.0;
	for ( i = 0; i < 4; i++ )
		for ( j = 0; j < 4; j++ )
			pixel_sum += pixel_count[i][j];
	
	if ( pixel_sum == 0 ) return false;
	
	for ( i = 0; i < 4; i++ )
		for ( j = 0; j < 4; j++ )
			if ( pixel_sum != 0 )
				pixel_density[ i * 4 + j ] = (float) pixel_count[i][j] / pixel_sum;	
	
	return true;
}

//----------------------------------------------------------------------------
// ImStaff
//----------------------------------------------------------------------------

ImStaff::ImStaff( ) :
    ImOperator( )
{
    m_y = 0;
    m_med = 0;
    m_x1 = 0;
    m_x2 = 0;
	m_lyricCentre = -1;		
	m_lyricBaseline = -1;	
	m_lyricTopline = -1;		
}


ImStaff::~ImStaff()
{
}


bool ImStaff::Load( TiXmlNode *node )
{
    TiXmlElement *current = node->ToElement();
    if ( !current )
        return false;

    if ( current->Attribute("y"))
        m_y = atoi(current->Attribute("y"));
    if ( current->Attribute("x1"))
        m_x1 = atoi(current->Attribute("x1"));
    if ( current->Attribute("x2"))
        m_x2 = atoi(current->Attribute("x2"));
	
	if ( current->Attribute("lyricBaseline"))
        m_lyricBaseline = atoi(current->Attribute("lyricBaseline"));
	if ( current->Attribute("lyricTopline"))
        m_lyricTopline = atoi(current->Attribute("lyricTopline"));
	if ( current->Attribute("lyricCentre"))
        m_lyricCentre = atoi(current->Attribute("lyricCentre"));

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

    return true;
}


bool ImStaff::Save( TiXmlNode *node )
{
    TiXmlElement *current = node->ToElement();
    if ( !current )
        return false;

    wxString tmp;

    tmp = wxString::Format("%d", m_y );
    current->SetAttribute( "y", tmp.c_str() );
    tmp = wxString::Format("%d", m_x1 );
    current->SetAttribute( "x1", tmp.c_str() );
    tmp = wxString::Format("%d", m_x2 );
    current->SetAttribute( "x2", tmp.c_str() );
	
    tmp = wxString::Format("%d", m_lyricBaseline );
    current->SetAttribute( "lyricBaseline", tmp.c_str() );
	tmp = wxString::Format("%d", m_lyricTopline );
    current->SetAttribute( "lyricTopline", tmp.c_str() );
	tmp = wxString::Format("%d", m_lyricCentre );
    current->SetAttribute( "lyricCentre", tmp.c_str() );

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

void ImStaff::SetValues( wxArrayInt *saved_values, int type )
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

wxArrayInt ImStaff::GetValuesToSave( int type )
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


bool ImStaff::GetImageFromPage( _imImage **image, _imImage *page, int y1, int y2 )
{
	wxASSERT_MSG( !(*image), "Image pointer must be NULL");

	if ( y2 == -1) {
		*image = imImageCreate( m_x2 - m_x1, STAFF_HEIGHT, page->color_space, page->data_type); 
		if ( !*image )
			return this->Terminate( ERR_MEMORY );

		imProcessCrop( page, *image, m_x1, y1);	
	} else {
		*image = imImageCreate( m_x2 - m_x1, y1 - y2, page->color_space, page->data_type);   
		if ( !*image )
			return this->Terminate( ERR_MEMORY );
	
		imProcessCrop( page, *image, m_x1, y2);
	}
    return true;
}

void ImStaff::GetMinMax(int *minx, int *maxx)
{
	*minx = m_x1;
	*maxx = m_x2;
}


void ImStaff::GetXandPos( int posx, int *x, int *vpos )
{
	*vpos = 0;
	*x = posx - m_x1;
	if ((posx < m_x2) && (posx >= m_x1 ) && ((posx - m_x1) < (int)m_positions.GetCount()) && ((posx - m_x1) > 0))
		*vpos = -m_positions[ posx - m_x1 ];
}

// To be modified
int ImStaff::CalcIndentation( int leftmargin )
{
    int indent = (m_x1 - leftmargin) / 10;
    if ( indent < 0 )
        indent = 0;
    return indent;	
}

int ImStaff::CalcEcart( int previous )
{
    return m_y - previous;
}
    

bool ImStaff::GetStaffBorders( int threshold_in_percent, bool analyse_segments )
{
    wxASSERT_MSG( m_opImMap, wxT("MAP Image cannot be NULL") );

    //if ( !GetImagePlane( &m_opIm, 0, SS_FACTOR_1 ) )
    //  return false;
    
    int i;
    
    if ( !GetImagePlane( &m_opIm ) )
        return false;

    m_opImTmp1 = imImageCreate( m_opIm->width / SS_FACTOR_1, m_opIm->height / SS_FACTOR_1, m_opIm->color_space, m_opIm->data_type);
    if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
    imProcessResize( m_opIm, m_opImTmp1, 0);
    SwapImages( &m_opIm, &m_opImTmp1 );

    // convovle
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
    imImage* kernel = imImageCreate( 3, 1, IM_GRAY, IM_INT);
    imImageSetAttribute(kernel, "Description", IM_BYTE, -1, (void*)"Erode");
    int* kernel_data = (int*)kernel->data[0];
    for(i = 0; i < kernel->count; i++)
        kernel_data[i] = 0;
    //imProcessBinMorphClose( m_opIm, m_opImTmp1 , 3, 1);
    imProcessBinMorphConvolve( m_opIm, m_opImTmp1, kernel, 0, 1);
    imImageDestroy(kernel);
    SwapImages( &m_opIm, &m_opImTmp1 );

    // roi image : seulement 120 pixels de haut
    m_opImTmp1 = imImageCreate( m_opIm->width, SS_STAFF_ROI_W / SS_FACTOR_1, m_opIm->color_space, m_opIm->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessCrop( m_opIm, m_opImTmp1, 0, ( STAFF_HEIGHT -  SS_STAFF_ROI_W) / ( 2 * SS_FACTOR_1 ) );

    // analyse de la projection verticale
    int f_width, avg;
    m_opHist = new int[ m_opImTmp1->width ];
    imAnalyzeProjectionV( m_opImTmp1, m_opHist );
    f_width = 30 / SS_FACTOR_1; // 30 px
    MedianFilter( m_opHist, m_opImTmp1->width , f_width, &avg );
    m_med = median( m_opHist, m_opImTmp1->width, false );

    // coupure de portee si en dessous de 25 % de la valeur medianne
    int x_staff = 0;
    bool staff = false;
    int staff_threshold = 0;
	if ( (threshold_in_percent != 0) || (threshold_in_percent != 100) )
		staff_threshold = m_med / (100  / threshold_in_percent); // threshold_in_percent		
    this->m_segments.Clear();
    for ( i = 0; i < m_opImTmp1->width; i++ )
    {
        if ( staff && (m_opHist[i] < staff_threshold ))
        {
            ImStaffSegment segment;
            segment.m_x1 = x_staff * SS_FACTOR_1;
            segment.m_x2 = i * SS_FACTOR_1;
            this->m_segments.Add( segment );
            staff = false;
        }
        else if ( !staff && (m_opHist[i] > staff_threshold )) // debut de portee
        {
            staff = true;
            x_staff = i;
        }
    }
    // check if last position not terminated
    if (staff)
    {           
            ImStaffSegment segment;
            segment.m_x1 = x_staff * SS_FACTOR_1;
            segment.m_x2 = i * SS_FACTOR_1;
            this->m_segments.Add( segment );
    }
    delete m_opHist;
    m_opHist = NULL;
    ImageDestroy( &m_opImTmp1 );
	
	if ( !analyse_segments ) // analyse is used to find text in staves
		return this->Terminate( ERR_NONE );	

    int nb_segments = (int)m_segments.GetCount();
    for(i = 0; i < nb_segments; i++)
    {
        m_opImTmp1 = imImageCreate( (m_segments[i].m_x2 - m_segments[i].m_x1) / SS_FACTOR_1,
            m_opIm->height, m_opIm->color_space, m_opIm->data_type );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
        imProcessCrop( m_opIm, m_opImTmp1, m_segments[i].m_x1 / SS_FACTOR_1, 0 );

        /*m_opImTmp1 = imImageCreate( (m_segments[i].m_x2 - m_segments[i].m_x1),
            m_opImMap->height, m_opImMap->color_space, m_opImMap->data_type );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
        imProcessCrop( m_opImMap, m_opImTmp1, m_segments[i].m_x1, 0 );*/

		//m_segments[i].SetProgressDlg( m_progressDlg );
        m_segments[i].SetMapImage( m_opImTmp1 );
        m_segments[i].AnalyzeSegment();
        ImageDestroy( &m_opImTmp1 );
    }

    return this->Terminate( ERR_NONE );
}



bool ImStaff::WriteMFC( wxString filename, int samplesCount, int period, int sampleSize, float *samples )
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
// Crashes with 4 staff lines!
void ImStaff::CalcMask( int height, int numstafflines, int mask[] ) 
{
    int line[5] = {1,1,1,1,1};
	
	int numgaps = numstafflines - 1;
	int distance = (height) / numgaps;
	int spacing = (STAFF_HEIGHT - height) / 2;
	int *pos = new int[numstafflines];
	pos[0] = spacing;
	for (int i = 1; i < numstafflines; i++) {
		pos[i] = pos[i-1] + distance;
	}
	
	/*
	int pos2[5] = {39, 64, 89, 114, 139}; // centre des lignes avec height = 100 px 
										 // 100 px de hauteur correspond à 104 px avec l'epaisseur des lignes
	if ( height != 104 ) // adapter les position si different de 100
	{
		height -= 4; // supprimer l'epaisseur de ligne;
		pos2[0] = pos2[2] - height / 2;
		pos2[1] = pos2[2] - height / 4;
		pos2[3] = pos2[2] + height / 4;
		pos2[4] = pos2[2] + height / 2;
	} */
	//wxLogMessage("Staff height: %d, num stafflines: %d", height, numstafflines);	

    memset(mask, 0, STAFF_HEIGHT * sizeof(int) );

	for (int p = 0; p < 5; p++)
	{
		memcpy(mask + pos[p] - 2, line, 5 * sizeof(int) );
    }
	delete[] pos;
}


// functors for ImStaff

void ImStaff::SaveImage(const int staff, wxArrayPtrVoid params )
{
    // param 0: image de la page
	// params 1: nom de base (ajouter par exemple .x.tif pour sauver l'image)

    imImage *page = (imImage*)params[0];    
	if ( !GetImageFromPage( &m_opIm, page, m_y - ( STAFF_HEIGHT / 2 )  ) )
		return;

	wxString filename = *(wxString*)params[1];
	filename << "_" << staff << ".0.tif"; // .0.tif stays for backward compatibility with previous versions with segments

    if ( !Write( filename, &m_opIm ) )
        return;

    this->Terminate( ERR_NONE );
}


void ImStaff::CalcStaffHeight(const int staff, wxArrayPtrVoid params )
{
    // param 0: image de la page
	// param 1: pointer to int[] = height ( output )

    imImage *page = (imImage*)params[0];    
	if ( !GetImageFromPage( &m_opIm, page, m_y - ( STAFF_HEIGHT / 2 ) ) )
		return;

	int *height = (int*)params[1];

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

	m_opLines1 = new int[ m_opIm->width ];
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
		for (i = STAFF_HEIGHT - 1; i > STAFF_HEIGHT / 2; i-- )
			if ( m_opHist[i] > width / 2 )
				break;
			else
				bottom = i;

		m_opLines1[lines] = bottom - top;
		lines++;

        x += step;
    }

	height += (staff);
	*height = median( m_opLines1, lines );
	//if ( height < 100 || height > 120)
	//	wxLogMessage("--- staff %d : height %d - mean max %d", staff, height, median( m_opLines2, lines ) );

    delete[] m_opLines1;
    m_opLines1 = NULL;

    delete[] m_opHist;
    m_opHist = NULL;

    this->Terminate( ERR_NONE );
}

void ImStaff::CalcCorrelation(const int staff, wxArrayPtrVoid params )
{
    // params 0: image de la page
	// params 1: nom de base (ajouter par exemple .x.tif pour sauver l'image)
	// params 2: hauteur de la portee
	// params 3: number of staff lines

    imImage *page = (imImage*)params[0];    
	if ( !GetImageFromPage( &m_opIm, page, m_y - ( STAFF_HEIGHT / 2 ) ) )
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
	int numstafflines = *(int*)params[3];
    CalcMask( height, numstafflines, mask );
	
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

void ImStaff::CalcFeatures(const int staff, wxArrayPtrVoid params )
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
	filename << "_" << staff << ".0.mfc"; // .0.mfc stays for backward compatibility with previous versions with segments
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
	if ( !GetImageFromPage( &m_opIm, page, m_y - ( STAFF_HEIGHT / 2 ) ) )
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
    }
	this->WriteMFC( filename, samples, step * 100, FEATURES_COUNT, values );

	free( values );
    this->Terminate( ERR_NONE );
}

void ImStaff::CalcLyricFeatures( const int staff, wxArrayPtrVoid params )
{
    // params 0: image de la page
	// params 1: nom de base du fichier de sortie (ajout de .x.mfc)
	// params 2: array holding coordinates for the top of each lyric subimage
	// params 3: array holding coordinates for the bottom of each lyric subimage
	// params 4: array holding overall projection of all the lyric lines
	// params 5: array holding offsets for all the lyric images
	// params 6: window width used for feature extraction
	
	if ( m_progressDlg && m_progressDlg->GetCancel() )
	{
        this->Terminate( ERR_NONE ); // do not print cancel message since it doesn't return a value
		return;
	}
	   
	imImage *page = (imImage*)params[0];
	wxString filename = *(wxString*)params[1];
	int *TopOfLyricLine = (int*)params[2];
	int *BottomOfLyricLine = (int*)params[3];
	double *overallProjection = (double*)params[4];
	int **offsets = (int**)params[5];
	int width = *(int*)params[6];
	
	if ( !GetImageFromPage( &m_opIm, page, TopOfLyricLine[staff], BottomOfLyricLine[staff] ) )
		return;
	
//	m_opImTmp1 = imImageCreate( m_opIm->height, m_opIm->width, m_opIm->color_space, m_opIm->data_type );	
//	CorrectLyricCurvature( m_opIm, m_opImTmp1 );

	FindLyricBaseLine( m_opIm, overallProjection, offsets[staff], width );
	
	// Save lyric images testing purposes
	filename << "_" << staff << "lyric.0.tif"; // .0.tif stays for backward compatibility with previous versions with segments
    if ( !Write( filename, &m_opIm ) )
        return;
	
    this->Terminate( ERR_NONE );
}
 
//  The dest image must have the same dimensions as the src image but with inverted dimension (i.e. a 90 degree rotation of the src)
//  The corrected image is saved in the src
void ImStaff::CorrectLyricCurvature( imImage *src, imImage *dest )
{
	imProcessRotate90( src, dest, true);
	
	imbyte *buffer = (imbyte*)dest->data[0];
	imbyte *tmp = (imbyte*)malloc( dest->height * dest->width * sizeof( imbyte ) );
	memset( tmp, 0, dest->height * dest->width );
	
	for ( int i = 0; i < dest->height; i++ ){
		if ( m_positions[i] < 0 && m_positions[i] < dest->width ){
			memcpy( tmp + ( i * dest->width ) + m_positions[i], buffer + ( i * dest->width ), dest->width - m_positions[i] );
			memcpy( buffer + ( i * dest->width ), tmp + ( i * dest->width ), dest->width );
		} else if ( m_positions[i] > 0 && m_positions[i] < dest->width ){
			memcpy( tmp + ( i * dest->width ), buffer + ( i * dest->width ) + m_positions[i], dest->width - m_positions[i] );
			memcpy( buffer + ( i * dest->width ), tmp + ( i * dest->width ), dest->width );
		}	
	}
	imProcessRotate90( dest, src, false);
	free ( tmp );
}

void ImStaff::FindLyricBaseLine( imImage *src, double *overallProjection, int *offsets, int windowWidth )
{
	int subimageSize = windowWidth;
	int height = src->height;
	int width = src->width;
	imbyte *buffer = (imbyte*)src->data[0];
	int *reference = (int*)malloc( height * sizeof(int) );

	int i, j, k;
	
	// Get vertical reference array by taking the projection of the first non-empty lyric subimage. 
	int temp = 0;
	j = 0;
	while ( ( temp == 0 ) && ( ( j + 1 ) * subimageSize < width ) ){
		for ( i = 0; i < height; i++ ){
			reference[i] = 0;
			for ( k = 0; k < subimageSize; k++ )
				if ( k + ( j * subimageSize ) < width && buffer[ k + ( j * subimageSize ) + ( i * width ) ] != 0 ) 
					reference[i]++;
			temp += reference[i];
		}
		j++;
	}
	
	// array holding the offsets for each lyric line
	double **alignedProjection = (double**)malloc( height * sizeof( double* ) );	
	for ( i = 0; i < height; i++ )
		alignedProjection[i] = (double*)malloc( (int)ceil( width / subimageSize ) * sizeof( double ) );
	
	// Loop through all subimages
	for (  k = 0; k < ceil( width / subimageSize ); k++ ){

		// Do a horizontal projection on the k-th lyric subimage
		double *projection = (double*)malloc( height * sizeof (double) );
		for (  i = 0; i < height; i++ ){
			projection[i] = 0;
			for (  j = 0; j < subimageSize; j++ )
				if ( j + ( k * subimageSize ) < width && buffer[ j + ( k * subimageSize) + ( i * width ) ] != 0 ) 
					projection[i]++; 
		}	
		
		// Add zero padding to the projection
		double *paddedProjection = (double*)malloc( 2 * height * sizeof(double) );
		for (  i = 0; i < 2 * height; i++ ){
			if ( i < height ) paddedProjection[i] = projection[i];
			else paddedProjection[i] = 0;
		} 
		
		// Perform a filter on the paddedprojection i.e filter(reference, 1, paddedProjection)
		// Also find the index of the maximum value in the result:
		int maxIndex = 0;
		double *convolution = (double*)malloc( 2 * height * sizeof(double) );
		for (  i = 0; i < 2 * height; i++ ){
			convolution[i] = 0;
			j = 0;
			while ( i - j >= 0 && j < height ){
				convolution[i] += reference[j] * paddedProjection[i-j];
				j++;
			}
			if ( convolution[maxIndex] < convolution[i] ) maxIndex = i;
		}
		
		int off = height / 2;
		int offset = maxIndex - off;
		if ( maxIndex != 0)
			offsets[k] = offset;

		// Copy and align lyric subimage projections into the aligned projection matrix
		for ( i = 0; i < height; i++ ){
			if ( offset >= off ) {
				int index = i + ( offset - off );
				if ( index < height ) alignedProjection[i][k] = projection[index];
				else alignedProjection[i][k] = 0;
			} else if ( offset < off ){
				int index = i - ( off - offset );
				if ( index >= 0 ) alignedProjection[i][k] = projection[index];
				else alignedProjection[i][k] = 0;				
			}
		}
		free( projection );
		free( paddedProjection );
		free( convolution );
	}
	
	for ( i = 0; i < height; i++ ){
		for ( j = 0; j < ceil ( width / subimageSize ); j++ ){
			overallProjection[i] += alignedProjection[i][j];
		}
	}

	free( reference );	
	for ( i = 0; i < height; i++ ) free( alignedProjection[i] );
	free( alignedProjection );
}

//Crop lyric images using the lyric baseline and topline and then run a recognition function on the lyric image
void ImStaff::ExtractLyricImages( const int staff, wxArrayPtrVoid params )
{
    // params 0: image de la page
	// params 1: recognition window width
	// params 2: overlap
	// params 3: nom de base du fichier de sortie (ajout de .x.mfc)
	// params 4: 2D array holding offsets for all the lyric images
	// params 5: Lyric baseline
	// params 6: Lyric top
	// params 7: array holding coordinates for the top of each lyric subimage
	// params 8: array holding coordinates for the bottom of each lyric subimage
	// params 9: baseline window width
	// params 10: fichier contenant la liste des fichiers mfc

	imImage *page = (imImage*)params[0];
	int width = *(int*)params[1];
	wxString filename = *(wxString*)params[3];
	int **offsets = (int**)params[4];
	int baseline = *(int*)params[5];
	int topline = *(int*)params[6];
	
	int imageHeight = baseline - topline;

	// Load saved lyric image
	//	m_opIm = imFileImageLoad( filename, 0, 0 );
	//	if ( m_opIm )
	//	return;
	
	//Should open the saved file here instead of recropping...
	int *TopOfLyricLine = (int*)params[7];
	int *BottomOfLyricLine = (int*)params[8];
	if ( !GetImageFromPage( &m_opIm, page, TopOfLyricLine[staff], BottomOfLyricLine[staff] ) )
		return;
	
	//This was removed because better results were obtained when cropping the lyrics without the curvature adjustment
	//  m_opImTmp1 = imImageCreate( m_opIm->height, m_opIm->width, m_opIm->color_space, m_opIm->data_type );	
	//  CorrectLyricCurvature( m_opIm, m_opImTmp1 );	
	//  ImageDestroy( &m_opImTmp1 );	
	
	// Crop lyric image using topline and baseline value
	// m_opIm = large image
	// m_opImTmp1 = cropped image
	int avg_offset = 0;
	int count = 0;
	for ( int i = 0; i < ceil( m_opIm->width / *(int*)params[9] ); i++ ){
		if ( offsets[staff][i] > 0 ) {
			avg_offset += offsets[staff][i];
			count++;
		}
	}
	avg_offset /= count;
	int top_offset = topline + avg_offset - ( m_opIm->height / 2 );	
	
	if ( top_offset + imageHeight >= m_opIm-> height )
		m_opImTmp1 = imImageCreate( m_opIm->width, m_opIm->height - top_offset, m_opIm->color_space, m_opIm->data_type );
	else 
		m_opImTmp1 = imImageCreate( m_opIm->width, imageHeight , m_opIm->color_space, m_opIm->data_type );
	imImageClear( m_opImTmp1 );
	imProcessCrop( m_opIm, m_opImTmp1, 0, top_offset );
	
	// Save cropped lyric image 
	wxString image_filename = filename;
	image_filename << "_" << staff << "croppedLyric.0.tif";
	if ( !Write( image_filename, &m_opImTmp1 ) )
        return;
	
	// Do feature recognition on the cropped lyric image by calling CalcLyricWinFeatures on subwindows of the image
	int x = 0;    
	if ( m_opImTmp1->width < width ) // force at least one correlation
		width = m_opIm->width;
    int step = width - *(int*)params[2];
	
    m_opImTmp2 = imImageCreate( width, m_opImTmp1->height, m_opImTmp1->color_space, m_opImTmp1->data_type );
    if ( !m_opImTmp2 || !width )
    {
        this->Terminate( ERR_MEMORY );
        return;
    }
	
	int size = LYRIC_FEATURES_COUNT * ( m_opImTmp1->width / step );
	float *values = (float*)malloc( size * sizeof(float) );
	memset(values, 0, size * sizeof(float) );
	//wxLogMessage("step %d", step );
	int samples = 0; 

	int white_spaces = 0;
    while (1)
    {
        if ( x + width > m_opImTmp1->width )
            break;
		
        imProcessCrop( m_opImTmp1, m_opImTmp2, x, 0);
		if ( CalcLyricWinFeatures( m_opImTmp2, values + ( samples * LYRIC_FEATURES_COUNT ) ) ){
			samples++;
			white_spaces = 0;
		} else if ( white_spaces < 2 ){
			memset( values + ( samples * LYRIC_FEATURES_COUNT ), 0, LYRIC_FEATURES_COUNT * sizeof(float) );
			white_spaces++;
			samples++;
		}
        x += step;
    }
	
	filename << "_" << staff << "lyric.0.mfc";
	wxString mfc_name = filename;
	mfc_name.Replace( "\\/", "/" );
	wxFile *file = (wxFile*)params[10];
	file->Write( mfc_name );
	file->Write( "\n" );
	
	this->WriteMFC( filename, samples, step * 100, LYRIC_FEATURES_COUNT, values );
	
	free( values );
	
	this->Terminate( ERR_NONE );
}					
					

