/////////////////////////////////////////////////////////////////////////////
// Name:        imoperator.cpp
// Author:      Laurent Pugin
// Created:     04/05/25
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "imoperator.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/ffile.h"
#include "wx/file.h"

#include "imoperator.h"

// WDR: class implementations


//----------------------------------------------------------------------------
// ImOperator
//----------------------------------------------------------------------------


ImOperator::ImOperator( )
{
    m_progressDlg = NULL;
    m_error = ERR_NONE;

    m_opImAlign = NULL;
    m_opImMask = NULL;
    m_opImTmp2 = NULL;
    m_opImTmp1 = NULL;
    m_opImMain = NULL;
    m_opIm = NULL;
    m_opImMap = NULL;

    m_opHist = NULL;
    m_opLines1 = NULL;
    m_opLines2 = NULL;
    m_opCols1 = NULL;
}

ImOperator::~ImOperator()
{
}

void ImOperator::SwapImages( _imImage **image1, _imImage **image2 )
{
    ImageDestroy( image1 );
    *image1 = *image2;
    *image2 = NULL;
}

void ImOperator::ImageDestroy( _imImage **image )
{
    if ( *image )
    {
        imImageDestroy( *image );
        *image = NULL;
    }

}

bool ImOperator::Terminate( int code, ... )
{
    // Attention que deux de ces pointeurs ne refere pas la meme adresse lors de l'appel de cette methode !
    ImageDestroy( &m_opImAlign );
    ImageDestroy( &m_opImMask );
    ImageDestroy( &m_opImTmp2 );
    ImageDestroy( &m_opImTmp1 );
    ImageDestroy( &m_opImMain );
    ImageDestroy( &m_opIm );
    ImageDestroy( &m_opImMap );

    if ( m_opHist )
        delete[] m_opHist;
    if ( m_opLines1 )
        delete[] m_opLines1;
    if ( m_opLines2 )
        delete[] m_opLines2;
    if ( m_opCols1 )
        delete[] m_opCols1;

    m_opHist = NULL;
    m_opLines1 = NULL;
    m_opLines2 = NULL;
    m_opCols1 = NULL;

    m_error = code;

    if ( code == ERR_NONE )
        return true; // normal ending


    // operation canceled
    if ( code == ERR_CANCELED )
    {
        wxLogMessage( _("Operation canceled") );
        return false;
    }

    // error
    va_list argptr;
    va_start( argptr, code );
    wxString msg;

    if ( code == ERR_UNKNOWN )
        msg = _("Unknown error");
    else if ( code == ERR_MEMORY )
        msg = _("Insufficient memory");
    else if ( code == ERR_FILE )
        msg.PrintfV( _("Error opening file '%s'"), argptr );
    else if ( code == ERR_READING )
        msg.PrintfV( _("Error reading image %d in file '%s'") , argptr );
    else if ( code == ERR_WRITING )
        msg.PrintfV( _("Error writing image in file '%s'") , argptr );
    
    va_end(argptr);
    wxLogError( msg );
    return false;
}

void ImOperator::SetProgressDlg( AxProgressDlg *dlg )
{
    wxASSERT_MSG( dlg, "Progress dialog pointer cannot be NULL");
    m_progressDlg = dlg;
}

void ImOperator::SetMapImage( _imImage *image )
{
    ImageDestroy( &m_opImMap );
    m_opImMap = imImageDuplicate( image );
}

bool ImOperator::Read( wxString file, _imImage **image, int index )
{
	wxASSERT_MSG( !file.IsEmpty(), "Filename  cannot be empty" );

    int error, width, height, color_mode, data_type;

    if ( image == NULL )
    {
        ImageDestroy( &m_opImMap );
        image = &m_opImMap;
    }

    imFile* ifile = imFileOpen( file.c_str(), &error );
    if ( !ifile )
        return this->Terminate( ERR_FILE , file.c_str());

    imFileReadImageInfo( ifile, index, &width, &height, &color_mode, &data_type );
    *image = imFileLoadBitmap( ifile, index, &error);
    imFileClose(ifile);
    if (!*image)
        return this->Terminate( ERR_READING , index, file.c_str());
    else 
        return true;
}

