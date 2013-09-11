/////////////////////////////////////////////////////////////////////////////
// Name:        muslayer.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musio.h"
#include "muslayer.h"
#include "muslayerelement.h"

#include "musdef.h"
#include "musdoc.h"
#include "musclef.h"
#include "musnote.h"
#include "mussymbol.h"

#include <typeinfo>

// sorting function
int SortElements(MusLayerElement **first, MusLayerElement **second)
{
	if ( (*first)->m_x_abs < (*second)->m_x_abs )
		return -1;
	else if ( (*first)->m_x_abs > (*second)->m_x_abs )
		return 1;
	else 
		return 0;
}

//----------------------------------------------------------------------------
// MusLayer
//----------------------------------------------------------------------------

MusLayer::MusLayer( int logLayerNb ):
	MusDocObject()
{
    wxASSERT( logLayerNb > 0 );  
    m_logLayerNb = logLayerNb;
    
	Clear( );
}

MusLayer::~MusLayer()
{
    
}

void MusLayer::Clear()
{
    ClearChildren();
	voix = 0;
}


bool MusLayer::Save( wxArrayPtrVoid params )
{
    // param 0: output stream
    MusFileOutputStream *output = (MusFileOutputStream*)params[0];       
    return !output->WriteLayer( this );
}


void MusLayer::AddElement( MusLayerElement *element, int idx )
{
	element->SetParent( this );
    if ( idx == -1 ) {
        m_children.Add( element );
    }
    else {
        m_children.Insert( element, idx );
    }
    Modify();
}

void MusLayer::CopyAttributes( MusLayer *nlayer )
{
	if ( !nlayer )
		return;

	nlayer->Clear();
	nlayer->voix = voix;
}

int MusLayer::GetLayerNo() const
{
    wxASSERT_MSG( m_parent, "LaidOutStaff cannot be NULL");
    
    return m_parent->m_children.Index( *this );
}



MusLayerElement *MusLayer::GetFirst( )
{
	if ( m_children.IsEmpty() )
		return NULL;
	return (MusLayerElement*)&m_children[0];
}

MusLayerElement *MusLayer::GetLast( )
{
	if ( m_children.IsEmpty() )
		return NULL;
	int i = (int)m_children.GetCount() - 1;
	return (MusLayerElement*)&m_children[i];
}

MusLayerElement *MusLayer::GetNext( MusLayerElement *element )
{	
    this->ResetList( this );
    
    if ( !element || m_list.IsEmpty() )
        return NULL;
    
	int i = m_list.IndexOf( element );
    
	if ((i == wxNOT_FOUND )|| (i >= (int)m_list.GetCount() - 1 )) 
        return NULL;
    
    ListOfMusObjects::compatibility_iterator item = m_list.Item( i + 1 );
    return (MusLayerElement*)item->GetData();
}

MusLayerElement *MusLayer::GetPrevious( MusLayerElement *element )
{
    this->ResetList( this );
    
    if ( !element || m_list.IsEmpty() )
        return NULL;
    
	int i = m_list.IndexOf( element );
    
	if ((i == wxNOT_FOUND ) || ( i <= 0 ))
        return NULL;
    
    ListOfMusObjects::compatibility_iterator item = m_list.Item( i - 1 );
    return (MusLayerElement*)item->GetData();
}

