 /////////////////////////////////////////////////////////////////////////////
// Name:        musobject.h
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef __MUS_OBJECT_H__
#define __MUS_OBJECT_H__

#include <list>
#include <string>
#include <typeinfo>
#include <vector>

#include "musdef.h"
 
class MusDoc;
class MusFunctor;
class MusObject;

typedef std::vector<MusObject*> ArrayOfMusObjects;

typedef std::list<MusObject*> ListOfMusObjects;

typedef std::vector<void*> ArrayPtrVoid;

//----------------------------------------------------------------------------
// MusObject
//----------------------------------------------------------------------------

/** 
 * This class represents a basic object
 */
class MusObject
{
public:
    // constructors and destructors
    MusObject();
    MusObject(std::string classid);
    virtual ~MusObject();
    
    /**
     * Copy constructor that also copy the children.
     * The children are copied using the MusObject::Clone virtual method that
     * Needs to be overwritten in the child class - we make it crash otherwise,
     * Because this will create problem if we don't check this (the parents will 
     * one the same child...)
     * UUID: the uuid is copied, is needs to be reset later if this is not wished
     */
    MusObject( const MusObject& object );
    
    /**
     * See copy constructor.
     */
    MusObject& operator=( const MusObject& object ); // copy assignement;
    
    /**
     * Method call for copying child classes
     */
    virtual MusObject* Clone();
    
    virtual bool operator==( MusObject& other );
    
    std::string GetUuid() { return m_uuid; };
    void SetUuid( std::string uuid );
    void ResetUuid( );
    
    /**
     * Children count
     * Used for classes with several types of children
     */
    int GetChildCount() { return (int)m_children.size(); };
    
    /**
     * Child access (generic)
     */
    MusObject *GetChild( int idx );
    
    /**
     * Clear the children vector and delete all the objects.
     */
    void ClearChildren();
    
    /**
     * Set the parent of the MusObject.
     * The current parent is expected to be NULL.
     */
    void SetParent( MusObject *parent );
    
    virtual std::string MusClassName( ) { return "[MISSING]"; };

    /**
     * Return the index position of the object in its parent (-1 if not found)
     */
    int GetIdx() const;

    /**
     * Look for the MusObject in the children and return its position (-1 if not found)
     */
    int GetChildIndex( const MusObject *child );
    
    /**
     * Insert a element at the idx position.
     */
    void InsertChild( MusObject *element, int idx );
    
    /**
     * Detach the child at the idx position (NULL if not found)
     * The parent pointer is set to NULL.
     */
    MusObject *DetachChild( int idx );
    
    /**
     * Remove and delete the child at the idx position.
     */
    void RemoveChildAt( int idx );
    
    /**
     * Return the first parent of the specified type.
     * The maxSteps parameter limit the search to a certain number of level if not -1.
     */
    MusObject *GetFirstParent( const std::type_info *elementType, int maxSteps = -1 );
    
    /**
     * Return the first of the specified type.
     */
    MusObject *GetFirstChild( const std::type_info *elementType );
    
    /**
     * Return the previous sibling object of the specified type.
     * If no type is specified, returns the previous object.
     * Returns NULL if not found in both cases.
     */
    MusObject *GetPreviousSibling( const std::type_info *elementType = NULL );
    
    /**
     * Return the next sibling object of the specified type.
     * If no type is specified, returns the next object.
     * Returns NULL if not found in both cases.
     */
    MusObject *GetNextSibling( const std::type_info *elementType = NULL );
    
    /**
     * Fill the list of all the children LayerElement.
     * This is used for navigating in a MusLayer (See MusLayer::GetPrevious and MusLayer::GetNext).
     */  
    void FillList( ListOfMusObjects *list );
    
    /**
     * Add a sameAs attribute to the object.
     * If a filename is given, the attribute will be filename#id.
     * If several value are added, they will be separated by a whitespace.
     */
    void AddSameAs( std::string id, std::string filename = "" );
    
    /**
     * Parse the sameAs attribute and return the one at the #idx position (if any).
     */
    bool GetSameAs( std::string *id, std::string *filename, int idx = 0 );
    
