/////////////////////////////////////////////////////////////////////////////
// Name:        imstaff.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "imstaff.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"

#include "imstaff.h"
#include "imstaffsegment.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfStaffSegments );

// WDR: class implementations

//----------------------------------------------------------------------------
// ImStaff
//----------------------------------------------------------------------------

ImStaff::ImStaff( ) :
    ImOperator( )
{
    m_y = 0;
    m_med = 0;
}


ImStaff::~ImStaff()
{
    m_segments.Clear();
}


bool ImStaff::Load( TiXmlNode *node )
{
    TiXmlElement *elem = NULL;
    TiXmlElement *current = node->ToElement();
    if ( !current )
        return false;

    if ( current->Attribute("y"))
        m_y = atoi(current->Attribute("y"));

    // staves
    for( node = current->FirstChild( "staffsegment" ); node; node = node->NextSibling( "staffsegment" ) )
    {
        elem = node->ToElement();
        if (!elem) return false;
        ImStaffSegment segment ;
        segment.Load( elem );
        m_segments.Add( segment );
    }

    return true;
}

/*
bool ImStaff::Load( DOMNode *node )
{
    XMLCh *name = NULL;
    XMLCh *attribute = NULL;
    DOMElement *elem = NULL;
    DOMElement *current = (DOMElement*)node;

 
    attribute = XMLString::transcode( "y" );
    m_y = atoi( XMLwxString::Convert( current->getAttribute(attribute) ) );

    try
    {
        name = XMLString::transcode("staffsegment");
        DOMNodeList *segments = NULL;
        segments = current->getElementsByTagName(name);
        if ( segments ) 
        {
            int i;
            for (i = 0; i < (int)segments->getLength(); i++)
            {
                elem = (DOMElement*)segments->item(i);
                if (elem == NULL) 
                    throw DOMException (DOMException::NOT_FOUND_ERR, name);

                // printer type (default,invoice,till or label)
                ImStaffSegment segment;
                segment.Load( elem );
                m_segments.Add( segment );
            }
        }
    }
    catch (const XMLException& toCatch) {
        wxLogError(XMLwxString::Convert(toCatch.getMessage()));
        return FALSE;
    }
    catch (const DOMException& toCatch) {
        wxLogError(XMLwxString::Convert(toCatch.msg));
        return FALSE;
    }
    catch (...) {
        wxLogError(XML_ERR_UNKNOWN);
        return FALSE;
    }

    XMLString::release(&name);
    XMLString::release(&attribute);
    return true;
}
*/

bool ImStaff::Save( TiXmlNode *node )
{
    TiXmlElement *current = node->ToElement();
    if ( !current )
        return false;

    wxString tmp;

    tmp = wxString::Format("%d", m_y );
    current->SetAttribute( "y", tmp.c_str() );

    for( int i = 0; i < (int)m_segments.GetCount() ; i++)
    {
        TiXmlElement elem ("staffsegment");
        m_segments[i].Save( &elem );
        current->InsertEndChild( elem );
    }

    return true;
}

/*
bool ImStaff::Save( DOMNode *node, DOMDocument *dom )
{
    DOMElement *elem = NULL;
    DOMElement *current = (DOMElement*)node;
    wxString tmp;

    tmp = wxString::Format("%d", m_y );
    current->setAttribute( X("y"), X( tmp.c_str() ) );

    for( int i = 0; i < (int)m_segments.GetCount() ; i++)
    {
        elem = dom->createElement(X("staffsegment"));
        m_segments[i].Save( elem, dom );
        current->appendChild( elem );
    }

    return true;
}
*/

void ImStaff::GetMinMax(int *minx, int *maxx)
{
	*minx = 0;
	*maxx = 0;

    int nb = (int)m_segments.GetCount();
    if (nb == 0)
        return;

    *minx = m_segments[0].m_x1;
    *maxx = m_segments[nb-1].m_x2;
}

void ImStaff::GetXandPos( int posx, int *x, int *vpos )
{
	*vpos = 0;
	*x = posx;
	int nb = (int)m_segments.GetCount();
	if ( nb > 0 )
		(*x) -= m_segments[0].m_x1;
	
	int i = 0;

	//posx -= (*x);
	while(i < nb)
	{
		ImStaffSegment *segment = &m_segments[i];
		if ((posx < segment->m_x2) && (posx >= segment->m_x1 ))
		{
			*vpos = -segment->m_positions[ posx - segment->m_x1 ];
			break;
		}
		i++;
	}
}

int ImStaff::CalcIndentation( int leftmargin )
{
    int nb = (int)m_segments.GetCount();
    if (nb == 0)
        return 0;

    int indent = (m_segments[0].m_x1 - leftmargin) / 10;
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



void ImStaff::Process(ImStaffSegmentFunctor *functor, int staff, int y, wxArrayPtrVoid params )
{
    int nb_segments = (int)m_segments.GetCount();
    int i;
    for(i = 0; i < nb_segments; i++)
    {
        functor->Call( &m_segments[i], staff, i, y, params );

    }
}


// WDR: handler implementations for ImStaff




// functors for ImStaffSegment

// generic functor that simply call the staffsegmentfunctor building a merged version - 
// uses the whole page width
//  
void ImStaff::MergeSegments(const int staff, const int y, wxArrayPtrVoid params, ImStaffSegmentFunctor *functor )
{
    wxASSERT_MSG( functor, "ImStaffSegmentFunctor cannot be null" );
	// param 0: image de la page
	
	imImage *page = (imImage*)params[0];

	ImStaffSegment *merged = new ImStaffSegment( );
	merged->m_x1 = 0;
	merged->m_x2 = page->width;

    int nb_segments = (int)m_segments.GetCount();
    int i;
    for(i = 0; i < nb_segments; i++)
    {
		// don't use all page with here : position in mlf files must be ajusted....
		if ( i == 0 )
			merged->m_x1 = m_segments[i].m_x1;
		merged->m_x2 = m_segments[i].m_x2;
		
		merged->m_positions.Add( 0, m_segments[i].m_x1 - merged->m_positions.GetCount() );
		WX_APPEND_ARRAY( merged->m_positions, m_segments[i].m_positions );
        //functor->Call( &m_segments[i], staff, i, y, params );

    }
	merged->m_positions.Add( 0, page->width - merged->m_positions.GetCount() );
	if ( i == 0 ) // only one segment in staff. Result will be identical
		functor->Call( merged, staff , 0, y, params);
	else // use -1 as segment number to indicate the the segments have been merged
		functor->Call( merged, staff , -1, y, params);
}
