/////////////////////////////////////////////////////////////////////////////
// Name:        impage.cpp
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "impage.h"
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

#include "impage.h"
#include "imstaff.h"
#include "imstaffsegment.h"

#include <math.h>

#include "app/axapp.h"

#include "recognition/rec.h" // not optimal, should be restructured...

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfStaves );

WX_DEFINE_OBJARRAY( ArrayOfRLE );

int SortRLE( ImRLE **first, ImRLE **second )
{
    if ( (*first)->val < (*second)->val )
        return -1;
    else if ( (*first)->val > (*second)->val )
        return 1;
    else
        return 0;
}

// from  tiff.h, not very good but I don't want to include the file....
#define      PHOTOMETRIC_MINISWHITE      0       /* min value is white */
#define      PHOTOMETRIC_MINISBLACK      1       /* min value is black */
#define      PHOTOMETRIC_RGB             2       /* RGB color model */

#define DESKEW_FACTOR 4
#define RESIZE_FACTOR 2
#define TIP_FACTOR_1 3

#define TP_STAFF_ROI_H 120 // hauteur de la zone des elements appartenant à la portee
#define TP_MARGIN_MIN 10 // marge minimal entre la portee et un element texte
#define TP_MARGIN_Y1 20 // marge inferieure depuis le centroid du texte
#define TP_MARGIN_Y2 35 // marge superieure depuis le centroid du texte
#define TP_MARGIN_Y_TITLE 35 // marge entre la portee superieur et le elements de titre
#define TP_MAX_SMALL_ELEMENT 100

//Static variables that hold selected binarization method
int ImPage::s_pre_page_binarization_method = PRE_BINARIZATION_BRINK;
int ImPage::s_pre_page_binarization_method_size = 15;
bool ImPage::s_pre_page_binarization_select = true;
int ImPage::s_num_staff_lines = 5;


//----------------------------------------------------------------------------
// ImPage
//----------------------------------------------------------------------------

ImPage::ImPage( wxString path, bool *isModified ) :
    ImOperator( ), AxUndo( 10 )
{	
	m_path = path;
	m_img0 = NULL;
	m_img1 = NULL;
	m_selection = NULL;
	m_isModified = 	isModified;
	
	m_pre_page_binarization_methodPtr = &ImPage::s_pre_page_binarization_method;
	m_pre_page_binarization_method_sizePtr = &ImPage::s_pre_page_binarization_method_size;
	
    Clear( );
}

ImPage::~ImPage()
{
	if ( m_img0 )
		ImageDestroy( &m_img0 );
	if ( m_img1 )
		ImageDestroy( &m_img1 );
	if ( m_selection )
		ImageDestroy( &m_selection );
}

void ImPage::Clear( )
{
    m_reduction = 1;
    m_skew = 0.0;
    m_resize = 1.0;
	m_resized = 1.0;
    m_optimization_resize_factor = 1.0;
    m_line_width = 0;
    m_space_width = 0;
    m_x1 = 0;
    m_x2 = 0;
    m_y1 = 0;
    m_original_width = 0;
    m_original_height = 0;
    m_rotated_width = 0;
    m_rotated_height = 0;
	m_staff_height = 0;
	m_num_staff_lines = ImPage::s_num_staff_lines;
	
	ImageDestroy( &m_img0 );
	ImageDestroy( &m_img1 );
	ImageDestroy( &m_selection );
    
    m_staves.Clear( );
}

bool ImPage::Load( TiXmlElement *file_root )
{
	if ( !file_root )
		return false;

	this->Clear();
	bool failed = false;

    if ( !failed && wxFileExists( m_path + "img0.tif" ) )
		failed = !Read( m_path + "img0.tif", &m_img0, 0 );

    if ( !failed && wxFileExists( m_path + "img1.tif" ) )
		failed = !Read( m_path + "img1.tif", &m_img1, 0 );

	
    //TiXmlDocument dom( ( m_path + "img0.xml" ) .c_str() );
	
	//if ( !failed )
	//	failed = !dom.LoadFile();

    //if ( failed )
    //    return false;

    TiXmlElement *root = NULL;
    TiXmlNode *node = NULL;
    TiXmlElement *elem = NULL;

    //root = dom.RootElement();
    //if ( !root ) return false;
	
	node = file_root->FirstChild( "impage" );
	if ( !node ) return false;
		
	root = node->ToElement();
    if ( !root ) return false;

    if ( root->Attribute("original_width"))
        m_original_width = atoi(root->Attribute("original_width"));
        
    if ( root->Attribute("original_height"))
        m_original_height = atoi(root->Attribute("original_height"));
        
    if ( root->Attribute("rotated_width"))
        m_rotated_width = atoi(root->Attribute("rotated_width"));
        
    if ( root->Attribute("rotated_height"))
        m_rotated_height = atoi(root->Attribute("rotated_height"));

    if ( root->Attribute("skew"))
        m_skew = atof(root->Attribute("skew"));
	
	// needed ???
    if ( root->Attribute("resized"))
        m_resized = atof(root->Attribute("resized"));

    if ( root->Attribute("x2"))
        m_x2 = atoi(root->Attribute("x2"));

    if ( root->Attribute("x1"))
        m_x1 = atoi(root->Attribute("x1"));

    if ( root->Attribute("y1"))
        m_y1 = atoi(root->Attribute("y1"));

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
	
	if ( root->Attribute("num_staff_lines")) {
		m_num_staff_lines = atoi(root->Attribute("num_staff_lines"));
	}

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
    
    return true;
}

bool ImPage::Save( TiXmlElement *file_root )
{
	bool failed = false;

    if ( !failed && m_img0 )
		failed = !WriteAsMAP( m_path + "img0.tif", &m_img0 );
	
    //if ( !failed && m_img0 )
	//	failed = !WriteAsMAP( m_path + "img3.tif", &m_img0 );
		
    if ( !failed && m_img1 )
		failed = !Write( m_path + "img1.tif", &m_img1 );	
	
	
	//TiXmlDocument dom( (m_path + "img0.xml").c_str() );

    wxString tmp;

    TiXmlElement root("impage");

    tmp = wxString::Format("%d", m_original_width );
    root.SetAttribute( "original_width", tmp.c_str() );
    
    tmp = wxString::Format("%d", m_original_height );
    root.SetAttribute( "original_height", tmp.c_str() );
    
    tmp = wxString::Format("%d", m_rotated_width );
    root.SetAttribute( "rotated_width", tmp.c_str() );
    
    tmp = wxString::Format("%d", m_rotated_height );
    root.SetAttribute( "rotated_height", tmp.c_str() );
    
    tmp = wxString::Format("%d", m_x1 );
    root.SetAttribute( "x1", tmp.c_str() );

    tmp = wxString::Format("%f", m_skew );
    root.SetAttribute( "skew", tmp.c_str() );
	
    tmp = wxString::Format("%f", m_resized );
    root.SetAttribute( "resized", tmp.c_str() );

    tmp = wxString::Format("%d", m_x1 );
    root.SetAttribute( "x1", tmp.c_str() );

    tmp = wxString::Format("%d", m_x2 );
    root.SetAttribute( "x2", tmp.c_str() );

    tmp = wxString::Format("%d", m_y1 );
    root.SetAttribute( "y1", tmp.c_str() );
    
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
	
	tmp = wxString::Format("%d", m_num_staff_lines );
	root.SetAttribute( "num_staff_lines", tmp.c_str());

    for( int i = 0; i < (int)m_staves.GetCount() ; i++)
    {
        TiXmlElement elem ("staff");
        m_staves[i].Save( &elem );
        root.InsertEndChild( elem );
    }

	if ( file_root )
	    file_root->InsertEndChild( root );

    //if ( !failed )
	//	failed = !dom.SaveFile();

    return !failed;
}


// undo
void ImPage::Load( AxUndoFile *undoPtr )
{
	if ( undoPtr->m_flags == IM_UNDO_CLASSIFICATION )
	{
		wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
		
		wxFile input( undoPtr->GetFilename().c_str(), wxFile::read );
		if ( input.IsOpened() )
		{
			int w, h, s;
			input.Read( &m_selection_pos.x, sizeof( int ) );
			input.Read( &m_selection_pos.y, sizeof( int ) );
			input.Read( &w, sizeof( int ) );
			input.Read( &h, sizeof( int ) );
			input.Read( &s, sizeof( int ) );

			imImage *imTmp = imImageCreate( w, h, m_img0->color_space, m_img0->data_type );
			if ( !imTmp )
				return;
				
			wxASSERT_MSG( s == imTmp->size, "Undo Load: Size of undos buffer mismatch");	
				
			input.Read( *imTmp->data, imTmp->size );
	
			imProcessInsert( m_img0, imTmp, m_img0, m_selection_pos.x, m_selection_pos.y ); 

			ImageDestroy( &imTmp );
		}
	}
}


void ImPage::Store( AxUndoFile *undoPtr )
{
	if ( undoPtr->m_flags == IM_UNDO_CLASSIFICATION )
	{
		wxASSERT_MSG( m_opImTmp2 , wxT("Store: m_opImTmp2 cannot be NULL") );
		
		wxFile output( undoPtr->GetFilename().c_str(), wxFile::write );
		if ( output.IsOpened() )
		{
			output.Write( &m_selection_pos.x, sizeof( int ) );
			output.Write( &m_selection_pos.y, sizeof( int ) );
			output.Write( &m_opImTmp2->width, sizeof( int ) );
			output.Write( &m_opImTmp2->height, sizeof( int ) );
			output.Write( &m_opImTmp2->size, sizeof( int ) );
			output.Write( *m_opImTmp2->data, m_opImTmp2->size );
		}
	}
}



void ImPage::CalcLeftRight( int *x1, int *x2)
{
    int nb = (int)m_staves.GetCount();
    if (nb == 0)
        return;

    int i;

    ImStaff *imStaff;
    imStaff = &m_staves[0];
    imStaff->GetMinMax(x1, x2);

    int minx = *x1, maxx = *x2;
    for (i = 1; i < nb; i++)
    {
        imStaff = &m_staves[i];
        imStaff->GetMinMax(&minx, &maxx);
        if (minx < *x1)
            *x1 = minx;
        if (maxx > *x2)
            *x2 = maxx;
    }
    return;
}


