/////////////////////////////////////////////////////////////////////////////
// Name:        musobject.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musobject.h"
#include "musrc.h"
#include "musdiv.h"
#include "muslayout.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLayoutObjects );

#include <algorithm>
using std::min;
using std::max;

//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

MusObject::MusObject() :
	wxObject()
{
    m_active = true;
    uuid_generate( m_uuid );
}

MusObject::~MusObject()
{
}

void MusObject::SetUuid( uuid_t uuid )
{ 
    uuid_copy( m_uuid, uuid );
};

wxString MusObject::GetUuidStr()
{
    char uuidStr[37];
    uuid_unparse( m_uuid, uuidStr ); 
    return wxString( uuidStr );
}

bool MusObject::FindWithUuid( wxArrayPtrVoid params )
{
    // param 0: the uuid we are looking for
    // parma 1: the pointer to the element
    uuid_t *uuid = (uuid_t*)params[0];  
    MusObject **element = (MusObject**)params[1];  
    
    if ( (*element) ) {
        return true;
    }
    
    if ( uuid_compare( *uuid, *this->GetUuid() ) == 0 ) {
        (*element) = this;
        //wxLogDebug("Found it!");
        return true;
    }
    //wxLogDebug("Still looking for uuid...");
    return false;
}

bool MusObject::ReplaceUuid( wxArrayPtrVoid params )
{
    // param 0: the uuid we are looking for (and have to replace
    // param 1: the uuid we will replace with
    // parma 2: the pointer to the element (when found)
    uuid_t *uuidSrc = (uuid_t*)params[0];
    uuid_t *uuidDst = (uuid_t*)params[1];
    MusObject **element = (MusObject**)params[2];  
    
    if ( (*element) ) {
        return true;
    }
    
    if ( uuid_compare( *uuidSrc, *this->GetUuid() ) == 0 ) {
        (*element) = this;
        this->SetUuid( *uuidDst );
        //wxLogDebug("Replaced it!");
        return true;
    }
    //wxLogDebug("Still looking for uuid...");
    return false;
}

bool MusObject::CheckFunctor( wxArrayPtrVoid params )
{
    this->Check();
    return false;
}


//----------------------------------------------------------------------------
// MusFunctor
//----------------------------------------------------------------------------

MusFunctor::MusFunctor( )
{ 
    m_success=false;
    m_reverse=false;
    obj_fpt=NULL; 
    obj_fpt_bool=NULL; 
}

MusFunctor::MusFunctor( void(MusObject::*_obj_fpt)( wxArrayPtrVoid ))
{ 
    m_success=false;
    m_reverse=false;
    obj_fpt=_obj_fpt; 
    obj_fpt_bool=NULL; 
}

MusFunctor::MusFunctor( bool(MusObject::*_obj_fpt_bool)( wxArrayPtrVoid ))
{ 
    m_success=false;
    m_reverse=false;
    obj_fpt_bool=_obj_fpt_bool; 
    obj_fpt=NULL;
}


void MusFunctor::Call( MusObject *ptr, wxArrayPtrVoid params )
{ 
    if (obj_fpt) {
        (*ptr.*obj_fpt)( params );
    }
    else if (obj_fpt_bool)
    {
        m_success = (*ptr.*obj_fpt_bool)( params );
    }
}

//----------------------------------------------------------------------------
// MusLogicalObject
//----------------------------------------------------------------------------

MusLogicalObject::MusLogicalObject() :
	MusObject()
{
    m_div = NULL;
}

MusLogicalObject::~MusLogicalObject()
{
}


bool MusLogicalObject::Check()
{
    wxASSERT( m_div );
    return m_div;
}


void MusLogicalObject::SetDiv( wxArrayPtrVoid params )
{
    // param 0: MusDiv
    wxASSERT( dynamic_cast<MusDiv*>((MusDiv*)params[0]) ); 
    
    m_div = (MusDiv*)params[0];  
}


//----------------------------------------------------------------------------
// MusLayoutObject
//----------------------------------------------------------------------------

MusLayoutObject::MusLayoutObject() :
	MusObject()
{
	m_layout = NULL;
    ResetBB();
}

