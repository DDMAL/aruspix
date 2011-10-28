/////////////////////////////////////////////////////////////////////////////
// Name:        imregister.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "imregister.h"
#endif

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"

#include "imregister.h"
#include "impage.h"
#include "imstaff.h"

#include "app/axapp.h"

#include "superimposition/sup.h" // not optimal, should be restructured...


// from  tiff.h, not very good but I don't want to include the file....
#define      PHOTOMETRIC_MINISWHITE      0       /* min value is white */
#define      PHOTOMETRIC_MINISBLACK      1       /* min value is black */
#define      PHOTOMETRIC_RGB             2       /* RGB color model */


//----------------------------------------------------------------------------
// ImRegister
//----------------------------------------------------------------------------

ImRegister::ImRegister( wxString path, bool *isModified ) :
    ImOperator( )
{	
	m_path = path;
	m_src1 = NULL;
	m_src2 = NULL;
	m_result = NULL;
	m_isModified = 	isModified;
    Clear( );
	
	// aditional temporary images
    m_im1 = NULL;
    m_im2 = NULL;
	
	m_imPage1Ptr = new ImPage( m_path );
	m_imPage2Ptr = new ImPage( m_path );
}

ImRegister::~ImRegister()
{
	if ( m_src1 )
		ImageDestroy( &m_src1 );
	if ( m_src2 )
		ImageDestroy( &m_src2 );
	if ( m_result )
		ImageDestroy( &m_result );
	if ( m_imPage1Ptr )
		delete m_imPage1Ptr;
	if ( m_imPage2Ptr )
		delete m_imPage2Ptr;
}

bool ImRegister::Terminate( int code,  ... )
{
    // Attention que deux de ces pointeurs ne refere pas la meme adresse lors de l'appel de cette methode !
    ImageDestroy( &m_im1 );
    ImageDestroy( &m_im2 );
	
	wxLogDebug("Terminate::ImRegister");
		
	va_list argptr;
    va_start( argptr, code ); 
	return ImOperator::Terminate( code, argptr );
}

void ImRegister::Clear( )
{	
	ImageDestroy( &m_src1 );
	ImageDestroy( &m_src2 );
	ImageDestroy( &m_result );
}

bool ImRegister::Load( TiXmlElement *file_root )
{
	if ( !file_root )
		return false;

	this->Clear();
	bool failed = false;

    if ( !failed )
		failed = (!wxFileExists( m_path + "src1.tif" ) || !Read( m_path + "src1.tif", &m_src1, 0 ));

    if ( !failed )
		failed = (!wxFileExists( m_path + "src2.tif" ) || !Read( m_path + "src2.tif", &m_src2, 0 ));

    if ( !failed )
		failed = (!wxFileExists( m_path + "result.tif" ) || !Read( m_path + "result.tif", &m_result, 0 ));

	/*  load more data ?
    TiXmlElement *root = NULL;
    TiXmlNode *node = NULL;
    TiXmlElement *elem = NULL;
	
	node = file_root->FirstChild( "imregister" );
	if ( !node ) return false;
		
	root = node->ToElement();
    if ( !root ) return false;

    if ( root->Attribute("skew"))
        m_skew = atof(root->Attribute("skew"));

	// needed ???
    if ( root->Attribute("resize"))
        m_resize = atof(root->Attribute("resize"));
	
	// needed ???
    if ( root->Attribute("resized"))
        m_resized = atof(root->Attribute("resized"));

    if ( root->Attribute("reduction"))
        m_reduction = atoi(root->Attribute("reduction"));

    if ( root->Attribute("x2"))
        m_x2 = atoi(root->Attribute("x2"));

    if ( root->Attribute("x1"))
        m_x1 = atoi(root->Attribute("x1"));

    if ( root->Attribute("width"))
        m_size.SetWidth( atoi(root->Attribute("width")) );

    if ( root->Attribute("height"))
        m_size.SetHeight( atoi(root->Attribute("height")) );

    if ( root->Attribute("line_width"))
        m_line_width = atoi(root->Attribute("line_width"));

    if ( root->Attribute("space_width"))
        m_space_width = atoi(root->Attribute("space_width"));

    if ( root->Attribute("staff_height"))
        m_staff_height = atoi(root->Attribute("staff_height"));

    // staves
    for( node = root->FirstChild( "staff" ); node; node = node->NextSibling( "staff" ) )
    {
        elem = node->ToElement();
        if (!elem) 
			return false;
        ImStaff imstaff;
        imstaff.Load( elem );
        m_staves.Add( imstaff );
    }
	*/
    
    return !failed;
}

