/////////////////////////////////////////////////////////////////////////////
// Name:        supfile.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_SUPERIMPOSITION

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "supfile.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"
#include "wx/filename.h"

#include "supfile.h"
#include "sup.h"
#include "im/impage.h"
//#include "im/imstaff.h"
//#include "im/imstaffsegment.h"

#include "app/axapp.h"
#include "app/axprocess.h"

// IMLIB
#include <im.h>
#include <im_counter.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_util.h>
#include <im_binfile.h>
#include <im_math_op.h>


// WDR: class implementations


/*
SupOldFile::SupOldFile( wxString path, wxString shortname, AxProgressDlg *dlg ):
	ImOperator( )
{
    m_im1 = NULL;
    m_im2 = NULL;
    m_im2_original = NULL;
    m_im_1_2 = NULL;
    m_im3 = NULL;
    m_s_im1 = NULL;
    m_s_im2 = NULL;
    m_new_im2 = NULL;
    m_selection = NULL;
	
	m_progressDlg = dlg;
}

SupOldFile::~SupOldFile()
{
}

bool SupOldFile::Terminate( int code,  ... )
{
    // Attention que deux de ces pointeurs ne refere pas la meme adresse lors de l'appel de cette methode !
    //ImageDestroy( &m_opImTmp1);
    ImageDestroy( &m_im1);
    ImageDestroy( &m_im2);
    ImageDestroy( &m_im2_original);
    ImageDestroy( &m_im_1_2);
    ImageDestroy( &m_im3);
    ImageDestroy( &m_s_im1);
    ImageDestroy( &m_s_im2);
    ImageDestroy( &m_new_im2);
    ImageDestroy( &m_selection);
		
	va_list argptr;
    va_start( argptr, code ); 
	return ImOperator::Terminate( code, argptr );

}
*/
/*
bool SupOldFile::Superimpose( const SupImController *imController1, 
        const SupImController *imController2, const wxString filename )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");

    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");

    if (!m_progressDlg->SetOperation( _("Preparation ...") ) )
        return this->Terminate( ERR_CANCELED );

	int counter = m_progressDlg->GetCounter();
    //int counter = imCounterBegin("Preparation");
    //imCounterTotal( counter, 1, "Preparation");
    //m_progressDlg->SetMaxProcessBar( 13 );
    //m_progressDlg->SetCounter( counter );

    wxPoint points1[4];
    wxPoint points2[4];
    points1[0] = imController1->m_points[0];
    points1[1] = imController1->m_points[1];
    points1[2] = imController1->m_points[2];
	points1[3] = imController1->m_points[3];
    points2[0] = imController2->m_points[0];
    points2[1] = imController2->m_points[1];
    points2[2] = imController2->m_points[2];
	points2[3] = imController2->m_points[3];

    // calculer la hauteur des segments et redimensionner
    int left1 = (int)sqrt( 
        pow( (double)(points1[0].x - points1[1].x) , 2) + 
        pow( (double)(points1[0].y - points1[1].y), 2));
    int right1 = (int)sqrt( 
        pow( (double)(points1[2].x - points1[3].x) , 2) + 
        pow( (double)(points1[2].y - points1[3].y), 2));
	int vmean1 = (left1 + right1) / 2; // moyenne des deux cotes
    //wxLogDebug(" segment 1 %d", segment1 );
    int left2 = (int)sqrt(
        pow( (double)(points2[0].x - points2[1].x), 2) + 
        pow( (double)(points2[0].y - points2[1].y), 2));
    int right2 = (int)sqrt(
        pow( (double)(points2[2].x - points2[3].x), 2) + 
        pow( (double)(points2[2].y - points2[3].y), 2));
	int vmean2 = (left2 + right2) / 2; // moyenne des deux cotes
    // facteur de redimensionnement
    float vfactor1 = (float)SupEnv::s_segmentSize / (float)vmean1;
    float vfactor2 = (float)SupEnv::s_segmentSize / (float)vmean2;
	
	
    // idem segments horizontaux
    int top1 = (int)sqrt( 
        pow( (double)(points1[0].x - points1[2].x) , 2) + 
        pow( (double)(points1[0].y - points1[2].y), 2));
    int bottom1 = (int)sqrt( 
        pow( (double)(points1[1].x - points1[3].x) , 2) + 
        pow( (double)(points1[1].y - points1[3].y), 2));
	int hmean1 = (top1 + bottom1) / 2; // moyenne des deux cotes
    //wxLogDebug(" segment 1 %d", segment1 );
    int top2 = (int)sqrt(
        pow( (double)(points2[0].x - points2[2].x), 2) + 
        pow( (double)(points2[0].y - points2[2].y), 2));
    int bottom2 = (int)sqrt(
        pow( (double)(points2[1].x - points2[3].x), 2) + 
        pow( (double)(points2[1].y - points2[3].y), 2));
	int hmean2 = (top2 + bottom2) / 2; // moyenne des deux cotes
    // facteur de redimensionnement, only for image 2 (the one for image 1 is identical to vfactor1
	float hfactor1 = vfactor1;
    float hfactor2 = hfactor1 * (float)hmean1 / (float)hmean2;
	wxLogDebug("vmean1 %d, hmean1 %d, vmean2 %d, hmean2 %d", vmean1, hmean1, vmean2, hmean2);
	wxLogDebug("vfactor1 %f, vfactor2 %f, hfactor1 %f, hfactor2 %f", vfactor1, vfactor2, hfactor1, hfactor2);

    if (!m_progressDlg->SetOperation( _("Preparartion of image 1 ...") ))
    {
        return this->Terminate( ERR_CANCELED );
    }
    m_im1 = GetImImage( imController1, ( IM_GRAY ));

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

    // ajuster la position des points
    points1[0].x = (int)(points1[0].x * hfactor1);
    points1[0].y = (int)(points1[0].y * vfactor1);
    points1[1].x = (int)(points1[1].x * hfactor1);
    points1[1].y = (int)(points1[1].y * vfactor1);
    points1[2].x = (int)(points1[2].x * hfactor1);
    points1[2].y = (int)(points1[2].y * vfactor1);
	points1[3].x = (int)(points1[3].x * hfactor1);
    points1[3].y = (int)(points1[3].y * vfactor1);


    if (!m_progressDlg->SetOperation( _("Preparation of image 2 ...") ))
        return this->Terminate( ERR_CANCELED );

    m_im2 = GetImImage( imController2, ( IM_GRAY ));

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

    points2[0].x = (int)(points2[0].x * hfactor2);
    points2[0].y = (int)(points2[0].y * vfactor2);
    points2[1].x = (int)(points2[1].x * hfactor2);
    points2[1].y = (int)(points2[1].y * vfactor2);
    points2[2].x = (int)(points2[2].x * hfactor2);
    points2[2].y = (int)(points2[2].y * vfactor2);
	points2[3].x = (int)(points2[3].x * hfactor2);
    points2[3].y = (int)(points2[3].y * vfactor2);

    // calculer les angles et pivoter
    int new_w, new_h;
    double cos0, sin0;

    // image 1
    // calculer l'angle
    if (!m_progressDlg->SetOperation( _("Rotation of image 1 ...") ))
        return this->Terminate( ERR_CANCELED );

    float left_alpha1 = float(points1[0].x - points1[1].x) / float(points1[0].y - points1[1].y);
    left_alpha1 = atan(left_alpha1);
	float right_alpha1 = float(points1[2].x - points1[3].x) / float(points1[2].y - points1[3].y);
    right_alpha1 = atan(right_alpha1);
	float alpha1 = (left_alpha1 + right_alpha1) / 2;

    sin0 = sin(-alpha1);
    cos0 = cos(-alpha1);
    imProcessCalcRotateSize( m_im1->width, m_im1->height, &new_w, &new_h, cos0, sin0 );
    // ajuster la position des points
    points1[0] = CalcPositionAfterRotation( points1[0], alpha1, m_im1->width, m_im1->height, new_w, new_h);
    points1[1] = CalcPositionAfterRotation( points1[1], alpha1, m_im1->width, m_im1->height, new_w, new_h);
    points1[2] = CalcPositionAfterRotation( points1[2], alpha1, m_im1->width, m_im1->height, new_w, new_h);

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

    float left_alpha2 = float(points2[0].x - points2[1].x) / float(points2[0].y - points2[1].y);
    left_alpha2 = atan( left_alpha2 );
    float right_alpha2 = float(points2[2].x - points2[3].x) / float(points2[2].y - points2[3].y);
    right_alpha2 = atan( right_alpha2 );
	float alpha2 = (left_alpha2 + right_alpha2) / 2;

    sin0 = sin(-alpha2);
    cos0 = cos(-alpha2);
    imProcessCalcRotateSize( m_im2->width, m_im2->height, &new_w, &new_h, cos0, sin0 );
    points2[0] = CalcPositionAfterRotation( points2[0], alpha2, m_im2->width, m_im2->height, new_w, new_h);
    points2[1] = CalcPositionAfterRotation( points2[1], alpha2, m_im2->width, m_im2->height, new_w, new_h);
    points2[2] = CalcPositionAfterRotation( points2[2], alpha2, m_im2->width, m_im2->height, new_w, new_h);

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

    //wxLogDebug("p1 %d %d; p2 %d %d; p3 %d %d", points1[0].x, points1[0].y, points1[1].x , points1[1].y , points1[2].x , points1[2].y);
    //wxLogDebug("p1 %d %d; p2 %d %d; p3 %d %d", points2[0].x, points2[0].y, points2[1].x , points2[1].y , points2[2].x , points2[2].y);

    // marges x
    int m1x1 = min( points1[0].x, points1[1].x );
    int m1x2 = min( points2[0].x, points2[1].x );
    int m2x1 = points1[2].x;
    int m2x2 = points2[2].x;
    // marges y
    int m1y1 = points1[0].y;
    int m1y2 = points2[0].y;
    int m2y1 = max( points1[1].y, points1[2].y );
    int m2y2 = max( points2[1].y, points2[2].y );

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
    m_im2_original = imImageDuplicate( m_im2 );
    if ( !m_im2_original )
        return this->Terminate( ERR_MEMORY );
    
    int x, y; // max;
    wxSize window(SupEnv::s_corr_x,SupEnv::s_corr_y);

    int s_w = width / SupEnv::s_split_x;
    int s_h = height / SupEnv::s_split_y;

    m_s_im1 = imImageCreate( s_w, s_h, m_im1->color_space, m_im1->data_type );
    if ( !m_s_im1 )
		return this->Terminate( ERR_MEMORY );

    m_s_im2 = imImageCreate( s_w, s_h, m_im2->color_space, m_im2->data_type );
    if ( !m_s_im2 )
        return this->Terminate( ERR_MEMORY );

    m_new_im2 = imImageCreate( m_im2->width + 2 * window.GetWidth(), m_im2->height + 2 * window.GetHeight(),
        m_im2->color_space, m_im2->data_type );
    if ( !m_new_im2 )
        return this->Terminate( ERR_MEMORY );

    memset( m_new_im2->data[0], 255, m_new_im2->size );
    if (m_im1->size < m_new_im2->size)
        imSetData( m_new_im2, m_im1, window.GetWidth(), window.GetHeight() );
 
    //int counter2 = imCounterBegin("Superposition");
    imCounterTotal( counter, SupEnv::s_split_y * SupEnv::s_split_x, "Superimpose");
    if (!m_progressDlg->SetOperation( _("Superimposition of the images ...") ) )
        return this->Terminate( ERR_CANCELED );
 
    for ( int s_y = 0; s_y < SupEnv::s_split_y; s_y++ )
    {
        for (int s_x = 0; s_x < SupEnv::s_split_x; s_x++ )
        {
            if ( !imCounterInc( counter ) )
            {
                m_progressDlg->ReactiveCounter( );
                return this->Terminate( ERR_CANCELED );
            }
            m_progressDlg->SuspendCounter( );

            imProcessCrop( m_im1, m_s_im1, origine.x + s_x * s_w, origine.y + s_y * s_h);
            imProcessCrop( m_im2, m_s_im2, origine.x + s_x * s_w, origine.y + s_y * s_h);
            
			/// !!!!!!! Won't work ....
			DistByCorrelation( m_s_im1, m_s_im2, window, &x, &y);
			//DistByCorrelation( m_s_im1, m_s_im2, window, &x, &y);
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
            m_selection = imImageCreate( selection_w, selection_h, m_im2->color_space, m_im2->data_type );
            if ( !m_selection )
                return this->Terminate( ERR_MEMORY );

            imProcessCrop( m_im2, m_selection, marge_x, marge_y);
            imSetData( m_new_im2, m_selection, 
                    window.GetWidth() + marge_x + x,
                    window.GetHeight() + marge_y + y);
            ImageDestroy( &m_selection );
            //wxLogDebug("Superposition %d %d", s_y * s_y, tot += wxGetElapsedTime( true ) );
            m_progressDlg->ReactiveCounter( );
        }
    }
    //imCounterEnd( counter );
    imProcessCrop( m_new_im2, m_im2, window.GetWidth(), window.GetHeight() );
    ImageDestroy( &m_new_im2 );
    ImageDestroy( &m_s_im1 );
    ImageDestroy( &m_s_im2 );

    if (!m_progressDlg->SetOperation( _("Writing image on disk ...") ) )
        return this->Terminate( ERR_CANCELED );

    m_im_1_2 = imImageCreate(im_width, im_height, IM_GRAY, IM_BYTE );
    if ( !m_im_1_2 )
        return this->Terminate( ERR_MEMORY );

    //imProcessBitwiseOp( im1, im2, im_1_2, IM_BIT_XOR );
    imProcessBitwiseOp( m_im1, m_im2, m_im_1_2, IM_BIT_AND );

    // superposition des 3 buffers
    m_im3 = imImageCreate(im_width, im_height, IM_RGB, IM_BYTE );
    if ( !m_im3 )
        return this->Terminate( ERR_MEMORY );

    memcpy( m_im3->data[0], m_im1->data[0], m_im1->size );
    memcpy( m_im3->data[1], m_im2->data[0], m_im2->size );
    memcpy( m_im3->data[2], m_im_1_2->data[0], m_im_1_2->size );
    //memset( im3->data[0], 255, im3->plane_size );
    //memset( im3->data[2], 255, im3->plane_size );
    ImageDestroy( &m_im_1_2 );

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
    return this->Terminate( ERR_NONE );
    //imCounterEnd(counter);
	
}
*/


