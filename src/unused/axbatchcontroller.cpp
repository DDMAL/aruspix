/////////////////////////////////////////////////////////////////////////////
// Name:        axbatchcontroller.cpp
// Author:      Laurent Pugin
// Created:     04/05/25
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axbatchcontroller.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/file.h"

#include "axbatchcontroller.h"

#include "wx/ptr_scpd.h"
wxDEFINE_SCOPED_PTR_TYPE(wxZipEntry);


// WDR: class implementations

//----------------------------------------------------------------------------
// AxBatchController
//----------------------------------------------------------------------------

AxBatchController::AxBatchController()
{
}


AxBatchController::~AxBatchController()
{

}

// WDR: handler implementations for AxBatchController
