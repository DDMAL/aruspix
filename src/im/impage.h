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
//#include "im/imbrink3classes.h"

class ImStaffFunctor;

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

enum
{
	PRE_BINARIZATION_BRINK = 0,
	PRE_BINARIZATION_SAUVOLA,
	PRE_BINARIZATION_BRINK3CLASSES
};

#define WIN_WIDTH 2
#define WIN_OVERLAP 0
#define LYRIC_WIN_WIDTH 2 
#define LYRIC_WIN_OVERLAP 0


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
    ImPage( wxString path, bool *isModified = NULL );
    virtual ~ImPage();
    
        void Clear( );
    // XML access
    bool Load( TiXmlElement *file_root );
    bool Save( TiXmlElement *file_root );
    // Processing
    bool Check( wxString infile, int max_size = -1, int min_size = -1, int index = 0 );
    bool Deskew( double max_alpha );
    bool FindStaves( int min, int max, bool normalize = true, bool crop = true ); // if normalize, then resize to get a staff height of 100
	bool BinarizeAndClean( );
    bool FindBorders( );
    bool FindOrnateLetters( );
    bool FindText( );
    bool FindTextInStaves( );
    bool ExtractStaves( ); 
    bool StaffCurvatures( );    
	bool GenerateMFC( wxString output_dir = "" );
	bool GenerateLyricMFC( wxString output_dir );
	bool ChangeClassification( int x1, int y1, int x2, int y2, int plane_number  );
	bool ChangeClassification( int plane_number  );
	bool MagicSelection( int x, int y, AxImage *selection, int *xmin, int *ymin );
	// Working methods
	bool SaveStaffImages(); // enregistre les images de portees dans working dir
    // moulinette
    virtual void Process(ImStaffFunctor *functor, wxArrayPtrVoid params, int counter = -1 );
	//virtual void Process(ImStaffFunctor *functor, wxArrayPtrVoid params, 
	//	ImStaffSegmentFunctor *subfunctor = NULL, int counter = -1 );
    // values
    void CalcLeftRight(int *x1, int *x2);
	int GetStaffCount( );
	//int GetStaffSegmentsCount( bool segment_only = false );
	void GetStaffSegmentOffsets( int staff_no, int offsets[],  int split_points[], int end_points[] );
    int ToViewY( int y ) { return m_size.GetHeight() - y;}
	
	// undo
	virtual void Load( AxUndoFile *undoPtr );
    virtual void Store( AxUndoFile *undoPtr );
    
private:
        int GetMedianStavesSpace( );
    int GetDeskewAlignement( _imImage *image, double alpha );
    void GetHorizontalStavesPosition( int values[], int size, int avg, int *x1, int *x2 );
    void GetVerticalStavesPosition( int values[], int size, int avg, int staves[], int *nb_staves );
    void CleanBorder( int row[], int size, _imImage *border, _imImage *image, int split );


public:
    _imImage *m_img0; // processed image, with pre-classification
	_imImage *m_img1; // processed image, greyscale (alternative)
	_imImage *m_selection; // buffer used to change classification
	wxPoint m_selection_pos;
	bool *m_isModified;
	wxString m_path; // path of the RecFile
    ArrayOfStaves m_staves;
    int m_reduction;
    double m_skew;
    double m_resize;
	double m_resized;
    double m_optimization_resize_factor;
    int m_original_width, m_original_height;
    int m_x1, m_x2, m_y1;
    wxSize m_size; // taille de l'image à reconnaitre
    int m_line_width;
    int m_space_width;
	int m_staff_height; // taille de la portee (calculee par correlation dans StaffCurvatures())
	int m_num_staff_lines; // num staff lines on this ImPage
	
	// static values not changed
	static int s_pre_page_binarization_method; // used in ImPage, idem
    static int s_pre_page_binarization_method_size;
	static bool s_pre_page_binarization_select;
	// the number of staff lines (4 for chant, or 5)
	static int s_num_staff_lines;
	
	int *m_pre_page_binarization_methodPtr;
	int *m_pre_page_binarization_method_sizePtr;
    
private:
    
};

#endif