bool ImRegister::Save( TiXmlElement *file_root )
{
	bool failed = false;

    if ( !failed && m_src1 )
		failed = !Write( m_path + "src1.tif", &m_src1 );
	
    if ( !failed && m_src2 )
		failed = !Write( m_path + "src2.tif", &m_src2 );
		
    if ( !failed && m_result )
		failed = !Write( m_path + "result.tif", &m_result );	
	
	/* write more data ?
    wxString tmp;

    TiXmlElement root("impage");
    
    tmp = wxString::Format("%d", m_reduction );
    root.SetAttribute( "reduction",  tmp.c_str() );

    tmp = wxString::Format("%f", m_skew );
    root.SetAttribute( "skew", tmp.c_str() );
	
    tmp = wxString::Format("%f", m_resized );
    root.SetAttribute( "resized", tmp.c_str() );

    tmp = wxString::Format("%d", m_x1 );
    root.SetAttribute( "x1", tmp.c_str() );

    tmp = wxString::Format("%d", m_x2 );
    root.SetAttribute( "x2", tmp.c_str() );

    tmp = wxString::Format("%d", m_size.GetWidth() );
    root.SetAttribute( "width", tmp.c_str() );

    tmp = wxString::Format("%d", m_size.GetHeight() );
    root.SetAttribute( "height", tmp.c_str() );

    tmp = wxString::Format("%d", m_line_width );
    root.SetAttribute( "line_width", tmp.c_str() );

    tmp = wxString::Format("%d", m_space_width );
    root.SetAttribute( "space_width",  tmp.c_str() );

    tmp = wxString::Format("%d", m_staff_height );
    root.SetAttribute( "staff_height",  tmp.c_str() );

    for( int i = 0; i < (int)m_staves.GetCount() ; i++)
    {
        TiXmlElement elem ("staff");
        m_staves[i].Save( &elem );
        root.InsertEndChild( elem );
    }

	if ( file_root )
	    file_root->InsertEndChild( root );
	*/
	
    return !failed;
}


bool ImRegister::Init( wxString filename1, wxString filename2 )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_imPage1Ptr, "ImPage1 cannot be NULL" );
	wxASSERT_MSG( m_imPage2Ptr, "ImPage2 cannot be NULL" );
	
    if (!m_progressDlg->SetOperation( _("Checking the image 1 ...") ) )
        return this->Terminate( ERR_CANCELED );

	m_imPage1Ptr->SetProgressDlg( m_progressDlg );
	
	if ( !m_imPage1Ptr->Check( filename1, 5000, 1200 ) )
		return false;
        
    if (!m_progressDlg->SetOperation( _("Checking the image 2 ...") ) )
        return this->Terminate( ERR_CANCELED );

	m_imPage2Ptr->SetProgressDlg( m_progressDlg );
	
	if ( !m_imPage2Ptr->Check( filename2, 5000, 1200 ) )
		return false;

	ImageDestroy( &m_src1 );
	ImageDestroy( &m_src2 );
	
    m_src1 = imImageDuplicate( m_imPage1Ptr->m_img0 );
    if ( !m_src1 )
        return this->Terminate( ERR_MEMORY );
	ImageDestroy( &m_src2 );
    m_src2 = imImageDuplicate( m_imPage2Ptr->m_img0 );
    if ( !m_src2 )
        return this->Terminate( ERR_MEMORY );

	if ( m_isModified ) 
		*m_isModified = true;
	return this->Terminate( ERR_NONE );
}



wxPoint ImRegister::CalcPositionAfterRotation( wxPoint point , float rot_alpha, 
                                  int w, int h, int new_w, int new_h)
{

    wxPoint center( w/2, h/2);
    int distCenterX = point.x - center.x;
    int distCenterY = point.y - center.y;
    // pythagore, distance entre le point d'origine et le centre
    int distCenter = (int)sqrt( pow( (double)distCenterX, 2 ) + pow( (double)distCenterY, 2 ) );

    // angle d'origine entre l'axe x et la droite passant par le point et le centre
    float alpha = atan ( (float)distCenterX / (float)(distCenterY) );
    
    wxPoint new_p = center;
    int new_distCenterX, new_distCenterY;

    new_distCenterX = abs( (int)( sin( alpha - rot_alpha ) * distCenter ) );
    new_p.x += (center.x > point.x) ? -new_distCenterX : new_distCenterX;

    new_distCenterY = abs( (int)( cos( alpha - rot_alpha ) * distCenter ) );
    new_p.y += (center.y > point.y) ? -new_distCenterY : new_distCenterY;
    
    // marges
    new_p.x += (new_w - w) / 2;
    new_p.y += (new_h - h) / 2;

    // verifier la position du point -> TODO traitement d'erreur
    if ((new_p.x < 0) || (new_p.x > new_w))
        new_p = point;
    else if ((new_p.y < 0) || (new_p.y > new_h))
        new_p = point;

    return new_p;
}