bool ImOperator::ExtractPlane( _imImage **image, _imImage **extrated_plane, int plane_number  )
{
	if ( !ConvertToMAP( image ) )
		return false;

	imImage *main_plane = imImageClone( *image );
    if (!main_plane)
        return this->Terminate( ERR_MEMORY );

    imProcessBitPlane( *image, main_plane, 0, 0);
    imProcessBitPlane( *image, *image, 0, 1); // reset
    imProcessBitwiseNot( main_plane, main_plane );
    imProcessBitwiseOp( main_plane, *extrated_plane, main_plane, IM_BIT_OR );
    imProcessBitwiseNot( main_plane, main_plane );
    imProcessArithmeticOp( *image, main_plane, *image, IM_BIN_ADD );
        
    for (int i = 0; i < plane_number; i ++ )
    {
        imProcessUnArithmeticOp( *extrated_plane, *extrated_plane, IM_UN_INC );
    }
    imProcessBitPlane( *image, *image, plane_number, 1); // reset
    imProcessArithmeticOp( *image, *extrated_plane, *image, IM_BIN_ADD );
    imImageDestroy( main_plane );
	
	return true;
}


bool ImOperator::ConvertToMAP( _imImage **image )
{
    if ( !imColorModeMatch( (*image)->color_space, IM_MAP ) ) // binary image as input
    {
        imImage *tmp = imImageCreate( (*image)->width, (*image)->height, IM_MAP, IM_BYTE );
        if (!tmp)
            return this->Terminate( ERR_MEMORY );
			
        imImageSetBinary( *image );
        imImageClear( tmp );
        imProcessArithmeticOp( tmp, *image, tmp, IM_BIN_ADD );
		SwapImages( image, &tmp );
    }

    long *pal = imPaletteGray();
    pal[0] = imColorEncode( 255, 255, 255 ); // fond blanc
    pal[1] = imColorEncode( 0, 0, 0 ); // noir
    pal[2] = imColorEncode( 228, 228, 228 ); // gris = bord
    pal[4] = imColorEncode( 0, 127, 0 ); // vert fonce = lettrine
    pal[8] = imColorEncode( 0, 255, 0 ); // vert clair = texte dans portee
    pal[16] = imColorEncode( 255, 127, 0 ); // orange = texte
    pal[32] = imColorEncode( 255, 227, 0 ); // jaune = titre
    pal[64] = imColorEncode( 0, 0, 255 ); // bleu =
    pal[128] = imColorEncode( 255, 0, 255 ); // magenta =
    imImageSetPalette( *image, pal, 256 );
	
	return true;
}


bool ImOperator::WriteAsMAP( wxString file, _imImage **image )
{
	wxASSERT_MSG( !file.IsEmpty(), "Filename  cannot be empty" );

	if ( !ConvertToMAP( image ) )
		return false;

    int error;
    imFile *ifile = imFileNew( file.c_str(), "TIFF", &error);
    if (error == IM_ERR_NONE)
    {
        imFileSetInfo( ifile, "RLE" ); // LZW Unisys par defaut
        imImageSetAttribute( *image, "Software", IM_BYTE, 8, "Aruspix" );
        imImageSetAttribute( *image, "Author", IM_BYTE, 14, "Laurent Pugin" );
        //imImageSetAttribute( *image, "Photometric", IM_BYTE,1,"1");
        error = imFileSaveImage( ifile, *image );
        imFileClose(ifile);
        //wxLogMessage("Fichier '%s' ecrit - erreur %d", file.c_str(), error );
    }

    if (error == IM_ERR_NONE)
        return true;
    else
        return this->Terminate( ERR_WRITING , file.c_str());
}


bool ImOperator::Write( wxString file, _imImage **image )
{
	wxASSERT_MSG( !file.IsEmpty(), "Filename  cannot be empty" );

    int error;
    
#if !defined(__WXMSW__) 
    //wxFFile f( file, "w" ); // necessaire sous Linux pour les masques de fichiers
    //f.Flush();
    //f.Close();
    //wxYield();
#endif

	//printf(file.c_str());
    imFile *ifile = imFileNew( file.c_str(), "TIFF", &error);
    if (error == IM_ERR_NONE)
    {
        imFileSetInfo( ifile, "RLE" ); // LZW Unisys par defaut
        imImageSetAttribute( *image, "Software", IM_BYTE, 8, "Aruspix" );
        imImageSetAttribute( *image, "Author", IM_BYTE, 14, "Laurent Pugin" );
        imImageSetAttribute( *image, "Photometric", IM_BYTE,1,"1");
        error = imFileSaveImage( ifile, *image );
		imFileClose(ifile);
        //wxLogMessage("Fichier '%s - erreur %d' ecrit", file.c_str(), error );
        if (error == IM_ERR_NONE)
            return true;
        else
            return this->Terminate( ERR_WRITING , file.c_str());
        
    }
    else
        return this->Terminate( ERR_WRITING , file.c_str());
}