bool ImPage::Check( wxString infile, int max_size, int min_size, int index )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");

    if (!m_progressDlg->SetOperation( _("Checking the image ...") ) )
        return this->Terminate( ERR_CANCELED );

    if ( !Read( infile, &m_opImMain, index ) )
        return false;
        
    this->m_original_width = m_opImMain->width;
    this->m_original_height = m_opImMain->height;
    this->m_rotated_width = m_opImMain->width;
    this->m_rotated_height = m_opImMain->height;
        
    // resize
    // the biggest side of the image is reduced to max_size when bigger than max_size (and max_side != -1)
    // the biggest side of the image is expanded to min_size when smaller 
    float reduce = max( (float)m_opImMain->width / (float)max_size, (float)m_opImMain->height / (float)max_size );
    float expand = min( (float)min_size / (float)m_opImMain->width, (float)min_size / (float)m_opImMain->height );
    // trick: both reduce and expand are > 1.0 when true because the fraction above is reverse
    if ( (expand > 1 ) || (reduce > 1) )
    {
        /* USE THIS FROM 1.6.3 ON */
        m_optimization_resize_factor = (expand > reduce) ? 1.0 / expand : reduce;
    
        if (!m_progressDlg->SetOperation( _("Resizing image ...") ))
            return this->Terminate( ERR_CANCELED );

        m_opImTmp1 = imImageCreate(  (int)(m_opImMain->width  / m_optimization_resize_factor), (int)(m_opImMain->height  / m_optimization_resize_factor),
            m_opImMain->color_space, m_opImMain->data_type);
    
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );

        if ( !imProcessResize( m_opImMain ,m_opImTmp1, 1) )
            return this->Terminate( ERR_CANCELED );
        
        SwapImages( &m_opImMain, &m_opImTmp1 );
        
        
        /* THIS FOR 1.6.2 ONLY */
        /*
        float resize_factor = (expand > reduce) ? 1.0 / expand : reduce;
	   
        if (!m_progressDlg->SetOperation( _("Resizing image ...") ))
           return this->Terminate( ERR_CANCELED );
	
         m_opImTmp1 = imImageCreate(  (int)(m_opImMain->width  / resize_factor), (int)(m_opImMain->height  / resize_factor),
	            m_opImMain->color_space, m_opImMain->data_type);
	   
        if ( !m_opImTmp1 )
           return this->Terminate( ERR_MEMORY );
	
        if ( !imProcessResize( m_opImMain ,m_opImTmp1, 1) )
	            return this->Terminate( ERR_CANCELED );
       
         SwapImages( &m_opImMain, &m_opImTmp1 );
         */
        
    }

    // verifier que l'image est de type IM_BYTE
    if ( m_opImMain->data_type != IM_BYTE )
    {
        m_opImTmp1 = imImageCreate( m_opImMain->width, m_opImMain->height, m_opImMain->color_space, IM_BYTE);
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
        imConvertDataType( m_opImMain, m_opImTmp1,0,0,0,0);
        SwapImages( &m_opImMain, &m_opImTmp1 );
    }

    // verifier que l'image n'est pas RGB
    if ( imColorModeMatch( m_opImMain->color_space, IM_RGB ) )
    {
        m_opImTmp1 = imImageCreate( m_opImMain->width, m_opImMain->height, IM_GRAY, IM_BYTE);
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
        imConvertColorSpace( m_opImMain, m_opImTmp1 );
        SwapImages( &m_opImMain, &m_opImTmp1 );
    }

    // convertir les images niveaux de gris en binaires 
    if ( imColorModeMatch( m_opImMain->color_space, IM_BINARY ) )
    {
        m_opImTmp1 = imImageCreate( m_opImMain->width, m_opImMain->height , IM_GRAY, IM_BYTE);
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
        imConvertColorSpace( m_opImMain, m_opImTmp1 );
        SwapImages( &m_opImMain, &m_opImTmp1 );

        // appliquer un filtre moyenneur
        if ( !m_progressDlg->SetOperation( _("Filtering (binary image) ...") ) )
            return this->Terminate( ERR_CANCELED );
        m_opImTmp1 = imImageClone( m_opImMain );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
        if ( !imProcessMeanConvolve( m_opImMain ,m_opImTmp1, 3 ) )
            return this->Terminate( ERR_CANCELED );
        SwapImages( &m_opImMain, &m_opImTmp1 );
    }

    // verifier negatif - positif : looking at the TIFF tag
	int error;
	bool minIsBlack = true;
	imFile* ifile = imFileOpen( infile.c_str(), &error);
	if (!ifile) 
		return this->Terminate( ERR_FILE , infile.c_str());
	
	int attrib_data_type, attrib_count;
	imFileReadImageInfo( ifile, 0, NULL, NULL, NULL, NULL);
	imFileGetAttributeList( ifile, NULL, &attrib_count);
	const void* attrib_data = imFileGetAttribute( ifile, "Photometric", &attrib_data_type, &attrib_count);
	if ( attrib_data && (attrib_count == 1) )
    {
		if ( (int)(*((imushort*)attrib_data)) == PHOTOMETRIC_MINISWHITE )
			minIsBlack = false;
	}

	imStats istats;
	imCalcImageStatistics( m_opImMain, &istats );
    if ( AxImage::s_checkIfNegative )
		minIsBlack = (istats.mean > 127) ? true : false;
	else if ( (istats.mean > 127) && !minIsBlack )
		wxLogWarning( _("Image is negative according to the mean, check the option in 'Preferences' to have it corrected") );
	
	if ( minIsBlack )
	{
		m_opImTmp1 = imImageClone( m_opImMain );
		if ( !m_opImTmp1 )
			return this->Terminate( ERR_MEMORY );
		imProcessNegative( m_opImMain, m_opImTmp1 );
		SwapImages( &m_opImMain, &m_opImTmp1 );
    }

	// historgramme (debug)
	//unsigned long histo[256];
	//imbyte *bufIm = (imbyte*)m_opImMain->data[0];
	//imCalcHistogram( bufIm, m_opImMain->plane_size, histo, 0 );
    //wxString im_histo = m_path + "im_histo.csv";
	//imSaveValues( (int*)histo, 256, im_histo.c_str() );
	
	///// TEST
    //m_opImTmp1 = imImageCreate( m_opImMain->width, m_opImMain->height, IM_BINARY, IM_BYTE );
    //if ( !m_opImTmp1 )
    //    return this->Terminate( ERR_MEMORY );
	//if ( !m_progressDlg->SetOperation( _("Binarization ...") ) )
	//	return this->Terminate( ERR_CANCELED );
	//wxLogMessage("Pugin binarization");
	//imProcessPuginThreshold( m_opImMain, m_opImTmp1, false );
	//SwapImages( &m_opImMain, &m_opImTmp1 );
	//////
    
    //ImageDestroy( &m_img0 );
	/*m_img0 = imImageClone( m_opImMain );
	imPhotogrammetric( m_opImMain, m_img0 );
    SwapImages( &m_opImMain, &m_img0 );*/
	//imPhotogrammetric( m_img1, m_img1 );
	

    // save file
	SwapImages( &m_img0, &m_opImMain );
	if ( m_isModified ) 
		*m_isModified = true;
        
	return this->Terminate( ERR_NONE );

}

bool ImPage::Deskew( double max_alpha )
{	
	wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
	
    if (!m_progressDlg->SetOperation( _("Skew detection ...") ) )
        return this->Terminate( ERR_CANCELED );

	this->SetMapImage( m_img0 );

    if ( !GetImage( &m_opIm, DESKEW_FACTOR ) )
        return false;

    imStats istats;
    imCalcImageStatistics( m_opIm, &istats );
    // binary images - forcer la lecture en niveau de gris -> verifier la palette
    //if ( imColorModeMatch( file_color_mode, IM_BINARY ) )
    if ( (m_opIm->palette_count == 2) || (istats.max == 1) )
    {
        m_opImTmp1 = imImageCreate( m_opIm->width, m_opIm->height , IM_GRAY, IM_BYTE);
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
        imConvertColorSpace( m_opIm, m_opImTmp1 );
        SwapImages( &m_opIm, &m_opImTmp1 );
    }
    
    // detection de l'inclinaison
    int counter = m_progressDlg->GetCounter();
    int count = ((int)max_alpha / 2 + 11 ) * m_opIm->height;
        // count = nb d'approximation : 5x à 0.25 + 5x à 1 + Xx à 4 selon max_alpha
        // counter incremente  par ligne dans la methode GetAlignement
    imCounterTotal( counter, count , "Skew detection ..." );

    // max alignment
    int max, align;
    double diff;
    double skew = 0.0;
    double skew1 = 0.0;
    double skew2 = 0.0;
	double skew3 = 0.0;

    // approximation par 4 degres ( nb de fois determine par le parametre max_alpha )
    max = 0;
    for ( diff = -max_alpha; diff <= max_alpha; diff += 4 )
    {
        align = GetDeskewAlignement( m_opIm, diff );
        if ( align > max )
        {
            skew3 = diff;
            max = align;
        }
    }

    // approximation par 1 degre ( 5x )
    max = 0;
    for ( diff = skew3 - 2.0; diff <= skew3 + 2.0; diff += 1 )
    {
        align = GetDeskewAlignement( m_opIm, diff );
        if ( align > max )
        {
            skew2 = diff;
            max = align;
        }
    }

    // approximation par 0.25 degre ( 5x )
    max = 0;
    for ( diff = skew2 - 0.5; diff <= skew2 + 0.5; diff += 0.25 )
    {
        align = GetDeskewAlignement( m_opIm, diff );
        if ( align > max )
        {
            skew1 = diff;
            max = align;
        }
    }
	
    // approximation par 0.0625 degre ( 5x )
    max = 0;
    for ( diff = skew1 - 0.125; diff <= skew1 + 0.125; diff += 0.0625 )
    {
        align = GetDeskewAlignement( m_opIm, diff );
        if ( align > max )
        {
            skew = diff;
            max = align;
        }
    }

    wxLogMessage("Skew %.5f", skew  );
    //ImageDestroy( &m_opImAlign );

    if ( skew != 0.0 )
    {
        if (!m_progressDlg->SetOperation( _("Deskew ...") ) )
            return this->Terminate( ERR_CANCELED );

        int new_w, new_h;
        double cos0, sin0;
        sin0 = sin( deg2rad( skew ) );
        cos0 = cos( deg2rad( skew ) );
        
        imProcessCalcRotateSize( m_opImMap->width, m_opImMap->height, 
                              &new_w, &new_h, cos0, sin0);
                              
        imProcessCalcRotateSize( this->m_original_width, this->m_original_height, 
                              &this->m_rotated_width, &this->m_rotated_height, cos0, sin0);
                              
        //new_w = m_opImMap->width;
        //new_h = m_opImMap->height;

        m_opImTmp1 = imImageCreate( new_w, new_h, m_opImMap->color_space, m_opImMap->data_type);
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );

        if ( !imProcessRotate( m_opImMap, m_opImTmp1, cos0, sin0, 1 ) )
            return this->Terminate( ERR_CANCELED );

        SwapImages( &m_opImMap, &m_opImTmp1 );

        this->m_skew = skew;
    }

    // save file
	SwapImages( &m_img0, &m_opImMap );
	if ( m_isModified ) 
		*m_isModified = true;
	return this->Terminate( ERR_NONE );
}


int ImPage::GetDeskewAlignement( _imImage *image, double alpha )
{
    wxASSERT_MSG( image, "Image cannot be NULL");

    double alpha_rad = deg2rad( alpha );
    imbyte *bufIm = (imbyte*)image->data[0];
    int h = image->height;
    int w = image->width;
    double xx, yy;
    int *hist = new int[h];
    int hist_val;

    int reduce = 64; // divise offset value to reduce maximum value (avoid overflow)
    if ( imColorModeMatch( image->color_space, IM_BINARY ) )
        reduce = 0;

    int x, y, offset;
    double cos_alpha = cos( alpha_rad );
    double sin_alpha = sin( alpha_rad );
    for (y = 0; y < h; y++)
    {
        hist_val = 0;
        for (x = 0; x < w; x++)
        {   
            xx = cos_alpha * x - sin_alpha * y;
            yy = sin_alpha * x + cos_alpha * y;
            xx = ceil( xx );
            yy = ceil( yy );
            if ( ( xx >= 0 ) && ( xx < w ) && ( yy >= 0 ) && ( yy < h ) )
            {
                offset = (int)(xx + yy * w);
                if ( reduce )
                    hist_val += -(bufIm[ offset ] - 255) / reduce;
                else if ( !bufIm[ offset ] )
                    hist_val++;

            }
        }
        hist[y] =  hist_val;

        if (!imCounterInc( this->m_progressDlg->GetCounter() ) )
        {
            delete hist;
            return this->Terminate( ERR_CANCELED );
        }
    }

    hist_val = 0;
    for (y = 0; y < h - 1; y++)
        hist_val += (int)pow( hist[y] - hist[y + 1], 2 ) / 100; // reduction par 100 - risque d'overflow !!!

    delete[] hist;
    
    return hist_val;
}


