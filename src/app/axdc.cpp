/////////////////////////////////////////////////////////////////////////////
// Name:        axdc.cpp
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma implementation "axdc.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/tokenzr.h"

#include "axdc.h"

//----------------------------------------------------------------------------
// AxFontInfo
//----------------------------------------------------------------------------

bool AxFontInfo::FromString(const wxString& s)
{
    long l;

    wxStringTokenizer tokenizer(s, _T(";"));

    wxString token = tokenizer.GetNextToken();
    //
    //  Ignore the version for now
    //

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    pointSize = (int)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    family = (int)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    style = (int)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    weight = (int)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    underlined = l != 0;

    faceName = tokenizer.GetNextToken();

#ifndef __WXMAC__
    if( !faceName )
        return false;
#endif

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    encoding = (int)l;

    return true;
}

wxString AxFontInfo::ToString() const
{
    wxString s;

    s.Printf(_T("%d;%d;%d;%d;%d;%d;%s;%d"),
             0,                                 // version
             pointSize,
             family,
             style,
             weight,
             underlined,
             faceName.GetData(),
             encoding);

    return s;
}


