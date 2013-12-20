/////////////////////////////////////////////////////////////////////////////
// Name:        musobject.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "musobject.h"

//----------------------------------------------------------------------------

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

//----------------------------------------------------------------------------

#include "mus.h"
#include "musaligner.h"
#include "musbeam.h"
#include "musclef.h"
#include "muskeysig.h"
#include "muslayer.h"
#include "muslayerelement.h"
#include "musmeasure.h"
#include "muspage.h"
#include "musrc.h"
#include "musscoredef.h"
#include "musstaff.h"
#include "mussystem.h"
#include "mustie.h"
#include "mustuplet.h"

//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

MusObject::MusObject()
{
    Init("m-");
}

MusObject::MusObject(std::string classid)
{
    Init(classid);
}

MusObject *MusObject::Clone( )
{
    // This should new happen because the method should be overwritten
    assert( false );
}

MusObject::MusObject( const MusObject& object )
{
    ClearChildren();
    m_parent = NULL;
    m_classid = object.m_classid;
    m_uuid = object.m_uuid; // for now copy the uuid - to be decided
    m_isModified = true;
    
    int i;
    for (i = 0; i < (int)object.m_children.size(); i++)
    {
        MusObject *current = object.m_children[i];
        MusObject* copy = current->Clone();
        copy->Modify();
        copy->SetParent( this );
        m_children.push_back( copy );
    }
}

MusObject& MusObject::operator=( const MusObject& object )
{
	if ( this != &object ) // not self assignement
	{
        ClearChildren();
        m_parent = NULL;
        m_classid = object.m_classid;
        m_uuid = object.m_uuid; // for now copy the uuid - to be decided
        m_isModified = true;
        
        int i;
        for (i = 0; i < (int)object.m_children.size(); i++)
        {
            MusObject *current = object.m_children[i];
            MusObject* copy = current->Clone();
            copy->Modify();
            copy->SetParent( this );
            m_children.push_back( copy );
        }
	}
	return *this;
}

MusObject::~MusObject()
{
    ClearChildren();
}

void MusObject::Init(std::string classid)
{
    m_parent = NULL;
    m_isModified = true;
    m_classid = classid;
    this->GenerateUuid();
}

void MusObject::SetUuid( std::string uuid )
{ 
    m_uuid = uuid;
};

void MusObject::ClearChildren()
{
    ArrayOfMusObjects::iterator iter;
    for (iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        delete *iter;
    }
    m_children.clear();
}

int MusObject::GetIdx() const
{
    assert( m_parent );
    
    return m_parent->GetChildIndex( this );
}

void MusObject::InsertChild( MusObject *element, int idx )
{
    if ( idx >= (int)m_children.size() ) {
        m_children.push_back( element );
    }
    ArrayOfMusObjects::iterator iter = m_children.begin();
    m_children.insert( iter+(idx), element );
}

MusObject *MusObject::DetachChild( int idx )
{
    if ( idx >= (int)m_children.size() ) {
        return NULL;
    }
    MusObject *child = m_children[idx];
    child->m_parent = NULL;
    ArrayOfMusObjects::iterator iter = m_children.begin();
    m_children.erase( iter+(idx) );
    return child;
}


MusObject* MusObject::GetChild( int idx )
{
    if ( (idx < 0) || (idx >= (int)m_children.size()) ) {
        return NULL;
    }
    return m_children[idx];
}

void MusObject::RemoveChildAt( int idx )
{
    if ( idx >= (int)m_children.size() ) {
        return;
    }
    delete m_children[idx];
    ArrayOfMusObjects::iterator iter = m_children.begin();
    m_children.erase( iter+(idx) );
}

void MusObject::GenerateUuid() {
    int nr = std::rand();
    char str[17];
    // I do not want to use a stream to do this!
    snprintf (str, 16, "%016d", nr);
    
    m_uuid = m_classid + std::string(str);
}

void MusObject::ResetUuid()
{
    GenerateUuid();
}

void MusObject::SetParent( MusObject *parent )
{
    assert( !m_parent );
    m_parent = parent;
}


bool MusObject::operator==( MusObject& other )
{
    // This should never happen.
    // The comparison is performed in the CmpFile::Align method.
    // We expect to compare only MusNote, MusRest, etc object for which we have an overwritten method
    Mus::LogError( "Missing comparison operator for '%s'", this->MusClassName().c_str() );
    return false;
}

int MusObject::GetChildIndex( const MusObject *child )
{
    ArrayOfMusObjects::iterator iter;
    int i;
    for (iter = m_children.begin(), i = 0; iter != m_children.end(); ++iter, i++)
    {
        if ( child == *iter ) {
            return i;
        }

    }
    return -1;
}