bool ImRegister::DetectPoints( wxPoint *points1, wxPoint *points2)
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_src1, "Src1 cannot be NULL");
	wxASSERT_MSG( m_src2, "Src2 cannot be NULL");
	wxASSERT_MSG( m_imPage1Ptr, "ImPage1 cannot be NULL" );
	wxASSERT_MSG( m_imPage2Ptr, "ImPage2 cannot be NULL" );
	
	bool failed = false;
    
    if (!m_progressDlg->SetOperation( _("Analyzing the image 1 ...") ) )
        return this->Terminate( ERR_CANCELED );
	
	if ( !failed ) 
        failed = !m_imPage1Ptr->Deskew( 10.0 ); // 2 operations max
        
    if ( !failed ) 
        failed = !m_imPage1Ptr->FindStaves(  3, 50, false, false );  // 4 operations max
	
    if (!m_progressDlg->SetOperation( _("Analyzing the image 2 ...") ) )
        return this->Terminate( ERR_CANCELED );
    
	if ( !failed ) 
        failed = !m_imPage2Ptr->Deskew( 10.0 ); // 2 operations max
        
    if ( !failed ) 
        failed = !m_imPage2Ptr->FindStaves(  3, 50, false, false );  // 4 operations max
		
	if ( failed )
		return this->Terminate( ERR_UNKNOWN );
	
	// in case the number of staves is not identical, we register from top one
	// as staves order is reversed, we have to detect the index of the bottom one as it is going
	// to be different from 0 for one of the pages
	int staff_nb = min( (int)m_imPage1Ptr->m_staves.GetCount(), (int)m_imPage2Ptr->m_staves.GetCount() );
	int bottom_staff1 = (int)m_imPage1Ptr->m_staves.GetCount() - staff_nb;
	int bottom_staff2 = (int)m_imPage2Ptr->m_staves.GetCount() - staff_nb;
	int top_staff1 = staff_nb - 1 + bottom_staff1;
	int top_staff2 = staff_nb - 1 + bottom_staff2;
	
	if ( (int)m_imPage1Ptr->m_staves.GetCount() != (int)m_imPage2Ptr->m_staves.GetCount() )
	{
		wxLogWarning( "Staff detection did not retrieve the same number of staves of both images" );
		wxLogDebug("Page1, staves %d %d; Page2, staves %d %d", top_staff1, bottom_staff1, top_staff2, bottom_staff2 );
	}
	
	
	int top_y1 = m_imPage1Ptr->m_staves[top_staff1].m_y;
	int top_y2 = m_imPage2Ptr->m_staves[top_staff2].m_y;
	int bottom_y1 = m_imPage1Ptr->m_staves[bottom_staff1].m_y;
	int bottom_y2 = m_imPage2Ptr->m_staves[bottom_staff2].m_y;
		
	// y is the center position off the staff. We want to get the top line of the first staff 
	// and the bottom line of the last as we will get better registration results
	if ( ( top_y1 - (STAFF_HEIGHT / (2 * m_imPage1Ptr->m_resize) ) > 0 ) 
		&& ( top_y2 - (STAFF_HEIGHT / (2 * m_imPage2Ptr->m_resize) ) > 0 ) )
	{
		top_y1 -= STAFF_HEIGHT / (2 * m_imPage1Ptr->m_resize);
		top_y2 -= STAFF_HEIGHT / (2 * m_imPage2Ptr->m_resize);
	}
	if ( ( bottom_y1 + (STAFF_HEIGHT / (2 * m_imPage1Ptr->m_resize) ) < m_imPage1Ptr->m_img1->height ) 
		&& ( bottom_y2 + (STAFF_HEIGHT / (2 * m_imPage2Ptr->m_resize) ) < m_imPage2Ptr->m_img1->height ) )
	{
		bottom_y1 += STAFF_HEIGHT / (2 * m_imPage1Ptr->m_resize);
		bottom_y2 += STAFF_HEIGHT / (2 * m_imPage2Ptr->m_resize);
	}
	
	
	points1[1] = wxPoint( m_imPage1Ptr->m_x1, bottom_y1 );
	points1[3] = wxPoint( m_imPage1Ptr->m_x2, bottom_y1 );
	points1[0] = wxPoint( m_imPage1Ptr->m_x1, top_y1 );
	points1[2] = wxPoint( m_imPage1Ptr->m_x2, top_y1 );

	points2[1] = wxPoint( m_imPage2Ptr->m_x1, bottom_y2 );
	points2[3] = wxPoint( m_imPage2Ptr->m_x2, bottom_y2 );
	points2[0] = wxPoint( m_imPage2Ptr->m_x1, top_y2 );
	points2[2] = wxPoint( m_imPage2Ptr->m_x2, top_y2 );
	
	ImageDestroy( &m_src1 );
    m_src1 = imImageDuplicate( m_imPage1Ptr->m_img1 );
    if ( !m_src1 )
        return this->Terminate( ERR_MEMORY );
	imProcessNegative( m_src1, m_src1 );
	ImageDestroy( &m_src2 );
    m_src2 = imImageDuplicate( m_imPage2Ptr->m_img1 );
    if ( !m_src2 )
        return this->Terminate( ERR_MEMORY );
	imProcessNegative( m_src2, m_src2 );
    
    /*
    wxLogDebug("point1 1 %d %d", points1[0].x, points1[0].y );
    wxLogDebug("point1 2 %d %d", points1[1].x, points1[1].y );
    wxLogDebug("point1 3 %d %d", points1[2].x, points1[2].y );
    wxLogDebug("point1 4 %d %d", points1[3].x, points1[3].y );
    
    wxLogDebug("point2 1 %d %d", points2[0].x, points2[0].y );
    wxLogDebug("point2 2 %d %d", points2[1].x, points2[1].y );
    wxLogDebug("point2 3 %d %d", points2[2].x, points2[2].y );
    wxLogDebug("point2 4 %d %d", points2[3].x, points2[3].y );
    */
	
	return this->Terminate( ERR_NONE );	
}


