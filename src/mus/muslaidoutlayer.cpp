/////////////////////////////////////////////////////////////////////////////
// Name:        muslaidoutlayer.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "muslaidoutlayer.h"
#include "muslaidoutlayerelement.h"

#include "musdef.h"
#include "musclef.h"

#include <typeinfo>

// sorting function
int SortElements(MusLaidOutLayerElement **first, MusLaidOutLayerElement **second)
{
	if ( (*first)->m_x_abs < (*second)->m_x_abs )
		return -1;
	else if ( (*first)->m_x_abs > (*second)->m_x_abs )
		return 1;
	else 
		return 0;
}

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLaidOutLayers );

//----------------------------------------------------------------------------
// MusLaidOutLayer
//----------------------------------------------------------------------------

MusLaidOutLayer::MusLaidOutLayer( int logLayerNb, int logStaffNb, MusSection *section, MusMeasure *measure ):
	MusLayoutObject()
{
    wxASSERT_MSG( section, "MusSection pointer cannot be NULL when creating a MusLaidOutLayer");
    
	Clear( );
    m_logLayerNb = logLayerNb;
    m_logStaffNb = logStaffNb;
    m_section = section;
    m_measure = measure;
}

MusLaidOutLayer::~MusLaidOutLayer()
{
    Deactivate();
}

bool MusLaidOutLayer::Check()
{
    wxASSERT( m_staff && m_section );
    return ( m_staff && m_section && MusLayoutObject::Check());
}

void MusLaidOutLayer::Clear()
{
    m_staff = NULL;
	m_elements.Clear();
	voix = 0;
    
    //
    //beamListPremier = NULL;
}


void MusLaidOutLayer::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    output->WriteLaidOutLayer( this );
    
    // save elements
    MusLaidOutLayerElementFunctor element( &MusLaidOutLayerElement::Save );
    this->Process( &element, params );
}

void MusLaidOutLayer::Load( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileInputStream *input = (MusFileInputStream*)params[0];       
    
    // load elements
    MusLaidOutLayerElement *element;
    while ( (element = input->ReadLaidOutLayerElement()) ) {
        element->Load( params );
        this->AddElement( element );
    }
}


void MusLaidOutLayer::AddElement( MusLaidOutLayerElement *element, int idx )
{
	element->SetLayer( this );
    element->SetLayout( m_layout );
    if ( idx == -1 ) {
        m_elements.Add( element );
    }
    else {
        m_elements.Insert( element, idx );
    }
    wxASSERT_MSG( element->m_layerElement, "Pointer to LayerElement cannot be NULL" );
    // also add it to the logical layer - how do we manage the position?
    // m_logLayer->m_elements.Add( element->m_layerElement );
}

void MusLaidOutLayer::CopyAttributes( MusLaidOutLayer *nlayer )
{
	if ( !nlayer )
		return;

	nlayer->Clear();
	nlayer->voix = voix;
}

int MusLaidOutLayer::GetLayerNo() const
{
    wxASSERT_MSG( m_staff, "LaidOutStaff cannot be NULL");
    
    return m_staff->m_layers.Index( *this );
}



MusLaidOutLayerElement *MusLaidOutLayer::GetFirst( )
{
	if ( m_elements.IsEmpty() )
		return NULL;
	return &m_elements[0];
}

MusLaidOutLayerElement *MusLaidOutLayer::GetLast( )
{
	if ( m_elements.IsEmpty() )
		return NULL;
	int i = (int)m_elements.GetCount() - 1;
	return &m_elements[i];
}

MusLaidOutLayerElement *MusLaidOutLayer::GetNext( MusLaidOutLayerElement *element )
{	
    if ( !element || m_elements.IsEmpty())
		return NULL;
        
	int i = m_elements.Index( *element );

	if ((i == wxNOT_FOUND ) || (i >= GetElementCount() - 1 )) 
		return NULL;

	return &m_elements[i + 1];
}

MusLaidOutLayerElement *MusLaidOutLayer::GetPrevious( MusLaidOutLayerElement *element )
{
    if ( !element || m_elements.IsEmpty())
		return NULL;
        
	int i = m_elements.Index( *element );

	if ((i == wxNOT_FOUND ) || ( i <= 0 ))
        return NULL;
	
    return &m_elements[i - 1];
}

