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

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusObjects );

#include <algorithm>
using std::min;
using std::max;

//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

MusObject::MusObject() :
	wxObject()
{
    m_parent = NULL;
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

void MusObject::ResetUuid()
{
    uuid_generate( m_uuid );
}


bool MusObject::operator==( MusObject& other)
{
    // This should never happen.
    // The comparison is performed in the CmpFile::Align method.
    // We expect to compare only MusNote, MusRest, etc object for which we have an overwritten method
    wxLogError( "Missing comparison operator for '%s'", this->MusClassName().c_str() );
    return false;
}

bool MusObject::Check() 
{
    return true; 
}

void MusObject::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_stopIt) {
        return;
    }
    
    functor->Call( this, params );

    MusObject *obj;
	int i;
    for (i = 0; i < (int)m_children.GetCount(); i++) 
	{
        obj = &m_children[i];
        obj->Process( functor, params );
	}
}

//----------------------------------------------------------------------------
// MusDocObject
//----------------------------------------------------------------------------

MusDocObject::MusDocObject() :
	MusObject()
{
	m_doc = NULL;
    ResetBB();
}

MusDocObject::~MusDocObject()
{
}

/*
void MusDocObject::SetDoc( wxArrayPtrVoid params )
{
    // param 0: MusDoc
    wxASSERT( dynamic_cast<MusDoc*>((MusDoc*)params[0]) ); 
    m_doc = (MusDoc*)params[0];  
}
*/ // ax2.3


void MusDocObject::UpdateContentBB( int x1, int y1, int x2, int y2) 
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

void MusDocObject::UpdateSelfBB( int x1, int y1, int x2, int y2 ) 
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

void MusDocObject::ResetBB() 
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

bool MusDocObject::HasContentBB() 
{
    return ( (m_contentBB_x1 != 0xFFFF) && (m_contentBB_y1 != 0xFFFF) && (m_contentBB_x2 != -0xFFFF) && (m_contentBB_y2 != -0xFFFF) );
}

bool MusDocObject::HasSelfBB() 
{
    return ( (m_selfBB_x1 != 0xFFFF) && (m_selfBB_y1 != 0xFFFF) && (m_selfBB_x2 != -0xFFFF) && (m_selfBB_y2 != -0xFFFF) );
}


//----------------------------------------------------------------------------
// MusFunctor
//----------------------------------------------------------------------------

MusFunctor::MusFunctor( )
{ 
    m_stopIt=false;
    m_reverse=false;
    obj_fpt=NULL; 
}

MusFunctor::MusFunctor( bool(MusObject::*_obj_fpt)( wxArrayPtrVoid ))
{ 
    m_stopIt=false;
    m_reverse=false;
    obj_fpt=_obj_fpt; 
}

void MusFunctor::Call( MusObject *ptr, wxArrayPtrVoid params )
{ 
    m_stopIt = (*ptr.*obj_fpt)( params );
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