    /**
     * Check if the content was modified or not
     */
    bool IsModified( ) { return m_isModified; };
    
    /**
     * Mark the object and its parent (if any) as modified
     */
    void Modify( bool modified = true );
    
    /**
     * Main method that processes functors.
     * For each object, it will call the functor.
     * Depending on the code returned by the functor, it will also process it for all children.
     */
    virtual void Process( MusFunctor *functor, ArrayPtrVoid params, MusFunctor *endFunctor = NULL );
    
    // functor methods
    /**
     * Add each LayerElements and its children to a list
     */
    virtual int AddLayerElementToList( ArrayPtrVoid params );
    
    /**
     * See MusLayer::CopyToLayer
     */ 
    virtual int CopyToLayer( ArrayPtrVoid params ) { return false; };
    
    /**
     * Find a MusObject with a specified uuid.
     * param 0: the uuid we are looking for.
     * param 1: the pointer to pointer to the MusObject retrieved (if found).
     */
    virtual int FindByUuid( ArrayPtrVoid params );
    
    /**
     * Save the content of and object by calling the appropriate MusFileOutputStream method
     * param 0: a pointer to the MusFileOutputStream.
     */
    virtual int Save( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Adjust the size of a system according to its content
     * See MusSystem::TrimSystem for actual implementation.
     */
    virtual int TrimSystem( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };

    /**
     * Lay out the X positions of the staff content looking that the bounding boxes.
     * The m_xShift is updated appropriately
     */
    virtual int SetBoundingBoxXShift( ArrayPtrVoid params );
    
    /**
     * Lay out the X positions of the staff content looking that the bounding boxes.
     * This is the MusFunctor called at the end of the measure or a layer.
     */
    virtual int SetBoundingBoxXShiftEnd( ArrayPtrVoid params );
    
    /**
     * Lay out the Y positions of the staff looking that the bounding box of each staff.
     * The m_yShift is updated appropriately
     */
    virtual int SetBoundingBoxYShift( ArrayPtrVoid params );
    
    /**
     * Align the content of a system.
     * For each LayerElement, instanciate its MusAlignment.
     * It creates it if no other note or event occurs at its position.
     */
    virtual int Align( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Set the position of the MusAlignment.
     * Looks at the time different with the previous MusAlignment.
     */
    virtual int SetAligmentXPos( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };

    /**
     * Set the position of the StaffAlignment.
     */
    virtual int SetAligmentYPos( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Set the initial scoreDef of each page.
     * This is necessary for integrating changes that occur within a page.
     */
    virtual int SetPageScoreDef( ArrayPtrVoid params );

    /**
     * Set the initial scoreDef of each page.
     * This is necessary for integrating changes that occur within a page.
     * param 0: bool clef flag.
     * param 1: bool keysig flag.
     * param 2: bool the mensur flag.
     */
    virtual int SetStaffDefDraw( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Replace all the staffDefs in a scoreDef.
     * param 0: a pointer to the scoreDef we are going to replace the staffDefs
     */
    virtual int ReplaceStaffDefsInScoreDef( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Correct the X alignment once the the content of a system has been aligned and laid out
     * See MusMeasure::IntegrateBoundingBoxXShift for actual implementation
     */
    virtual int IntegrateBoundingBoxXShift( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Correct the Y alignment once the the content of a system has been aligned and laid out
     * See MusSystem::IntegrateBoundingBoxYShift for actual implementation
     */
    virtual int IntegrateBoundingBoxYShift( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Align the measures by adjusting the m_xRel position looking at the MusMeasureAligner.
     */
    virtual int AlignMeasures( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Store the width of the system in the MusMeasureAligner for justification
     * This method is called at the end of a system
     */
    virtual int AlignMeasuresEnd( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Justify the X positions
     */
    virtual int JustifyX( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };
    
    /**
     * Align the system by adjusting the m_yRel position looking at the MusSystemAligner.
     */
    virtual int AlignSystems( ArrayPtrVoid params ) { return FUNCTOR_CONTINUE; };

public:
    ArrayOfMusObjects m_children;
    MusObject *m_parent;
    std::string m_sameAs;
    
protected:
    std::string m_uuid;
    std::string m_classid;

private:
    
    void GenerateUuid();
    void Init(std::string);
    
    /**
     * Indicated whether the object content is up-to-date or not.
     * This is usefull for object using sub-lists of objects when drawing.
     * For example, Beam has a list of children notes and this value indicates if the
     * list needs to be updated or not. Is is moslty and optimization feature.
     */
    bool m_isModified;
    
};


//----------------------------------------------------------------------------
// MusDocObject
//----------------------------------------------------------------------------

/** 
 * This class represents a basic object in the layout domain
 */
class MusDocObject: public MusObject
{
public:
    // constructors and destructors
    MusDocObject();
    MusDocObject(std::string classid);
    virtual ~MusDocObject();
    
    /**
     * Refreshes the views from MusDoc.
     * From other MusDocObject, simply pass it to its parent until MusDoc is reached.
     */
    virtual void Refresh();
    
    void UpdateContentBB( int x1, int y1, int x2, int y2);
    void UpdateSelfBB( int x1, int y1, int x2, int y2 );
    bool HasContentBB();
    bool HasSelfBB();
    void ResetBB();

    int m_contentBB_x1, m_contentBB_y1, m_contentBB_x2, m_contentBB_y2;
    int m_selfBB_x1, m_selfBB_y1, m_selfBB_x2, m_selfBB_y2; 
    
    /**
     * Is true if the bounding box (self or content) has been updated at least once.
     * We need this to avoid not updating bounding boxes to screw up the layout with their intial values.
     */
    bool HasUpdatedBB( ) { return m_updatedBB; };
    
private:
    bool m_updatedBB;
    
protected:
    
public:
    
};


//----------------------------------------------------------------------------
// MusObjectListInterface
//----------------------------------------------------------------------------

/** 
 * This class is an interface for elements maintaining a list of children
 * LayerElement for processing.
 * The list is a flatten list of pointers to children elements.
 * It is not an abstract class but should not be instanciate directly.
 */
class MusObjectListInterface
{
public:
    // constructors and destructors
    MusObjectListInterface() {};
    virtual ~MusObjectListInterface() {};
    MusObjectListInterface( const MusObjectListInterface& interface ); // copy constructor;
    MusObjectListInterface& operator=( const MusObjectListInterface& interface ); // copy assignement;

    /**
     * Look for the MusObject in the list and return its position (-1 if not found)
     */
    int GetListIndex( const MusObject *listElement );
    
    /**
     * Returns the previous object in the list (NULL if not found)
     */
    MusObject *GetListPrevious( const MusObject *listElement );

    /**
     * Returns the next object in the list (NULL if not found)
     */
    MusObject *GetListNext( const MusObject *listElement );
    
    /**
     * Return the list.
     * Before returning the list, it checks that the list is up-to-date with MusObject::IsModified
     * If not, it updates the list and also calls FilterList.
     * Because this is an interface, we need to pass the object - not the best design.
     */
    ListOfMusObjects *GetList( MusObject *node );
    
protected:
    /**
     * Filter the list for a specific class.
     * For example, keep only notes in Beam
     */
    virtual void FilterList() {};

public:    
    /**
     * Reset the list of children and call FilterList().
     * As for GetList, we need to pass the object.
     */
    void ResetList( MusObject *node );
        
    ListOfMusObjects m_list;
    
};

//----------------------------------------------------------------------------
// abstract base class MusFunctor
//----------------------------------------------------------------------------

/** 
 * This class is an abstact Functor for the object hierarchy.
 * Needs testing.
 */
class MusFunctor
{
private:
    int (MusObject::*obj_fpt)( ArrayPtrVoid params );   // pointer to member function
    
public:
    
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
    MusFunctor( );
    MusFunctor( int(MusObject::*_obj_fpt)( ArrayPtrVoid ));
	virtual ~MusFunctor() {};
    
    // override function "Call"
    virtual void Call( MusObject *ptr, ArrayPtrVoid params );
    
    int m_returnCode;
    bool m_reverse;
    
private:
    
};

#endif
