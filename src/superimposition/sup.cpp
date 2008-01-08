/////////////////////////////////////////////////////////////////////////////
// Name:        sup.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_SUPERIMPOSITION

#ifdef __GNUG__
    #pragma implementation "sup.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/config.h"
#include "wx/valgen.h"
#include "wx/file.h"

#include "sup.h"
#include "app/axapp.h"
#include "app/axprogressdlg.h"
#include "app/axframe.h"

// IMLIB
#include <im.h>
#include <im_counter.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_util.h>
#include <im_binfile.h>
#include <im_math_op.h>



// statics
int SupEnv::s_segmentSize = 1200;
int SupEnv::s_split_x = 4;
int SupEnv::s_split_y = 8;
int SupEnv::s_corr_x = 30;
int SupEnv::s_corr_y = 50;
int SupEnv::s_interpolation = 0;
bool SupEnv::s_filter1 = true;
bool SupEnv::s_filter2 = true;

/*
#define ERR_NONE 0
#define ERR_CANCELED 1
#define ERR_MEMORY 2
*/

// WDR: class implementations

//----------------------------------------------------------------------------
// SupFile
//----------------------------------------------------------------------------


SupFile::SupFile( wxString path, wxString shortname, ProgressDlg *dlg ):
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

SupFile::~SupFile()
{
}

/*
void SupFile::SwapImages( _imImage **image1, _imImage **image2 )
{
    ImageDestroy( image1 );
    *image1 = *image2;
    *image2 = NULL;
}

void SupFile::ImageDestroy( _imImage **image )
{
    if ( *image )
    {
        imImageDestroy( *image );
        *image = NULL;
    }

}
*/

bool SupFile::Terminate( int code,  ... )
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

    /*if ( code == ERR_CANCELED )
        wxLogWarning(_("Operation canceled") );
    else if ( code == ERR_MEMORY )
        wxLogWarning(_("Insufficient memory") );
    m_error = code;*/
}


bool SupFile::Superimpose( const SupImController *imController1, 
        const SupImController *imController2, const wxString filename )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");

    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");

    if (!m_progressDlg->SetOperation( _("Preparation ...") ) )
        return this->Terminate( ERR_CANCELED );

	int counter = m_progressDlg->GetCounter();
    /*int counter = imCounterBegin("Preparation");
    imCounterTotal( counter, 1, "Preparation");
    m_progressDlg->SetMaxProcessBar( 13 );
    m_progressDlg->SetCounter( counter );*/

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


/*
void SupFile::DistByCorrelationFFT(const AxImage image1, const AxImage image2,
                                wxSize window, int *decalageX, int *decalageY)*/
/*
void SupFile::DistByCorrelationFFT(const _imImage *im1, const _imImage *im2,
                                wxSize window, int *decalageX, int *decalageY)
{
    wxASSERT_MSG(decalageX, wxT("decalageX cannot be NULL") );
    wxASSERT_MSG(decalageY, wxT("decalagY cannot be NULL") );
    wxASSERT_MSG(im1, wxT("Image 1 cannot be NULL") );
    wxASSERT_MSG(im2, wxT("Image 2 cannot be NULL") );

    
    //imImage *im1 = GetImImage(&image1, IM_GRAY);
    //imImage *im2 = GetImImage(&image2, IM_GRAY);

    imImage *corr = imImageCreate( im1->width, im1->height, im1->color_space, IM_CFLOAT);
    imProcessCrossCorrelation( im1, im2, corr );
    imImage *corrCrop = imImageCreate( window.GetWidth() * 2 + 1, window.GetHeight() * 2 + 1,
        corr->color_space, IM_CFLOAT );
    int xmin = im1->width / 2 - window.GetWidth();
    int ymin = im1->height / 2 - window.GetHeight();
    imProcessCrop( corr, corrCrop, xmin, ymin );

    imImage *corrReal = imImageCreate( corrCrop->width , corrCrop->height , corrCrop->color_space, IM_BYTE );
    imConvertDataType( corrCrop, corrReal, IM_CPX_MAG, IM_GAMMA_LINEAR, 0, IM_CAST_MINMAX);

    int width = corrReal->width;
    int height = corrReal->height;
    int max = 0, maxX = 0, maxY = 0;
    imbyte *buf = (imbyte*)corrReal->data[0];

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if ( buf[y * width + x] > max )
            {
                max = buf[y * width + x];
                maxX = x;
                maxY = y;

            }
        }
    }

    *decalageX = maxX - window.GetWidth();
    *decalageY = maxY - window.GetHeight();

    //int error;
    //imFile* ifile = NULL;
    //ifile = imFileNew("D:/Mes Images/corr1.tif", "TIFF", &error);
    //imFileSaveImage(ifile,corrReal);
    //imFileClose(ifile);

    imImageDestroy( corrReal );
    imImageDestroy( corrCrop );
    imImageDestroy( corr );
}
*/