MusLaidOutLayerElement *MusLaidOutLayer::GetAtPos( int x )
{
	MusLaidOutLayerElement *element = this->GetFirst();
	if ( !element )
		return NULL;

	//int xx = 0;
//	while (this->GetNext(element) && ((int)element->m_x_abs < x) )
//		element = this->GetNext( element );

	int dif = x - element->m_x_abs;
	while ( this->GetNext( element ) && (int)element->m_x_abs < x ){
		element = this->GetNext( element );
		if ( (int)element->m_x_abs > x && dif < (int)element->m_x_abs - x )
			return this->GetPrevious( element );
		dif = x - element->m_x_abs;
	}
	
	return element;
}

MusLaidOutLayerElement *MusLaidOutLayer::Insert( MusLayerElement *element, int x )
{
	if ( !element ) { 
        return NULL;
    }
    
    // This is temporary and is OK because we have only one single layout for now.
    // Inserting elements should be done from the logical tree and then update the layout
    
    MusLayerElement *insertElement = element->GetChildCopy();
    
    MusSection *section = m_section;
    MusMeasure *measure = m_measure;

    // First we find the element after which we are inserting the element
    // If not, it will be NULL
    // We are also updating the section and measure ( TODO, not necessary for now )
    int idx = 0;
	MusLaidOutLayerElement *next = this->GetFirst();
	while ( next && (next->m_x_abs < x) )
	{
        idx++;
        // update section and measure if necessary (no section breaks and measure breaks for now)
		if ( this->GetNext( next ) )
			next = this->GetNext( next );
		else
			break;
	}
    
    // The we need to find the staff and layer in the logical tree where to insert the element
    MusStaff *staff = NULL;
    if ( measure ) {
        staff = measure->GetStaff( m_logStaffNb );
    }
    else if ( section ) { // it should be the case, but just to make sure
        staff = section->GetStaff( m_logStaffNb );
    }
    MusLayer * layer = NULL;
    if ( staff ) {
        layer = staff->GetLayer( m_logLayerNb );
    }
    
    if ( !layer ) {
        wxLogError( "Cannot insert element" );
        delete insertElement;
        return NULL;
    }
    
    // Insert in the logical tree
    layer->Insert( insertElement, next->m_layerElement );
    
    // Insert in the layout tree
    MusLaidOutLayerElement *laidOutElement = new MusLaidOutLayerElement( insertElement );
    laidOutElement->m_x_abs = x;
    AddElement( laidOutElement, idx );
    
	m_layout->RefreshViews();
    //
	return laidOutElement;
}


void MusLaidOutLayer::Delete( MusLaidOutLayerElement *element )
{
    wxASSERT_MSG( m_layout, "MusLayout cannot be NULL when deleting a MusLaidOutLayerElement" );
    
	if ( !element ) {
        return;
    }
    
    bool is_clef = false;
        
    if ( element->IsClef() ) {
        is_clef = true;
        //m_r->OnBeginEditionClef();
    }
	
    // This is temporary and is OK because we have only one single layout for now.
    // Deleting elements should be done from the logical tree and then update the layout
    // Ultimately, this method should be used for deleting element in the layout only?
                 
    // This is the logical element we need to delete - delete it handles everything!
    // - The LaidOutLayerElement is removed from its parent list
    // - The LaidOutLayerElement is delete
    // - The LayerElement is removed from its parent list (and deleted)
    delete element->m_layerElement;

	if ( is_clef )
	{
        //m_r->OnEndEditionClef();
	}
    
    m_layout->RefreshViews();
}


// Dans la direction indiquee (direction), cavale sur tout element qui n'est pas un
// symbol, de la nature indiquee (flg). Retourne le ptr si succes, ou 
// l'element de depart; le ptr succ est vrai si symb trouve.

