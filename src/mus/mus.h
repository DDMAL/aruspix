 /////////////////////////////////////////////////////////////////////////////
// Name:        mus.h
// Author:      Laurent Pugin
// Created:     2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_H__
#define __MUS_H__

#include <string>

//----------------------------------------------------------------------------
// Mus
//----------------------------------------------------------------------------

/**
 * This class provides static helpers 
 * Most of them differ if they are used in Aruspix or in the command line tool
 */

class Mus
{
public:
    // Some statis utiliry functions
    static std::string GetAxVersion();
    static std::string GetResourcesPath();
    static void SetResourcesPath(std::string p) {m_respath = p;}
    static std::string GetMusicFontDescStr();
    static std::string GetNeumeFontDescStr();
    static std::string GetLyricFontDescStr();
    static int GetFontPosCorrection();
    static std::string GetFileVersion(int vmaj, int vmin, int vrev);
    static std::string StringFormat( const char *fmt, ... );
    static std::string GetFilename( std::string fullpath );
    
    static void LogDebug(  const char *fmt, ... );
    static void LogError(  const char *fmt, ... );
    static void LogMessage(  const char *fmt, ... );
    static void LogWarning(  const char *fmt, ... );
    
    /**
     * Utility for comparing doubles
     */
    static bool AreEqual(double dFirstVal, double dSecondVal);
    
private:
    static std::string m_respath;
};


//----------------------------------------------------------------------------
// MusEnv
//----------------------------------------------------------------------------

/** 
 * This class contains the document environment variables.
 * It remains from the Wolfgang parameters strcuture.
 * TODO - Integrate them in MEI.
 */
class MusEnv 
{
public:
    // constructors and destructors
    MusEnv();
    virtual ~MusEnv();
    
    
public:
    /** landscape paper orientation */
    char m_landscape;
    /** staff line width */
    unsigned char m_staffLineWidth;
    /** stem width */
    unsigned char m_stemWidth;
    /** barline width */
    unsigned char m_barlineWidth;
    /** beam width */
    unsigned char m_beamWidth;
    /** beam white width */
    unsigned char m_beamWhiteWidth;
    /** maximum beam slope */
    unsigned char m_beamMaxSlope;
    /** minimum beam slope */
    unsigned char m_beamMinSlope;     
    /** small staff size ratio numerator */
    unsigned char m_smallStaffNum;
    /** small staff size ratio denominator */
    unsigned char m_smallStaffDen;
    /** grace size ratio numerator */
    unsigned char m_graceNum;
    /** grace size ratio denominator */
    unsigned char m_graceDen;
    /** stem position correction */
    signed char m_stemCorrection;
    /** header and footer type */
    unsigned int m_headerType;
    /** notation mode. Since since Aruspix 1.6.1 */
    int m_notationMode;
};


#endif