// WDR: handler implementations for SupOldFile


//----------------------------------------------------------------------------
// SupFile
//----------------------------------------------------------------------------

SupFile::SupFile( wxString name, SupEnv *env )
	: AxFile( name, AX_FILE_DEFAULT, AX_FILE_RECOGNITION )
{
	m_envPtr = env;

	m_imPagePtr1 = NULL;
	m_imPagePtr2 = NULL;
	
	m_isSuperimposed = false;
}



SupFile::~SupFile()
{
	if ( m_imPagePtr1 )
		delete m_imPagePtr2;
	if ( m_imPagePtr1 )
		delete m_imPagePtr2;
}


void SupFile::NewContent( )
{
	wxASSERT_MSG( !m_imPagePtr1, "ImPage should be NULL" );
	wxASSERT_MSG( !m_imPagePtr2, "ImPage should be NULL" );
        
	// new ImPage and Load
    m_imPagePtr1 = new ImPage( m_basename, &m_isModified );
	m_imPagePtr2 = new ImPage( m_basename, &m_isModified );
	
	m_isSuperimposed = false;
}


void SupFile::OpenContent( )
{
	this->NewContent();
	
	// start
    bool failed = false;
	
	// saving has to be fixed before...
	/*
	failed = !m_imPagePtr1->Load( m_xml_root );
	if ( failed )
		return;

	failed = !m_imPagePtr2->Load( m_xml_root );
	if ( failed )
		return;
		
	// if both ImPage loaded, superimposed is true??
	*/	
	m_isSuperimposed = true;
	
	return;       
}