MusLaidOutLayerElement *MusLaidOutLayer::GetFirst( MusLaidOutLayerElement *element, unsigned int direction, const std::type_info *elementType, bool *succ)
{	
    MusLaidOutLayerElement *original = element;

	*succ = false;
	if (element == NULL)
		return (element);

	int i = m_elements.Index( *element, (direction==BACKWARD) );
	if ( i == wxNOT_FOUND )
		return (element);

    *succ = true; // we assume we will find it. Change to false if not
    while ( typeid(*element->m_layerElement) != *elementType )
	{
		if (direction==BACKWARD)
		{	
			if (i < 1) {
                *succ = false;
                break;
            }
			i--;
			element = &m_elements[i];
		}
		else
		{	if (i >= (int)m_elements.GetCount() - 1 ) {
                *succ = false;
                break;
            }
			i++;
			element = &m_elements[i];
		}
	}	

	return (*succ ? element : original);
}

MusLaidOutLayerElement *MusLaidOutLayer::GetFromMusLayerElement( MusLayerElement *element) {
    
    for (unsigned int i = 0; i < m_elements.Count(); i++) {
        
        if (m_elements[i].m_layerElement == element)
            return &m_elements[i];
    }
    
    return NULL;
}

MusClef* MusLaidOutLayer::GetClef( MusLaidOutLayerElement *test )
{
	bool succ=false;

    if (!test) {
        return NULL;
    }
	
    if ( !test->IsClef() )
    {	
        test = GetFirst(test, BACKWARD, &typeid(MusClef), &succ);
    }

    MusClef *clef = dynamic_cast<MusClef*>(test->m_layerElement);
    return clef;
}

int MusLaidOutLayer::GetClefOffset( MusLaidOutLayerElement *test )
{
    MusClef *clef = GetClef(test);
    if (!clef) {
        return 0;
    }
    return clef->GetClefOffset();
    
}


// alternateur de position d'octave 
void MusLaidOutLayer::getOctDec (int ft, int _ot, int rupt, int *oct)
{
	if (ft>0)	// ordre ACCID_SHARP 
		if (rupt % 2 == 0)	// direction: au premier appel; puis on alterne...
			// test a revoir (mauvais, ne marche pas si rupt est impair) 
			*oct -=1;
		else *oct = _ot;
	else
		if (rupt % 2 == 0)	*oct +=1;
		else *oct -= 1;
	return;
}

/*
static char armatKey[] = {PITCH_F,PITCH_C,PITCH_G,PITCH_D,PITCH_A,PITCH_E,PITCH_B};
int MusLaidOutLayer::armatDisp ( MusDC *dc )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if ( !Check() )
		return 0;

	// calculer x_abs, c et oct
    // y_note calcule le decalage en fonction du code de hauteur c et du
    // decalage eventuel implique par l'oct courante et la clef. Ce
    // decalage est retourne par testcle() qui presuppose l'existence de
    // la table poscle
	int step, oct;
	unsigned int xrl;
	MusLaidOutLayerElement *element;
	int dec;
	int pos=1, fact, i, c, clid=UT1, rupture=-1;	// rupture, 1e pos (in array) ou
		// un shift d'octave est necessaire; ensuite, fonctionne par modulo
		// pour pairs (pour ACCID_SHARP) et impair (pour ACCID_FLAT)
	int _oct;


	step = m_r->m_step1*8;
	//xrl = step + (this->indent? this->indent*10 : 0);
    // ax2 : we don't have staff->indent anymore. this is stored at the system level
    xrl = step;

	if (this->notAnc)
		xrl += m_r->m_step1;

	step = m_r->m_accidWidth[this->staffSize][0];

	dec = this->GetClefOffset(xrl);	// clef courante

	element = this->GetFirst();
	if (element && clefIndex.compte )
	{
		element = this->no_note ( element,FORWARD, CLE, &pos);
		if (element != NULL && element->m_x_abs < xrl && pos)
			this->GetClef (element,(char *)&clid);
	}

	if (this->armTyp==ACCID_SHARP) 	// direction de parcourt de l'array 
	{	pos = 0; fact = 1;	}
	else
	{	pos = 6; fact = -1;	}

	oct = 0;		// default

	if (clid==FA4 || clid==FA5 || clid==FA3 || clid==UT5)
		oct = pos ? -2 : -1;

	else
	{	if (pos && (clid==UT4 || clid==UT3 || clid==UT2 || clid==SOLva))
			oct = -1;

		else if (!pos && (clid==SOL1 || clid==SOL2))
			oct = 1;
	}

	if (!pos)	// c'est des DIESES
	{	if (clid == UT4)
			rupture = 2;
		else if (clid == UT3 || clid < FA3 || clid == SOLva || clid == FA5)
								// ut3, sol1-2, fa4, solva 
			rupture = 4;
	}
	else
	{	if (clid == FA3 || clid == UT5)
			rupture = 2;
		else if (clid == UT2)
			rupture = 4;
	}
	_oct = oct;

	for (i = 0; i < this->armNbr; i++, pos += fact, xrl+=step)
	{
		c = armatKey [pos];
		if (i%2 && this->armTyp == ACCID_FLAT) oct += 1;
		else oct = _oct; 
		if (clid == UT1 && i==1) oct = _oct;
// correctif pour BEMOLS, clef Ut1, descente apres SI initial. Controler??? 
		if (rupture==i)
		{	this->getOctDec (fact,_oct,rupture, &oct); rupture = i+1;	}

		//if (!modMetafile || in (xrl, drawRect.left, drawRect.right) && in (this->m_y_drawing, drawRect.top, drawRect.bottom+m_staffSize[staffSize]))
			((MusSymbol1*)element)->dess_symb ( dc,xrl,this->CalculatePitchPosY(c,dec, oct),ALTER,this->armTyp , this);
	}
	return xrl;

}
*/