bool ImOperator::GetImagePlane( _imImage **image , int plane, int factor )
{
    wxASSERT_MSG( !(*image), "Image pointer must be NULL");

    imImage *imTmp;

    if ( m_opImMap == NULL )
        return this->Terminate( ERR_UNKNOWN );
    
    ImageDestroy( image );
    *image = imImageCreate( m_opImMap->width, m_opImMap->height, IM_BINARY, IM_BYTE );
    if ( !*image )
        return this->Terminate( ERR_MEMORY );
    imProcessBitPlane( m_opImMap, *image, plane, 0 );

    // resize
    for(int i = 1; i < factor; i*= 2 )
    {
        int removeX = (*image)->width % 2;
        int removeY = (*image)->height % 2;
        if (removeX || removeY) // controler si width ou height sont impaires -> si oui, crop image
        {
            imTmp = imImageCreate( (*image)->width - removeX, (*image)->height - removeY,
                (*image)->color_space, (*image)->data_type);
            if ( !imTmp )
                return this->Terminate( ERR_MEMORY );

            imProcessCrop( *image, imTmp, 0, 0);
            SwapImages( image, &imTmp );
        }

        imTmp = imImageCreate( (*image)->width / 2, (*image)->height /2, IM_BINARY, IM_BYTE );
        if ( !imTmp )
            return this->Terminate( ERR_MEMORY );

        imProcessReduceBy4( *image , imTmp );
        SwapImages( image, &imTmp );
    }

    return true;
}


bool ImOperator::GetImage( _imImage **image, int factor,  int binary_method, bool median_filtering )
{
    wxASSERT_MSG( !(*image), "Image pointer must be NULL");

    imImage *imTmp;

    int color_type = IM_GRAY;
    if ( binary_method != -1 )
        color_type = IM_BINARY;

    if ( m_opImMap == NULL )
        return this->Terminate( ERR_UNKNOWN );
    
    ImageDestroy( image );
    *image = imImageDuplicate( m_opImMap );
    if ( !*image )
        return this->Terminate( ERR_MEMORY );

    // binary
    if ( (binary_method != -1) && !imColorModeMatch( (*image)->color_space, IM_BINARY ) )
    {
        imTmp = imImageCreate( (*image)->width, (*image)->height, IM_BINARY, IM_BYTE);
        if ( !imTmp )
            return this->Terminate( ERR_MEMORY );
		if ( binary_method == IM_BINARIZATION_OTSU )
			imProcessOtsuThreshold( *image, imTmp );
		else if ( binary_method == IM_BINARIZATION_MINMAX )
			imProcessMinMaxThreshold( *image, imTmp );
		else if ( binary_method == IM_BINARIZATION_BRINK )
			imProcessBrinkThreshold( *image, imTmp, false );
		else if ( binary_method == IM_BINARIZATION_BRINK3CLASSES )
		{
			wxLogMessage("Brink 3 Classes Binarization (Resize)" );
			imProcessBrinkThreshold( *image, imTmp, false );
		}
		else // should not happen, but just in case
		{	
			wxLogWarning("Fix threshold used when resizing" );
			imProcessThreshold( m_opImMain, m_opImTmp1, 127, 1);
		}
        SwapImages( image, &imTmp );
    }


    // resize
    for(int i = 1; i < factor; i*= 2 )
    {
        int removeX = (*image)->width % 2;
        int removeY = (*image)->height % 2;
        if (removeX || removeY) // controler si width ou height sont impaires -> si oui, crop image
        {
            imTmp = imImageCreate( (*image)->width - removeX, (*image)->height - removeY,
                (*image)->color_space, (*image)->data_type);
            if ( !imTmp )
                return this->Terminate( ERR_MEMORY );

            imProcessCrop( *image, imTmp, 0, 0);
            SwapImages( image, &imTmp );
        }

        imTmp = imImageCreate( (*image)->width / 2, (*image)->height /2, color_type, IM_BYTE );
        if ( !imTmp )
            return this->Terminate( ERR_MEMORY );

        imProcessReduceBy4( *image , imTmp );
        SwapImages( image, &imTmp );
    }

    // median filtering
    if ( median_filtering == true )
    {
        imTmp = imImageClone( *image );
        if ( !imTmp )
            return this->Terminate( ERR_MEMORY );
        if ( !imProcessMedianConvolve( *image ,imTmp, 3 ) )
            return this->Terminate( ERR_CANCELED );
        SwapImages( image, &imTmp );
    }

    return true;
}


