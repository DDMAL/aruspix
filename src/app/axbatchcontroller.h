/////////////////////////////////////////////////////////////////////////////
// Name:        axbatchcontroller.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __axbatchcontroller_H__
#define __axbatchcontroller_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "axbatchcontroller.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "progressdlg.h"

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

// WDR: class declarations

//----------------------------------------------------------------------------
// AxBatchController
//----------------------------------------------------------------------------

class AxBatchController
{
public:
    // constructors and destructors
    AxBatchController::AxBatchController( );
    virtual AxBatchController::~AxBatchController();
	
    // WDR: method declarations for AxBatchController
    
public:
    // WDR: member variable declarations for AxBatchController
   
private:
    // WDR: handler declarations for AxBatchController

};

#endif
