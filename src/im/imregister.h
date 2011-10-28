/////////////////////////////////////////////////////////////////////////////
// Name:        imregister.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __imregister_H__
#define __imregister_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "imregister.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "im/imoperator.h"

class ImPage;


//----------------------------------------------------------------------------
// ImRegister
//----------------------------------------------------------------------------

class ImRegister: public ImOperator 
{
public:
    // constructors and destructors
    ImRegister( wxString path, bool *isModified = NULL );
    virtual ~ImRegister();
    
        void Clear( );
	// we need to free extra image here...
	virtual bool Terminate( int code = 0, ... );
    // XML access
    bool Load( TiXmlElement *file_root );
    bool Save( TiXmlElement *file_root );
    // Processing
	bool DetectPoints( wxPoint *points1, wxPoint *points2 );
	bool Init( wxString filename1, wxString filename2 );
    bool Register( wxPoint *points1, wxPoint *points2 );
    
private:
    wxPoint CalcPositionAfterRotation( wxPoint point , float rot_alpha, 
                                  int w, int h, int new_w, int new_h);
	bool SubRegister( wxPoint origine, wxSize window, wxSize size, int level, int row, int column );


public:
    _imImage *m_src1; // original1 but processed image (resized, scaled, ...)
	_imImage *m_src2; // original2 but processed image (resized, scaled, ...)
	_imImage *m_result; // result image
	// additional images for processing
	_imImage *m_im1;
    _imImage *m_im2;
	wxPoint m_reg_points1[4];
    wxPoint m_reg_points2[4];
	bool *m_isModified;
	wxString m_path; // path of the SupFile
	// impage
	ImPage *m_imPage1Ptr;
	ImPage *m_imPage2Ptr;
    
private:
        int m_counter;
    int m_sub_register_total;

};

#endif