bool ImPage::FindStaves( int min, int max, bool normalize, bool crop )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");

    int resize_factor = RESIZE_FACTOR;
    if ( this->m_reduction == 1 )
        resize_factor = 1;
    
    if (!m_progressDlg->SetOperation( _("Binarization for staff detection ...") ) )
        return this->Terminate( ERR_CANCELED );

	this->SetMapImage( m_img0 );

    if ( !GetImage( &m_opIm, resize_factor, *m_pre_image_binarization_methodPtr, true ) )
        return false;

    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    imImageClear( m_opImTmp1 );

    ArrayOfRLE ImRLE_spaces;  // run length des espaces blancs
    ImRLE_spaces.Alloc( m_opIm->height * m_opIm->width / 10 ); // 10% de l'image
    ArrayOfRLE ImRLE_lines; // run length des lignes
    ImRLE_lines.Alloc( m_opIm->height * m_opIm->width / 20 ); // 5% de l'image


    imbyte *bufIm = (imbyte*)m_opIm->data[0];
    imbyte *bufImTmp = (imbyte*)m_opImTmp1->data[0];
    int h = m_opIm->height;
    int w = m_opIm->width;
    min = h * min / 1000; // parameters in 0/00
    max = h * max / 1000;
    int* maxRLE = new int[max]; // tableau du nombre de runs ; index = valeur du run ( 0 à max - 1)
    memset( maxRLE, 0, max * sizeof(int) );

    if (!m_progressDlg->SetOperation( _("Detecting staff size ...") ) )
        return this->Terminate( ERR_CANCELED );
    int counter = m_progressDlg->GetCounter();
    int count = w + max;
    imCounterTotal( counter, count , "Detecting staff size ..." );

    int x, y;

    ImRLE run;
    for (x = 0; x < w; x++)
    {
        run.type = 0;
        run.x = x;
        run.y = 0;
        run.val = 0;
        for (y = 0; y < h; y++)
        {
            int offset = y * w + x;
            if ( bufIm[ offset ] == run.type )
                run.val++;
            else // changement
            {
                run.y = offset;
                if ( ( run.type == 0 ) && ( run.val > min ) && ( run.val < max ) )
                // run blanc dans la fourchette min / max
                {
                    ImRLE_spaces.Add( run ); // garder le run (avec la valeur de l'offset dans y
                    maxRLE[ run.val ] += 1; // nombre de runs de cette valeur
                }
                run.type = ( run.type == 1 ) ? 0 : 1 ;
                run.val = 0;
            }
        }
        if ( !imCounterInc( counter ) )
            return this->Terminate( ERR_CANCELED );
    }

    if ( ImRLE_spaces.IsEmpty() )
        return this->Terminate( ERR_UNKNOWN );

    int val, sum, max_run, max_val, i, j;

    val = 0;
    sum = 0;
    max_run = 0;
    max_val = 0;

    // trouver la valeur du run le plus nombreux = espace entre portees
    for ( i = 0; i < max; i++ )
    {
        if ( maxRLE[i] > max_run )
        {
            max_run = maxRLE[i];
            max_val = i;
        }
        if ( !imCounterInc( counter ) )
            return this->Terminate( ERR_CANCELED );
    }

    //wxString rle_histo = m_path + "rle_histo.csv";
	//imSaveValues( maxRLE, max, rle_histo.c_str() );

    int space = max_val;
    delete[] maxRLE;


    // draw runs spaces in image
    if (!m_progressDlg->SetOperation( _("Detecting space between staff lines ...") ) )
        return this->Terminate( ERR_CANCELED );
    count = (int)ImRLE_spaces.GetCount();
    imCounterTotal( counter, count , "Detecting space between staff lines ..." );

    int err_margin = space * 15 / 100; // 15 % de marge d'erreur
    for(i = 0; i < (int)ImRLE_spaces.GetCount(); i++)
    {
        if ( ( ImRLE_spaces[i].val >= space - err_margin) &&  ( ImRLE_spaces[i].val <= space  + err_margin) )
        {
            for (j = 0; j < ImRLE_spaces[i].val; j++ )
            {
                int offset = ImRLE_spaces[i].y - j * w;
                    bufImTmp[ offset ] = 1;
            }
        }
        if ( !imCounterInc( counter ) )
            return this->Terminate( ERR_CANCELED );
    }
    SwapImages( &m_opIm, &m_opImTmp1 );

    //wxString rle = m_path + "rle.tif";
    //if ( !Write( rle, &m_opIm ) )
    //    return false;

    // convolve pour allonger le run
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    imImage* kernel = imImageCreate( 7, 1, IM_GRAY, IM_INT);
    imImageSetAttribute(kernel, "Description", IM_BYTE, -1, (void*)"Erode");

    int* kernel_data = (int*)kernel->data[0];
    for(i = 0; i < kernel->count; i++)
        kernel_data[i] = 0;

    if ( !m_progressDlg->SetOperation( _("Sharpen staff lines ...") ) )
        return this->Terminate( ERR_CANCELED );
    imProcessBinMorphConvolve( m_opIm, m_opImTmp1, kernel, 0, 1);
    imImageDestroy(kernel);
    SwapImages( &m_opIm, &m_opImTmp1 );

    //wxString rle_conv = m_path + "rle_conv.tif";
    //if ( !Write( rle_conv, &m_opIm ) )
    //    return false;

    // calculer l'epaisseur des lignes de portees - procede identique
    count = w;
    if ( !m_progressDlg->SetOperation( _("Detecting staff line width ...") ) )
        return this->Terminate( ERR_CANCELED );
    imCounterTotal( counter, count , "Detecting staff line width ..." );

    // lines
    maxRLE = new int[space];
    memset( maxRLE, 0, space * sizeof(int) );

    bufIm = (imbyte*)m_opIm->data[0];
    for (x = 0; x < w; x++)
    {
        run.type = 0;
        run.x = x;
        run.y = 0;
        run.val = 0;
        for (y = 0; y < h; y++)
        {
            int offset = y * w + x;
            if ( bufIm[ offset ] == run.type )
                run.val++;
            else // changement
            {
                run.y = offset;
                if ( ( run.type == 0 ) && ( run.val < space ) )
                {
                    ImRLE_lines.Add( run );
                    maxRLE[ run.val ] += 1;
                }
                run.type = ( run.type == 1 ) ? 0 : 1 ;
                run.val = 0;
            }
        }
        if ( !imCounterInc( counter ) )
            return this->Terminate( ERR_CANCELED );
    }

    val = 0;
    sum = 0;
    max_run = 0;
    max_val = 0;

    if ( !ImRLE_lines.IsEmpty() )
    {

        for ( i = 0; i < space; i++ )
        {
            if ( maxRLE[i] > max_run )
            {
                max_run = maxRLE[i];
                max_val = i;
            }
        }
    }
    int line = max_val;
    delete[] maxRLE;


    // garder les valeurs
    this->m_line_width = line * resize_factor;
    this->m_space_width = space * resize_factor;

	int num_spaces = ImPage::s_num_staff_lines - 1;
	// We count 1 more line than there actually is
	int num_lines = ImPage::s_num_staff_lines + 1;
    double normalization_factor = 100.0 / (double)(num_spaces * this->m_space_width + num_lines * this->m_line_width) ; 
        // 6 lines to correct approximation error ( empiric ! )
    double factor = (double)resize_factor / (double)RESIZE_FACTOR; 
        // toujours 2 une fois que l'image à ete normalisee
        // EX si 4 avant normalisation, image des staves X2
	this->m_resize = normalization_factor;

    wxLogMessage("Line width=%d; Space beetween lines=%d", this->m_line_width, this->m_space_width );
	
	// binary images
	//if ( imColorModeMatch( file_color_mode, IM_BINARY ) )
	if ( m_opImMap->palette_count == 2 )
	{
		m_opImTmp1 = imImageCreate( m_opImMap->width, m_opImMap->height , IM_GRAY, IM_BYTE);
		if ( !m_opImTmp1 )
			return this->Terminate( ERR_MEMORY );

		imConvertColorSpace( m_opImMap, m_opImTmp1 );
		SwapImages( &m_opImMap, &m_opImTmp1 );
	}
	
	// resize main image
    if ( normalize && (normalization_factor != 1.0) )
    {
        if (!m_progressDlg->SetOperation( _("Normalize image size ...") ) )
            return this->Terminate( ERR_CANCELED );

		wxLogMessage("Normalization factor %f", normalization_factor / m_optimization_resize_factor );

        // resize
        m_opImTmp1 = imImageCreate( (int)(m_opImMap->width * normalization_factor), (int)(m_opImMap->height * normalization_factor), 
        m_opImMap->color_space, m_opImMap->data_type );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );

        imProcessResize( m_opImMap, m_opImTmp1, 1 );
        SwapImages( &m_opImMap, &m_opImTmp1 );
		
		this->m_resized = this->m_resize / this->m_optimization_resize_factor;
		this->m_resize = 1.0;
    }

    // resize image convolved -> keep for staves position detection
    m_opImTmp1 = imImageCreate( 
        (int)(m_opIm->width / STAVES_CONV_REDUCTION * factor * normalization_factor),
        (int)(m_opIm->height / STAVES_CONV_REDUCTION  * factor * normalization_factor), 
        m_opIm->color_space, m_opIm->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    imProcessResize( m_opIm, m_opImTmp1, 0 );
    SwapImages( &m_opIm, &m_opImTmp1 );

    if ( !m_progressDlg->SetOperation( _("Detect staves position ...") ) )
        return this->Terminate( ERR_CANCELED );

    int f_width, avg;
    int width = m_opIm->width;
    int height = m_opIm->height;
    // projection verticale
    m_opHist = new int[ width ];
    imAnalyzeProjectionV( m_opIm, m_opHist );

    //wxString staves_v = m_path + "staves_v.csv";
	//imSaveValues( m_opHist, width, staves_v.c_str() );

    f_width = 100 / RESIZE_FACTOR / 4; // 25 % de la hauteur de la portee
    MedianFilter( m_opHist, width, f_width, &avg );

    //wxString staves_v_med = m_path + "staves_v_med.csv";
	//imSaveValues( m_opHist, width, staves_v_med.c_str() );

    int x1, x2;
    GetHorizontalStavesPosition( m_opHist, width, avg, &x1, &x2 );
    this->m_x1 = x1 * RESIZE_FACTOR * STAVES_CONV_REDUCTION / this->m_resize;
    this->m_x2 = x2 * RESIZE_FACTOR * STAVES_CONV_REDUCTION / this->m_resize;
    wxLogMessage("Borders position %d %d", this->m_x1, this->m_x2 );
    
    delete[] m_opHist;
    m_opHist = NULL;

    // projection horizontales
    m_opHist = new int[ height ];
    imAnalyzeProjectionH( m_opIm, m_opHist );

    //wxString staves_h = m_path + "staves_h.csv";
	//imSaveValues( m_opHist, height, staves_h.c_str() );

    f_width = 100 / RESIZE_FACTOR / 10; // 10 % de la hauteur de la portee
    MedianFilter( m_opHist, height, f_width, &avg );

    //wxString staves_h_med = m_path + "staves_h_med.csv";
	//imSaveValues( m_opHist, height, staves_h_med.c_str() );

    m_opLines1 = new int[MAX_STAVES];
    int nb_staves;
    GetVerticalStavesPosition( m_opHist, height, avg, m_opLines1, &nb_staves );
    //wxLogMessage("%d - %d", m_page_x1, m_page_x2 );
    delete[] m_opHist;
    m_opHist = NULL;
    
    if (nb_staves == 0)
        return this->Terminate( ERR_UNKNOWN );
		
	int y1, y2;
	y1 = 0;
		
	if ( crop )
	{	
		// crop
		if ( !GetImage( &m_opImMain ) )
			return false;

		x1 = std::max( 0, this->m_x1  - RecEnv::s_pre_margin_left ); // 30 px en moins = de marge d'erreur
		x2 = std::min( m_opImMain->width - 1 , this->m_x2 + RecEnv::s_pre_margin_right ); // 20 px en plus = de marge d'erreur
		this->m_x1 = x1;
		this->m_x2 = x2;
		y1 = std::max ( 0, m_opLines1[0] - 50 - RecEnv::s_pre_margin_bottom ); // 120 px en dessous de la derniere portee
		y2 = std::min ( m_opImMain->height -1 , m_opLines1[nb_staves - 1] + 50 + RecEnv::s_pre_margin_top ); // 150 px en dessus de la premiere portee
        this->m_y1 = m_opImMain->height -1 - y2;

		m_opImTmp1 = imImageCreate( x2 - x1, y2 - y1, m_opImMain->color_space, m_opImMain->data_type );    
		imProcessCrop( m_opImMain, m_opImTmp1, x1, y1);
		SwapImages( &m_opImMain, &m_opImTmp1 );
		this->m_size = wxSize( m_opImMain->width, m_opImMain->height );
		SwapImages( &m_img0, &m_opImMain );
		if ( m_isModified ) 
			*m_isModified = true;
			
		// crop staves image
		m_opImTmp1 = imImageCreate( (x2 - x1) / (RESIZE_FACTOR * STAVES_CONV_REDUCTION), (y2 - y1)  / (RESIZE_FACTOR * STAVES_CONV_REDUCTION),
			m_opIm->color_space, m_opIm->data_type );    
		imProcessCrop( m_opIm, m_opImTmp1, x1  / (RESIZE_FACTOR * STAVES_CONV_REDUCTION), y1  / (RESIZE_FACTOR * STAVES_CONV_REDUCTION));
		SwapImages( &m_opIm, &m_opImTmp1 );
	}
	else
	{
		SwapImages( &m_img0, &m_opImMap );
		if ( m_isModified ) 
			*m_isModified = true;
            
        this->m_size = wxSize( m_img0->width, m_img0->height );
        x1 = 0;
        x2 = m_img0->width - 1;
        this->m_x1 = x1;
		this->m_x2 = x2;
        y1 = 0;
        y2 = m_img0->height -1;
        this->m_y1 = m_img0->height -1 - y2;

	}
	
	//wxString staves_file = m_path + "staves.tif";
    //if ( !Write( staves_file, &m_opIm ) )
    //    return false;	
    ImageDestroy( &m_opIm );
	
	// keep grayscale alternative
	ImageDestroy( &m_img1 );
	m_img1 = imImageClone( m_img0 );
	if ( !m_img1 )
		return this->Terminate( ERR_MEMORY );
	imProcessNegative( m_img0, m_img1 );
		
	// keep staff positions
    this->m_staves.Clear( );
    //wxLogDebug("Page dimension: %d - %d", m_img1->height, m_img1->width );
    for( i = nb_staves; i > 0; i--) // flip staves order
    {
        ImStaff imstaff;
        imstaff.m_y = m_opLines1[i-1] - y1;
        this->m_staves.Add( imstaff );
		//wxLogDebug("Staff %d: y=%d", i, imstaff.m_y );
    }
	
	return this->Terminate( ERR_NONE );
}