/*
void SupFile::DistByCorrelation(const AxImage image1, const AxImage image2,
                                wxSize window, int *decalageX, int *decalageY)*/

void SupFile::DistByCorrelation(_imImage *im1, _imImage *im2,
                                wxSize window, int *decalageX, int *decalageY)
{
    wxASSERT_MSG(decalageX, wxT("decalageX cannot be NULL") );
    wxASSERT_MSG(decalageY, wxT("decalagY cannot be NULL") );
    wxASSERT_MSG(im1, wxT("Image 1 cannot be NULL") );
    wxASSERT_MSG(im2, wxT("Image 2 cannot be NULL") );

    
	imProcessNegative( im1, im1 );
	imProcessNegative( im2, im2 );
    //imImage *im1 = GetImImage(&image1, IM_GRAY);
    //imImage *im2 = GetImImage(&image2, IM_GRAY);

    // zero padding
    //{
        imImage *imTmp1 = imImageCreate(
            im1->width +  window.GetWidth() * 2,
            im1->height +  window.GetHeight() * 2,
            im1->color_space, im1->data_type);
        imProcessAddMargins(im1 ,imTmp1, window.GetWidth(), window.GetHeight());
        //imImageDestroy(im1);
        //im1 = imTmp1;
    //}


    int conv_width = 2 * window.GetWidth();
    int conv_height = 2 * window.GetHeight();
    //imImage *conv = imImageCreate(conv_width, conv_height, IM_GRAY, IM_BYTE);
    imImage *mask = imImageCreate(im2->width, im2->height,im2->color_space, im2->data_type);
    imbyte *bufIm2 = (imbyte*)im2->data[0];
    //imbyte *bufConv = (imbyte*)conv->data[0];
    //int offset;
    int maxSum = 0, maxX = 0, maxY = 0;
    for (int y = 0; y < conv_height; y++)
    {
        for (int x = 0; x < conv_width; x++)
        {
            imProcessCrop(imTmp1,mask, x, y);
            imbyte *bufMask = (imbyte*)mask->data[0]; 
            int sum = 0;
            for (int i = 0; i < mask->plane_size; i++)
            {
                sum += (bufIm2[i] / 255) * (bufMask[i] / 255);
            }
            //offset = y * conv_width + x;
            //bufConv[offset] = sum;
            if (sum > maxSum)
            {
                maxSum = sum;
                maxX = x;
                maxY = y;
            }
        }
    }

    *decalageX = maxX - window.GetWidth();
    *decalageY = maxY - window.GetHeight();
    imImageDestroy(imTmp1);
    //imImageDestroy(im2);

	//int error;
    //imFile* ifile = imFileNew("D:/test.tif", "TIFF", &error);
    //imFileSaveImage(ifile,conv);
    //imFileClose(ifile);

    //imImageDestroy(conv);
    imImageDestroy(mask);
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


//----------------------------------------------------------------------------
// SupImController
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(SupImController,AxImageController)

// WDR: event table for SupImController

BEGIN_EVENT_TABLE(SupImController,AxImageController)
END_EVENT_TABLE()

SupImController::SupImController( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style , int flags) :
    AxImageController( parent, id, position, size, style, flags )
{
    m_selectCounter = 0;
}

SupImController::SupImController()
{
}

SupImController::~SupImController()
{
}

void SupImController::CloseDraggingSelection(wxPoint start, wxPoint end)
{
    m_points[m_selectCounter] = end;

    if (m_selectCounter == 0)
    {
        m_selectCounter++;
		//wxLogDebug("point 1");
    }
    else if (m_selectCounter == 1)
    {
        m_selectCounter++;
		//wxLogDebug("point 2");
    }
    else if (m_selectCounter == 2)
    {
        m_selectCounter++;
		//wxLogDebug("point 3");
    }
    else if (m_selectCounter == 3)
    {
        m_selectCounter = 0;
		//wxLogDebug("point 4");
        this->m_viewPtr->EndSelection();
    }
}


// WDR: handler implementations for SupImController


//----------------------------------------------------------------------------
// SupPanel
//----------------------------------------------------------------------------

// WDR: event table for SupPanel

BEGIN_EVENT_TABLE(SupPanel,wxPanel)
END_EVENT_TABLE()

SupPanel::SupPanel( wxWindow *parent, wxWindowID id,
    const wxPoint &position, const wxSize& size, long style ) :
    wxPanel( parent, id, position, size, style )
{
    WindowFunc2( this, TRUE ); 
}

// WDR: handler implementations for SupPanel


//----------------------------------------------------------------------------
// SupEnv
//----------------------------------------------------------------------------

// WDR: event table for SupEnv

BEGIN_EVENT_TABLE(SupEnv,AxEnv)
	// copy paste 
    EVT_MENU( ID_PASTE, SupEnv::OnPaste )
    EVT_MENU( ID_COPY, SupEnv::OnCopy )
    EVT_MENU( ID_CUT, SupEnv::OnCut )
	EVT_UPDATE_UI( ID_PASTE , SupEnv::OnUpdateUI )
	EVT_UPDATE_UI( ID_COPY , SupEnv::OnUpdateUI )
	EVT_UPDATE_UI( ID_CUT , SupEnv::OnUpdateUI )
	// other
    EVT_MENU( ID2_NEXT, SupEnv::OnNextBoth )
    EVT_MENU( ID2_PREVIOUS, SupEnv::OnPreviousBoth )
    EVT_MENU( ID2_OPEN1, SupEnv::OnOpen )
    EVT_MENU( ID2_OPEN2, SupEnv::OnOpen )
    EVT_MENU( ID2_POINTS, SupEnv::OnPutPoints )
    EVT_MENU( ID2_SUPERIMPOSE, SupEnv::OnSuperimpose )
    EVT_UPDATE_UI_RANGE( wxID_LOWEST, ENV_IDS_MAX, SupEnv::OnUpdateUI )
    EVT_MENU( ID_CLOSE, SupEnv::OnClose )
    EVT_MENU( ID2_GOTO1, SupEnv::OnGoto )
    EVT_MENU( ID2_GOTO2, SupEnv::OnGoto )
    EVT_MENU( ID2_PREVIOUS1, SupEnv::OnPrevious )
    EVT_MENU( ID2_PREVIOUS2, SupEnv::OnPrevious )
    EVT_MENU( ID2_NEXT1, SupEnv::OnNext )
    EVT_MENU( ID2_NEXT2, SupEnv::OnNext )
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(SupEnv,AxEnv)

SupEnv::SupEnv():
    AxEnv()
{
    m_imControl1Ptr = NULL;
    m_imControl2Ptr = NULL;
    m_view1Ptr = NULL;
    m_view2Ptr = NULL;

    this->m_envMenuBarFuncPtr = MenuBarFunc2;
}

SupEnv::~SupEnv()
{
    if (m_envWindowPtr) m_envWindowPtr->Destroy();
    m_envWindowPtr = NULL;
}

void SupEnv::LoadWindow()
{
    this->m_envWindowPtr = new SupPanel(m_framePtr,-1);
    if (!m_envWindowPtr)
        return;

    SupPanel *panel = ((SupPanel*)m_envWindowPtr);
    if (!panel)
        return;

    wxSplitterWindow *splitter = panel->GetSplitter1();
    if (!splitter)
        return;

    splitter->SetWindowStyleFlag( wxSP_FULLSASH );
    splitter->SetMinimumPaneSize( 100 );

    m_imControl1Ptr = new SupImController( splitter, ID2_CONTROLLER1  );
    m_view1Ptr = new AxScrolledWindow( m_imControl1Ptr, ID2_VIEW1 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_view1Ptr->m_popupMenu.AppendSeparator();
    m_view1Ptr->m_popupMenu.Append(ID2_POINTS,_("Put points"));
    m_imControl1Ptr->Init( this, m_view1Ptr );


    m_imControl2Ptr = new SupImController( splitter, ID2_CONTROLLER2 );
    m_view2Ptr = new AxScrolledWindow( m_imControl2Ptr, ID2_VIEW2 , wxDefaultPosition, 
        wxDefaultSize, wxHSCROLL| wxVSCROLL | wxSUNKEN_BORDER);
    m_view2Ptr->m_popupMenu.AppendSeparator();
    m_view2Ptr->m_popupMenu.Append(ID2_POINTS,_("Put points"));
    m_imControl2Ptr->Init( this, m_view2Ptr );

    splitter->SplitVertically(m_imControl1Ptr,m_imControl2Ptr);
}

void SupEnv::LoadConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Superimposition");

    // superimposition
    SupEnv::s_segmentSize = pConfig->Read("SegmentSize", 1200);
    SupEnv::s_interpolation = pConfig->Read("Interpolation", 0L);
    SupEnv::s_split_x = pConfig->Read("SplitX", 4);
    SupEnv::s_split_y = pConfig->Read("SplitY", 8);
    SupEnv::s_corr_x = pConfig->Read("CorrX", 30);
    SupEnv::s_corr_y = pConfig->Read("CorrY", 50);
    SupEnv::s_filter1 = (pConfig->Read("Filter1",1)==1);
    SupEnv::s_filter2 = (pConfig->Read("Filter2",1)==1);

    pConfig->SetPath("/");
}



void SupEnv::SaveConfig()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    wxASSERT_MSG( pConfig, wxT("pConfig cannot be NULL") );
    pConfig->SetPath("/Superimposition");

    // superimposition
    pConfig->Write("SegmentSize", SupEnv::s_segmentSize);
    pConfig->Write("Interpolation", SupEnv::s_interpolation);
    pConfig->Write("SplitX", SupEnv::s_split_x);
    pConfig->Write("SplitY", SupEnv::s_split_y);
    pConfig->Write("CorrX", SupEnv::s_corr_x);
    pConfig->Write("CorrY", SupEnv::s_corr_y);
    pConfig->Write("Filter1", SupEnv::s_filter1);
    pConfig->Write("Filter2", SupEnv::s_filter2);

    pConfig->SetPath("/");
}

