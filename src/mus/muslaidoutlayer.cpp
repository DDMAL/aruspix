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
	if ( (*first)->m_xrel < (*second)->m_xrel )
		return -1;
	else if ( (*first)->m_xrel > (*second)->m_xrel )
		return 1;
	else 
		return 0;
}

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLaidOutLayers );

//----------------------------------------------------------------------------
// MusLaidOutLayer
//----------------------------------------------------------------------------

MusLaidOutLayer::MusLaidOutLayer( MusLayer *logLayer ):
	MusLayoutObject()
{
	Clear( );
    m_logLayer = logLayer;
}

MusLaidOutLayer::~MusLaidOutLayer()
{
}

void MusLaidOutLayer::Clear()
{
    m_staff = NULL;
	m_elements.Clear();
	voix = 0;
    
    //
    beamListPremier = NULL;
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


void MusLaidOutLayer::AddElement( MusLaidOutLayerElement *element )
{
	element->SetLayer( this );
	m_elements.Add( element );
    wxASSERT_MSG( element->m_layerElement, "Pointer to LayerElement cannot be NULL" );
    // also add it to the logical layer - how do we manage the position?
    // m_logLayer->m_layerElements.Add( element->m_layerElement );
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
//	while (this->GetNext(element) && ((int)element->m_xrel < x) )
//		element = this->GetNext( element );

	int dif = x - element->m_xrel;
	while ( this->GetNext( element ) && (int)element->m_xrel < x ){
		element = this->GetNext( element );
		if ( (int)element->m_xrel > x && dif < (int)element->m_xrel - x )
			return this->GetPrevious( element );
		dif = x - element->m_xrel;
	}
	
	return element;
}

MusLaidOutLayerElement *MusLaidOutLayer::Insert( MusLaidOutLayerElement *element )
{
    /*
	if ( !element ) return NULL;

	// copy element
	if ( element->IsSymbol() )
		element = new MusSymbol1( *(MusSymbol1*)element );
	else if ( element->IsNote() )
		element = new MusNote1( *(MusNote1*)element );
	else if ( element->IsNeume() )
		element = new MusNeume( *(MusNeume*)element );
	else if ( element->IsNeumeSymbol() )
		element = new MusNeumeSymbol( *(MusNeumeSymbol*)element );
//	else if ( element->IsNeume() ) 
//	{
	//	//copying a neume causes issues
//		element = new MusNeume( *(MusNeume*)element );
//		((MusNeume*)element)->n_pitches.resize(1);
//	}

	int idx = 0;
	MusLaidOutLayerElement *tmp = this->GetFirst();
	while ( tmp && (tmp->m_xrel < element->m_xrel) )
	{
		idx++;
		if ( this->GetNext( tmp ) )
			tmp = this->GetNext( tmp );
		else
			break;
	}

	if ( tmp &&  ((element->IsSymbol() && (((MusSymbol1*)element)->flag == CLE))
		|| (element->IsNeumeSymbol() && ((((MusNeumeSymbol*)element)->getValue() == NEUME_SYMB_CLEF_C) || (((MusNeumeSymbol*)element)->getValue() == NEUME_SYMB_CLEF_F)))) )
		
		m_r->OnBeginEditionClef();

	m_elements.Insert( element, idx );
	
	if ( (element->IsSymbol() && (((MusSymbol1*)element)->flag == CLE))
		|| (element->IsNeumeSymbol() && ((((MusNeumeSymbol*)element)->getValue() == NEUME_SYMB_CLEF_C) || (((MusNeumeSymbol*)element)->getValue() == NEUME_SYMB_CLEF_F))) )
		
		m_r->OnEndEditionClef();

	if (m_r)
		m_r->DoRefresh();
    */
    
    wxLogError( "MusLaidOutLayer::Insert missing in ax2" );
	return element;
}

void MusLaidOutLayer::Append( MusLaidOutLayerElement *element, int step )
{
	if ( !element ) return;
    
    // insert at the end of the staff with a random step
    MusLaidOutLayerElement *last = this->GetLast();
    if (last) {
        element->m_xrel += last->m_xrel + step;
    }
    else {
        element->m_xrel += step;
    }
	AddElement( element );
}


void MusLaidOutLayer::Delete( MusLaidOutLayerElement *element )
{
    /*
	if ( !element ) return;

	if ( m_r ) // effacement
	{
		if ( (element->IsSymbol() && (((MusSymbol1*)element)->flag == CLE))
			|| (element->IsNeumeSymbol() && ((((MusNeumeSymbol*)element)->getValue() == NEUME_SYMB_CLEF_C) || (((MusNeumeSymbol*)element)->getValue() == NEUME_SYMB_CLEF_F))) )
			
			m_r->OnBeginEditionClef();
	}
	
	m_elements.Detach( element->no );

	if ( m_r )
	{
		if ( (element->IsSymbol() && (((MusSymbol1*)element)->flag == CLE))
			|| (element->IsNeumeSymbol() && ((((MusNeumeSymbol*)element)->getValue() == NEUME_SYMB_CLEF_C) || (((MusNeumeSymbol*)element)->getValue() == NEUME_SYMB_CLEF_F))) )
			m_r->OnEndEditionClef();
		
		m_r->DoRefresh();
	}
	
	delete element;
    */
    wxLogError( "MusLaidOutLayer::Delete missing in ax2" );
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

	// calculer m_xrel, c et oct
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
		if (element != NULL && element->m_xrel < xrl && pos)
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

		//if (!modMetafile || in (xrl, drawRect.left, drawRect.right) && in (this->yrel, drawRect.top, drawRect.bottom+m_staffSize[staffSize]))
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
	int dif = x - lyric->m_xrel;
	while ( this->GetNextLyric( lyric ) && (int)lyric->m_xrel < x ){
		lyric = this->GetNextLyric( lyric );
		if ( (int)lyric->m_xrel > x && dif < (int)lyric->m_xrel - x )
			return this->GetPreviousLyric( lyric );
		dif = x - lyric->m_xrel;
	}
		
	return lyric;
}

void MusLaidOutLayer::DeleteLyric( MusSymbol1 *symbol )
{
	if ( !symbol ) return;
	
	
	if ( m_r ) // effacement
	{
		if ( symbol->IsSymbol() && (((MusSymbol1*)symbol)->IsLyric()) )
			m_r->OnBeginEditionClef();
	}
	
	MusNote1 *note = symbol->m_note_ptr;
	for ( int i = 0; i < (int)note->m_lyrics.GetCount(); i++ ){
		MusSymbol1 *lyric = &note->m_lyrics[i];
		if ( symbol == lyric )
			note->m_lyrics.Detach(i);
	}
	
	if ( m_r )
	{
		if ( symbol->IsSymbol() && (((MusSymbol1*)symbol)->IsLyric()) )
			m_r->OnEndEditionClef();
		m_r->DoRefresh();
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
        x_last = element->m_xrel;
    }
	int i;
    for (i = 0; i < this->GetElementCount(); i++) 
	{
        /*
		if ( m_elements[i].IsNote() )
		{
			MusNote1 *nnote = new MusNote1( *(MusNote1*)&m_elements[i] );
            nnote->m_xrel += x_last;
			staff->m_elements.Add( nnote );
		}
		else
		{
			MusSymbol1 *nsymbol = new MusSymbol1( *(MusSymbol1*)&m_elements[i] );
            nsymbol->m_xrel += x_last;
			staff->m_elements.Add( nsymbol );
		}
        */
	}
}


void MusLaidOutLayer::GetMaxXY( wxArrayPtrVoid params )
{
    wxASSERT_MSG( m_staff, "Pointer to staff cannot be NULL" );

    // param 0: int
    int *max_x = (int*)params[0];
    int *max_y = (int*)params[1]; // !!this is given in term of staff line space

	MusLaidOutLayerElement *element = this->GetLast();
    if (element) {
        int last_max = element->m_xrel;
        //if (!element->IsSymbol() || (((MusSymbol1*)element)->flag != BARRE)) {
        //    last_max += 35; // abirtrary margin;
        //}
        if ((*max_x) < last_max) {
            *max_x = last_max;
        }
    }
    (*max_y) += m_staff->ecart;
}