/*
// Gets the y coordinate of the previous lyric. If lyric is NULL, it will return the y coordinate of the first lyric 
// in the stave. If there are no lyrics in the Stave -1 is returned.
int MusLaidOutLayer::GetLyricPos( MusSymbol1 *lyric )
{
	MusSymbol1 *tmp;
	if ( !lyric ){
		if ( !( tmp = GetFirstLyric() ) )
			return -1;
		return tmp->dec_y;
	}
	
	if ( !( tmp = GetPreviousLyric( lyric ) ) )
		return -1;
	return tmp->dec_y;
}

MusSymbol1 *MusLaidOutLayer::GetPreviousLyric( MusSymbol1 *lyric )
{
	if ( !lyric || m_elements.IsEmpty() || !lyric->m_note_ptr || lyric->m_note_ptr->no < 0 )
		return NULL;
	
	// If there are other lyrics attached to the note that lyric is attached to...
	if ( (int)lyric->m_note_ptr->m_lyrics.GetCount() > 1 ){
		bool check = false; // Keeps track if we have past the pointer to this element in m_lyrics
		for ( int i = (int)lyric->m_note_ptr->m_lyrics.GetCount() - 1; i >= 0; i-- ){
			MusSymbol1 *previousLyric = &lyric->m_note_ptr->m_lyrics[i];
			if ( check ) return previousLyric;
			if ( previousLyric == lyric ) check = true;
		}
	}
	// Check previous note in staff for lyric
	int no = lyric->m_note_ptr->no - 1;
	while ( no >= 0 ){
		if ( m_elements[ no ].IsNote() ){
			for ( int i = (int) ((MusNote1*)&m_elements[ no ])->m_lyrics.GetCount() - 1; i >= 0 ; i-- ){
				MusSymbol1 *previousLyric = &((MusNote1*)&m_elements[ no ])->m_lyrics[i];
				if ( previousLyric ) return previousLyric;
			}
		}
		no--;
	}
	return NULL;
}

MusSymbol1 *MusLaidOutLayer::GetNextLyric( MusSymbol1 *lyric )
{	
	if ( !lyric || m_elements.IsEmpty() || !lyric->m_note_ptr || lyric->m_note_ptr->no > (int)m_elements.GetCount() - 1 )
		return NULL;
	
	// If there are other lyrics attached to the note that lyric is attached to...
	if ( (int)lyric->m_note_ptr->m_lyrics.GetCount() > 1 ){
		bool check = false; // Keeps track if we have past the pointer to this element in m_lyrics
		for ( int i = 0; i < (int)lyric->m_note_ptr->m_lyrics.GetCount(); i++ ){
			MusSymbol1 *nextLyric = &lyric->m_note_ptr->m_lyrics[i];
			if ( check ) 
				return nextLyric;
			if ( nextLyric == lyric ) 
				check = true;
		}
	}
	// Check next note in staff for lyric
	int no = lyric->m_note_ptr->no + 1;
	while ( no < (int)m_elements.GetCount() ){
		if ( m_elements[ no ].IsNote() ){
			for ( int i = 0; i < (int) ((MusNote1*)&m_elements[ no ])->m_lyrics.GetCount(); i++ ){
				MusSymbol1 *nextLyric = &((MusNote1*)&m_elements[ no ])->m_lyrics[i];
				if ( nextLyric )
					return nextLyric;
			}
		}
		no++;
	}
	return NULL;
}

MusSymbol1 *MusLaidOutLayer::GetFirstLyric( )
{
	if ( m_elements.IsEmpty() )
		return NULL;
	int no = 0;
	while ( no < (int)m_elements.GetCount() ){
		if ( m_elements[ no ].IsNote() ){
			for ( int i = 0; i < (int) ((MusNote1*)&m_elements[ no ])->m_lyrics.GetCount(); i++ ){
				MusSymbol1 *lyric = &((MusNote1*)&m_elements[ no ])->m_lyrics[i];
				if ( lyric )
					return lyric;
			}
		}
		no++;
	}
	return NULL;	
}

MusSymbol1 *MusLaidOutLayer::GetLastLyric( )
{
	if ( m_elements.IsEmpty() )
		return NULL;
	int no = (int)m_elements.GetCount() - 1;
	while ( no >= 0 ){
		if ( m_elements[ no ].IsNote() ) {
			for ( int i = (int) ((MusNote1*)&m_elements[ no ])->m_lyrics.GetCount() - 1; i >= 0 ; i-- ){
				MusSymbol1 *lyric = &((MusNote1*)&m_elements[ no ])->m_lyrics[i];
				if ( lyric )
					return lyric;
			}
		}
		no--;
	}
	return NULL;
}

MusSymbol1 *MusLaidOutLayer::GetLyricAtPos( int x )
{
	MusSymbol1 *lyric = this->GetFirstLyric();
	if ( !lyric )
		return NULL;
	
	//int xx = 0;
	int dif = x - lyric->m_x_abs;
	while ( this->GetNextLyric( lyric ) && (int)lyric->m_x_abs < x ){
		lyric = this->GetNextLyric( lyric );
		if ( (int)lyric->m_x_abs > x && dif < (int)lyric->m_x_abs - x )
			return this->GetPreviousLyric( lyric );
		dif = x - lyric->m_x_abs;
	}
		
	return lyric;
}

void MusLaidOutLayer::DeleteLyric( MusSymbol1 *symbol )
{
	if ( !symbol ) return;
	
	MusNote1 *note = symbol->m_note_ptr;
	for ( int i = 0; i < (int)note->m_lyrics.GetCount(); i++ ){
		MusSymbol1 *lyric = &note->m_lyrics[i];
		if ( symbol == lyric )
			note->m_lyrics.Detach(i);
	}
	
	delete symbol;
}

MusNote1 *MusLaidOutLayer::GetNextNote( MusSymbol1 * lyric )
{
	if ( !lyric || m_elements.IsEmpty() || !lyric->m_note_ptr || lyric->m_note_ptr->no >= (int)m_elements.GetCount() - 1 )
		return NULL;
	
	int no = lyric->m_note_ptr->no + 1;
	for ( int i = no; i < (int)m_elements.GetCount(); i++ ){
		MusLaidOutLayerElement *element = &m_elements[i];
		if ( element->IsNote() && ((MusNote1*)element)->sil == _NOT )
			return (MusNote1*)element; 
	}
	return NULL;
}

MusNote1 *MusLaidOutLayer::GetPreviousNote( MusSymbol1 * lyric )
{
	if ( !lyric || m_elements.IsEmpty() || !lyric->m_note_ptr || lyric->m_note_ptr->no <= 0 )
		return NULL;
	
	int no = lyric->m_note_ptr->no - 1;
	for ( int i = no; i >= 0; i-- ){
		MusLaidOutLayerElement *element = &m_elements[i];
		if ( element->IsNote() && ((MusNote1*)element)->sil == _NOT )
			return (MusNote1*)element; 
	}
	return NULL;
}

//Switches the note association of lyric from oldNote to newNote and modifies the two notes accordingly
//bool beginning: indicates if we want to add the lyric to beginning or end of the lyric array in newNote 
//		true = beginning of array
//		false = end of array
void MusLaidOutLayer::SwitchLyricNoteAssociation( MusSymbol1 *lyric, MusNote1 *oldNote, MusNote1* newNote, bool beginning )
{
	if ( !lyric || !oldNote || !newNote )
		return;
	
	lyric->m_note_ptr = newNote;
	if ( beginning )
		newNote->m_lyrics.Insert( lyric, 0 );
	else
		newNote->m_lyrics.Insert( lyric, newNote->m_lyrics.GetCount() );
	
	for ( int i = 0; i < (int)oldNote->m_lyrics.GetCount(); i++ ){
		MusSymbol1 *element = &oldNote->m_lyrics[i];
		if ( element == lyric ){
			oldNote->m_lyrics.Detach(i);
			break;
		}			
	}
}

void MusLaidOutLayer::AdjustLyricLineHeight( int delta ) 
{
	for ( int i = 0; i < (int)m_elements.GetCount(); i++ ){
		MusLaidOutLayerElement *element = &m_elements[i];
		if ( element->IsNote() ){
			for ( int j = 0; j < (int)((MusNote1*)element)->m_lyrics.GetCount(); j++ ){
				MusSymbol1 *lyric = &((MusNote1*)element)->m_lyrics[j];
				lyric->dec_y += delta;
			}
		}
	}
}
*/