void SupEnv::RealizeToolbar( )
{
	ToolBarFunc2( m_framePtr->GetToolBar() ); // function generated by wxDesigner
}

void SupEnv::ParseCmd( wxCmdLineParser *parser )
{
    wxASSERT_MSG( parser, wxT("Parser cannot be NULL") );
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( parser->Parse( false ) == 0) 
    {
        if ( parser->GetParamCount() > 1 )
            m_imControl1Ptr->Open( parser->GetParam (1) );
        if ( parser->GetParamCount() > 2 )
            m_imControl2Ptr->Open( parser->GetParam (2) );
    }
}

void SupEnv::UpdateTitle( )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxString msg;
    if ( m_imControl1Ptr->Ok() )
    {
        msg += m_imControl1Ptr->GetTitleMsg().c_str();
    }

    if ( m_imControl2Ptr->Ok() )
    {
        msg += ( msg.IsEmpty() ) ? "" : " - ";
        msg += m_imControl2Ptr->GetTitleMsg().c_str();
    }

    SetTitle( "%s", msg.c_str() );
}


// WDR: handler implementations for SupEnv

void SupEnv::OnPrevious( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( (event.GetId() == ID2_PREVIOUS1) && m_imControl1Ptr->HasPrevious() )
        m_imControl1Ptr->Previous();
    else if ((event.GetId() == ID2_PREVIOUS2) && m_imControl2Ptr->HasPrevious() )
        m_imControl2Ptr->Previous();      
}

