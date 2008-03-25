/////////////////////////////////////////////////////////////////////////////
// Name:        imregister.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "imregister.h"
#endif

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

// WDR: class implementations

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
	
    if (!m_progressDlg->SetOperation( _("Checking the images ...") ) )
        return this->Terminate( ERR_CANCELED );

	m_imPage1Ptr->SetProgressDlg( m_progressDlg );
	
	if ( !m_imPage1Ptr->Check( filename1 ) )
		return false;

	m_imPage2Ptr->SetProgressDlg( m_progressDlg );
	
	if ( !m_imPage2Ptr->Check( filename2 ) )
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
	
	if ( !failed ) 
        failed = !m_imPage1Ptr->Deskew( 10.0 ); // 2 operations max
        
    if ( !failed ) 
        failed = !m_imPage1Ptr->FindStaves(  3, 50, false, false );  // 4 operations max
	
	if ( !failed ) 
        failed = !m_imPage2Ptr->Deskew( 10.0 ); // 2 operations max
        
    if ( !failed ) 
        failed = !m_imPage2Ptr->FindStaves(  3, 50, false, false );  // 4 operations max
		
	if ( failed )
		return this->Terminate( ERR_UNKNOWN );
		
	if ( (int)m_imPage1Ptr->m_staves.GetCount() != (int)m_imPage2Ptr->m_staves.GetCount() )
		wxLogWarning( "Staff detection did not retrieve the same number of staves of both images" );
		
	int staff_nb = min( (int)m_imPage1Ptr->m_staves.GetCount(), (int)m_imPage2Ptr->m_staves.GetCount() );
	
	int x1, x2;
	
	//m_imPage1Ptr->m_staves[0].GetMinMax( &x1, &x2 );
	points1[1] = wxPoint( m_imPage1Ptr->m_x1, m_imPage1Ptr->m_staves[0].m_y );
	points1[3] = wxPoint( m_imPage1Ptr->m_x2, m_imPage1Ptr->m_staves[0].m_y );
	m_imPage1Ptr->m_staves[staff_nb - 1].GetMinMax( &x1, &x2 );
	points1[0] = wxPoint( m_imPage1Ptr->m_x1, m_imPage1Ptr->m_staves[staff_nb - 1].m_y );
	points1[2] = wxPoint( m_imPage1Ptr->m_x2, m_imPage1Ptr->m_staves[staff_nb - 1].m_y );

	m_imPage2Ptr->m_staves[0].GetMinMax( &x1, &x2 );
	points2[1] = wxPoint( m_imPage2Ptr->m_x1, m_imPage2Ptr->m_staves[0].m_y );
	points2[3] = wxPoint( m_imPage2Ptr->m_x2, m_imPage2Ptr->m_staves[0].m_y );
	m_imPage2Ptr->m_staves[staff_nb - 1].GetMinMax( &x1, &x2 );
	points2[0] = wxPoint( m_imPage2Ptr->m_x1, m_imPage2Ptr->m_staves[staff_nb - 1].m_y );
	points2[2] = wxPoint( m_imPage2Ptr->m_x2, m_imPage2Ptr->m_staves[staff_nb - 1].m_y );
	
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
	
	return this->Terminate( ERR_NONE );	
}