void ImPage::GetHorizontalStavesPosition( int values[], int size, int avg, int *x1, int *x2 )
{
    wxASSERT_MSG( ( x1 && x2 ), "x1 and x2 cannot be NULL");
    *x1 = 0;
    *x2 = 0;
    int x_new = 0; // to keep new bloc;
    int threshold = avg * 50 / 100; // seuil à 50 %

    bool bloc = false;
    int i;

    for ( i = 0; i < size; i++ )
    {
        if ( !bloc )
        {
            if ( values[i] >= threshold ) // new bloc
            {
                bloc = true;
                x_new = i;
            }
            else
                continue;
        }
        else
        {
            if ( values[i] < threshold ) // end of bloc
            {
                if ( i - x_new > *x2 - *x1 ) // bigger bloc
                {
                    *x1 = x_new;
                    *x2 = i;
                }
                bloc = false;
            }
        }
    }
    if ( bloc  && ( i - x_new > *x2 - *x1) ) // bloc not terminated and biggest bloc
    {
        *x1 = x_new;
        *x2 = i - 1;
    }
}



void ImPage::GetVerticalStavesPosition( int values[], int size, int avg, int staves[], int *nb_staves )
{
    wxASSERT_MSG( ( staves && nb_staves ), "Staves array and nb_of_staves cannot be NULL");

    *nb_staves = 0;
    int y_new = 0; // to keep new staff;
    int threshold = avg * 66 / 100; // seuil 66 %
    int min_staff_height = 100 / RESIZE_FACTOR / STAVES_CONV_REDUCTION * 25 / 100;
        // 100 = portee -> 25 %


    bool staff = false;
    int i;

    //for ( i = 0; i < size; i++ )
    int start = 60 / RESIZE_FACTOR / STAVES_CONV_REDUCTION;
        // pas de portee au minimum à 10 px du bord ! -> centre à 10 + 50 ( demi portee ) 
    if ( start > size )
        start = size;
    int end = size - start;

    for ( i = start; i < end; i++)
    {
        if ( *nb_staves > MAX_STAVES - 1 )
            break;

        if ( !staff )
        {
            if ( values[i] >= threshold ) // new staff
            {
                staff = true;
                y_new = i;
            }
        }
        else
        {
            if ( values[i] < threshold ) // end of bloc
            {
                if ( i - y_new > min_staff_height ) // staff height
                {
                    staves[ *nb_staves ] = ( y_new + ( i - y_new ) / 2 ) * 
                        RESIZE_FACTOR * STAVES_CONV_REDUCTION  / this->m_resize;
                    (*nb_staves)++;
                }
                staff = false;
            }
        }
    }
}
		