void SupEnv::OnNext( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( (event.GetId() == ID2_NEXT1) && m_imControl1Ptr->HasNext() )
        m_imControl1Ptr->Next();
    else if ((event.GetId() == ID2_NEXT2) && m_imControl2Ptr->HasNext() )
        m_imControl2Ptr->Next();    
}

void SupEnv::OnGoto( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( (event.GetId() == ID2_GOTO1) && m_imControl1Ptr->CanGoto() )
        m_imControl1Ptr->Goto();
    else if ((event.GetId() == ID2_GOTO2) && m_imControl2Ptr->CanGoto() )
        m_imControl2Ptr->Goto();      
}

void SupEnv::OnClose( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( m_imControl1Ptr->Ok() )
        m_imControl1Ptr->Close();
    if ( m_imControl2Ptr->Ok() )
        m_imControl2Ptr->Close();   
}

void SupEnv::OnNextBoth( wxCommandEvent &event )
{ 
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if (!m_imControl1Ptr->HasNext() || !m_imControl2Ptr->HasNext())
        return;

    m_imControl1Ptr->Next();
    m_imControl2Ptr->Next();
}

void SupEnv::OnPreviousBoth( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if (!m_imControl1Ptr->HasPrevious() || !m_imControl2Ptr->HasPrevious())
        return;

    m_imControl1Ptr->Previous();
    m_imControl2Ptr->Previous();
}