MusLayerElement *MusLayer::GetAtPos( int x )
{
	MusLayerElement *element = this->GetFirst();
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

MusLayerElement *MusLayer::Insert( MusLayerElement *element, int x )
{
	if ( !element ) { 
        return NULL;
    }
    
    // This is temporary and is OK because we have only one single layout for now.
    // Inserting elements should be done from the logical tree and then update the layout
    
    MusLayerElement *insertElement = element->GetChildCopy();

    // First we find the element after which we are inserting the element
    // If not, it will be NULL
    // We are also updating the section and measure ( TODO, not necessary for now )
    int idx = 0;
	MusLayerElement *next = this->GetFirst();
	while ( next && (next->m_x_abs < x) )
	{
        idx++;
        // update section and measure if necessary (no section breaks and measure breaks for now)
		if ( this->GetNext( next ) )
			next = this->GetNext( next );
		else
			break;
	}
    
    // Insert in the logical tree
    insertElement->m_x_abs = x;
    AddElement( insertElement, idx );
    
	Refresh();
    //
	return insertElement;
}


void MusLayer::Insert( MusLayerElement *layerElement, MusLayerElement *before )
{
    int idx = 0;
    if ( before ) {
        idx = m_children.Index( *before );
    }
    AddElement( layerElement , idx );
}


void MusLayer::Delete( MusLayerElement *element )
{
	if ( !element ) {
        return;
    }
    
    bool is_clef = false;
        
    if ( element->IsClef() ) {
        is_clef = true;
        //m_r->OnBeginEditionClef();
    }
	
    int pos = m_children.Index( *element );
    m_children.RemoveAt( pos );
    Modify();

	if ( is_clef )
	{
        //m_r->OnEndEditionClef();
	}
    
    Refresh();
}


// Dans la direction indiquee (direction), cavale sur tout element qui n'est pas un
// symbol, de la nature indiquee (flg). Retourne le ptr si succes, ou 
// l'element de depart; le ptr succ est vrai si symb trouve.

MusLayerElement *MusLayer::GetFirst( MusLayerElement *element, unsigned int direction, const std::type_info *elementType, bool *succ)
{	
    MusLayerElement *original = element;

	*succ = false;
	if (element == NULL)
		return (element);

	int i = m_children.Index( *element, (direction==BACKWARD) );
	if ( i == wxNOT_FOUND )
		return (element);

    *succ = true; // we assume we will find it. Change to false if not
    while ( typeid(*element) != *elementType )
	{
		if (direction==BACKWARD)
		{	
			if (i < 1) {
                *succ = false;
                break;
            }
			i--;
			element = (MusLayerElement*)&m_children[i];
		}
		else
		{	if (i >= (int)m_children.GetCount() - 1 ) {
                *succ = false;
                break;
            }
			i++;
			element = (MusLayerElement*)&m_children[i];
		}
	}	

	return (*succ ? element : original);
}

void MusLayer::CheckXPosition( MusLayerElement *currentElement )
{
    if (!currentElement) {
        return;
    }
    
    MusLayerElement *previous = GetPrevious( currentElement );
    MusLayerElement *next = GetNext( currentElement );
    
    if ( previous && previous->m_x_abs >= currentElement->m_x_abs ) {
        currentElement->m_x_abs = previous->m_x_abs + 2;
    }
    if ( next && next->m_x_abs <= currentElement->m_x_abs ) {
        currentElement->m_x_abs = next->m_x_abs - 2;
    }
}

MusClef* MusLayer::GetClef( MusLayerElement *test )
{
	bool succ=false;

    if (!test) {
        return NULL;
    }
	
    if ( !test->IsClef() )
    {	
        test = GetFirst(test, BACKWARD, &typeid(MusClef), &succ);
    }

    MusClef *clef = dynamic_cast<MusClef*>(test);
    return clef;
}

int MusLayer::GetClefOffset( MusLayerElement *test )
{
    MusClef *clef = GetClef(test);
    if (!clef) {
        return 0;
    }
    return clef->GetClefOffset();
    
}


// alternateur de position d'octave 
void MusLayer::getOctDec (int ft, int _ot, int rupt, int *oct)
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

void MusLayer::RemoveClefAndCustos()
{
    MusClef *currentClef = NULL;
    
    int i;
    int elementCount =  this->GetElementCount();
    for (i = 0; i < elementCount; i++)
    {
        if ( ((MusLayerElement*)&m_children[i])->IsClef() ) {
            MusClef *clef = dynamic_cast<MusClef*>(&m_children[i]);
            // we remove the clef because it is the same as the previous one
            if ( currentClef && ((*currentClef) == (*clef)) ) {
                // check if it is a F clef with a Longa before
                if ( (i > 0) && ((MusLayerElement*)&m_children[i - 1])->IsNote() )
                {
                    MusNote *note = dynamic_cast<MusNote*>(&m_children[i - 1]);
                    if ( note && (note->m_dur == DUR_LG) )
                    {
                        bool removeLonga = false;
                        // we check only for the pitch, not the octave, but should be enough
                        if ( (clef->m_clefId == FA3) && ( note->m_pname == PITCH_G ) )
                            removeLonga = true;
                        else if ( (clef->m_clefId == FA4) && ( note->m_pname == PITCH_B ) )
                            removeLonga = true;
                        else if ( (clef->m_clefId == FA5) && ( note->m_pname == PITCH_D ) )
                            removeLonga = true;
                        if ( removeLonga ) {
                            this->Delete( note );
                            elementCount--;
                            i--;
                        }
                    }
                }
                this->Delete( clef );
                elementCount--;
                // now remove alterations (keys)
                for (; i < elementCount; i++) {
                    MusSymbol *accid = dynamic_cast<MusSymbol*>(&m_children[i]);
                    if ( accid && accid->IsSymbol( SYMBOL_ACCID ) ) {
                        this->Delete( accid );
                        elementCount--;
                        i--;                        
                    }
                    else
                        break;
                }
                i--;
                
            }
            else {
                currentClef = clef;
            }
        }
        else if ( ((MusLayerElement*)&m_children[i])->IsSymbol( SYMBOL_CUSTOS ) ) {
            MusSymbol *symbol = dynamic_cast<MusSymbol*>(&m_children[i]);
            this->Delete( symbol );
            elementCount--;
            i--;
        }
    }
}

/*
static char armatKey[] = {PITCH_F,PITCH_C,PITCH_G,PITCH_D,PITCH_A,PITCH_E,PITCH_B};
int MusLayer::armatDisp ( MusDC *dc )
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
	MusLayerElement *element;
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
int MusLayer::GetLyricPos( MusSymbol1 *lyric )
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

MusSymbol1 *MusLayer::GetPreviousLyric( MusSymbol1 *lyric )
{
	if ( !lyric || m_children.IsEmpty() || !lyric->m_note_ptr || lyric->m_note_ptr->no < 0 )
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
		if ( m_children[ no ].IsNote() ){
			for ( int i = (int) ((MusNote1*)&m_children[ no ])->m_lyrics.GetCount() - 1; i >= 0 ; i-- ){
				MusSymbol1 *previousLyric = &((MusNote1*)&m_children[ no ])->m_lyrics[i];
				if ( previousLyric ) return previousLyric;
			}
		}
		no--;
	}
	return NULL;
}

MusSymbol1 *MusLayer::GetNextLyric( MusSymbol1 *lyric )
{	
	if ( !lyric || m_children.IsEmpty() || !lyric->m_note_ptr || lyric->m_note_ptr->no > (int)m_children.GetCount() - 1 )
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
	while ( no < (int)m_children.GetCount() ){
		if ( m_children[ no ].IsNote() ){
			for ( int i = 0; i < (int) ((MusNote1*)&m_children[ no ])->m_lyrics.GetCount(); i++ ){
				MusSymbol1 *nextLyric = &((MusNote1*)&m_children[ no ])->m_lyrics[i];
				if ( nextLyric )
					return nextLyric;
			}
		}
		no++;
	}
	return NULL;
}

MusSymbol1 *MusLayer::GetFirstLyric( )
{
	if ( m_children.IsEmpty() )
		return NULL;
	int no = 0;
	while ( no < (int)m_children.GetCount() ){
		if ( m_children[ no ].IsNote() ){
			for ( int i = 0; i < (int) ((MusNote1*)&m_children[ no ])->m_lyrics.GetCount(); i++ ){
				MusSymbol1 *lyric = &((MusNote1*)&m_children[ no ])->m_lyrics[i];
				if ( lyric )
					return lyric;
			}
		}
		no++;
	}
	return NULL;	
}

MusSymbol1 *MusLayer::GetLastLyric( )
{
	if ( m_children.IsEmpty() )
		return NULL;
	int no = (int)m_children.GetCount() - 1;
	while ( no >= 0 ){
		if ( m_children[ no ].IsNote() ) {
			for ( int i = (int) ((MusNote1*)&m_children[ no ])->m_lyrics.GetCount() - 1; i >= 0 ; i-- ){
				MusSymbol1 *lyric = &((MusNote1*)&m_children[ no ])->m_lyrics[i];
				if ( lyric )
					return lyric;
			}
		}
		no--;
	}
	return NULL;
}

MusSymbol1 *MusLayer::GetLyricAtPos( int x )
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

void MusLayer::DeleteLyric( MusSymbol1 *symbol )
{
	if ( !symbol ) return;
	
	MusNote1 *note = symbol->m_note_ptr;
	for ( int i = 0; i < (int)note->m_lyrics.GetCount(); i++ ){
		MusSymbol1 *lyric = &note->m_lyrics[i];
		if ( symbol == lyric )
			note->m_lyrics.Detach(i);
	}
	
	this->Delete( symbol );
}

MusNote1 *MusLayer::GetNextNote( MusSymbol1 * lyric )
{
	if ( !lyric || m_children.IsEmpty() || !lyric->m_note_ptr || lyric->m_note_ptr->no >= (int)m_children.GetCount() - 1 )
		return NULL;
	
	int no = lyric->m_note_ptr->no + 1;
	for ( int i = no; i < (int)m_children.GetCount(); i++ ){
		MusLayerElement *element = &m_children[i];
		if ( element->IsNote() && ((MusNote1*)element)->sil == _NOT )
			return (MusNote1*)element; 
	}
	return NULL;
}

MusNote1 *MusLayer::GetPreviousNote( MusSymbol1 * lyric )
{
	if ( !lyric || m_children.IsEmpty() || !lyric->m_note_ptr || lyric->m_note_ptr->no <= 0 )
		return NULL;
	
	int no = lyric->m_note_ptr->no - 1;
	for ( int i = no; i >= 0; i-- ){
		MusLayerElement *element = &m_children[i];
		if ( element->IsNote() && ((MusNote1*)element)->sil == _NOT )
			return (MusNote1*)element; 
	}
	return NULL;
}

//Switches the note association of lyric from oldNote to newNote and modifies the two notes accordingly
//bool beginning: indicates if we want to add the lyric to beginning or end of the lyric array in newNote 
//		true = beginning of array
//		false = end of array
void MusLayer::SwitchLyricNoteAssociation( MusSymbol1 *lyric, MusNote1 *oldNote, MusNote1* newNote, bool beginning )
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

void MusLayer::AdjustLyricLineHeight( int delta ) 
{
	for ( int i = 0; i < (int)m_children.GetCount(); i++ ){
		MusLayerElement *element = &m_children[i];
		if ( element->IsNote() ){
			for ( int j = 0; j < (int)((MusNote1*)element)->m_lyrics.GetCount(); j++ ){
				MusSymbol1 *lyric = &((MusNote1*)element)->m_lyrics[j];
				lyric->dec_y += delta;
			}
		}
	}
}
*/

// functors

bool MusLayer::CopyToLayer( wxArrayPtrVoid params )
{  
    // Things we might want to add: 
    // - checking that the parent is a staff to avoid copying MusApp
    // - adding a parent nbLogStaff and a nbLogLayer parameter for copying a specific staff / layer
    
    
    // param 0: the MusLayer we need to copy to
	MusLayer *destinationLayer = (MusLayer*)params[0]; 
    // param 1: the uuid of the start element (if any)
    uuid_t *start = (uuid_t*)params[1];
    // param 2: the uuid of the end element (if any)
    uuid_t *end = (uuid_t*)params[2];
    // param 3: we have a start element and have started
    bool *has_started = (bool*)params[3];
    // param 4: we have an end element and have ended
    bool *has_ended = (bool*)params[4];
    // param 5: we want a new uuid for the copied elements
    bool *new_uuid = (bool*)params[5];
    
    if ( (*has_ended) ) {
        return true;
    }
    
    int i;
    for ( i = 0; i < this->GetElementCount(); i++ ) 
    {
        // check if we have a start uuid or if we already passed it
        if ( !uuid_is_null( *start ) && !(*has_started) ) {
            if ( uuid_compare( (*start), *(&m_children[i])->GetUuid() ) == 0 ) {
                (*has_started) = true;
            } 
            else {
                continue;
            }
        }
        
        // copy and add it
        MusLayerElement *copy = ((MusLayerElement*)&m_children[i])->GetChildCopy( (*new_uuid) );
        destinationLayer->AddElement( copy );
        
        // check if we have a end uuid and if we have reached it. 
        if ( !uuid_is_null( *end ) ) {
            if ( uuid_compare( *end, *(&m_children[i])->GetUuid() ) == 0 ) {
                (*has_ended) = true;
                return true;
            }
        }

        
    }
    return false;

}