void MusObject::Modify( bool modified )
{    
    // if we have a parent and a new modification, propagate it
    if ( m_parent && !m_isModified && modified ) {
        m_parent->Modify();
    }
    m_isModified = modified;
}

void MusObject::FillList( ListOfMusObjects *list )
{
    MusFunctor addToList( &MusObject::AddMusLayerElementToList );
    ArrayPtrVoid params;
    params.push_back ( &list );
    this->Process( &addToList, params );

    /* // For debuging
    ListOfMusObjects::iterator iter;
    for (iter = list->begin(); iter != list->end(); ++iter)
    {
        MusObject *current = *iter;
        Mus::LogDebug("%s", current->MusClassName().c_str() );
    }
    */
}

void MusObject::AddSameAs( std::string id, std::string filename )
{
    std::string sameAs = filename;
    if ( !filename.empty() ) {
        sameAs += "#";
    }
    sameAs += id;
    
    if ( !m_sameAs.empty() ) {
        m_sameAs += " ";
    }
    m_sameAs += sameAs;
}

MusObject *MusObject::GetFirstParent( const std::type_info *elementType, int maxSteps )
{
    if ( (maxSteps == 0) || !m_parent ) {
        return NULL;
    }
    
    if ( typeid(*m_parent) == *elementType ) {
        return m_parent;
    }
    else {
        return ( m_parent->GetFirstParent( elementType, maxSteps - 1 ) );
    }
}


MusObject *MusObject::GetFirstChild( const std::type_info *elementType )
{
    ArrayOfMusObjects::iterator iter;
    int i;
    for (iter = m_children.begin(), i = 0; iter != m_children.end(); ++iter, i++)
    {
        if ( typeid(**iter) == *elementType )
        {
            return *iter;
        }
    }
    return NULL;
}


bool MusObject::GetSameAs( std::string *id, std::string *filename, int idx )
{
    int i = 0;
    
    std::istringstream iss( m_sameAs );
    std::string token;
    while( getline( iss, token, ' '))
    {
        if ( i == idx ) {
            size_t pos = token.find( "#" );
            if (pos != std::string::npos) {
                (*filename) = token.substr( 0, pos );
                (*id) = token.substr( pos + 1, std::string::npos );
            }
            else {
                (*filename) = "";
                (*id) = token;
            }
            return true;
        }
        i++;
    }
    return false;
}

void MusObject::Process(MusFunctor *functor, ArrayPtrVoid params, MusFunctor *endFunctor )
{
    if (functor->m_returnCode == FUNCTOR_STOP) {
        return;
    }
    
    functor->Call( this, params );
    
    // do not go any deeper in this case
    if (functor->m_returnCode == FUNCTOR_SIBLINGS) {
        functor->m_returnCode = FUNCTOR_CONTINUE;
        return;
    }

    MusObject *obj;
	int i;
    for (i = 0; i < (int)m_children.size(); i++)
	{
        obj = m_children[i];
        obj->Process( functor, params, endFunctor);
	}
    
    if ( endFunctor ) {
        endFunctor->Call( this, params );
    }
}

//----------------------------------------------------------------------------
// MusDocObject
//----------------------------------------------------------------------------


// Note: since it is one line of code
// I am not making a new function for the two
// constructors.
MusDocObject::MusDocObject():
    MusObject("md-")
{
	ResetBB();
}

MusDocObject::MusDocObject(std::string classid) :
    MusObject(classid)
{
	//m_doc = NULL;
    ResetBB();
}

MusDocObject::~MusDocObject()
{
}

void MusDocObject::Refresh() 
{
    // if we have a parent MusDocObject, propagate it
    if ( dynamic_cast<MusDocObject*>(m_parent) ) {
        ((MusDocObject*)m_parent)->Refresh();
    }
}


void MusDocObject::UpdateContentBB( int x1, int y1, int x2, int y2) 
{
    //Mus::LogDebug("CB Was: %i %i %i %i", m_contentBB_x1, m_contentBB_y1, m_contentBB_x2 ,m_contentBB_y2);
    
    int min_x = std::min( x1, x2 );
    int max_x = std::max( x1, x2 );
    int min_y = std::min( y1, y2 );
    int max_y = std::max( y1, y2 );
    
    if (m_contentBB_x1 > min_x) m_contentBB_x1 = min_x;
    if (m_contentBB_y1 > min_y) m_contentBB_y1 = min_y;
    if (m_contentBB_x2 < max_x) m_contentBB_x2 = max_x;
    if (m_contentBB_y2 < max_y) m_contentBB_y2 = max_y;
    
    m_updatedBB = true;
    //Mus::LogDebug("CB Is:  %i %i %i %i", m_contentBB_x1,m_contentBB_y1, m_contentBB_x2, m_contentBB_y2);
}