void SupFile::SaveContent( )
{
	wxASSERT_MSG( m_imPagePtr1, "ImPage1 should not be NULL" );
	wxASSERT_MSG( m_imPagePtr2, "ImPage2 should not be NULL" );
	wxASSERT( m_xml_root );
		
	if ( !m_isSuperimposed )
		return;
	
	// saving won't work like this as it is going to overwrite img1 and img2
	//m_imPagePtr1->Save( m_xml_root ); // save in the SupFile directory
	//m_imPagePtr2->Save( m_xml_root ); // save in the SupFile directory
}

void SupFile::CloseContent( )
{
	// nouveau fichier ?
    if ( m_imPagePtr1 )
    {
        delete m_imPagePtr1;
        m_imPagePtr1 = NULL;
    }

	// old ImPage
    if ( m_imPagePtr2 )
    {
        delete m_imPagePtr2;
        m_imPagePtr2 = NULL;
    }
	
	m_isSuperimposed = false;
}

// static
bool SupFile::IsSuperimposed( wxString filename )
{
	// don't know which file to check....
	//return !AxFile::GetPreview( filename, "page.wwg"  ).IsEmpty();
	return true;
}

// functors

bool SupFile::Superimpose( wxArrayPtrVoid params, AxProgressDlg *dlg )
{
	wxASSERT_MSG( dlg, "AxProgressDlg cannot be NULL" );
	
    // params 0: wxString: output_dir
	wxString image_file1 = *(wxString*)params[0];
	wxString image_file2 = *(wxString*)params[1];
	
	m_imPagePtr1->SetProgressDlg( dlg );
	m_imPagePtr2->SetProgressDlg( dlg );

    wxString ext, shortname1, shortname2;
    wxFileName::SplitPath( image_file1, NULL, &shortname1, &ext );
	wxFileName::SplitPath( image_file2, NULL, &shortname2, &ext );

    dlg->SetMaxJobBar( 17 );
    dlg->SetJob( shortname1 );
    wxYield();
		
    bool failed = false;
	/*
    if ( RecEnv::s_check && !failed ) 
        failed = !m_imPagePtr->Check( image_file, 2500 ); // 2 operations max
	
	//// TEST
	//m_error = m_imPagePtr->GetError();
	//this->m_isPreprocessed = true;
	//return true;
		
    if ( RecEnv::s_deskew && !failed ) 
        failed = !m_imPagePtr->Deskew( 2.0 ); // 2 operations max
    //op.m_inputfile = output + "/deskew." + shortname + ".tif";
        
    if ( RecEnv::s_staves_position && !failed ) 
        failed = !m_imPagePtr->FindStaves(  3, 50 );  // 4 operations max
    //op.m_inputfile = output + "/resize." + shortname + ".tif";

    if ( RecEnv::s_binarize_and_clean  && !failed ) 
        failed = !m_imPagePtr->BinarizeAndClean( );  // 3 operations max

    if ( RecEnv::s_find_borders && !failed ) 
        failed = !m_imPagePtr->FindBorders( );  // 1 operation max
    //op.m_inputfile = output + "/border." + shortname + ".tif";
        
    if ( RecEnv::s_find_ornate_letters && !failed ) 
        failed = !m_imPagePtr->FindOrnateLetters( ); // 1 operation max

    if ( RecEnv::s_find_text_in_staves && !failed ) 
        failed = !m_imPagePtr->FindTextInStaves( ); // 1 operation max

    if ( RecEnv::s_find_text && !failed ) 
        failed = !m_imPagePtr->FindText( ); // 1 operation max
		
	if (!failed)
		this->m_isPreprocessed = true;
			
	m_error = m_imPagePtr->GetError();
	*/
	return !failed;
}



wxPoint SupFile::CalcPositionAfterRotation( wxPoint point , float rot_alpha, 
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

// WDR: handler implementations for SupFile

#endif //AX_SUPERIMPOSITION