MusLayoutObject::~MusLayoutObject()
{
}

bool MusLayoutObject::Check()
{
    wxASSERT( m_layout );
    return m_layout;
}

void MusLayoutObject::SetLayout( wxArrayPtrVoid params )
{
    // param 0: MusLayout
    wxASSERT( dynamic_cast<MusLayout*>((MusLayout*)params[0]) ); 
    
    m_layout = (MusLayout*)params[0];  
    //ResetBB();
}


void MusLayoutObject::UpdateContentBB( int x1, int y1, int x2, int y2) 
{
    //printf("CB Was: %i %i %i %i\n", m_contentBB_x1, m_contentBB_y1, m_contentBB_x2 ,m_contentBB_y2);
    
    int min_x = min( x1, x2 );
    int max_x = max( x1, x2 );
    int min_y = min( y1, y2 );
    int max_y = max( y1, y2 );
    
    if (m_contentBB_x1 > min_x) m_contentBB_x1 = min_x;
    if (m_contentBB_y1 > min_y) m_contentBB_y1 = min_y;
    if (m_contentBB_x2 < max_x) m_contentBB_x2 = max_x;
    if (m_contentBB_y2 < max_y) m_contentBB_y2 = max_y;
    
    m_updatedBB = true;
    //printf("CB Is:  %i %i %i %i\n", m_contentBB_x1,m_contentBB_y1, m_contentBB_x2, m_contentBB_y2);
}

void MusLayoutObject::UpdateSelfBB( int x1, int y1, int x2, int y2 ) 
{
    //printf("SB Was: %i %i %i %i\n", m_selfBB_x1,m_selfBB_y1, m_selfBB_x2 ,m_selfBB_y2);
    
    int min_x = min( x1, x2 );
    int max_x = max( x1, x2 );
    int min_y = min( y1, y2 );
    int max_y = max( y1, y2 );
    
    if (m_selfBB_x1 > min_x) m_selfBB_x1 = min_x;
    if (m_selfBB_y1 > min_y) m_selfBB_y1 = min_y;
    if (m_selfBB_x2 < max_x) m_selfBB_x2 = max_x;
    if (m_selfBB_y2 < max_y) m_selfBB_y2 = max_y;
    
    m_updatedBB = true;
    
    //printf("SB Is:  %i %i %i %i\n", m_selfBB_x1,m_selfBB_y1, m_selfBB_x2 ,m_selfBB_y2);
    
}

void MusLayoutObject::ResetBB() 
{
    m_contentBB_x1 = 0xFFFF;
    m_contentBB_y1 = 0xFFFF;
    m_contentBB_x2 = -0xFFFF;
    m_contentBB_y2 = -0xFFFF;
    m_selfBB_x1 = 0xFFFF;
    m_selfBB_y1 = 0xFFFF; 
    m_selfBB_x2 = -0xFFFF;
    m_selfBB_y2 = -0xFFFF;
    
    m_updatedBB = false;
}

bool MusLayoutObject::HasContentBB() 
{
    return ( (m_contentBB_x1 != 0xFFFF) && (m_contentBB_y1 != 0xFFFF) && (m_contentBB_x2 != -0xFFFF) && (m_contentBB_y2 != -0xFFFF) );
}

bool MusLayoutObject::HasSelfBB() 
{
    return ( (m_selfBB_x1 != 0xFFFF) && (m_selfBB_y1 != 0xFFFF) && (m_selfBB_x2 != -0xFFFF) && (m_selfBB_y2 != -0xFFFF) );
}

//----------------------------------------------------------------------------
// MusEnv
//----------------------------------------------------------------------------

MusEnv::MusEnv()
{
    m_landscape = true;
    m_staffLineWidth = 2;
    m_stemWidth = 3;
    m_barlineWidth = 3;
    m_beamWidth = 10;
    m_beamWhiteWidth = 5;
    m_beamMaxSlope = 30;
    m_beamMinSlope = 10;
    m_leftMarginOddPage = 10;
    m_leftMarginEvenPage = 10;
    
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





