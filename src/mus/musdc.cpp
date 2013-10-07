/////////////////////////////////////////////////////////////////////////////
// Name:        musdc.cpp
// Author:      Laurent Pugin
// Created:     2010
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musdc.h"

//----------------------------------------------------------------------------

#include <sstream>
#include <string>

//----------------------------------------------------------------------------

#include "musdoc.h"

//----------------------------------------------------------------------------
// MusFontInfo
//----------------------------------------------------------------------------

bool MusFontInfo::FromString(const std::string& ss)
{
    std::istringstream iss( ss );
    std::string token;

    getline( iss, token, ';');
    //
    //  Ignore the version for now
    //

    getline( iss, token, ';');
    pointSize = atoi( token.c_str() );

    getline( iss, token, ';');
    family = atoi( token.c_str() );

    getline( iss, token, ';');
    style = atoi( token.c_str() );

    getline( iss, token, ';');
    weight = atoi( token.c_str() );

    getline( iss, token, ';');
    underlined = (atoi( token.c_str() ) != 0);

    getline( iss, token, ';');
    faceName = token;
#ifndef __WXMAC__
    if( faceName.empty() )
        return false;
#endif

    getline( iss, token, ';');
    encoding = atoi( token.c_str() );;

    return true;
}

std::string MusFontInfo::ToString() const
{
    std::string s = Mus::StringFormat( "%d;%d;%d;%d;%d;%d;%s;%d",
             0,                                 // version
             pointSize,
             family,
             style,
             weight,
             underlined,
             faceName.c_str(),
             encoding);

    return s;
}