bool ImPage::BinarizeAndClean( )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
	wxASSERT_MSG( ( this->m_resize == 1.0) , "Image should be normalized cannot be NULL");

    if ( !m_progressDlg->SetOperation( _("Filtering ...") ) )
        return this->Terminate( ERR_CANCELED );

	this->SetMapImage( m_img0 );

	if ( !GetImage( &m_opImMain ) )
      return false;

	// binarisation APRES filtrage
	// pas de difference sensible, mais mieux niveau bruit selon Bunke

    m_opImTmp1 = imImageClone( m_opImMain );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    if ( !imProcessMedianConvolve( m_opImMain ,m_opImTmp1, 3 ) )
        return this->Terminate( ERR_CANCELED );
    SwapImages( &m_opImMain, &m_opImTmp1 );
		
    m_opImTmp1 = imImageCreate( m_opImMain->width, m_opImMain->height, IM_BINARY, IM_BYTE );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

	if ( *ImPage::m_pre_page_binarization_methodPtr == PRE_BINARIZATION_SAUVOLA )
	{
		if ( !m_progressDlg->SetOperation( _("Binarization ...") ) )
			return this->Terminate( ERR_CANCELED );
		wxLogMessage("Sauvola binarization (region size is %d)", ImPage::s_pre_page_binarization_method_size );
		imProcessSauvolaThreshold( m_opImMain, m_opImTmp1, ImPage::s_pre_page_binarization_method_size, 0.5, 128, 20, 150, false );
	}
	else if ( *ImPage::m_pre_page_binarization_methodPtr == PRE_BINARIZATION_BRINK )
	{
		if ( !m_progressDlg->SetOperation( _("Binarization ...") ) )
			return this->Terminate( ERR_CANCELED );
		wxLogMessage( "Brink (2 classes) binarization" );
		int T = imProcessBrink2ClassesThreshold( m_opImMain, m_opImTmp1, false, BRINK_AND_PENDOCK );
		wxLogMessage("Binarization threshold: %d", T );
	}
	else if ( *ImPage::m_pre_page_binarization_methodPtr == PRE_BINARIZATION_BRINK3CLASSES )
	{
		if ( !m_progressDlg->SetOperation( _("Binarization ...") ) )
			return this->Terminate( ERR_CANCELED );
		wxLogMessage( "Brink (3 classes) binarization" );
		int T = imProcessBrink3ClassesThreshold( m_opImMain, m_opImTmp1, false, BRINK_AND_PENDOCK );	
		wxLogMessage( "Binarization threshold: %d", T );
	}
	else // should not happen, but just in case
	{	
		wxLogWarning("Fix threshold used" );
		imProcessThreshold( m_opImMain, m_opImTmp1, 127, 1);
	}
	//int otsu = imProcessKittlerThreshold( m_opImMain, m_opImTmp1 );
    //imProcessPercentThreshold(m_opImMap, m_opImMain, 15);
    SwapImages( &m_opImMain, &m_opImTmp1 );

    // nettoyage par pruning
    imStats istats;
    imCalcImageStatistics( m_opImMain, &istats );
	
    // verifier negatif - positif : mean < 0.5
	// just in case, usually should not happen. It was the case when using 
	// impages binarized externally using another algorithm
    if ( istats.mean > 0.5 )
    {
        m_opImTmp1 = imImageClone( m_opImMain );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
        imProcessNegative( m_opImMain, m_opImTmp1 );
        SwapImages( &m_opImMain, &m_opImTmp1 );
		imCalcImageStatistics( m_opImMain, &istats );
    }

    double ink_factor = (double)this->m_space_width / (double)this->m_line_width;
    if ( !ink_factor )
        ink_factor = 1.0;
    double mean_factor = 1.0 - istats.mean;
    if ( !mean_factor )
        mean_factor = 1.0;
    double threshold_d = 10 * (pow( 1.0 / ink_factor + 1.0 / mean_factor, 2 ));
	int threshold = min( (int)threshold_d, TP_MAX_SMALL_ELEMENT );

    // add margin - bug si des elements touche le bord dans imProcessPrune  
    m_opImTmp1 = imImageCreate( m_opImMain->width + 2, m_opImMain->height + 2, 
        m_opImMain->color_space, m_opImMain->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessAddMargins( m_opImMain, m_opImTmp1, 1, 1 );
    SwapImages( &m_opImMain, &m_opImTmp1 );

	// debug values
	/*wxString clean_morph = m_path + "clean_morph.csv";
	fid = fopen(clean_morph.c_str(), "a" );
	if ( fid )
	{
		fprintf(fid,"%s;%f;%f;%f\n",
			this->m_shortname.c_str(),
			istats.mean,
			istats.stddev,
			(1 - istats.mean) / istats.stddev);
		fclose( fid );
	}*/

    if ( (1 - istats.mean) / istats.stddev  < 2.0 )
    {
		if ( !m_progressDlg->SetOperation( _("Cleaning ...") ) )
			return this->Terminate( ERR_CANCELED );
	
        wxLogMessage("Cleaning by morphological opening (mean / stddev = %f)" , istats.mean / istats.stddev  );
        m_opImTmp1 = imImageClone( m_opImMain );
        if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
        imProcessBinMorphOpen( m_opImMain, m_opImTmp1, 3, 1);
        SwapImages( &m_opImMain, &m_opImTmp1 );

    }

    /*imImage *NewImage = imImageCreate(m_opImMain->width, m_opImMain->height, IM_GRAY, IM_USHORT);
    imAnalyzeFindRegions ( m_opImMain, NewImage, 8, 1);
    imImageDestroy( NewImage );*/

    wxLogMessage("Removing small elements (minimal size = %d)" , threshold );
    m_opImTmp1 = imImageClone( m_opImMain );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessPrune( m_opImMain, m_opImTmp1, 8, threshold, 0 );
    SwapImages( &m_opImMain, &m_opImTmp1 );
    
    m_opImTmp1 = imImageCreate( m_opImMain->width - 2, m_opImMain->height - 2, 
        m_opImMain->color_space, m_opImMain->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessCrop( m_opImMain, m_opImTmp1, 1, 1);
    SwapImages( &m_opImMain, &m_opImTmp1 );

    if ( !ConvertToMAP( &m_opImMain ) )
        return false;
		
	SwapImages( &m_img0, &m_opImMain );
	if ( m_isModified ) 
		*m_isModified = true;
	return this->Terminate( ERR_NONE );
}


bool ImPage::FindOrnateLetters( )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
	
    if ( !m_progressDlg->SetOperation( _("Find ornate letters ...") ) )
        return this->Terminate( ERR_CANCELED );

	this->SetMapImage( m_img0 );

    if ( !GetImagePlane( &m_opImMain ) )
        return false;

    m_opIm = imImageDuplicate( m_opImMain );
    if ( !m_opIm )
        return this->Terminate( ERR_MEMORY );

    // resize
    m_opImTmp1 = imImageCreate( m_opIm->width / TIP_FACTOR_1, m_opIm->height / TIP_FACTOR_1, m_opIm->color_space, m_opIm->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessResize( m_opIm, m_opImTmp1, 0 );
    SwapImages( &m_opIm, &m_opImTmp1 );

    // close
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessBinMorphClose( m_opIm, m_opImTmp1, 3, 2);
    SwapImages( &m_opIm, &m_opImTmp1 );

    // open
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessBinMorphOpen( m_opIm, m_opImTmp1, 3, 1);
    SwapImages( &m_opIm, &m_opImTmp1 );

    // fill holes
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessFillHoles( m_opIm, m_opImTmp1, 4);
    SwapImages( &m_opIm, &m_opImTmp1 );

    // open
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessBinMorphOpen( m_opIm, m_opImTmp1, 5, 1);
    SwapImages( &m_opIm, &m_opImTmp1 );

    // prune
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessPrune( m_opIm, m_opImTmp1, 4, (int)(pow( 100 / TIP_FACTOR_1, 2 )), 0);
    SwapImages( &m_opIm, &m_opImTmp1 );

    // close
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imProcessBinMorphClose( m_opIm, m_opImTmp1, 5, 2);
    SwapImages( &m_opIm, &m_opImTmp1 );

    // supprimer les zones d'elements dont la hauteur moyenne < 140
    PruneElementsZone( m_opIm, 140 / TIP_FACTOR_1, 0, IM_PRUNE_CLEAR_HEIGHT );

    // supprimer les zones d'elements dont la largeur moyenne < 120 
    PruneElementsZone( m_opIm, 120 / TIP_FACTOR_1, 0, IM_PRUNE_CLEAR_WIDTH );

    // supprimer les zones d'elements dont la hauteur moyenne < 140 
    PruneElementsZone( m_opIm, 140 / TIP_FACTOR_1, 0, IM_PRUNE_CLEAR_HEIGHT );

    // supprimer les zones d'elements dont la largeur moyenne < 120 ou > 600
    PruneElementsZone( m_opIm, 120 / TIP_FACTOR_1, 600 / TIP_FACTOR_1, IM_PRUNE_CLEAR_WIDTH );

    // supprimer les zones d'elements dont la hauteur moyenne > 600
    PruneElementsZone( m_opIm, 120 / TIP_FACTOR_1, 600 / TIP_FACTOR_1, IM_PRUNE_CLEAR_HEIGHT );


    m_opImTmp2 = imImageClone( m_opImMain );
    if ( !m_opImTmp2 )
        return this->Terminate( ERR_MEMORY );
    imImageClear( m_opImTmp2 );
    //imProcessBitwiseNot( m_opImTmp2, m_opImTmp2 );

    m_opImTmp1 = imImageCreate(m_opIm->width, m_opIm->height, IM_GRAY, IM_USHORT);
    if (!m_opImTmp1)
        return this->Terminate( ERR_MEMORY );

    int region_count = imAnalyzeFindRegions(m_opIm, m_opImTmp1, 4, 1);
    if (region_count)
    {
        int* boxes = (int*)malloc(4 * region_count * sizeof(int));
        memset(boxes, 0, 4 *  region_count * sizeof(int));
        imAnalyzeBoundingBoxes(m_opImTmp1, boxes, region_count);
        int margins[4] = {50, 10, 50, 50};
        this->MoveElements( m_opImMain, m_opImTmp2, boxes, region_count, margins, TIP_FACTOR_1 );
        free( boxes );
    }
    ImageDestroy( &m_opImTmp1 );

    if ( !ExtractPlane( &m_opImMap, &m_opImTmp2, IMAGE_ORNATE_LETTER ) )
        return false;

    // save file
	SwapImages( &m_img0, &m_opImMap );
	if ( m_isModified ) 
		*m_isModified = true;
	return this->Terminate( ERR_NONE );
}



bool ImPage::FindText( )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
	
    if ( !m_progressDlg->SetOperation( _("Find text elements and lyrics ...") ) )
        return this->Terminate( ERR_CANCELED );

	this->SetMapImage( m_img0 );

    if ( !GetImagePlane( &m_opImMain ) )
        return false;

    m_opImTmp1 = imImageClone( m_opImMain );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    int nb_staves = (int)this->m_staves.GetCount();
    if ( nb_staves == 0 )
        return this->Terminate( ERR_NONE );

    // suppression des portees - deplacement par bounding boxes
    int st;
    int x1 = 0;
    int x2 = m_opImMain->width - 1;
    int* boxes = (int*)malloc(4 * nb_staves * sizeof(int));
    memset(boxes, 0, 4 *  nb_staves * sizeof(int));
    for (st = 0; st < nb_staves; st++ )
    {
        int idx = st * 4;
        boxes[idx + 0] = x1;
        boxes[idx + 1] = x2;
        boxes[idx + 2] = m_staves[st].m_y - TP_STAFF_ROI_H / 2;
        boxes[idx + 3] = m_staves[st].m_y + TP_STAFF_ROI_H / 2;
    }

    int y_margin = STAFF_HEIGHT - TP_STAFF_ROI_H / 2;
    int margins[4] = {0, 0, y_margin, y_margin};
    this->MoveElements( m_opImMain, m_opImTmp1, boxes, st, margins, 1 );
    free( boxes );
    //SwapImages( &m_opImMain, &m_opImTmp1 );
    ImageDestroy( &m_opImTmp1 );


    // extraction du text - centre sous la portee
    int med = this->GetMedianStavesSpace();
    // position du texte sous la portee = espace (medianne) / 2
    int text_centroid = med / 2;
    int y_margin1 = min( text_centroid - STAFF / 2 - TP_MARGIN_MIN, TP_MARGIN_Y1 );
    int y_margin2 = min( text_centroid - STAFF / 2 - TP_MARGIN_MIN, TP_MARGIN_Y2 );

    m_opImTmp1 = imImageCreate(m_opImMain->width, m_opImMain->height, IM_GRAY, IM_USHORT);
    if (!m_opImTmp1)
        return this->Terminate( ERR_MEMORY );

    int region_count = imAnalyzeFindRegions(m_opImMain, m_opImTmp1, 4, 1);
    if (!region_count)
        return this->Terminate( ERR_NONE );

    // calcul des centroids
    float* cx = (float*)malloc(region_count*sizeof(float));
    memset(cx, 0, region_count*sizeof(float));
    float* cy = (float*)malloc(region_count*sizeof(float));
    memset(cy, 0, region_count*sizeof(float));
    imAnalyzeMeasureCentroid (m_opImTmp1, NULL, region_count, cx, cy);
    
    int y_min, y_max, i;
    imbyte* img_data = NULL;
    imushort* region_data = NULL;

    imImageClear( m_opImMain );
    img_data = (imbyte*)m_opImMain->data[0];
    region_data = (imushort*)m_opImTmp1->data[0];
    for (i = 0; i < m_opImTmp1->count; i++)
    {
        *img_data = 0;
        // conserver si le centroid y dans une fourchette ( -15 + 35 autour du centroid de texte
        if (*region_data)
        {
            float centroid = cy[ (*region_data) -1 ];
            for (st = 0; st < nb_staves; st++ )
            {
                y_min = max( 0, m_staves[st].m_y - text_centroid - y_margin1 );
                y_max = max( 0, m_staves[st].m_y - text_centroid + y_margin2 );
                if ( ( centroid > y_min ) && ( centroid < y_max ) )
                {
                    *img_data = 1;
                    *region_data = 0;
                    break;
                }
            }
        }
        img_data++;
        region_data++;
    }

    if ( !ExtractPlane( &m_opImMap, &m_opImMain, IMAGE_LYRICS ) )
    {
        free(cx);
        free(cy);
        return false;
    }

    imImageClear( m_opImMain );
    img_data = (imbyte*)m_opImMain->data[0];
    region_data = (imushort*)m_opImTmp1->data[0];
    //y_min = min( m_opImMain->height - 1, m_staves[nb_staves - 1].m_y + STAFF / 2 + TP_MARGIN_Y_TITLE );
    y_min = min( m_opImMain->height - 1, m_staves[0].m_y + STAFF / 2 + TP_MARGIN_Y_TITLE );
    for (i = 0; i < m_opImTmp1->count; i++)
    {
        // conserver si le centroid y au dessus de y min
        if (*region_data)
        {
            float centroid = cy[ (*region_data) -1 ];
            if ( centroid > y_min )
            {
                *img_data = 1;
                *region_data = 0;
            }
        }
        img_data++;
        region_data++;
    }
    free(cx);
    free(cy);
	
    if ( !ExtractPlane( &m_opImMap, &m_opImMain, IMAGE_TITLE ) )
        return false;

    // save file
	SwapImages( &m_img0, &m_opImMap );
	
	// keep grayscale alternative
	//ImageDestroy( &m_img0 );
	//m_img0 = imImageDuplicate( m_img1 );
	//imPhotogrammetric( m_img1, m_img0 );
	//imPhotogrammetric( m_img1, m_img1 );
	
	return this->Terminate( ERR_NONE );
}