bool ImRegister::Register( wxPoint *points1, wxPoint *points2)
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_src1, "Src1 cannot be NULL");
	wxASSERT_MSG( m_src2, "Src2 cannot be NULL");

    if (!m_progressDlg->SetOperation( _("Registration ...") ) )
        return this->Terminate( ERR_CANCELED );

	int counter = m_progressDlg->GetCounter();

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
	//float vfactor1 = (float)SupEnv::s_segmentSize / (float)vmean1;
	float vfactor2 = vfactor1 *(float)vmean1 / (float)vmean2;
    //float vfactor2 = (float)SupEnv::s_segmentSize / (float)vmean2;
	
	
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
	wxLogDebug("vmean1 %d, hmean1 %d, vmean2 %d, hmean2 %d", vmean1, hmean1, vmean2, hmean2);
	wxLogDebug("vfactor1 %f, vfactor2 %f, hfactor1 %f, hfactor2 %f", vfactor1, vfactor2, hfactor1, hfactor2);

    if (!m_progressDlg->SetOperation( _("Preparartion of image 1 ...") ))
        return this->Terminate( ERR_CANCELED );

    m_im1 = imImageDuplicate( m_src1 );
    if ( !m_im1 )
        return this->Terminate( ERR_MEMORY );

    // median filtering
    if (SupEnv::s_filter1)
    {
        if ( !m_progressDlg->SetOperation( _("Filtering image ...") ))
            return this->Terminate( ERR_CANCELED );

        m_opImTmp1 = imImageClone( m_im1 );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );

        if ( !imProcessMedianConvolve( m_im1 ,m_opImTmp1, 3 ) )
            return this->Terminate( ERR_CANCELED );

        SwapImages( &m_im1, &m_opImTmp1 );

    }
    // resize
    if (!m_progressDlg->SetOperation( _("Resizing image ...") ))
        return this->Terminate( ERR_CANCELED );

    m_opImTmp1 = imImageCreate(  (int)(m_im1->width  * hfactor1), (int)(m_im1->height  * vfactor1),
            m_im1->color_space, m_im1->data_type);
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    if ( !imProcessResize( m_im1 ,m_opImTmp1, SupEnv::s_interpolation) )
        return this->Terminate( ERR_CANCELED );

    SwapImages( &m_im1, &m_opImTmp1 );

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
        if (!m_progressDlg->SetOperation( _("Filtering image ...") ))
            return this->Terminate( ERR_CANCELED );

        m_opImTmp1 = imImageClone(  m_im2 );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
   
        if ( !imProcessMedianConvolve( m_im2 ,m_opImTmp1, 3 ) )
            return this->Terminate( ERR_CANCELED );

        SwapImages( &m_im2, &m_opImTmp1 );
    }
    // resize
    if (!m_progressDlg->SetOperation( _("Resizing image ...") ))
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
    int m1x1 = min( m_reg_points1[0].x, m_reg_points1[1].x );
    int m1x2 = min( m_reg_points2[0].x, m_reg_points2[1].x );
    int m2x1 = m_reg_points1[2].x;
    int m2x2 = m_reg_points2[2].x;
    // marges y
    int m1y1 = m_reg_points1[0].y;
    int m1y2 = m_reg_points2[0].y;
    int m2y1 = max( m_reg_points1[1].y, m_reg_points1[2].y );
    int m2y2 = max( m_reg_points2[1].y, m_reg_points2[2].y );

    // min de crop des 2 images
    int minx1 = (m1x1 > m1x2 ) ? m1x1 - m1x2 : 0;
    int minx2 = (m1x2 > m1x1 ) ? m1x2 - m1x1 : 0;
    int miny1 = (m1y1 > m1y2 ) ? m1y1 - m1y2 : 0;
    int miny2 = (m1y2 > m1y1 ) ? m1y2 - m1y1 : 0;

    // largeur et hauteur de la zone de superposition + point d'orgine
    int width = min( ( m2x1 - m1x1 ), ( m2x2 - m1x2 ) );
    int height = min( ( m2y1 - m1y1 ), ( m2y2 - m1y2 ) );
    wxPoint origine( min ( m1x1, m1x2 ), min ( m1y1, m1y2 ) );

    // largeur et hauteur des nouvelles images (zone de superposition + marge minimale )
    int im_width = origine.x + width + min( m_im1->width - m1x1 - width, m_im2->width - m1x2 - width );
    int im_height = origine.y + height + min( m_im1->height - m1y1 - height, m_im2->height - m1y2 - height );
    
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
    
    int x, y, maxCorr;
    wxSize window(SupEnv::s_corr_x,SupEnv::s_corr_y);

    int s_w = width / SupEnv::s_split_x;
    int s_h = height / SupEnv::s_split_y;

    m_opImTmp1 = imImageCreate( s_w, s_h, m_im1->color_space, m_im1->data_type );
    if ( !m_opImTmp1 )
		return this->Terminate( ERR_MEMORY );

    m_opImTmp2 = imImageCreate( s_w, s_h, m_im2->color_space, m_im2->data_type );
    if ( !m_opImTmp2 )
        return this->Terminate( ERR_MEMORY );

    m_opImAlign = imImageCreate( m_im2->width + 2 * window.GetWidth(), m_im2->height + 2 * window.GetHeight(),
        m_im2->color_space, m_im2->data_type );
    if ( !m_opImAlign )
        return this->Terminate( ERR_MEMORY );

    memset( m_opImAlign->data[0], 255, m_opImAlign->size );
    if (m_im1->size < m_opImAlign->size)
        imSetData( m_opImAlign, m_im1, window.GetWidth(), window.GetHeight() );
 
    //int counter2 = imCounterBegin("Superposition");
    imCounterTotal( counter, SupEnv::s_split_y * SupEnv::s_split_x, "Superimpose");
    if (!m_progressDlg->SetOperation( _("Superimposition of the images ...") ) )
        return this->Terminate( ERR_CANCELED );
 
    for ( int s_y = 0; s_y < SupEnv::s_split_y; s_y++ )
    {
        for (int s_x = 0; s_x < SupEnv::s_split_x; s_x++ )
        {
            if ( !imCounterInc( counter ) || m_progressDlg->GetCanceled() )
            {
                m_progressDlg->ReactiveCounter( );
                return this->Terminate( ERR_CANCELED );
            }
            m_progressDlg->SuspendCounter( );

            imProcessCrop( m_im1, m_opImTmp1, origine.x + s_x * s_w, origine.y + s_y * s_h);
			//imProcessThreshold( m_opImTmp1, m_opImTmp1, 127, 255 );
            imProcessCrop( m_im2, m_opImTmp2, origine.x + s_x * s_w, origine.y + s_y * s_h);
			//imProcessThreshold( m_opImTmp2, m_opImTmp2, 127, 255 );
            
			DistByCorrelation( m_opImTmp1, m_opImTmp2, window, &x, &y, &maxCorr );
            wxLogDebug( "Correlation decalage (position %d-%d) %d-%d", s_x, s_y, x, y );
            int marge_x = origine.x + s_x * s_w;
            int marge_y = origine.y + s_y * s_h;
            int selection_w = s_w;
            int selection_h = s_h;
            if (s_x == 0)
            {
                marge_x = 0;
                selection_w += origine.x;
            }
            if (s_y == 0)
            {
                marge_y = 0;
                selection_h += origine.y;
            }
            if (s_x == SupEnv::s_split_x - 1)
            {
                selection_w = m_im2->width - marge_x;
            }
            if (s_y == SupEnv::s_split_y - 1)
            {
                selection_h = m_im2->height - marge_y;
            }   
            m_opImMask = imImageCreate( selection_w, selection_h, m_im2->color_space, m_im2->data_type );
            if ( !m_opImMask )
                return this->Terminate( ERR_MEMORY );

            imProcessCrop( m_im2, m_opImMask, marge_x, marge_y);
            imSetData( m_opImAlign, m_opImMask, 
                    window.GetWidth() + marge_x + x,
                    window.GetHeight() + marge_y + y);
            ImageDestroy( &m_opImMask );
            //wxLogDebug("Superposition %d %d", s_y * s_y, tot += wxGetElapsedTime( true ) );
            m_progressDlg->ReactiveCounter( );
        }
    }
    //imCounterEnd( counter );
    imProcessCrop( m_opImAlign, m_im2, window.GetWidth(), window.GetHeight() );
    ImageDestroy( &m_opImAlign );
    ImageDestroy( &m_opImTmp1 );
    ImageDestroy( &m_opImTmp2 );

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
	
	/*	
    wxFile file( filename, wxFile::write ); // necessaire sous Linux pour les masques de fichiers
    file.Close();

    int error;
    imFile* ifile = NULL;
    wxString desc;
    ifile = imFileNew( filename.c_str(), "TIFF", &error);
    if (error == IM_ERR_NONE)
    {
        m_opImTmp1 = imImageClone( m_im3 );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );

        imProcessFlip( m_im3, m_opImTmp1 );
        SwapImages( &m_im3, &m_opImTmp1 );
        imImageSetAttribute( m_im3, "Software", IM_BYTE, 8, "Aruspix" );
        imImageSetAttribute( m_im3, "Author", IM_BYTE, 14, "Laurent Pugin" );
		imFileSetInfo( ifile, "RLE" ); // LZW Unisys par defaut
        imFileSaveImage( ifile, m_im3);

        // original 2 -> necessaire pour visualiser sans les coupures de correlation
        m_opImTmp1 = imImageClone( m_im2_original );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );

        imProcessFlip( m_im2_original, m_opImTmp1 );
        SwapImages( &m_im2_original, &m_opImTmp1 );
        imImageSetAttribute( m_im2_original, "Software", IM_BYTE, 8, "Aruspix" );
        imImageSetAttribute( m_im2_original, "Author", IM_BYTE, 14, "Laurent Pugin" );
        imFileSaveImage( ifile, m_im2_original);
    }
    imFileClose( ifile );
	*/
	
	if ( m_isModified ) 
		*m_isModified = true;
	
    return this->Terminate( ERR_NONE );	
}





// WDR: handler implementations for ImRegister