void SupEnv::OnPutPoints( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( m_imControl1Ptr->Ok() )
    {
        m_view1Ptr->BeginSelection( SHAPE_POINT );
    }
    if ( m_imControl2Ptr->Ok() )
    {
        m_view2Ptr->BeginSelection( SHAPE_POINT );
    }
}

void SupEnv::OnSuperimpose( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if ( !m_imControl1Ptr->Ok() || !m_imControl2Ptr->Ok() )
        return;

    wxString filename = wxFileSelector( _("Save"), wxGetApp().m_lastDirTIFF_out , _T(""), NULL, IMAGE_FILES, wxSAVE );
    if (filename.IsEmpty())
        return;
	wxGetApp().m_lastDirTIFF_out = wxPathOnly( filename );

    // debug
    /*m_imControl1Ptr->m_points[0] = wxPoint(26,40);
    m_imControl1Ptr->m_points[1] = wxPoint(26,840);
    m_imControl1Ptr->m_points[2] = wxPoint(580,840);
    m_imControl2Ptr->m_points[0] = wxPoint(26,40);
    m_imControl2Ptr->m_points[1] = wxPoint(26,840);
    m_imControl2Ptr->m_points[2] = wxPoint(580,840);*/


    ProgressDlg *dlg = new ProgressDlg( m_framePtr, -1, _("Superposition") );
    dlg->Center( wxBOTH );
    dlg->Show();
    dlg->SetMaxBatchBar( 1 );
//    dlg->SetBatchBar( "Superposition (1/1)", 0 );
    wxYield();

    SupFile sp( "", "", dlg );
    if ( sp.Superimpose( m_imControl1Ptr, m_imControl2Ptr, filename  ))

    imCounterEnd( dlg->GetCounter() );
    dlg->GetGaugeBatch()->SetValue( 100 );
#ifdef __WXMAC__
	dlg->Show(false);  // otherwise doesn't stop	
#endif	
    dlg->ShowModal( ); // stop process
    dlg->Destroy();
    
    if ( sp.GetError()!= ERR_NONE ) // operation annulee ou failed
        return;

    int res = wxMessageBox("Visualiser le resultat de la superposition?", wxGetApp().GetAppName() ,
                            wxYES | wxNO | wxICON_QUESTION );
    if (res != wxYES)
        return;

    static const wxCmdLineEntryDesc cmdLineDesc[] = {
        {wxCMD_LINE_PARAM,  NULL, NULL, _("input files"),
         wxCMD_LINE_VAL_STRING,
         wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE},
        {wxCMD_LINE_NONE}
    };

    wxCmdLineParser parser( cmdLineDesc );
    parser.SetCmdLine("cmd \"" + filename + "\"");
    ((AxFrame*)m_framePtr)->SetEnvironment( "DspEnv" );
    ((AxFrame*)m_framePtr)->ParseCmd( &parser );
}