bool ImPage::FindBorders( )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
	
    if ( !m_progressDlg->SetOperation( _("Find borders ...") ) )
        return this->Terminate( ERR_CANCELED );

	this->SetMapImage( m_img0 );

    if ( !GetImagePlane( &m_opImMain ) )
        return false;

    m_opIm = imImageDuplicate( m_opImMain );
    if ( !m_opIm )
        return this->Terminate( ERR_MEMORY );

    /*
	// debug
	m_opHist = new int[ m_opIm->width ];
    imAnalyzeProjectionV( m_opIm, m_opHist );
    wxString borders_v = m_path + "borders_v.csv";
	imSaveValues( m_opHist, m_opIm->width, borders_v.c_str() );
    //f_width = 100 / RESIZE_FACTOR / 4; // 25 % de la hauteur de la portee
    //MedianFilter( m_opHist, width, f_width, &avg );
    //wxString staves_v_med = m_path + "staves_v_med.csv";
	//imSaveValues( m_opHist, width, staves_v_med.c_str() );
	delete[] m_opHist;
	m_opHist = NULL;*/

    // buffer avec les colonnes à traiter ( toutes, initialiser à 1 )
    int i;
    int w1 = m_opIm->width;
    m_opCols1 = new int[ w1 ];
    for ( i = 0; i < w1; i++ )
        m_opCols1[i] = 1;

    // buffer avec les lignes à traiter == 1 - initialiser à toutes
    int h1 = m_opIm->height;
    m_opLines1 = new int[ h1 ];
    for ( i = 0; i < h1; i++ )
        m_opLines1[i] = 1;

    // mettre à zero les zones à ignorer : staff position -60 / + 60
    for( i = 0; i < (int)this->m_staves.GetCount(); i++)
    {
        int staff_height = 120;
        int pos = this->m_staves[i].m_y;
        if ( ( pos < (staff_height / 2 ) ) ||
            ( pos > h1 - ( staff_height / 2) - 1 ) ) 
            continue;
        memset( m_opLines1 + pos - staff_height / 2, 0, staff_height * sizeof(int) );
    }

    // images avec les lignes à considerer uniquement -> somme de m_opLines1
    int h2 = sum( m_opLines1, h1 );
    m_opImTmp1 = imImageCreate( m_opIm->width, h2, m_opIm->color_space, m_opIm->data_type );
    if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );

    imbyte *bufIm = (imbyte*)m_opIm->data[0];
    imbyte *bufImTmp = (imbyte*)m_opImTmp1->data[0];

    // copier les lignes
    int i2;
    for ( i = 0, i2 = 0; i < h1; i++ )
    {
        if ( (m_opLines1[i] == 0) || (i2 >= h2) )
            continue;

        int offset1 = i * m_opIm->width;
        int offset2 = i2 * m_opIm->width;
        memcpy( &bufImTmp[ offset2], &bufIm[ offset1 ], m_opIm->width * sizeof(imbyte) );
        i2++;
    }
    SwapImages( &m_opIm, &m_opImTmp1 );

    // bord gauche et droit
    m_opImMask = imImageCreate( 100, m_opIm->height, m_opIm->color_space, m_opIm->data_type );
    if ( !m_opImMask )
            return this->Terminate( ERR_MEMORY );
    m_opImTmp1 = imImageClone( m_opIm );
    if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
    m_opImTmp2 = imImageClone( m_opImMain );
    if ( !m_opImTmp2 )
            return this->Terminate( ERR_MEMORY );

    // bord gauche
    imProcessCrop( m_opIm, m_opImMask, 0, 0 );
    CleanBorder( m_opLines1, h1, m_opImMask, m_opImMain, 2 );

    // bord droit - flip
    imProcessMirror( m_opIm, m_opImTmp1 );
    imProcessMirror( m_opImMain, m_opImTmp2 );
    imProcessCrop( m_opImTmp1, m_opImMask, 0, 0 );
    CleanBorder( m_opLines1, h1, m_opImMask, m_opImTmp2, 2 );
    imProcessMirror( m_opImTmp2, m_opImMain );

    // fin gauche et droit
    ImageDestroy( &m_opImTmp1 );
    ImageDestroy( &m_opImTmp2 );
    ImageDestroy( &m_opImMask );


    // bords du haut et du bas
    m_opImMask = imImageCreate( 100, m_opIm->width, m_opIm->color_space, m_opIm->data_type );
    if ( !m_opImMask )
            return this->Terminate( ERR_MEMORY );
    m_opImTmp1 = imImageCreate( m_opIm->height, m_opIm->width, m_opIm->color_space, m_opIm->data_type );
    if ( !m_opImTmp1 )
            return this->Terminate( ERR_MEMORY );
    m_opImTmp2 = imImageCreate( m_opImMain->height, m_opImMain->width, m_opImMain->color_space, m_opImMain->data_type );
    if ( !m_opImTmp2 )
            return this->Terminate( ERR_MEMORY );

    // bas
    imProcessRotate90( m_opIm, m_opImTmp1, 1 );
    imProcessRotate90( m_opImMain, m_opImTmp2, 1 );
    imProcessCrop( m_opImTmp1, m_opImMask, 0, 0 );
    CleanBorder( m_opCols1, w1, m_opImMask, m_opImTmp2, 1 );
    imProcessRotate90( m_opImTmp2, m_opImMain, -1 );

    // haut
    imProcessRotate90( m_opIm, m_opImTmp1, -1 );
    imProcessRotate90( m_opImMain, m_opImTmp2, -1 );
    imProcessCrop( m_opImTmp1, m_opImMask, 0, 0 );
    CleanBorder( m_opCols1, w1, m_opImMask, m_opImTmp2, 1 );
    imProcessRotate90( m_opImTmp2, m_opImMain, 1 );

    // fin haut et bas
    ImageDestroy( &m_opImTmp1 );
    ImageDestroy( &m_opImMask );
    ImageDestroy( &m_opImTmp2 );


    //imProcessBitwiseNot( m_opImMain, m_opImMain );
    if ( !GetImagePlane( &m_opImTmp1 ) )
        return false;
    imProcessBitwiseNot( m_opImTmp1, m_opImTmp1 );
    imProcessArithmeticOp( m_opImTmp1, m_opImMain, m_opImTmp1, IM_BIT_OR );
    imProcessBitwiseNot( m_opImTmp1, m_opImTmp1 );

    if ( !ExtractPlane( &m_opImMap, &m_opImTmp1, IMAGE_BLANK ) )
        return false;

    // save file
	SwapImages( &m_img0, &m_opImMap );
	return this->Terminate( ERR_NONE );
}

void ImPage::CleanBorder( int rows[], int size, _imImage *border, _imImage *image, int split )
{
    int i;
    imbyte *bufIm = NULL;
    imbyte *bufImTmp = NULL;

    imImage *tmp = imImageClone( border );
    if ( !tmp )
        return;


    // convolution : kernel ou close + fillholes
    imImage* kernel = imImageCreate( 1, 5, IM_GRAY, IM_INT);
    imImageSetAttribute(kernel, "Description", IM_BYTE, -1, (void*)"Erode");
    int* kernel_data = (int*)kernel->data[0];
    for(i = 0; i < kernel->count; i++)
        kernel_data[i] = 0;

    //imProcessBinMorphClose( border, tmp , 3, 1);
    imProcessBinMorphConvolve( border, tmp, kernel, 0, 1);
    imImageDestroy(kernel);

    imImage *tmp2 = imImageClone( tmp );
    if ( !tmp2 )
    {
        imImageDestroy( tmp );
        return;
    }
    imProcessFillHoles( tmp, tmp2, 4 ); // bug with inplace call !!!!!
    SwapImages( &tmp, &tmp2 );

    // histogramme vertical sur 100 px
    int hist_width = tmp->width;
    int x, y;
    //int split = 3; // nombre de split horizontaux
    int y1, y2;
    int x1 = 0;
    int x2 = hist_width;
    bool has_border = false;
    for( int sp = 0; sp < split; sp++ )
    {
        y1 = (int)((double)sp / split * tmp->height);
        y2 = (int)((double)(sp + 1) / split * tmp->height);
        imImage *tmp2 = imImageCreate( x2 - x1, y2 - y1, tmp->color_space, tmp->data_type); 
        if ( !tmp2 )
        {
            imImageDestroy( tmp );
            return;
        }

        imProcessCrop( tmp, tmp2, x1, y1 );

        m_opHist = new int[ tmp2->width ];
        imAnalyzeProjectionV( tmp2, m_opHist );
            
        int x_border;
        int max = max_val( m_opHist, tmp2->width, &x_border );

        int px = 1; // true if border
        // verification de la valeur maximal - 
        // eventuellement en plus verifier le nombre de segment - risque d'enlever du texte en bas !
        if ( max < 0.60 * tmp2->height ) 
            px = 0;
        else
            has_border = true;

        // mettre les pixels du bord à max à 1 = remplissage jusq'au bord detecte
        //bufIm = (imbyte*)m_opIm->data[0];
        bufImTmp = (imbyte*)tmp->data[0];
        for (y = y1; y < y2; y++)
        {
            for (x = 0; x < x_border + 15; x++) // adaptation empirique de 15 pixels
            {
                if (x >= tmp->width)
                    break; // verification à cause de 15 pixels ajoutes
                int offset = y * tmp->width + x;
                bufImTmp[ offset ] = px;
            }
        }

        imImageDestroy( tmp2 );
        delete[] m_opHist;
        m_opHist = NULL;
    }


    if ( !has_border )
    {
        imImageDestroy( tmp );
        return;
    }

    int h2 = tmp->height;
    int h1 = image->height;
    int i2;

    // calculer la largeur du bord pour chaque ligne
    m_opLines2 = new int[ h2 ];
    bufImTmp = (imbyte*)tmp->data[0];
    for ( i = 0; i < h2; i++ )
    {
        int j;
        int offset = i * tmp->width;
        for ( j = 0; j < hist_width ; j++ )
            if ( bufImTmp[ offset + j ] == 0 )
                break;
        m_opLines2[i] = j;
    }
    this->MedianFilter( m_opLines2, h2, 20 );


    // effacer le bord dans l'image
    bufIm = (imbyte*)image->data[0];
    int last_val = m_opLines2[0];
    int val, offset;
    for ( i = 0, i2 = 0; i < h1; i++ )
    {
        if ( (rows[i] != 0) && (i2 < h2) )
        {
            last_val = m_opLines2[i2];
            i2++;
        }
        val = last_val;
        offset = i * image->width;
        for (x = 0; x < 10; x++) // adaptation empirique de 15 pixels
        {
            if (bufIm[ offset + val ] == 0) // pixel blanc
                break;
            val++;
        }

        memset( &bufIm[ offset ], 0, val * sizeof(imbyte) );
    }
    delete[] m_opLines2;
    m_opLines2 = NULL;

    imImageDestroy( tmp );

}



bool ImPage::FindTextInStaves( )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
	
    if ( !m_progressDlg->SetOperation( _("Find text in staves ...") ) )
        return this->Terminate( ERR_CANCELED );

	this->SetMapImage( m_img0 );

    if ( !GetImagePlane( &m_opImMain ) )
        return false;

    // staff image
    m_opImTmp1 = imImageCreate( m_opImMain->width, STAFF_HEIGHT, m_opImMain->color_space, m_opImMain->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    int nb_staves = (int)this->m_staves.GetCount();
    int x1 = 0, st;
    for (st = 0; st < nb_staves; st++ )
    {
        int stave_y = this->m_staves[st].m_y - ( STAFF_HEIGHT / 2 );
        if ( (stave_y < 0) || (stave_y >= m_opImMain->height) )
            continue;

        imProcessCrop( m_opImMain, m_opImTmp1, x1, stave_y);
		//this->m_staves[st].SetProgressDlg( m_progressDlg );
        this->m_staves[st].SetMapImage( m_opImTmp1 );
        this->m_staves[st].GetStaffBorders( 25, true );
    }
    ImageDestroy( &m_opImTmp1 );

	float c = 0;
    int i, segments_count = 0;
    for (st = 0; st < nb_staves; st++ )
    {
        int nb_segments = (int)m_staves[st].m_segments.GetCount();
        for(i = 0; i < nb_segments; i++)
        {
            c += m_staves[st].m_segments[i].m_compactness;
            segments_count++;
        }
    }
    c /= segments_count;

    m_opImTmp1 = imImageClone( m_opImMain );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
    imImageClear( m_opImTmp1 );    

    int to_remove = 0;
    int* boxes = (int*)malloc(4 * segments_count * sizeof(int));
    memset(boxes, 0, 4 *  segments_count * sizeof(int));
    for (st = 0; st < nb_staves; st++ )
    {
        int nb_segments = (int)m_staves[st].m_segments.GetCount();
        for(i = nb_segments - 1; i >= 0; i--)
        {
            if ( m_staves[st].m_segments[i].m_compactness / c < 0.2 )
            {
                int idx = to_remove * 4;
                boxes[idx + 0] = m_staves[st].m_segments[i].m_x1;
                boxes[idx + 1] = m_staves[st].m_segments[i].m_x2;
                boxes[idx + 2] = m_staves[st].m_y - SS_STAFF_ROI_H / 2;
                boxes[idx + 3] = m_staves[st].m_y + SS_STAFF_ROI_H / 2;
                to_remove++;
				//wxLogMessage(_("Staff segment detected (%d-%d)"), 
				//		m_staves[st].m_segments[i].m_x1,m_staves[st].m_segments[i].m_x2);
                //wxLogMessage("x1 %d x2 %d y %d",
                //    m_staves[st].m_segments[i].m_x1,m_staves[st].m_segments[i].m_x2,m_staves[st].m_y);
                m_staves[st].m_segments.RemoveAt(i);
			}   
		}
	}

	int margins[4] = {10, 10, 70, 70};
    this->MoveElements( m_opImMain, m_opImTmp1, boxes, to_remove, margins, 1 );
    free( boxes );
		
	if ( !ExtractPlane( &m_opImMap, &m_opImTmp1, IMAGE_TEXT_IN_STAFF ) )
		return false;

	// save file
	SwapImages( &m_img0, &m_opImMap );
	return this->Terminate( ERR_NONE );
}



