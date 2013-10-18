/////////////////////////////////////////////////////////////////////////////
// Name:        mus.cpp
// Author:      Laurent Pugin
// Created:     2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "mus.h"

//----------------------------------------------------------------------------

#include "musdef.h"

//----------------------------------------------------------------------------

// AX_APP is define is the Aruspix wxWidget application only
#ifdef AX_APP
#include "wx/wxprec.h"
#include "app/axapp.h"
#endif

// Initialize static respource path
std::string Mus::m_respath = "/usr/local/share/aruspix";

//----------------------------------------------------------------------------
// Mus
//----------------------------------------------------------------------------

std::string Mus::GetAxVersion() {
#ifndef AX_APP
    return std::string("command line"); // we need to add versioning
#else
    return AxApp::s_version.mb_str();
#endif
}

std::string Mus::GetResourcesPath() {
#ifndef AX_APP
    //hardcode galore
    return m_respath;
#else
    return wxGetApp().m_resourcesPath.mb_str();
#endif
}

std::string Mus::GetMusicFontDescStr() {
#ifndef AX_APP
    return std::string("0;13;70;90;90;0;Leipzig 4.7;33");
#else
    return wxGetApp().m_musicFontDesc.mb_str();
#endif
}

std::string Mus::GetLyricFontDescStr() {
#ifndef AX_APP
    return std::string("0;12;70;93;90;0;Garamond;0");
#else
    return wxGetApp().m_lyricFontDesc.mb_str();
#endif
}


int Mus::GetFontPosCorrection(){
#ifndef AX_APP
    return 0;
#else
    return wxGetApp().m_fontPosCorrection;
#endif
}

std::string Mus::GetFileVersion(int vmaj, int vmin, int vrev) {
    return Mus::StringFormat("%04d.%04d.%04d", vmaj, vmin, vrev );
}

std::string Mus::StringFormat(const char *fmt, ...)
{
    std::string str( 2048, 0 );
    va_list args;
    va_start ( args, fmt );
    vsnprintf ( &str[0], 2048, fmt, args );
    va_end ( args );
    str.resize( strlen( str.data() ) );
    return str;
}

std::string Mus::GetFilename( std::string fullpath )
{
    // remove extension
    std::string name = fullpath;
    size_t lastdot = name.find_last_of(".");
    if (lastdot != std::string::npos) {
        name = name.substr(0, lastdot);
    }
    size_t lastslash = name.find_last_of("/");
    if (lastslash != std::string::npos) {
        name = name.substr(lastslash + 1, std::string::npos);
    }
    return name;
}

void Mus::LogDebug(const char *fmt, ...)
{
#ifdef EMSCRIPTEN
    return;
#endif
    va_list args;
    va_start ( args, fmt );
#ifndef AX_APP
    #if defined(DEBUG)
    vprintf( fmt, args );
    printf("\n");
    #endif
#else
    wxVLogDebug( fmt, args );
#endif
    va_end ( args );
}

void Mus::LogError(const char *fmt, ...)
{
#ifdef EMSCRIPTEN
    return;
#endif
    va_list args;
    va_start ( args, fmt );
#ifndef AX_APP
    vprintf( fmt, args );
    printf("\n");
#else
    wxVLogError( fmt, args );
#endif
    va_end ( args );
}

void Mus::LogMessage(const char *fmt, ...)
{
#ifdef EMSCRIPTEN
    return;
#endif
    va_list args;
    va_start ( args, fmt );
#ifndef AX_APP
    vprintf( fmt, args );
    printf("\n");
#else
    wxVLogMessage( fmt, args );
#endif
    va_end ( args );
}

void Mus::LogWarning(const char *fmt, ...)
{
#ifdef EMSCRIPTEN
    return;
#endif
    va_list args;
    va_start ( args, fmt );
#ifndef AX_APP
    vprintf( fmt, args );
    printf("\n");
#else
    wxVLogWarning( fmt, args );
#endif
    va_end ( args );
}

//----------------------------------------------------------------------------
// MusEnv
//----------------------------------------------------------------------------

MusEnv::MusEnv()
{
    m_landscape = false;
    m_staffLineWidth = 2;
    m_stemWidth = 3;
    m_barlineWidth = 3;
    m_beamWidth = 10;
    m_beamWhiteWidth = 5;
    m_beamMaxSlope = 30;
    m_beamMinSlope = 10;
    
    // originally in MusParameters2
    m_smallStaffNum = 16;
    m_smallStaffDen = 20;
    m_graceNum = 3;
    m_graceDen = 4;
	m_stemCorrection = 0;
    m_headerType = 0;
    
    // additional parameters
    m_notationMode = MUS_MENSURAL_MODE;
    //m_notationMode = MUS_CMN_MODE;
}

MusEnv::~MusEnv()
{
}
