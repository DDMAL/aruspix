/////////////////////////////////////////////////////////////////////////////
// Name:        muscontroller.h
// Author:      Laurent Pugin
// Created:     17/10/2013
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_CONTROLLER_H__
#define __MUS_CONTROLLER_H__

#include "musdoc.h"
#include "musrc.h"

#include <string>

typedef enum _file_types {
    mei_file = 0,
    pae_file,
    darms_file
} ConvertFileType;

//----------------------------------------------------------------------------
// MusController
//----------------------------------------------------------------------------

class MusController
{
public:
    // constructors and destructors
    MusController();
    virtual ~MusController();
    
    /**
     * Load a file with the specified type.
     */
    bool LoadFile( std::string filename, ConvertFileType type );
    
    /**
     * Load a string data witht he specified type.
     */
    bool LoadString( std::string data, ConvertFileType type );
    
    /**
     * Render the page in SVG and returns it as a string
     */
    std::string RenderToSvg( int pageNo =  1, bool xml_tag = false );

    /**
     * Render the page in SVG and save it to the file.
     */
    bool RenderToSvgFile( std::string filename, int pageNo =  1 );

    /**
     * @name Set and get a std::string into a char * buffer.
     * This is used for returning a string buffer to emscripten.
     * The buffer is freed when reset or in MusController destructor.
     */
    ///@{
    void SetCString( std::string data );
    char *GetCString( );
    ///@}
    
    /**
     * @name Set and get the border
     */
    ///@{
    void SetBorder( int border ) { m_border = border; };
    int GetBorder() { return m_border; };
    ///@}
    
    /**
     * @name Set and get the scale
     */
    ///@{
    void SetScale( int scale ) { m_scale = scale; };
    int GetScale() { return m_scale; };
    ///@}
    
private:
    
public:
    
private:
    MusDoc m_doc;
    MusRC m_rc;
    int m_border;
    int m_scale;
    /**
     *
     */
    char *m_cString;
};

#endif