bool ImPage::ExtractStaves( )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
	
    if ( !m_progressDlg->SetOperation( _("Extract staves ...") ) )
        return this->Terminate( ERR_CANCELED );

	this->SetMapImage( m_img0 );

    if ( !GetImagePlane( &m_opImMain ) )
        return false;

    // staff image
    m_opImTmp1 = imImageCreate( m_opImMain->width, STAFF_HEIGHT, m_opImMain->color_space, m_opImMain->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );

    int nb_staves = (int)this->m_staves.GetCount();
    int x1 = 0, st;
    for (st = 0; st < nb_staves; st++ )
    {
        int stave_y = this->m_staves[st].m_y - ( STAFF_HEIGHT / 2 );
        if ( ( stave_y < 0 ) || ( stave_y >= m_opImMain->height ) )
            continue;

        imProcessCrop( m_opImMain, m_opImTmp1, x1, stave_y );
		//this->m_staves[st].SetProgressDlg( m_progressDlg );
        this->m_staves[st].SetMapImage( m_opImTmp1 );
        this->m_staves[st].GetStaffBorders( 20, false );
    }
	
	// merge segments separated by less than 80 pixels
    for ( st = 0; st < nb_staves; st++ )
    {
        int nb_segments = (int)m_staves[st].m_segments.GetCount();
        for(int i = nb_segments - 1; i >= 1; i--)
        {
            if ( ( m_staves[st].m_segments[i].m_x1 - m_staves[st].m_segments[i-1].m_x2 ) < 80 ) // merge
            {
				m_staves[st].m_segments[i-1].m_x2 = m_staves[st].m_segments[i].m_x2;
                m_staves[st].m_segments.RemoveAt(i);
			}   
		}
	}
	
	// remove segments smaller by less than 80 pixels
    for (st = 0; st < nb_staves; st++ )
    {
        int nb_segments = (int)m_staves[st].m_segments.GetCount();
        for(int i = nb_segments - 1; i >= 0; i--)
        {
            if ( ( m_staves[st].m_segments[i].m_x2 - m_staves[st].m_segments[i].m_x1 ) < 80 ) // remove
            {
				m_staves[st].m_segments.RemoveAt(i);
			}   
		}
	}
	
	// set m_x1 and m_x2 for all staves (or remove empty staves)
    for (st = nb_staves - 1; st >= 0; st-- )
    {
        int nb_segments = (int)m_staves[st].m_segments.GetCount();
		if (nb_segments == 0)
		{
			m_staves.RemoveAt(st); // The staves are removed permanentely, as the staff detection is performed in preprocessing only
			//m_staves[st].m_x1 = 0;
			//m_staves[st].m_x2 = 0;
		}
		else
		{
			m_staves[st].m_x1 = m_staves[st].m_segments[0].m_x1;
			m_staves[st].m_x2 = m_staves[st].m_segments[nb_segments-1].m_x2;
		}
	}
	
	return this->Terminate( ERR_NONE );
}


bool ImPage::MagicSelection( int x, int y, AxImage *selection, int *xmin, int *ymin )
{
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");

	ImageDestroy( &m_selection );
	
	this->SetMapImage( m_img0 );	
	
	y = ToViewY( y );
	
	m_opImMain = imImageDuplicate( m_opImMap );
    if ( !m_opImMain )
        return this->Terminate( ERR_MEMORY );
		
	imImageSetBinary( m_opImMain );
	imImageMakeBinary( m_opImMain );
	
    m_opImTmp1 = imImageCreate(m_opImMain->width, m_opImMain->height, IM_GRAY, IM_USHORT);
    if (!m_opImTmp1)
        return this->Terminate( ERR_MEMORY );
	
    int region_count = imAnalyzeFindRegions(m_opImMain, m_opImTmp1, 4, 1);
    if (!region_count)
        return this->Terminate( ERR_NONE );
		
	imushort* img_data = (imushort*)m_opImTmp1->data[0];
	imbyte* img_data_main = (imbyte*)m_opImMain->data[0];
	imushort pixel = *(img_data + (y * m_opImTmp1->width) + x);

    if (!pixel)
        return this->Terminate( ERR_NONE );
		
	for (int i = 0; i < m_opImTmp1->count; i++)
	{
		if (*img_data)
		{
			if ( (*img_data) == pixel )
				(*img_data) = 1;
			else
			{
				(*img_data_main) = 0;
				(*img_data) = 0;
			}
		}
		img_data_main++;
		img_data++;
	}
	
	int box[4];
	memset( box, 0, 4 * sizeof( int ) );
	imAnalyzeBoundingBoxes(m_opImTmp1, box, 1);
	box[1] += 1; // bug in imAnalyzeBoundingBoxes ??? needed
	box[3] += 1;
	
    // selection image
    m_selection = imImageCreate( box[1] - box[0], box[3] - box[2], IM_BINARY, IM_BYTE );
    if ( !m_selection )
        return this->Terminate( ERR_MEMORY );
	
	imProcessCrop( m_opImMain, m_selection, box[0], box[2] );	
	m_selection_pos.x = box[0];
	m_selection_pos.y = box[2];
	
	// change in red
	ImageDestroy( &m_opImTmp1 );
	m_opImTmp1 = imImageCreate( m_selection->width, m_selection->height, IM_MAP, IM_BYTE );
	if (!m_opImTmp1)
		return this->Terminate( ERR_MEMORY );
        
	imImageClear( m_opImTmp1 );
	imProcessArithmeticOp( m_opImTmp1, m_selection, m_opImTmp1, IM_BIN_ADD );
    long *pal = imPaletteGray();
    pal[0] = imColorEncode( 255, 255, 255 ); // fond blanc
    pal[1] = imColorEncode( 255, 0, 0 ); // rouge
    imImageSetPalette( m_opImTmp1, pal, 256 );
	
	SetImImage( m_opImTmp1, selection );
	*xmin = m_selection_pos.x;
	*ymin = ToViewY( m_selection_pos.y ) - m_selection->height; 
	
	//Write("/Users/puginlocal/test.tif" , &m_selection );
	
	return this->Terminate( ERR_NONE );

}



bool ImPage::ChangeClassification( int _x1, int _y1, int _x2, int _y2, int plane_number  )
{
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");

	this->SetMapImage( m_img0 );
	int x1, x2, y1, y2;

	_y2 = ToViewY( _y2 );
	_y1 = ToViewY( _y1 );
	y1 = min( _y1, _y2 );
	y2 = max( _y1, _y2 );
	x1 = min( _x1, _x2 );
	x2 = max( _x1, _x2 );
	m_selection_pos.x = x1;
	m_selection_pos.y = y1;
	
	
    // selection image
	//ImageDestroy( &m_selection );
    m_opImTmp2 = imImageCreate( x2 - x1, y2 - y1, m_opImMap->color_space, m_opImMap->data_type );
    if ( !m_opImTmp2 )
        return this->Terminate( ERR_MEMORY );
	
	imProcessCrop( m_opImMap, m_opImTmp2, x1, y1 );

	PrepareCheckPoint( UNDO_PART, IM_UNDO_CLASSIFICATION );	

	imImageSetBinary( m_opImTmp2 );
	imImageMakeBinary( m_opImTmp2 );
	
	imProcessArithmeticConstOp( m_opImTmp2, pow(2, plane_number), m_opImTmp2, IM_BIN_MUL );
	imProcessInsert( m_opImMap, m_opImTmp2, m_opImMap, m_selection_pos.x, m_selection_pos.y ); 

	CheckPoint( UNDO_PART, IM_UNDO_CLASSIFICATION );
	
	//ImageDestroy( &m_selection );
	
	if ( m_isModified ) 
		*m_isModified = true;
	
	// save file
	SwapImages( &m_img0, &m_opImMap );
	return this->Terminate( ERR_NONE );
}


bool ImPage::ChangeClassification( int plane_number  )
{
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
	wxASSERT_MSG( m_selection, "Selection cannot be NULL");

	if ( !m_opImMap )
		this->SetMapImage( m_img0 );
		
	// buffer (selection size)
	m_opImTmp1 = imImageCreate( m_selection->width, m_selection->height, m_opImMap->color_space, m_opImMap->data_type );
    if ( !m_opImTmp1 )
        return this->Terminate( ERR_MEMORY );
		
	// map selection from which the planes are extracted
	m_opImTmp2 = imImageDuplicate( m_opImTmp1 );
    if ( !m_opImTmp2 )
        return this->Terminate( ERR_MEMORY );
	imProcessCrop( m_opImMap, m_opImTmp2, m_selection_pos.x, m_selection_pos.y );
	
	PrepareCheckPoint( UNDO_PART, IM_UNDO_CLASSIFICATION );	
	
	// destination buffer
	m_opImMain = imImageClone( m_opImTmp1 );
    if ( !m_opImMain )
        return this->Terminate( ERR_MEMORY );
	imImageClear( m_opImMain) ;
		
    for (int i = 0; i <= IMAGE_PLANES; i ++ )
    {
		imProcessBitPlane( m_opImTmp2, m_opImTmp1, i, 0);
		if ( i == plane_number )
			imProcessBitwiseOp( m_opImTmp1, m_selection, m_opImTmp1, IM_BIT_OR );
		else
		{
		    imProcessBitwiseNot( m_opImTmp1, m_opImTmp1 );
			imProcessBitwiseOp( m_opImTmp1, m_selection, m_opImTmp1, IM_BIT_OR );
			imProcessBitwiseNot( m_opImTmp1, m_opImTmp1 );
		}
		imProcessArithmeticConstOp( m_opImTmp1, pow(2, i), m_opImTmp1, IM_BIN_MUL );
		imProcessArithmeticOp( m_opImTmp1, m_opImMain, m_opImMain, IM_BIN_ADD );
		
    }
	//ImageDestroy( &m_selection );
	
	imProcessInsert( m_opImMap, m_opImMain, m_opImMap, m_selection_pos.x, m_selection_pos.y ); 
	
	// for undo
	ImageDestroy( &m_opImTmp2 );
	m_opImTmp2 = imImageDuplicate( m_opImMain );
    if ( !m_opImTmp2 )
        return this->Terminate( ERR_MEMORY );
	CheckPoint( UNDO_PART, IM_UNDO_CLASSIFICATION );	
	
	if ( m_isModified ) 
		*m_isModified = true;
	
	// save file
	SwapImages( &m_img0, &m_opImMap );
	return this->Terminate( ERR_NONE );
}


int ImPage::GetMedianStavesSpace( )
{
    int nb = (int)this->m_staves.GetCount();

    if ( nb < 2 )
        return 0;

    int *spaces = new int[ nb - 1 ];
    for (int st = 0; st < nb - 1; st++ )
        spaces[st] = m_staves[st].m_y - m_staves[st + 1].m_y;
    int med = median(spaces, nb - 1 );

    delete[] spaces;
    return med;
}

int ImPage::GetStaffCount( )
{
    int nb = (int)this->m_staves.GetCount();
	return nb;
}

/*
// retourne le nombre de segment de staff dans cette page
// une portee sans segment vaut 1 if segment_only == false !!!
int ImPage::GetStaffSegmentsCount( bool segment_only )
{
    int nb = (int)this->m_staves.GetCount();

	if ( nb == 0 )
		return 0;

	int count = nb;
    for (int i = 0; i < nb; i++)
    {
        ImStaff *imStaff = &m_staves[i];
		if ( !imStaff->m_segments.IsEmpty() )
	        count += (int)imStaff->m_segments.GetCount() - 1;
		else if ( segment_only )
			count -= 1; // no segment in the staff, remove 

    }
    return count;
}
*/

/*
// retourne pour chaque segment l'offset (m_x1) et les points ou commencent un nouveau segment (entre m_x2 et m_x1 du suivant)
// split_point == -1 pour le dernier segment
void ImPage::GetStaffSegmentOffsets( int staff_no, int offsets[], int split_points[], int end_points[] )
{
	if ( staff_no >= (int)this->m_staves.GetCount() )
		return;

	ImStaff *imStaff = &m_staves[staff_no];
	if ( !imStaff->m_segments.IsEmpty() )
	{
		int last_x2 = -1;
		for(int j = 0; j < (int)imStaff->m_segments.GetCount(); j++ )
		{
			if ( j > 255 ) // because allocation of offsets and split_points is not checked...
				break;
		
			ImStaffSegment *segment = &imStaff->m_segments[j];
			offsets[j] = segment->m_x1;
			end_points[j] = segment->m_x2 - segment->m_x1;
			split_points[j] = -1;
			if ( last_x2 != -1 ) // plusieurs segment, mettre a jour le split_point precedent
				split_points[j - 1] = ( segment->m_x1 + last_x2 ) / 2;
			last_x2 = segment->m_x2;
		}
	}
    return;
}
*/