bool ImRegister::Register( wxPoint *points1, wxPoint *points2)
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_src1, "Src1 cannot be NULL");
	wxASSERT_MSG( m_src2, "Src2 cannot be NULL");

    if (!m_progressDlg->SetOperation( _("Registration ...") ) )
        return this->Terminate( ERR_CANCELED );

	int i;
    // copy m_reg_points locally for registration
	for (i = 0; i < 4; i++ )
	{
		m_reg_points1[i] = points1[i];
		m_reg_points2[i] = points2[i];
	}

    // calculer la hauteur des segments et redimensionner
    int left1 = (int)sqrt( 
        pow( (double)(m_reg_points1[0].x - m_reg_points1[1].x) , 2) + 
        pow( (double)(m_reg_points1[0].y - m_reg_points1[1].y), 2));
    int right1 = (int)sqrt( 
        pow( (double)(m_reg_points1[2].x - m_reg_points1[3].x) , 2) + 
        pow( (double)(m_reg_points1[2].y - m_reg_points1[3].y), 2));
	int vmean1 = (left1 + right1) / 2; // moyenne des deux cotes
    //wxLogDebug(" segment 1 %d", segment1 );
    int left2 = (int)sqrt(
        pow( (double)(m_reg_points2[0].x - m_reg_points2[1].x), 2) + 
        pow( (double)(m_reg_points2[0].y - m_reg_points2[1].y), 2));
    int right2 = (int)sqrt(
        pow( (double)(m_reg_points2[2].x - m_reg_points2[3].x), 2) + 
        pow( (double)(m_reg_points2[2].y - m_reg_points2[3].y), 2));
	int vmean2 = (left2 + right2) / 2; // moyenne des deux cotes
    // facteur de redimensionnement
    float vfactor1 = 1.0;
	float vfactor2 = vfactor1 *(float)vmean1 / (float)vmean2;
	
	
    // idem segments horizontaux
    int top1 = (int)sqrt( 
        pow( (double)(m_reg_points1[0].x - m_reg_points1[2].x) , 2) + 
        pow( (double)(m_reg_points1[0].y - m_reg_points1[2].y), 2));
    int bottom1 = (int)sqrt( 
        pow( (double)(m_reg_points1[1].x - m_reg_points1[3].x) , 2) + 
        pow( (double)(m_reg_points1[1].y - m_reg_points1[3].y), 2));
	int hmean1 = (top1 + bottom1) / 2; // moyenne des deux cotes
    //wxLogDebug(" segment 1 %d", segment1 );
    int top2 = (int)sqrt(
        pow( (double)(m_reg_points2[0].x - m_reg_points2[2].x), 2) + 
        pow( (double)(m_reg_points2[0].y - m_reg_points2[2].y), 2));
    int bottom2 = (int)sqrt(
        pow( (double)(m_reg_points2[1].x - m_reg_points2[3].x), 2) + 
        pow( (double)(m_reg_points2[1].y - m_reg_points2[3].y), 2));
	int hmean2 = (top2 + bottom2) / 2; // moyenne des deux cotes
    // facteur de redimensionnement, only for image 2 (the one for image 1 is identical to vfactor1
	float hfactor1 = vfactor1;
    float hfactor2 = hfactor1 * (float)hmean1 / (float)hmean2;
	
    //wxLogDebug("vmean1 %d, hmean1 %d, vmean2 %d, hmean2 %d", vmean1, hmean1, vmean2, hmean2);
	//wxLogDebug("vfactor1 %f, vfactor2 %f, hfactor1 %f, hfactor2 %f", vfactor1, vfactor2, hfactor1, hfactor2);

    if (!m_progressDlg->SetOperation( _("Preparartion of image 1 ...") ))
        return this->Terminate( ERR_CANCELED );

    m_im1 = imImageDuplicate( m_src1 );
    if ( !m_im1 )
        return this->Terminate( ERR_MEMORY );

    // median filtering
    if (SupEnv::s_filter1)
    {
        if ( !m_progressDlg->SetOperation( _("Filtering image 1 ...") ))
            return this->Terminate( ERR_CANCELED );

        m_opImTmp1 = imImageClone( m_im1 );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );

        if ( !imProcessMedianConvolve( m_im1 ,m_opImTmp1, 3 ) )
            return this->Terminate( ERR_CANCELED );

        SwapImages( &m_im1, &m_opImTmp1 );

    }
    // resize
    /*
    if (!m_progressDlg->SetOperation( _("Resizing image ...") ))
        return this->Terminate( ERR_CANCELED );

    m_opImTmp1 = imImageCreate(  (int)(m_im1->width  * hfactor1), (int)(m_im1->height  * vfactor1),
            m_im1->color_space, m_im1->data_type);
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    if ( !imProcessResize( m_im1 ,m_opImTmp1, SupEnv::s_interpolation) )
        return this->Terminate( ERR_CANCELED );

    SwapImages( &m_im1, &m_opImTmp1 );
    */

    // ajuster la position des m_reg_points
    m_reg_points1[0].x = (int)(m_reg_points1[0].x * hfactor1);
    m_reg_points1[0].y = (int)(m_reg_points1[0].y * vfactor1);
    m_reg_points1[1].x = (int)(m_reg_points1[1].x * hfactor1);
    m_reg_points1[1].y = (int)(m_reg_points1[1].y * vfactor1);
    m_reg_points1[2].x = (int)(m_reg_points1[2].x * hfactor1);
    m_reg_points1[2].y = (int)(m_reg_points1[2].y * vfactor1);
	m_reg_points1[3].x = (int)(m_reg_points1[3].x * hfactor1);
    m_reg_points1[3].y = (int)(m_reg_points1[3].y * vfactor1);


    if (!m_progressDlg->SetOperation( _("Preparation of image 2 ...") ))
        return this->Terminate( ERR_CANCELED );

    m_im2 = imImageDuplicate( m_src2 );
    if ( !m_im2 )
        return this->Terminate( ERR_MEMORY );

    // median filtering
    if (SupEnv::s_filter2)
    {
        if (!m_progressDlg->SetOperation( _("Filtering image 2 ...") ))
            return this->Terminate( ERR_CANCELED );

        m_opImTmp1 = imImageClone(  m_im2 );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
   
        if ( !imProcessMedianConvolve( m_im2 ,m_opImTmp1, 3 ) )
            return this->Terminate( ERR_CANCELED );

        SwapImages( &m_im2, &m_opImTmp1 );
    }
    // resize
    if (!m_progressDlg->SetOperation( _("Resizing image 2 ...") ))
        return this->Terminate( ERR_CANCELED );

    m_opImTmp1 = imImageCreate(  (int)(m_im2->width  * hfactor2), (int)(m_im2->height  * vfactor2),
            m_im2->color_space, m_im2->data_type);
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    if ( !imProcessResize( m_im2 ,m_opImTmp1, SupEnv::s_interpolation ) )
        return this->Terminate( ERR_CANCELED );

    SwapImages( &m_im2, &m_opImTmp1 );

    m_reg_points2[0].x = (int)(m_reg_points2[0].x * hfactor2);
    m_reg_points2[0].y = (int)(m_reg_points2[0].y * vfactor2);
    m_reg_points2[1].x = (int)(m_reg_points2[1].x * hfactor2);
    m_reg_points2[1].y = (int)(m_reg_points2[1].y * vfactor2);
    m_reg_points2[2].x = (int)(m_reg_points2[2].x * hfactor2);
    m_reg_points2[2].y = (int)(m_reg_points2[2].y * vfactor2);
	m_reg_points2[3].x = (int)(m_reg_points2[3].x * hfactor2);
    m_reg_points2[3].y = (int)(m_reg_points2[3].y * vfactor2);

    // calculer les angles et pivoter
    int new_w, new_h;
    double cos0, sin0;

    // image 1
    // calculer l'angle
    if (!m_progressDlg->SetOperation( _("Rotation of image 1 ...") ))
        return this->Terminate( ERR_CANCELED );

    float left_alpha1 = float(m_reg_points1[0].x - m_reg_points1[1].x) / float(m_reg_points1[0].y - m_reg_points1[1].y);
    left_alpha1 = atan(left_alpha1);
	float right_alpha1 = float(m_reg_points1[2].x - m_reg_points1[3].x) / float(m_reg_points1[2].y - m_reg_points1[3].y);
    right_alpha1 = atan(right_alpha1);
	float alpha1 = (left_alpha1 + right_alpha1) / 2;

    sin0 = sin(alpha1);
    cos0 = cos(alpha1);
    imProcessCalcRotateSize( m_im1->width, m_im1->height, &new_w, &new_h, cos0, sin0 );
    // ajuster la position des m_reg_points
    m_reg_points1[0] = CalcPositionAfterRotation( m_reg_points1[0], alpha1, m_im1->width, m_im1->height, new_w, new_h);
    m_reg_points1[1] = CalcPositionAfterRotation( m_reg_points1[1], alpha1, m_im1->width, m_im1->height, new_w, new_h);
    m_reg_points1[2] = CalcPositionAfterRotation( m_reg_points1[2], alpha1, m_im1->width, m_im1->height, new_w, new_h);

    m_opImTmp1 = imImageCreate(new_w, new_h, m_im1->color_space, m_im1->data_type);
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    imImageCopyAttributes( m_im1, m_opImTmp1);
    if ( !imProcessRotate( m_im1, m_opImTmp1, cos0, sin0, SupEnv::s_interpolation) )
        return this->Terminate( ERR_CANCELED );

    SwapImages( &m_im1, &m_opImTmp1 );


    // idem image 2
    if (!m_progressDlg->SetOperation( _("Rotation of image 2 ...") ))
        return this->Terminate( ERR_CANCELED );

    float left_alpha2 = float(m_reg_points2[0].x - m_reg_points2[1].x) / float(m_reg_points2[0].y - m_reg_points2[1].y);
    left_alpha2 = atan( left_alpha2 );
    float right_alpha2 = float(m_reg_points2[2].x - m_reg_points2[3].x) / float(m_reg_points2[2].y - m_reg_points2[3].y);
    right_alpha2 = atan( right_alpha2 );
	float alpha2 = (left_alpha2 + right_alpha2) / 2;

    sin0 = sin(alpha2);
    cos0 = cos(alpha2);
    imProcessCalcRotateSize( m_im2->width, m_im2->height, &new_w, &new_h, cos0, sin0 );
    m_reg_points2[0] = CalcPositionAfterRotation( m_reg_points2[0], alpha2, m_im2->width, m_im2->height, new_w, new_h);
    m_reg_points2[1] = CalcPositionAfterRotation( m_reg_points2[1], alpha2, m_im2->width, m_im2->height, new_w, new_h);
    m_reg_points2[2] = CalcPositionAfterRotation( m_reg_points2[2], alpha2, m_im2->width, m_im2->height, new_w, new_h);

    m_opImTmp1 = imImageCreate( new_w, new_h, m_im2->color_space, m_im2->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    imImageCopyAttributes( m_im2, m_opImTmp1 );
    if ( !imProcessRotate( m_im2, m_opImTmp1, cos0, sin0, SupEnv::s_interpolation ) )
        return this->Terminate( ERR_CANCELED );

    SwapImages( &m_im2, &m_opImTmp1 );

    // deplacer (crop ou marges)
    if (!m_progressDlg->SetOperation( _("Calculation of margins ...") ) )
       return this->Terminate( ERR_CANCELED );

    //wxLogDebug("p1 %d %d; p2 %d %d; p3 %d %d", m_reg_points1[0].x, m_reg_points1[0].y, m_reg_points1[1].x , m_reg_points1[1].y , m_reg_points1[2].x , m_reg_points1[2].y);
    //wxLogDebug("p1 %d %d; p2 %d %d; p3 %d %d", m_reg_points2[0].x, m_reg_points2[0].y, m_reg_points2[1].x , m_reg_points2[1].y , m_reg_points2[2].x , m_reg_points2[2].y);


    // marges x
    int im1_mx1 = min( m_reg_points1[0].x, m_reg_points1[1].x );
    int im2_mx1 = min( m_reg_points2[0].x, m_reg_points2[1].x );
    int im1_mx2 = m_reg_points1[2].x;
    int im2_mx2 = m_reg_points2[2].x;
    // marges y
    int im1_my1 = m_reg_points1[0].y;
    int im2_my1 = m_reg_points2[0].y;
    int im1_my2 = max( m_reg_points1[1].y, m_reg_points1[2].y );
    int im2_my2 = max( m_reg_points2[1].y, m_reg_points2[2].y );

    // min de crop des 2 images
    int minx1 = (im1_mx1 > im2_mx1 ) ? im1_mx1 - im2_mx1 : 0;
    int minx2 = (im2_mx1 > im1_mx1 ) ? im2_mx1 - im1_mx1 : 0;
    int miny1 = (im1_my1 > im2_my1 ) ? im1_my1 - im2_my1 : 0;
    int miny2 = (im2_my1 > im1_my1 ) ? im2_my1 - im1_my1 : 0;

    // largeur et hauteur de la zone de superposition + point d'orgine
    int width = min( ( im1_mx2 - im1_mx1 ), ( im2_mx2 - im2_mx1 ) );
    int height = min( ( im1_my2 - im1_my1 ), ( im2_my2 - im2_my1 ) );
    wxPoint origine( min ( im1_mx1, im2_mx1 ), min ( im1_my1, im2_my1 ) );

    // largeur et hauteur des nouvelles images (zone de superposition + marge minimale )
    int im_width = origine.x + width + min( m_im1->width - im1_mx1 - width, m_im2->width - im2_mx1 - width );
    int im_height = origine.y + height + min( m_im1->height - im1_my1 - height, m_im2->height - im2_my1 - height );
    
    m_opImTmp1 = imImageCreate(im_width, im_height, m_im1->color_space, m_im1->data_type);
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    imProcessCrop( m_im1 , m_opImTmp1, minx1, miny1 );
    SwapImages( &m_im1, &m_opImTmp1 );

    m_opImTmp1 = imImageCreate(im_width, im_height, m_im2->color_space, m_im2->data_type);
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    imProcessCrop( m_im2 , m_opImTmp1, minx2, miny2 );
    SwapImages( &m_im2, &m_opImTmp1 );

    // garder l'image original pour le fichier
	ImageDestroy( &m_src1 );
    m_src1 = imImageClone( m_im1 );
    if ( !m_src1 )
        return this->Terminate( ERR_MEMORY );
	imProcessNegative( m_im1, m_src1 );
	ImageDestroy( &m_src2 );
    m_src2 = imImageClone( m_im2 );
    if ( !m_src2 )
        return this->Terminate( ERR_MEMORY );
	imProcessNegative( m_im2, m_src2 );
    
    // we can start with a fairly wide window
    wxSize window( max( SupEnv::s_corr_x, width / 25 ), max( SupEnv::s_corr_y, height / 25 ) );
	wxLogDebug( "Window %d x %d", window.GetWidth(), window.GetHeight() );

    m_opImAlign = imImageCreate( m_im2->width + 2 * window.GetWidth(), m_im2->height + 2 * window.GetHeight(),
        m_im2->color_space, m_im2->data_type );
    if ( !m_opImAlign )
        return this->Terminate( ERR_MEMORY );

    memset( m_opImAlign->data[0], 255, m_opImAlign->size );
    if (m_im1->size < m_opImAlign->size)
        imSetData( m_opImAlign, m_im1, window.GetWidth(), window.GetHeight() );
	
    int c;
    m_sub_register_total = 1;
    for ( c = 0; c <= SupEnv::s_subWindowLevel; c++ ) {
        m_sub_register_total += pow( pow( 2, c + 1 ), 2 );
    }
    
    if (!m_progressDlg->SetOperation( _("Image registration ..." ) ) )
       return this->Terminate( ERR_CANCELED );
    
	m_counter = imCounterBegin("Image registration");
	imCounterTotal(m_counter, m_sub_register_total, "Image registration");
    
    if ( !SubRegister( origine, window, wxSize( width, height ), 0, 1, 1 ) )
            return this->Terminate( ERR_CANCELED );
    
    imCounterEnd( m_counter );
    imProcessCrop( m_opImAlign, m_im2, window.GetWidth(), window.GetHeight() );
    ImageDestroy( &m_opImAlign );

    if (!m_progressDlg->SetOperation( _("Writing image on disk ...") ) )
        return this->Terminate( ERR_CANCELED );

	// image for negative and bitwise operation
    m_opImAlign = imImageCreate(im_width, im_height, IM_GRAY, IM_BYTE );
    if ( !m_opImAlign )
        return this->Terminate( ERR_MEMORY );
		
    // superposition in result imge
	ImageDestroy( &m_result );
    m_result = imImageCreate(im_width, im_height, IM_RGB, IM_BYTE );
    if ( !m_result )
        return this->Terminate( ERR_MEMORY );
	
	imProcessNegative( m_im1, m_opImAlign );
	memcpy( m_result->data[0], m_opImAlign->data[0], m_opImAlign->size );
	imProcessNegative( m_im2, m_opImAlign );
	memcpy( m_result->data[1], m_opImAlign->data[0], m_opImAlign->size );	

    imProcessBitwiseOp( m_im1, m_im2, m_opImAlign, IM_BIT_XOR );
    //imProcessBitwiseOp( m_im1, m_im2, m_opImAlign, IM_BIT_AND );

    memcpy( m_result->data[2], m_opImAlign->data[0], m_opImAlign->size );
	
	if ( m_isModified ) 
		*m_isModified = true;
	
    return this->Terminate( ERR_NONE );	
}