void SupEnv::OnPaste( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID2_VIEW1)
        m_imControl1Ptr->Paste();
    else if (win->GetId() == ID2_VIEW2)
        m_imControl2Ptr->Paste();
}

void SupEnv::OnCopy( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID2_VIEW1)
        m_imControl1Ptr->Copy();
    else if (win->GetId() == ID2_VIEW2)
        m_imControl2Ptr->Copy();
}

void SupEnv::OnCut( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    if (win->GetId() == ID2_VIEW1)
        m_imControl1Ptr->Cut();
    else if (win->GetId() == ID2_VIEW2)
        m_imControl2Ptr->Cut();
}



void SupEnv::OnOpen( wxCommandEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    if (event.GetId() == ID2_OPEN1)
        m_imControl1Ptr->Open();
    else if (event.GetId() == ID2_OPEN2)
        m_imControl2Ptr->Open();
}

void SupEnv::OnUpdateUI( wxUpdateUIEvent &event )
{
    wxASSERT_MSG( m_imControl1Ptr, wxT("Image controller 1 cannot be NULL") );
    wxASSERT_MSG( m_imControl2Ptr, wxT("Image controller 2 cannot be NULL") );

    wxWindow* win = wxWindow::FindFocus();
    if (!win) 
        return;

    int id = event.GetId();

    if (id == ID_CUT)
    {
        if (win->GetId() == ID2_VIEW1)
            event.Enable( m_imControl1Ptr->CanCut() );
        else if (win->GetId() == ID2_VIEW2)
            event.Enable( m_imControl2Ptr->CanCut() );
        else
            event.Enable(false);
    }
    else if (id == ID_COPY)
    {
        if (win->GetId() == ID2_VIEW1)
            event.Enable( m_imControl1Ptr->CanCopy() );
        else if (win->GetId() == ID2_VIEW2)
            event.Enable( m_imControl2Ptr->CanCopy() );
        else
            event.Enable(false);
    }
    else if (id == ID_PASTE)
    {
        if (win->GetId() == ID2_VIEW1)
            event.Enable( m_imControl1Ptr->CanPaste() );
        else if (win->GetId() == ID2_VIEW2)
            event.Enable( m_imControl2Ptr->CanPaste() );
        else
            event.Enable(false);
    }
    else if (id == ID2_POINTS)
    {
        event.Enable( m_imControl1Ptr->Ok() && m_imControl2Ptr->Ok() );
    }
    else if (id == ID2_SUPERIMPOSE)
    {
        event.Enable( m_imControl1Ptr->Ok() && m_imControl2Ptr->Ok() );
    }
    else if (id == ID2_NEXT)
    {
        event.Enable( m_imControl1Ptr->HasNext() && m_imControl2Ptr->HasNext() );
    }
    else if (id == ID2_NEXT1)
    {
        event.Enable( m_imControl1Ptr->HasNext());
    }
    else if (id == ID2_NEXT2)
    {
        event.Enable( m_imControl2Ptr->HasNext());
    }
    else if (id == ID2_PREVIOUS)
    {
        event.Enable( m_imControl1Ptr->HasPrevious() && m_imControl2Ptr->HasPrevious() );
    }
    else if (id == ID2_PREVIOUS1)
    {
        event.Enable( m_imControl1Ptr->HasPrevious() );
    }
    else if (id == ID2_PREVIOUS2)
    {
        event.Enable( m_imControl2Ptr->HasPrevious());
    }
    else if (id == ID2_GOTO1)
    {
        event.Enable( m_imControl1Ptr->CanGoto() );
    }
    else if (id == ID2_GOTO2)
    {
        event.Enable( m_imControl2Ptr->CanGoto());
    }
    else
        event.Enable(true);
}

#endif // AX_SUPERIMPOSITION