bool ImPage::SaveStaffImages( )
{
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");

	wxArrayPtrVoid params; // tableau de pointeurs pour parametres

	this->SetMapImage( m_img0 );

    if ( !GetImagePlane( &m_opImMain ) )
        return false;

    // sauver l'image dans le working directory
	wxString filename = m_path + "/staff";
	params.Add( m_opImMain );
	params.Add( &filename );
    ImStaffFunctor staffFuncSI(&ImStaff::SaveImage);
    this->Process(&staffFuncSI, params );

    return this->Terminate( ERR_NONE );
}


//******* fonction origininale ******/

bool ImPage::StaffCurvatures( )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");

    if (!m_progressDlg->SetOperation( _("Detect staff curvature ...") ) )
        return this->Terminate( ERR_CANCELED );

	wxArrayPtrVoid params; // tableau de pointeurs pour parametres

	this->SetMapImage( m_img0 );

    if ( !GetImagePlane( &m_opImMain ) )
        return false;
		
    // counter
    int counter = m_progressDlg->GetCounter();
    int count = this->m_staves.GetCount(); //GetStaffSegmentsCount();
    imCounterTotal( counter, count * 2 , "Detect staff curvature ..." );

    // calculer la hauteur de la portee par projection d'histgramme (ImStaff::CalcStaffHeight)
	int st = this->GetStaffCount();
	m_opLines1 = new int[ st ];
	memset( m_opLines1, 0, st * sizeof( int ) );
	params.Add( m_opImMain );
	params.Add( m_opLines1 );
    ImStaffFunctor staffFuncSH( &ImStaff::CalcStaffHeight );
    this->Process( &staffFuncSH, params, counter );
	// valeur medianne de tous les segments de portee
	m_staff_height = median( m_opLines1, st );
	//int sw = m_space_width * this->m_resize;
	//int lw = m_line_width * this->m_resize;
	//wxLogMessage( "---staff space %d staff line = %d %d ----- height %d",
	//	sw, lw, 4*sw + 5*lw, med );

	// calculer la position de la portee par correlation (ImStaffSegment::CalcCorrelation)
	// les valeurs sont conservee dans m_positions de chaque segment
	wxString filename = m_path + "staff";
    params.Clear();
	params.Add( m_opImMain );	
	params.Add( &filename );
	params.Add( &m_staff_height );
	params.Add( &m_num_staff_lines );
    ImStaffFunctor staffFunc( &ImStaff::CalcCorrelation );
    this->Process( &staffFunc, params, counter );

	return this->Terminate( ERR_NONE );
}


bool ImPage::GenerateMFC( wxString output_dir )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");

    if (!m_progressDlg->SetOperation( _("Prepare staves ...") ) )
        return this->Terminate( ERR_CANCELED );
	
	if ( output_dir.IsEmpty() )
		//output_dir = wxGetApp().m_workingDir + "/";
		output_dir = m_path;

	wxArrayPtrVoid params; // tableau de pointeurs pour parametres

	if ( m_staff_height <= 0 )
		return false;

	this->SetMapImage( m_img0 );

    if ( !GetImagePlane( &m_opImMain ) )
        return false;
		
    // counter
    int counter = m_progressDlg->GetCounter();
    int count = this->m_staves.GetCount();//GetStaffSegmentsCount();
    imCounterTotal( counter, count, "Prepare staves ..." );
	
	// calculer les features (ImStaffSegment::CalcFeatures)
	wxString input = output_dir + "mfc.input";
	wxString filename = output_dir + "staff";
	wxFile finput( input, wxFile::write );
	if ( !finput.IsOpened() )
		return this->Terminate( ERR_FILE, input.c_str() );
	int win = WIN_WIDTH;
	int overlap = WIN_OVERLAP;
	params.Clear();
    params.Add( m_opImMain );
    params.Add( &win );
	params.Add( &overlap );
	params.Add( &filename );
	params.Add( &finput );
	params.Add( &m_staff_height );
    ImStaffFunctor staffFuncFeatures(&ImStaff::CalcFeatures);
	this->Process(&staffFuncFeatures, params, counter );

	finput.Close();

    return this->Terminate( ERR_NONE );
}

bool ImPage::GenerateLyricMFC( wxString output_dir )
{
    wxASSERT_MSG( m_progressDlg, "Progress dialog cannot be NULL");
	wxASSERT_MSG( m_img0, "Img0 cannot be NULL");
	
    if (!m_progressDlg->SetOperation( _("Prepare lyrics ...") ) )
        return this->Terminate( ERR_CANCELED );
	
	if ( output_dir.IsEmpty() )
		//output_dir = wxGetApp().m_workingDir + "/";
		output_dir = m_path;
	
	wxArrayPtrVoid params; // tableau de pointeurs pour parametres
	
	if ( m_staff_height <= 0 )
		return false;
	
	this->SetMapImage( m_img0 );
	
    if ( !GetImagePlane( &m_opImMain, 4 ) )
        return false;
	
    // counter
    int counter = m_progressDlg->GetCounter();
    int count = this->m_staves.GetCount();//GetStaffSegmentsCount();
    imCounterTotal( counter, count, "Prepare lyrics ..." );
	
	int st = this->GetStaffCount();
	m_opLines1 = new int[ st ];		// will hold the position for the top of each lyric subimage
	m_opLines2 = new int[ st ];		// will hold the position for the bottom of each lyric subimage
	memset( m_opLines1, 0, st * sizeof( int ) );
	memset( m_opLines2, 0, st * sizeof( int ) );
	
	int i, j;
	int maxLyricHeight = 0;
	
	//Calculate coordinates for removal of lyric subimages
	for ( i = 0; i < st; i++ ){
		m_opLines1[i] = this->m_staves[ i ].m_y - ( STAFF / 2 );
		
		if ( i + 1 < st ) {
			m_opLines2[i] = this->m_staves[ i + 1 ].m_y + ( STAFF / 2 );
			int height = m_opLines1[i] - m_opLines2[i]; 
			if ( height > maxLyricHeight )
				maxLyricHeight = height;			
		} else {
			if ( m_opLines1[i] > maxLyricHeight )
				m_opLines2[i] = m_opLines1[i] - maxLyricHeight;
			else
				m_opLines2[i] = 0;
		}
	}
	

	int windowWidth = 30;		// Window width used for finding lyric baseline 							
	
	// array holding overall projection of all the lyric lines
	double *overallProjection = (double*)malloc( maxLyricHeight * sizeof( double ) );				
	for ( i = 0; i < maxLyricHeight; i++ ) overallProjection[i] = 0;
	
	// array holding the offsets for each lyric line
	int **offsets = (int**)malloc( st * sizeof( int* ) );	
	for ( i = 0; i < st; i++ )
		offsets[i] = (int*)malloc( (int)ceil( m_opImMain->width / windowWidth ) * sizeof( int ) );
	for ( i = 0; i < st; i++ )
		for ( j = 0; j < ceil( m_opImMain->width / windowWidth ); j++ )
			offsets[i][j] = -1;

	// calculer les features (ImStaffSegment::CalcFeatures)
	wxString input = output_dir + "mfclyric.input";
	wxString filename = output_dir + "staff";
	wxFile finput( input, wxFile::write );
	if ( !finput.IsOpened() )
		return this->Terminate( ERR_FILE, input.c_str() );

	params.Clear();
    params.Add( m_opImMain );
	params.Add( &filename );
	params.Add( m_opLines1 );
	params.Add( m_opLines2 );
	params.Add( overallProjection );
	params.Add( offsets );
	params.Add( &windowWidth );
	
    ImStaffFunctor lyricFuncFeatures( &ImStaff::CalcLyricFeatures );
	this->Process( &lyricFuncFeatures, params, counter );
		
	// Find sum of overallProjection
	double sum = 0;		
	for ( i = 0; i < maxLyricHeight; i++ ){
		sum += overallProjection[i];
	}
									   
	// Find index of largest value in the projection and store in 'centre'
	int centre = 0;
	for ( i = 0; i < maxLyricHeight; i++ ){
		overallProjection[i] = overallProjection[i] / sum;
		if ( overallProjection[centre] < overallProjection[i] ) centre = i;
	}

	// Find top of lyric lines
	int topline = -1;
	for ( i = 0; i < centre; i++ ){
		if ( overallProjection[i] < exp( -6.0 ) )
			if ( topline == -1 || i > topline ) topline = i;
	}

	// Find bottom of lyric lines
	int baseline = -1;
	for ( i = centre; i < maxLyricHeight; i++ ){
		if ( overallProjection[i] < exp( -6.0 ) ){
			if ( baseline == -1 || i < baseline ) baseline = i;
		}
	}
	
	// Store the lyric attributes that were just found making necessary adjustments for each lyric line (offsets)  
	for ( i = 0; i < st; i++ ){
		int avg_offset = 0;
		int count = 0;
		for ( j = 0; j < ceil( m_opImMain->width / windowWidth ); j++ ){
			if ( offsets[i][j] > 0 ){
				avg_offset += offsets[i][j];
				count++;
			}
		}
		avg_offset /= count;
		this->m_staves[i].m_lyricBaseline = baseline + avg_offset + ( ( m_opLines1[i] - m_opLines2[i] ) / 2 );
		this->m_staves[i].m_lyricTopline = topline + avg_offset + ( ( m_opLines1[i] - m_opLines2[i] ) / 2 );
		this->m_staves[i].m_lyricCentre = centre + avg_offset + ( ( m_opLines1[i] - m_opLines2[i] ) / 2 );
	}
	
	int win = LYRIC_WIN_WIDTH;
	int overlap = LYRIC_WIN_OVERLAP;
	params.Clear();
	params.Add( m_opImMain );
	params.Add( &win );
	params.Add( &overlap );
	params.Add( &filename );
	params.Add( offsets );
	params.Add( &baseline );
	params.Add( &topline );
	params.Add( m_opLines1 );
	params.Add( m_opLines2 );
	params.Add( &windowWidth );
	params.Add( &finput );
	
	counter = m_progressDlg->GetCounter();
    count = this->m_staves.GetCount();//GetStaffSegmentsCount();
    imCounterTotal( counter, count, "Extract/Recognize lyrics ..." );	
	
	ImStaffFunctor lyricExtraction( &ImStaff::ExtractLyricImages );
	this->Process( &lyricExtraction, params, counter );
	finput.Close();

	free(overallProjection);	
	for ( i = 0; i < st; i++ ) free( offsets[i] );
	free(offsets);

    return this->Terminate( ERR_NONE );
}


void ImPage::Process(ImStaffFunctor *functor, wxArrayPtrVoid params, int counter )
{
    int nb_staves = (int)this->m_staves.GetCount();
    int st;
    for (st = 0; st < nb_staves; st++ )
    {
		if ( m_staves[st].IsVoid() )
			continue;
			
        //int stave_y = this->m_staves[st].m_y - ( STAFF_HEIGHT / 2 );
        functor->Call( &this->m_staves[st] , st , params);
		//this->m_staves[st].Process( functor, st, stave_y, params );
		
		if ( ( counter != -1 ) && !imCounterInc( this->m_progressDlg->GetCounter() ) )
		{
            this->Terminate( ERR_CANCELED );
			return;
		}	
    }
}

/*
void ImPage::Process(ImStaffFunctor *functor, wxArrayPtrVoid params, ImStaffSegmentFunctor *subfunctor, int counter )
{
    int nb_staves = (int)this->m_staves.GetCount();
    int st;
    for (st = 0; st < nb_staves; st++ )
    {
        int stave_y = this->m_staves[st].m_y - ( STAFF_HEIGHT / 2 );
		functor->Call( &this->m_staves[st] , st , stave_y, params, subfunctor);
		
		if (  ( counter != -1 ) && !imCounterInc( this->m_progressDlg->GetCounter() ) )
		{
            this->Terminate( ERR_CANCELED );
			return;
		}		
    }
}
*/