bool ImRegister::SubRegister( wxPoint origine, wxSize window, wxSize size, int level, int row, int column )
{
    
	int x = 0, y = 0, maxCorr;
    
    wxSize subwindow = window;
    
    m_opImTmp1 = imImageCreate( size.GetWidth(), size.GetHeight(), m_im1->color_space, m_im1->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    m_opImTmp2 = imImageCreate( size.GetWidth(), size.GetHeight(), m_im2->color_space, m_im2->data_type );
    if ( !m_opImTmp2 )
        return this->Terminate( ERR_MEMORY );
        
    imProcessCrop( m_im1, m_opImTmp1, origine.x, origine.y);
    imProcessCrop( m_im2, m_opImTmp2, origine.x, origine.y);

    m_progressDlg->SuspendCounter();
    DistByCorrelation( m_opImTmp1, m_opImTmp2, window, &x, &y, &maxCorr );
    m_progressDlg->ReactiveCounter();

    if (!imCounterInc(m_counter))
        return false;
    
    ImageDestroy( &m_opImTmp1 );
    ImageDestroy( &m_opImTmp2 );

    //wxLogDebug( "Correlation decalage %d %d", x, y );
    
    // use the detected shift to determine the next window size (but at least 5 pixels)
    subwindow = wxSize( max(abs(3*x), SupEnv::s_split_x), max(abs(3*y), SupEnv::s_split_y) );

	if ( level > SupEnv::s_subWindowLevel )
    // end of the recursion
    {  
        int pos_x = origine.x;
        int pos_y = origine.y;
        int width = size.GetWidth();
        int height = size.GetHeight();
        
        int plevel = pow( 2, level );
        
        if ((column / plevel) == 1)
            width = 100000; // extreme value, assume to be more than the maximum
        if ((row / plevel) == 1)
            height = 100000;
        if (column == 1) {
            width += origine.x;
            pos_x = 0;
        }
        if (row == 1)
        {
            height += origine.y;
            pos_y = 0; 
        }
        
        if (imProcessSafeCrop( m_im2, &width, &height, &pos_x, &pos_y ) )
        {
            m_opImMask = imImageCreate( width, height, m_im2->color_space, m_im2->data_type );
            if ( !m_opImMask )
                    return this->Terminate( ERR_MEMORY );
            
            //if (( row == 1 ) || ((row / plevel) == 1) || (column == 1) || ((column / plevel) == 1) ) // border only, for debug
            {
                imProcessCrop( m_im2, m_opImMask, pos_x, pos_y );
                imSetData( m_opImAlign, m_opImMask, 
                            (m_opImAlign->width - m_im2->width) / 2 + pos_x - x,
                            (m_opImAlign->height - m_im2->height) / 2 + pos_y - y);
                
            }
            ImageDestroy( &m_opImMask );
        }
	
		return true;
	}
	
    // move the content, including the border if necessary
    int move_x = origine.x;
    int move_y = origine.y;
    int move_width = size.GetWidth();
    int move_height = size.GetHeight();
        
    int plevel = pow( 2, level );
    if ((column / plevel) == 1)
        move_width = 100000; // extreme value, assume to be more than the maximum
    if ((row / plevel) == 1)
        move_height = 100000; 
    if (column == 1) {
        move_width += origine.x;
        move_x = 0;
    }
    if (row == 1)
    {
        move_height += origine.y;
        move_y = 0; 
    }
    
    // this method return the maximum values for croping
    if (!imProcessSafeCrop( m_im2, &move_width, &move_height, &move_x, &move_y ) ) {
        return this->Terminate( ERR_UNKNOWN );
    }
    m_opImTmp1 = imImageCreate( move_width, move_height, m_im2->color_space, m_im2->data_type );
	if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
        
	imProcessCrop( m_im2, m_opImTmp1, move_x, move_y);
    // actually move the data
	imSetData( m_im2, m_opImTmp1, move_x - x, move_y - y);
    ImageDestroy( &m_opImTmp1 );

    // the problem here is that we have a recusion: we cannot use a m_opXXX image because it would be overriden
    // we use a local variable 'buffer' which would NOT be destroyed if a Terminate occur deeper in the recursion
    // => potential memory leak if the program keep failing....
	imImage *buffer = imImageCreate( size.GetWidth(), size.GetHeight(), m_im2->color_space, m_im2->data_type );
	if ( !buffer )
        return this->Terminate( ERR_MEMORY );
				
    imProcessCrop( m_im2, buffer, origine.x - x, origine.y - y);
    
    
	origine.x -= x;
	origine.y -= y;

	// for subsize as we done what to loose pixels because of odd values
	wxSize subsize1 = size / 2;
	wxSize subsize2 = size - subsize1;
	wxSize subsize3( subsize1.GetWidth(), subsize2.GetHeight() ); 
	wxSize subsize4( subsize2.GetWidth(), subsize1.GetHeight() );
    
    wxPoint origine1 = origine;
    wxPoint origine2 = origine + subsize1;
    wxPoint origine3 = wxPoint( origine.x, origine.y + subsize1.GetHeight() );
    wxPoint origine4 = wxPoint( origine.x + subsize1.GetWidth(), origine.y );

    level++;
    row *= 2;
    column *= 2;
    
    if ( !SubRegister( origine1, subwindow, subsize1, level, row - 1, column - 1 ) )
    {
        imImageDestroy( buffer );
        return false;
    }
    
        
    // next level, we need to copy again the mask because the content might have been modified during the previous recursion
    m_opImTmp1 = imImageCreate( subsize2.GetWidth(), subsize2.GetHeight(), m_im2->color_space, m_im2->data_type );
	if ( !m_opImTmp1 )
			return this->Terminate( ERR_MEMORY );
    imProcessCrop( buffer, m_opImTmp1, origine2.x - origine1.x, origine2.y - origine1.y );
    imSetData( m_im2, m_opImTmp1, origine2.x, origine2.y );
    ImageDestroy( &m_opImTmp1 );	
    if ( !SubRegister( origine2, subwindow, subsize2, level, row, column ) )
    {
        imImageDestroy( buffer );
        return false;
    }
        
    // next
    m_opImTmp1 = imImageCreate( subsize3.GetWidth(), subsize3.GetHeight(), m_im2->color_space, m_im2->data_type );
	if ( !m_opImTmp1 )
			return this->Terminate( ERR_MEMORY );
    imProcessCrop( buffer, m_opImTmp1, 0, origine3.y - origine1.y );
    imSetData( m_im2, m_opImTmp1, origine3.x, origine3.y );
    ImageDestroy( &m_opImTmp1 );	
    if ( !SubRegister( origine3, subwindow, subsize3, level, row, column - 1 ) )
    {
        imImageDestroy( buffer );
        return false;
    }


    // next
    m_opImTmp1 = imImageCreate( subsize4.GetWidth(), subsize4.GetHeight(), m_im2->color_space, m_im2->data_type );
	if ( !m_opImTmp1 )
			return this->Terminate( ERR_MEMORY );
    imProcessCrop( buffer, m_opImTmp1, origine4.x - origine1.x, 0 );
    imSetData( m_im2, m_opImTmp1, origine4.x, origine4.y );
    ImageDestroy( &m_opImTmp1 );	
    if ( !SubRegister( origine4, subwindow, subsize4, level, row - 1, column ) )
    {
        imImageDestroy( buffer );
        return false;
    }

    imImageDestroy( buffer );
	return true;
}




