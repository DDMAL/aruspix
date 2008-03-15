/////////////////////////////////////////////////////////////////////////////
// Name:        imoperator.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __imoperator_H__
#define __imoperator_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "imoperator.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "app/axprogressdlg.h"
//struct _imImage;

// IMLIB
#include <im.h>
#include <im_counter.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_util.h>
#include <im_binfile.h>
#include <im_math_op.h>
#include <im_palette.h>

#include "imext.h"

#define MAX_STAVES 24
#define STAVES_CONV_REDUCTION 2
#define STAFF_HEIGHT 180
#define STAFF 100
#define STAFF_INTERLIGN 13

#define IMAGE_MUSIC 0
#define IMAGE_BLANK 1
#define IMAGE_ORNATE_LETTER 2
#define IMAGE_TEXT_IN_STAFF 3
#define IMAGE_LYRICS 4
#define IMAGE_TITLE 5

#define IMAGE_PLANES IMAGE_TITLE

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

enum
{
	IM_BINARIZATION_OTSU = 0,
	IM_BINARIZATION_MINMAX,
	IM_BINARIZATION_BRINK
};

enum
{
	IM_PRUNE_CLEAR_HEIGHT = 0,
	IM_PRUNE_CLEAR_WIDTH,
	IM_PRUNE_CLEAR_MIN
};

// WDR: class declarations


//----------------------------------------------------------------------------
// ImOperator
//----------------------------------------------------------------------------

#define AX_PI 3.14159265358979323846

class ImOperator
{
public:
    // constructors and destructors
	ImOperator();
    virtual ~ImOperator();
    
    // WDR: method declarations for ImOperator
    int GetError( ) { return m_error; }
	void SetProgressDlg( AxProgressDlg *dlg );
    void SetMapImage( _imImage *image );
    //wxString GetShortName() { return m_shortname; }
    
protected:
    // WDR: member variable declarations for ImOperator
    void MedianFilter( int values[], int size, int filter_size, int *avg_ptr = NULL);
    void PruneElementsZone( _imImage *image, int min_threshold, int max_threshold, int type = IM_PRUNE_CLEAR_HEIGHT );
    void MoveElements( _imImage *src, _imImage *dest, int bounding_boxes[],
        int count, int margins[4], int factor = 1 );
    void DistByCorrelation(const _imImage *image1, const _imImage *image2,
                                wxSize window, int *decalageX, int *decalageY, int *max);
    //void DistByCorrelationFFT(const _imImage *image1, const _imImage *image2,
    //                            wxSize window, int *decalageX, int *decalageY);
    
	// Memory managment methods
    bool Terminate( int code = 0, ... );
    bool GetImagePlane( _imImage **image , int plane = 0, int factor = 1 );
    bool GetImage( _imImage **image, int factor = 1 , int binary_method = -1, bool median_filtering = false );
    bool Read( wxString file, _imImage **image, int index );
    bool Write( wxString file, _imImage **image );
    bool WriteAsMAP( wxString file, _imImage **image );
	bool ExtractPlane( _imImage **image, _imImage **extrated_plane, int plane_number  );
	bool ConvertToMAP( _imImage **image );
    void SwapImages( _imImage **image1, _imImage **image2 );
    void ImageDestroy( _imImage **image );

    // deg2rad
    inline double deg2rad( double deg ) { return deg * AX_PI / 180.0; }

protected:
    // WDR: handler declarations for ImOperator
    AxProgressDlg *m_progressDlg;
    int m_error;

    _imImage *m_opImMap;
    _imImage *m_opIm;
    _imImage *m_opImMain;
    _imImage *m_opImTmp1;
    _imImage *m_opImTmp2;
    _imImage *m_opImMask;
    _imImage *m_opImAlign;

    int *m_opHist;
    int *m_opLines1;
    int *m_opLines2;
    int *m_opCols1;


public: // DEBUG variables
    wxString m_inputfile; // utilise dans la methode Read
};

#endif