void ImOperator::PruneElementsZone( _imImage *image, int min_threshold, int max_threshold, int type )
{
    imImage *region_image = imImageCreate(image->width, image->height, IM_GRAY, IM_USHORT);
    if (!region_image)
        return;

    int region_count = imAnalyzeFindRegions(image, region_image, 4, 1);
    if (region_count)
    {
        if  ( type == IM_PRUNE_CLEAR_HEIGHT ) // min height
            imAnalyzeClearHeight(region_image, region_count, min_threshold, max_threshold);
        else if  ( type == IM_PRUNE_CLEAR_WIDTH ) // min width
            imAnalyzeClearWidth(region_image, region_count, min_threshold, max_threshold);
		else // IM_PRUNE_CLEAR_MIN
			imAnalyzeClearMin(region_image, region_count, min_threshold );

        imushort* region_data = (imushort*)region_image->data[0];
        imbyte* img_data = (imbyte*)image->data[0];

        for (int i = 0; i < image->count; i++)
        {
            if (*region_data)
                *img_data = 1;
            else
                *img_data = 0;
            region_data++;
            img_data++;
        }
    }
    imImageDestroy(region_image);
}


void ImOperator::MoveElements( _imImage *src, _imImage *dest, int boxes[], int count, int margins[4], int factor )
{
    int i;
    for (i = 0; i < count * 4; i += 4)
    {
        if ( (boxes[i+1] <= boxes[i+0]) || (boxes[i+3] <= boxes[i+2]) )
            continue;

        // bounding box
        imImage *box = imImageCreate( 
            (boxes[i+1] - boxes[i+0]) * factor, 
            (boxes[i+3] - boxes[i+2]) * factor, 
            src->color_space, src->data_type);
        if (!box)
            return;
        imImageClear( box ); // zone blanche
        imProcessBitwiseNot( box, box );

        int mx1 = max( factor * boxes[i+0] - margins[0] , 0 );
        int mx2 = min( factor * boxes[i+1] + margins[1] , src->width - 1 );
        int my1 = max( factor * boxes[i+2] - margins[2] , 0 );
        int my2 = min( factor * boxes[i+3] + margins[3] , src->height - 1  );
        int mmx1 = factor * boxes[i+0] - mx1; 
        int mmy1 = factor * boxes[i+2] - my1;

        // bounding box avec marges d'effacements
        imImage *box_m1 = imImageCreate( mx2 - mx1, my2 - my1, src->color_space, src->data_type);
        if (!box_m1)
        {   
            imImageDestroy( box );
            return;
        }
        imProcessCrop( src, box_m1, mx1, my1); // copie de l'image d'origine
        imSetData( box_m1, box, mmx1, mmy1 );  // zone blanche à l'interieure des marges

        // box avec marges d'effacement + 1 pixels supplementaire de marge -> les zones ne doivent pas
        // toucher le bord pour Prune
        imImage *box_mm1 = imImageCreate( mx2 - mx1 + 2, my2 - my1 + 2, src->color_space, src->data_type);
        if (!box_mm1)
        {   
            imImageDestroy( box );
            imImageDestroy( box_m1 );
            return;
        }
        imProcessAddMargins( box_m1, box_mm1, 1, 1);
        // effacement des pixels non connects -> zones plus petites que le box
        // attention si les marges sont larges !! eventuellement verifier le centroide ?
        imProcessPrune( box_mm1, box_mm1, 4, box->height * box->width , 0 ); 

        // suppression des marges d'un pixel supplementaire
        imProcessCrop( box_mm1, box_m1, 1, 1);
        // copie de la zone depuis l'image source vers la bounding box
        imProcessCrop( src, box, mx1 + mmx1, my1 + mmy1 );
        // copie de la bounding box vers la zone avec marges d'effacement
        imSetData( box_m1, box, mmx1, mmy1 );
        // copie dans l'image destination
        imSetData( dest, box_m1, mx1, my1 );

        // effacement dans l'image source
        imImage *box_m2 = imImageCreate( mx2 - mx1, my2 - my1, src->color_space, src->data_type);
        if (!box_m2)
        {   
            imImageDestroy( box );
            imImageDestroy( box_m1 );
            imImageDestroy( box_mm1 );
            return;
        }
        imProcessCrop( src, box_m2, mx1, my1);
        imProcessBitwiseNot( box_m1, box_m1 );
        imProcessBitwiseOp( box_m1, box_m2, box_m2, IM_BIT_AND );
        imSetData( src, box_m2, mx1, my1 );
        
        imImageDestroy( box );
        imImageDestroy( box_m1 );
        imImageDestroy( box_m2 );
        imImageDestroy( box_mm1 );
    }
}


