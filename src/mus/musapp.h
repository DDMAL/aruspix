/////////////////////////////////////////////////////////////////////////////
// Name:        musapp.h
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_APP_H__
#define __MUS_APP_H__

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "muslayer.h"

class MusLayerRdg;

//----------------------------------------------------------------------------
// MusLayerApp
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <app> element within a <layer> element.
 */
class MusLayerApp: public MusLayerElement
{
public:
    // constructors and destructors
    MusLayerApp();
    virtual ~MusLayerApp();
    
    void AddLayerRdg( MusLayerRdg *layerRdg );
    
private:
    
public:
    /** The children MusLayerRdg objects */
    ArrayOfMusLayerRdgs m_rdgs;

private:
    
};

//----------------------------------------------------------------------------
// MusLayerRdg
//----------------------------------------------------------------------------

/** 
 * This class models the MEI <rdg> element for a <app> in a <layer>
 */
class MusLayerRdg: public MusLayer
{
public:
    // constructors and destructors
    MusLayerRdg();
    virtual ~MusLayerRdg();
    
    /** the parent MusLayerApp setter */
    void SetLayerApp( MusLayerApp *app ) { m_app = app; };  
    
private:
    
public:
    /** The children MusLayerElement objects */
    //ArrayOfMusLayerElements m_elements;
    /** The parent MusLayerApp */
    MusLayerApp *m_app;
    /** The source id */
    wxString m_srcId;
    
private:
    
};


#endif