// functors for MusLaidOutLayer

void MusLaidOutLayer::Process(MusFunctor *functor, wxArrayPtrVoid params )
{
    if (functor->m_success) {
        return;
    }
    
    MusLaidOutLayerElementFunctor *elementFunctor = dynamic_cast<MusLaidOutLayerElementFunctor*>(functor);
    MusLaidOutLayerElement *element;
	int i;
    for (i = 0; i < (int)m_elements.GetCount(); i++) 
	{
        element = &m_elements[i];
        functor->Call( element, params );
        if (elementFunctor) { // is is a MusLaidOutLayerElementFunctor, call it
            elementFunctor->Call( element, params );
        }
        else { // process it further
            // nothing we can do...
        }
	}
}

void MusLaidOutLayer::CopyElements( wxArrayPtrVoid params )
{
    // param 0: MusLaidOutLayer
    MusLaidOutLayer *staff = (MusLaidOutLayer*)params[0];

	MusLaidOutLayerElement *element = staff->GetLast();
    int x_last = 0;
    if (element) {
        x_last = element->m_x_abs;
    }
	int i;
    for (i = 0; i < this->GetElementCount(); i++) 
	{
        /*
		if ( m_elements[i].IsNote() )
		{
			MusNote1 *nnote = new MusNote1( *(MusNote1*)&m_elements[i] );
            nnote->m_x_abs += x_last;
			staff->m_elements.Add( nnote );
		}
		else
		{
			MusSymbol1 *nsymbol = new MusSymbol1( *(MusSymbol1*)&m_elements[i] );
            nsymbol->m_x_abs += x_last;
			staff->m_elements.Add( nsymbol );
		}
        */
	}
}


void MusLaidOutLayer::GetMaxXY( wxArrayPtrVoid params )
{
    /*
    wxASSERT_MSG( m_staff, "Pointer to staff cannot be NULL" );

    // param 0: int
    int *max_x = (int*)params[0];
    int *max_y = (int*)params[1]; // !!this is given in term of staff line space

	MusLaidOutLayerElement *element = this->GetLast();
    if (element) {
        int last_max = element->m_x_abs;
        //if (!element->IsSymbol() || (((MusSymbol1*)element)->flag != BARRE)) {
        //    last_max += 35; // abirtrary margin;
        //}
        if ((*max_x) < last_max) {
            *max_x = last_max;
        }
    }
    (*max_y) += m_staff->ecart;
    */ // ax2
}