// don't works on binary images !!!!!!!
/*
void ImOperator::DistByCorrelationFFT(const _imImage *im1, const _imImage *im2,
                                wxSize window, int *decalageX, int *decalageY)
{
    wxASSERT_MSG(decalageX, wxT("decalageX cannot be NULL") );
    wxASSERT_MSG(decalageY, wxT("decalagY cannot be NULL") );
    wxASSERT_MSG(im1, wxT("Image 1 cannot be NULL") );
    wxASSERT_MSG(im2, wxT("Image 2 cannot be NULL") );
    wxASSERT_MSG(im1->palette_count!=2, wxT("Image 1 cannot be binary") );
    wxASSERT_MSG(im2->palette_count!=2, wxT("Image 2 cannot be binary") );

    
    //imImage *im1 = GetImImage(&image1, IM_GRAY);
    //imImage *im2 = GetImImage(&image2, IM_GRAY);

    imImage *corr = imImageCreate( im1->width, im1->height, IM_GRAY, IM_CFLOAT);
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


/// works on binary images 
void ImOperator::DistByCorrelation( _imImage *im1, _imImage *im2,
                                wxSize window, int *decalageX, int *decalageY, int *maxCorr)
{
    wxASSERT_MSG(decalageX, wxT("decalageX cannot be NULL") );
    wxASSERT_MSG(decalageY, wxT("decalagY cannot be NULL") );
    wxASSERT_MSG(maxCorr, wxT("maxCorr cannot be NULL") );
    wxASSERT_MSG(im1, wxT("Image 1 cannot be NULL") );
    wxASSERT_MSG(im2, wxT("Image 2 cannot be NULL") );
	
	//imProcessNegative( im1, im1 );
	//imProcessNegative( im2, im2 );

    // zero padding
	imImage *imTmp1 = imImageCreate(
            im1->width +  window.GetWidth() * 2,
            im1->height +  window.GetHeight() * 2,
            im1->color_space, im1->data_type);
	imProcessAddMargins(im1 ,imTmp1, window.GetWidth(), window.GetHeight());

    int conv_width = 2 * window.GetWidth();
    int conv_height = 2 * window.GetHeight();
    imImage *mask = imImageCreate(im2->width, im2->height,im2->color_space, im2->data_type);
    imbyte *bufIm2 = (imbyte*)im2->data[0];
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
                //sum += (bufIm2[i]) * (bufMask[i]) / (255 * 255);
            }
            if (sum > maxSum)
            {
				//wxLogDebug("Sum %d", sum );
                maxSum = sum;
                maxX = x;
                maxY = y;
            }
        }
    }

    *decalageX = maxX - window.GetWidth();
    *decalageY = maxY - window.GetHeight();
    *maxCorr = maxSum;
    imImageDestroy(imTmp1);
    imImageDestroy(mask);
}


void ImOperator::MedianFilter( int values[], int size, int filter_size, int *avg_ptr )
{
    int i;
    int pos, current_size;
    int half_size = filter_size / 2;
    int *tmp = new int[size];
    int avg = 0;

    for ( i = 0; i < size; i++ )
    {
        pos = ( i - half_size < 0 ) ? 0 : i - half_size;
        current_size = ( pos + filter_size > size - 1) ? size - pos : filter_size;
        int *win = new int[ current_size ];
        //memset( win, 0, sizeof(int)*current_size );
        //int win[10] = {1,2,3,4,5,6,7,8,9,0};
        memcpy( win, values + pos, sizeof(int)*current_size);
        tmp[i] = median( win, current_size );
        avg += tmp[i];
        delete[] win;
    }
    memcpy( values, tmp, sizeof(int) * size);
    delete[] tmp;

    avg = avg / size;
    if ( avg_ptr )
        *avg_ptr = avg;
}

// WDR: handler implementations for ImOperator