void MusDocObject::UpdateSelfBB( int x1, int y1, int x2, int y2 ) 
{
    //Mus::LogDebug("SB Was: %i %i %i %i", m_selfBB_x1,m_selfBB_y1, m_selfBB_x2 ,m_selfBB_y2);
    
    int min_x = std::min( x1, x2 );
    int max_x = std::max( x1, x2 );
    int min_y = std::min( y1, y2 );
    int max_y = std::max( y1, y2 );
    
    if (m_selfBB_x1 > min_x) m_selfBB_x1 = min_x;
    if (m_selfBB_y1 > min_y) m_selfBB_y1 = min_y;
    if (m_selfBB_x2 < max_x) m_selfBB_x2 = max_x;
    if (m_selfBB_y2 < max_y) m_selfBB_y2 = max_y;
    
    m_updatedBB = true;
    
    //Mus::LogDebug("SB Is:  %i %i %i %i", m_selfBB_x1,m_selfBB_y1, m_selfBB_x2 ,m_selfBB_y2);
    
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
// MusObjectListInterface
//----------------------------------------------------------------------------


MusObjectListInterface::MusObjectListInterface( const MusObjectListInterface& interface )
{
    // actually nothing to do, we just don't want the list to be copied
    m_list.clear();
}

MusObjectListInterface& MusObjectListInterface::operator=( const MusObjectListInterface& interface )
{
    // actually nothing to do, we just don't want the list to be copied
    if ( this != &interface ) {
        this->m_list.clear();
    }
	return *this;
}

void MusObjectListInterface::ResetList( MusObject *node )
{
    // nothing to do, the list if up to date
    if ( !node->IsModified() ) {
        return;
    }
    
    m_list.clear();
    node->FillList( &m_list );
    this->FilterList();
    node->Modify( false );
}

ListOfMusObjects *MusObjectListInterface::GetList( MusObject *node )
{   
    ResetList( node );
    return &m_list;
}


int MusObjectListInterface::GetListIndex( const MusObject *listElement )
{
    ListOfMusObjects::iterator iter;
    int i;
    for (iter = m_list.begin(), i = 0; iter != m_list.end(); ++iter, i++)
    {
        if ( listElement == *iter ) {
            return i;
        }
        
    }
    return -1;
}

MusObject *MusObjectListInterface::GetListPrevious( const MusObject *listElement )
{
    ListOfMusObjects::iterator iter;
    int i;
    for (iter = m_list.begin(), i = 0; iter != m_list.end(); ++iter, i++)
    {
        if (listElement == *iter) {
            if (i > 0) {
                return *(--iter);
            }
            else {
                return NULL;
            }
        }
        
    }
    return NULL;
}

MusObject *MusObjectListInterface::GetListNext( const MusObject *listElement )
{
    ListOfMusObjects::reverse_iterator iter;
    int i;
    for (iter = m_list.rbegin(), i = 0; iter != m_list.rend(); ++iter, i++)
    {
        if ( listElement == *iter ) {
            if (i > 0) {
                return *(--iter);
            }
            else {
                return NULL;
            }
        }
        
    }
    return NULL;
}

//----------------------------------------------------------------------------
// MusFunctor
//----------------------------------------------------------------------------

MusFunctor::MusFunctor( )
{ 
    m_returnCode = FUNCTOR_CONTINUE;
    m_reverse = false;
    obj_fpt = NULL; 
}

MusFunctor::MusFunctor( int(MusObject::*_obj_fpt)( ArrayPtrVoid ))
{ 
    m_returnCode = FUNCTOR_CONTINUE;
    m_reverse = false;
    obj_fpt = _obj_fpt; 
}

void MusFunctor::Call( MusObject *ptr, ArrayPtrVoid params )
{
    // we should have return codes (not just bool) for avoiding to go further down the tree in some cases
    m_returnCode = (*ptr.*obj_fpt)( params );
}

//----------------------------------------------------------------------------
// MusObject functor methods
//----------------------------------------------------------------------------

int MusObject::AddMusLayerElementToList( ArrayPtrVoid params )
{
    // param 0: the ListOfMusObjects
    ListOfMusObjects **list = (ListOfMusObjects**)params[0];
    //if ( dynamic_cast<MusLayerElement*>(this ) ) {
        (*list)->push_back( this );
    //}
    return FUNCTOR_CONTINUE;
}

int MusObject::FindByUuid( ArrayPtrVoid params )
{
    // param 0: the uuid we are looking for
    // param 1: the pointer to pointer to the MusObject
    std::string *uuid = (std::string*)params[0];  
    MusObject **element = (MusObject**)params[1];  
    
    if ( (*element) ) {
        // this should not happen, but just in case
        return FUNCTOR_STOP;
    }
    
    if ( *uuid == this->GetUuid()) {
        (*element) = this;
        //Mus::LogDebug("Found it!");
        return FUNCTOR_STOP;
    }
    //Mus::LogDebug("Still looking for uuid...");
    return FUNCTOR_CONTINUE;
}


int MusObject::SetPageScoreDef( ArrayPtrVoid params )
{

    // param 0: the current scoreDef
    MusScoreDef *currentScoreDef = (MusScoreDef*)params[0];
    MusStaffDef **currentStaffDef = (MusStaffDef**)params[1];
    

    // starting a new page
    MusPage *current_page = dynamic_cast<MusPage*>(this);
    if ( current_page  ) {
        currentScoreDef->SetRedraw( true, true, false );
        current_page->m_drawingScoreDef = *currentScoreDef;
        return FUNCTOR_CONTINUE;
    }
    
   
    // starting a new system
    MusSystem *current_system = dynamic_cast<MusSystem*>(this);
    if ( current_system  ) {
        currentScoreDef->SetRedraw( true, true, false );
        return FUNCTOR_CONTINUE;
    }

    // starting a new staff
    MusStaff *current_staff = dynamic_cast<MusStaff*>(this);
    if ( current_staff  ) {
        (*currentStaffDef) = currentScoreDef->GetStaffDef( current_staff->GetStaffNo() );
        return FUNCTOR_CONTINUE;
    }

    // starting a new layer
    MusLayer *current_layer = dynamic_cast<MusLayer*>(this);
    if ( current_layer  ) {
        current_layer->SetDrawingValues( currentScoreDef, (*currentStaffDef) );
        return FUNCTOR_CONTINUE;
    }
    
    // starting a new clef
    MusClef *current_clef = dynamic_cast<MusClef*>(this);
    if ( current_clef  ) {
        currentScoreDef->ReplaceClef( current_clef );
        return FUNCTOR_CONTINUE;
    }
    
    // starting a new keysig
    MusKeySig *current_keysig = dynamic_cast<MusKeySig*>(this);
    if ( current_keysig  ) {
        currentScoreDef->ReplaceKeySig( current_keysig );
        return FUNCTOR_CONTINUE;
    }
    
    return FUNCTOR_CONTINUE;
}


int MusObject::SetBoundingBoxXShift( ArrayPtrVoid params )
{
    // param 0: the minimu position (i.e., the width of the previous element)
    // param 1: the maximum width in the current measure
    int *min_pos = (int*)params[0];
    int *measure_width = (int*)params[1];

    // starting a new measure
    MusMeasure *current_measure = dynamic_cast<MusMeasure*>(this);
    if ( current_measure  ) {
        // we reset the measure width and the minimum position
        (*measure_width) = 0;
        (*min_pos) = 0;
        if (current_measure->GetLeftBarlineType() != BARLINE_NONE) {
            current_measure->GetLeftBarline()->SetBoundingBoxXShift( params );
        }
        return FUNCTOR_CONTINUE;
    }
    
    // starting an new layer
    MusLayer *current_layer = dynamic_cast<MusLayer*>(this);
    if ( current_layer  ) {
        // mininimum position is the with for the last (previous) layer
        // we keep it if is higher than what we had so far
        // this will be used for shifting the right barline
        (*measure_width) = std::max( (*measure_width), (*min_pos) );
        // reset it as the minimum position
        (*min_pos) = SPACING_MINPOS;
        if (current_layer->GetClefAttr()) {
            current_layer->GetClefAttr()->SetBoundingBoxXShift( params );
        }
        if (current_layer->GetKeySigAttr()) {
            current_layer->GetKeySigAttr()->SetBoundingBoxXShift( params );
        }
        return FUNCTOR_CONTINUE;
    }

    MusLayerElement *current = dynamic_cast<MusLayerElement*>(this);
    if ( !current  ) {
        return FUNCTOR_CONTINUE;
    }
    
    // we should have processed aligned before
    assert( current->GetAlignment() );

    if ( !current->HasUpdatedBB() ) {
        // this is all we need for empty elements
        current->GetAlignment()->SetXRel(*min_pos);
        return FUNCTOR_CONTINUE;
    }
    
    if ( dynamic_cast<MusBeam*>(current) ) {
        return FUNCTOR_CONTINUE;
    }
    
    if ( dynamic_cast<MusTie*>(current) ) {
        return FUNCTOR_CONTINUE;
    }
    
    if ( dynamic_cast<MusTuplet*>(current) ) {
        return FUNCTOR_CONTINUE;
    }
    
    // the negative offset it the part of the bounding box that overflows on the left
    // |____x_____|
    //  ---- = negative offset
    int negative_offset = current->GetAlignment()->GetXRel() - current->m_contentBB_x1;
    
    // this will probably never happen
    if ( negative_offset < 0 ) {
        negative_offset = 0;
    }
    
    // check if the element overlaps with the preceeding one given by (*min_pos)
    int overlap = 0;
    if ( (current->GetAlignment()->GetXRel() - negative_offset) < (*min_pos) ) {
        overlap = (*min_pos) - current->GetAlignment()->GetXRel() + negative_offset;
        // shift the current element
        current->GetAlignment()->SetXShift( overlap );
    }
    
    //Mus::LogDebug("%s min_pos %d; negative offset %d;  x_rel %d; overlap %d", current->MusClassName().c_str(), (*min_pos), negative_offset, current->GetAlignment()->GetXRel(), overlap );
    
    // the next minimal position if given by the right side of the bounding box + the spacing of the element
    (*min_pos) = current->m_contentBB_x2 + current->GetHorizontalSpacing();
    current->GetAlignment()->SetMaxWidth( current->m_contentBB_x2 - current->GetAlignment()->GetXRel() + current->GetHorizontalSpacing() );
    
    return FUNCTOR_CONTINUE;
}

int MusObject::SetBoundingBoxXShiftEnd( ArrayPtrVoid params )
{
    // param 0: the minimu position (i.e., the width of the previous element)
    // param 1: the maximum width in the current measure
    int *min_pos = (int*)params[0];
    int *measure_width = (int*)params[1];
    
    // ending a measure
    MusMeasure *current_measure = dynamic_cast<MusMeasure*>(this);
    if ( current_measure  ) {
        // as minimum position of the barline use the measure width
        (*min_pos) = (*measure_width);
        if (current_measure->GetRightBarlineType() != BARLINE_NONE) {
            current_measure->GetRightBarline()->SetBoundingBoxXShift( params );
        }
        return FUNCTOR_CONTINUE;
    }
    
    // ending a layer
    MusLayer *current_layer = dynamic_cast<MusLayer*>(this);
    if ( current_layer  ) {
        // mininimum position is the with the layer
        // we keep it if is higher than what we had so far
        // this will be used for shifting the right barline
        (*measure_width) = std::max( (*measure_width), (*min_pos) );
        return FUNCTOR_CONTINUE;
    }
    
    return FUNCTOR_CONTINUE;
}

int MusObject::SetBoundingBoxYShift( ArrayPtrVoid params )
{
    // param 0: the height of the previous staff
    int *min_pos = (int*)params[0];
    
    // starting a new measure
    MusMeasure *current_measure = dynamic_cast<MusMeasure*>(this);
    if ( current_measure  ) {
        (*min_pos) = 0;
    }
    
    MusStaff *current = dynamic_cast<MusStaff*>(this);
    if ( !current  ) {
        return FUNCTOR_CONTINUE;
    }
    
    // at this stage we assume we have instanciated the alignment pointer
    assert( current->GetAlignment() );
    
    // This is the value that need to be removed to fit everything
    int negative_offset = current->GetAlignment()->GetYRel() - current->m_contentBB_y2;
    
    // this will probably never happen
    if ( negative_offset > 0 ) {
        negative_offset = 0;
    }
    
    // check if the staff overlaps with the preceeding one given by (*min_pos)
    int overlap = 0;
    if ( (current->GetAlignment()->GetYRel() - negative_offset) > (*min_pos) ) {
        overlap = (*min_pos) - current->GetAlignment()->GetYRel() + negative_offset;
        current->GetAlignment()->SetYShift( overlap );
    }
    
    //Mus::LogDebug("%s min_pos %d; negative offset %d;  x_rel %d; overlap %d", current->MusClassName().c_str(), (*min_pos), negative_offset, current->GetAlignment()->GetXRel(), overlap );
    
    // the next minimal position if given by the right side of the bounding box + the spacing of the element
    (*min_pos) = current->m_contentBB_y1;
    
    // do not go further down the tree in this case
    return FUNCTOR_SIBLINGS;
}








