/////////////////////////////////////////////////////////////////////////////
// Name:        imregister.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
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


// WDR: class declarations

//----------------------------------------------------------------------------
// ImRegister
//----------------------------------------------------------------------------

class ImRegister: public ImOperator 
{
public:
    // constructors and destructors
    ImRegister::ImRegister( wxString path, bool *isModified = NULL );
    virtual ImRegister::~ImRegister();
    
    // WDR: method declarations for ImRegister
    void ImRegister::Clear( );
	// we need to free extra image here...
	virtual bool Terminate( int code = 0, ... );
    // XML access
    bool ImRegister::Load( TiXmlElement *file_root );
    bool ImRegister::Save( TiXmlElement *file_root );
    // Processing
    bool ImRegister::Register( );
    
private:
    // WDR: member variable declarations for ImRegister
	wxPoint CalcPositionAfterRotation( wxPoint point , float rot_alpha, 
                                  int w, int h, int new_w, int new_h);


public:
    // WDR: member variable declarations for ImRegister
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
    
private:
    // WDR: handler declarations for ImRegister

};

#endif
