 /////////////////////////////////////////////////////////////////////////////
// Name:        vrv.h
// Author:      Laurent Pugin
// Created:     2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_H__
#define __MUS_H__

#include <string>

namespace vrv {

//----------------------------------------------------------------------------
// Vrv
//----------------------------------------------------------------------------

/**
 * This class provides static helpers 
 * Most of them differ if they are used in Aruspix or in the command line tool
 */

class Vrv
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
    
    /**
     * Utility for converting UTF16 (std::wstring) to UTF-8
     */
    static std::string UTF16to8(const wchar_t * in);

    /**
     * Utility for converting UTF-8 to UTF16 (std::wstring)
     */
    static std::wstring UTF8to16(const char * in);
    
    
private:
    static std::string m_respath;
};


//----------------------------------------------------------------------------
// MusEnv
//----------------------------------------------------------------------------

/** 
 * This class contains the document default environment variables.
 * Some of them are not available as is in MEI - to be solved
 */
class MusEnv 
{
public:
    // constructors and destructors
    MusEnv();
    virtual ~MusEnv();
    
    
public:
    /** The reference interline definition  */
    int m_interlDefin;
    /** The landscape paper orientation flag */
    char m_landscape;
    /** The staff line width */
    unsigned char m_staffLineWidth;
    /** The stem width */
    unsigned char m_stemWidth;
    /** The barline width */
    unsigned char m_barlineWidth;
    /** The beam width */
    unsigned char m_beamWidth;
    /** The beam white width */
    unsigned char m_beamWhiteWidth;
    /** The maximum beam slope */
    unsigned char m_beamMaxSlope;
    /** The minimum beam slope */
    unsigned char m_beamMinSlope;     
    /** The small staff size ratio numerator */
    unsigned char m_smallStaffNum;
    /** The small staff size ratio denominator */
    unsigned char m_smallStaffDen;
    /** The grace size ratio numerator */
    unsigned char m_graceNum;
    /** The grace size ratio denominator */
    unsigned char m_graceDen;
    /** The stem position correction */
    signed char m_stemCorrection;
    /** The header and footer type */
    unsigned int m_headerType;
    /** The notation mode (cmn or mensural) */
    int m_notationMode;
    /** The current page height */
    int m_pageHeight;
    /** The current page height */
    int m_pageWidth;
    /** The current page witdth */
    int m_pageLeftMar;
    /** The current page left margin */
    int m_pageRightMar;
    /** The current page right margin */
    int m_pageTopMar;
};

} // namespace vrv
    
#endif
