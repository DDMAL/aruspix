/////////////////////////////////////////////////////////////////////////////
// Name:        impage.h
// Author:      Laurent Pugin
// Created:     2004
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifndef __impage_H__
#define __impage_H__

#if defined(__GNUG__) && ! defined(__APPLE__)
    #pragma interface "impage.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/dynarray.h"

#include "im/imoperator.h"
//#include "rec_wdr.h"

#include "app/axundo.h"

class ImStaffFunctor;
class ImStaffSegmentFunctor;

class ImStaff;
WX_DECLARE_OBJARRAY( ImStaff, ArrayOfStaves);

class ImRLE;
WX_DECLARE_OBJARRAY(ImRLE, ArrayOfRLE);

// levels of undo
// save either staff, page or file depending on operation
enum
{
	IM_UNDO_CLASSIFICATION = 0
};

#define WIN_WIDTH 2
#define WIN_OVERLAP 0

// WDR: class declarations

//----------------------------------------------------------------------------
// ImRLE
//----------------------------------------------------------------------------

class ImRLE: public wxObject
{
public:
    // constructors and destructors
    ImRLE() {};
    ~ImRLE() {};

public:
    int type, val, x, y;
};

//----------------------------------------------------------------------------
// ImPage
//----------------------------------------------------------------------------

class ImPage: public ImOperator, public AxUndo 
{
public:
    // constructors and destructors
    ImPage::ImPage( wxString path, bool *isModified = NULL );
    virtual ImPage::~ImPage();
    
    // WDR: method declarations for ImPage
    void ImPage::Clear( );
    // XML access
    bool ImPage::Load( TiXmlElement *file_root );
    bool ImPage::Save( TiXmlElement *file_root );
    // Processing
    bool ImPage::Check( wxString infile, int max_binary, int index = 0 );
    bool ImPage::Deskew( double max_alpha);
    bool ImPage::FindStaves( int min, int max, bool normalize = true, bool crop = true ); // if normalize, then resize to get a staff height of 100
	bool ImPage::BinarizeAndClean( );
    bool ImPage::FindBorders( );
    bool ImPage::FindOrnateLetters( );
    bool ImPage::FindText( );
    bool ImPage::FindTextInStaves( );
    bool ImPage::StaffSegments( ); 
    bool ImPage::StaffCurvatures( );    
	bool ImPage::GenerateMFC( bool merged = false, wxString output_dir = "" );
	bool ImPage::ChangeClassification( int x1, int y1, int x2, int y2, int plane_number  );
	bool ImPage::ChangeClassification( int plane_number  );
	bool ImPage::MagicSelection( int x, int y, AxImage *selection, int *xmin, int *ymin );
	// Working methods
	bool ImPage::SaveSegmentsImages(); // enregistre les images des segments de portees dans working dir
    // moulinette
    virtual void ImPage::Process(ImStaffSegmentFunctor *functor, wxArrayPtrVoid params, int counter = -1 );
	virtual void ImPage::Process(ImStaffFunctor *functor, wxArrayPtrVoid params, 
		ImStaffSegmentFunctor *subfunctor = NULL, int counter = -1 );
    // values
    void ImPage::CalcLeftRight(int *x1, int *x2);
	int ImPage::GetStaffSegmentsCount( bool segment_only = false );
	void ImPage::GetStaffSegmentOffsets( int staff_no, int offsets[],  int split_points[], int end_points[] );
    int ImPage::ToViewY( int y ) { return m_size.GetHeight() - y;}
	
	// undo
	virtual void Load( AxUndoFile *undoPtr );
    virtual void Store( AxUndoFile *undoPtr );
    
private:
    // WDR: member variable declarations for ImPage
    int ImPage::GetMedianStavesSpace( );
    int ImPage::GetDeskewAlignement( _imImage *image, double alpha );
    void ImPage::GetHorizontalStavesPosition( int values[], int size, int avg, int *x1, int *x2 );
    void ImPage::GetVerticalStavesPosition( int values[], int size, int avg, int staves[], int *nb_staves );
    void ImPage::CleanBorder( int row[], int size, _imImage *border, _imImage *image, int split );


public:
    // WDR: member variable declarations for ImPage
	_imImage *m_img0; // processed image, with pre-classification
	_imImage *m_img1; // processed image, grascale (alternative)
	// HACK : page with removed staves
	_imImage *m_img2; // processed image, removed staves
	_imImage *m_selection; // buffer used to change classification
	wxPoint m_selection_pos;
	bool *m_isModified;
	wxString m_path; // path of the RecFile
    ArrayOfStaves m_staves;
    int m_reduction;
    double m_skew;
    double m_resize;
	double m_resized;
    int m_x1, m_x2;
    wxSize m_size; // taille de l'image à reconnaitre
    int m_line_width;
    int m_space_width;
	int m_staff_height; // taille de la portee (calculee par correlation dans StaffCurvatures())
    
private:
    // WDR: handler declarations for ImPage

};

#endif
