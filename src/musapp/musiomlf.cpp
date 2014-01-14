/////////////////////////////////////////////////////////////////////////////
// Name:        musiomlf.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Authors and others. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/filename.h"
#include "wx/textfile.h"
#include "wx/tokenzr.h"
#include "wx/txtstrm.h"

#include "musiomlf.h"
#include "musmlfdic.h"

#include "verovio/page.h"
#include "verovio/system.h"
#include "verovio/staff.h"
#include "verovio/layer.h"
#include "verovio/layerelement.h"

#include "verovio/clef.h"
#include "verovio/mensur.h"
#include "verovio/note.h"
#include "verovio/rest.h"
#include "verovio/symbol.h"

#include "im/impage.h"
#include "im/imstaff.h"

//#include "app/axprocess.h"
//#include "app/axprogressdlg.h"
//#include "app/axapp.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMLFSymbols );

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

using namespace vrv;

// utilise pour la transposition de silences

static char _sil1[] = {'A','C','C','E','E','G','G','B','B'};
static int _oct1[] =  { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , -1};

static char _sil0[] = {'B','B','D','D','F','F','A','A','C'};
static int _oct0[] =  { 0 , -1, 0 , 0 , 0 , 0 , 0 , 0 , 0 };

static char _note[] = {'B','C','D','E','F','G','A','B','C'};
//static char *_val[] = {"0","1","2","3","4","5","6","7","8"};

IMPLEMENT_DYNAMIC_CLASS(MusMLFSymbol, wxObject)

//----------------------------------------------------------------------------
// MusMLFSymbol
//----------------------------------------------------------------------------

MusMLFSymbol::MusMLFSymbol( ) :
	wxObject()
{
	m_type = 0;
	m_value = 0;
	m_pitch = 0;
	m_oct = 0;
	m_flag = 0;
	m_position = 0;
}

void MusMLFSymbol::SetValue( char type, wxString subtype, int position, int value, char pitch, int oct, int flag )
{
	m_type = type;
	m_subtype = subtype;
	m_position = position;
	m_value = value;
	m_pitch = pitch;
	m_oct = oct;
	m_flag = flag;
}


wxString MusMLFSymbol::GetLabel( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbol") );
		return "";
	}

	wxString label, label_width;
	label << m_type;

	if ((m_type == TYPE_NOTE) || (m_type == TYPE_REST))
	{	
		label << "_" << m_value << "_" << m_pitch << "_" << m_oct;
		if ( m_flag )
			label << "_" << m_flag;
	}
	else if (m_type == TYPE_KEY)
	{
		label << "_" << m_subtype << m_value;
	}
	else if  (m_type == TYPE_ALTERATION)
	{
		label << "_" << m_subtype << m_value << "_" << m_pitch << "_" << m_oct;
	}
	else if ((m_type == TYPE_CUSTOS) || (m_type == TYPE_POINT))
	{
		label << "_" << m_pitch << "_" << m_oct;
	}
	else if ((m_type == TYPE_MESURE) || (m_type == TYPE_SYMBOLE))
	{
		label << "_" << m_subtype;
	}
		
	return label;
}


int MusMLFSymbol::GetNbOfStates( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbol") );
		return 0;
	}
	
	int states = 0;

	if (m_type == TYPE_NOTE)
	{	
		switch (m_value)
		{
		case (0): states = 6; break;
		case (1): states = 6; break;
		case (2): states = 6; break;
		case (3): states = 6; break;
		case (4): states = 6; break;
		case (5): states = 6; break;
		case (6): states = 6; break;
		case (7): states = 6; break;
		}
	}
	else if (m_type == TYPE_REST)
	{
		switch (m_value)
		{
		case (0): states = 4; break;
		case (1): states = 4; break;
		case (2): states = 4; break;
		case (3): states = 4; break;
		case (4): states = 5; break;
		case (5): states = 5; break;
		case (6): states = 5; break;
		}	
	}
	else if (m_type == TYPE_KEY)
	{
		if (m_subtype == "S") states = 7;
		else if (m_subtype == "F") states = 6;
		else if (m_subtype == "U") states = 6;
	}
	else if  (m_type == TYPE_ALTERATION)
	{
		if (m_subtype == "D") states = 6;
		else if (m_subtype == "B") states = 6;
		else if (m_subtype == "H") states = 6;
	}
	else if (m_type == TYPE_CUSTOS)
	{
		states = 6;
	}
	else if (m_type == TYPE_POINT)
	{
		states = 4;
	}
	else if (m_type == TYPE_MESURE)
	{
		states = 6;
	}
	else if (m_type == TYPE_SYMBOLE)
	{
		if (m_subtype == "B") states = 4;
	}
	
	if ( states == 0 )
		wxLogWarning( _("Unknown states number for %s"), this->GetLabel().c_str() );

	return states;
}

int MusMLFSymbol::GetWidth( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbol") );
		return 0;
	}
	
	int width = 0;

	if (m_type == TYPE_NOTE)
	{	
		switch (m_value)
		{
		case (0): width = 30; break;
		case (1): width = 30; break;
		case (2): width = 30; break;
		case (3): width = 30; break;
		case (4): width = 30; break;
		case (5): width = 30; break;
		case (6): width = 30; break;
		case (7): width = 30; break;
		}
	}
	else if (m_type == TYPE_REST)
	{
		switch (m_value)
		{
		case (0): width = 10; break;
		case (1): width = 10; break;
		case (2): width = 10; break;
		case (3): width = 10; break;
		case (4): width = 20; break;
		case (5): width = 20; break;
		case (6): width = 20; break;
		}	
	}
	else if (m_type == TYPE_KEY)
	{
		if (m_subtype == "S") width = 30;
		else if (m_subtype == "F") width = 30;
		else if (m_subtype == "U") width = 30;
	}
	else if  (m_type == TYPE_ALTERATION)
	{
		if (m_subtype == "D") width = 30;
		else if (m_subtype == "B") width = 30;
		else if (m_subtype == "H") width = 30;
	}
	else if (m_type == TYPE_CUSTOS)
	{
		width = 30;
	}
	else if (m_type == TYPE_POINT)
	{
		width = 10;
	}
	else if (m_type == TYPE_MESURE)
	{
		width = 30;
	}
	else if (m_type == TYPE_SYMBOLE)
	{
		if (m_subtype == "B") width = 10;
	}
	
	if ( width == 0 )
		wxLogWarning( _("Unknown width number for %s"), this->GetLabel().c_str() );

	return width;
}

// retourne le label du type

wxString MusMLFSymbol::GetLabelType( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbol") );
		return "";
	}

	wxString label_width;
	label_width << m_type;

	if ((m_type == TYPE_NOTE) || (m_type == TYPE_REST))
	{	
		label_width << "_" << m_value;
		int flag = m_flag;
		if (flag & NOTE_STEM)
			flag -= NOTE_STEM; // remove not stem flag in type. will be treated in the image
		if ( flag )
			label_width << "_" << flag;
	}
	else if ((m_type == TYPE_KEY) || (m_type == TYPE_ALTERATION) || (m_type == TYPE_SYMBOLE))
	{
		label_width << "_" << m_subtype;
	}
	else if (m_type == TYPE_MESURE)
	{
		label_width << "_" << m_subtype;
	}
	return label_width;
}


//----------------------------------------------------------------------------
// FileOutputStream
//----------------------------------------------------------------------------

MusMLFOutput::MusMLFOutput( Doc *doc, wxString filename, MusMLFDictionary *dict, wxString model_symbol_name ) :
    wxFileOutputStream( filename )
{
    m_doc = doc;
    m_filename = filename;
    wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	m_addHeader = true;
	m_pagePosition = false;
	m_hmmLevel = false;
	m_layer = NULL;
	m_mlf_class_name = model_symbol_name;
	m_dict = dict;
}


MusMLFOutput::MusMLFOutput( Doc *doc, int fd, wxString filename, MusMLFDictionary *dict,  wxString model_symbol_name ) :
	wxFileOutputStream( fd )
{
    m_doc = doc;
    m_filename = filename;
    wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	m_addHeader = false;
	m_pagePosition = false;
	m_hmmLevel = false;
	m_layer = NULL;
	m_mlf_class_name = model_symbol_name;
	m_dict = dict;
}

MusMLFOutput::~MusMLFOutput()
{
}

// specific
/*
 Removed in version 2.0.0
 
Staff *MusMLFOutput::SplitSymboles( Staff *staff )
{
	unsigned int k;

	Staff *nstaff = new Staff();
	staff->CopyAttributes( nstaff );
	Note1 *nnote = NULL;
	Symbol1 *nsymbol1 = NULL;
	Symbol1 *nsymbol2 = NULL;

    for (k = 0;k < staff->GetElementCount() ; k++ )
    {
		if ( staff->m_elements[k].IsNote() )
		{
			nnote = new Note1( *(Note1*)&staff->m_elements[k] );
			// alteration
			if (nnote->acc != 0)
			{
				nsymbol2 = new Symbol1();
				nsymbol2->flag = ALTER;
				nsymbol2->calte = nnote->acc;
				nsymbol2->m_xAbs = nnote->m_xAbs - 10*4; // 10 = pas par defaut
				nsymbol2->dec_y = nnote->dec_y;
				nsymbol2->oct = nnote->oct;
				nsymbol2->code = nnote->code;
				nstaff->m_elements.Add( nsymbol2 );
				nnote->acc = 0;
			}
			nstaff->m_elements.Add( nnote );
			// point
			if (nnote->point == POINTAGE)
			{
				if ( nnote->sil == _SIL )
					wxLogWarning( _("Pointed rest not allowed, point has been removed.") );
				else
				{
					nsymbol2 = new Symbol1();
					nsymbol2->flag = PNT;
					nsymbol2->point = 0;
					nsymbol2->m_xAbs = nnote->m_xAbs + 30;
					nsymbol2->dec_y = nnote->dec_y;
					nsymbol2->oct = nnote->oct;
					nsymbol2->code = nnote->code;
					nstaff->m_elements.Add( nsymbol2 );
				}
				nnote->point = 0;
			}				
		}
		if ( staff->m_elements[k].IsSymbol() )
		{
			nsymbol1 = new Symbol1( *(Symbol1*)&staff->m_elements[k] );
			nstaff->m_elements.Add( nsymbol1 );
			// barre TODO repetion points
			if (nsymbol1->flag == BARRE)
			{			
				if ( nsymbol1->code == 'D' ) // double barre
				{
					nsymbol2 = new Symbol1( *nsymbol1 );
					nsymbol2->code = '|';
					nsymbol2->m_xAbs += 10;
					nstaff->m_elements.Add( nsymbol2 );
					nsymbol1->code = '|';
				}
			}
			// indication de mesure - splitter indication avec chiffre ET symbol
			else if (nsymbol1->flag == IND_MES)
			{
				if ((nsymbol1->code & 1) && (nsymbol1->code != 1)) // pas chiffre seuls
				{
					nsymbol2 = new Symbol1( *nsymbol1 );
					nsymbol2->code = 1;
					nsymbol2->m_xAbs += 10*5; // 10 = pas par defaut
					nstaff->m_elements.Add( nsymbol2 );
					nsymbol1->code -= 1;
				}
			}
		}
    }
	nstaff->CheckIntegrity( );
	return nstaff;
}
*/

void MusMLFOutput::StartLabel( )
{
	wxString label = wxString::Format("\"*/%s_%d.0.lab\"\n", m_shortname.c_str(), m_staff_i );
	Write( label, label.Length() );	
	m_symbols.Clear();

}

void MusMLFOutput::EndLabel( int offset, int end_point )
{
	for (int i = 0; i < (int)m_symbols.GetCount(); i++ )
	{
		wxString word;
		wxString word_label = m_symbols[i].GetLabel();
		if ( word_label == "" ) // skip empty labels
			continue;
		int pos;
		
		// si verifier dans le dictionnaire, tenir jour la liste dans 
		// The dictionnary is now ready to handle several hmm (and states)
		// per word. To enable this, the MusMLFSymbol class would have to be modified,
		// as for now a word has has only one hmm and we assume that the hmm name
		// is identical to the word (but lowercase)
		if ( m_dict && m_dict->Index( word_label ) == wxNOT_FOUND )
		{
			
			MusMLFWord *w = new MusMLFWord();
			w->m_word = word_label;
			wxString hmm = word_label;
			hmm.MakeLower();
			w->m_hmms.Add( hmm );
			w->m_states.push_back( m_symbols[i].GetNbOfStates() );
			m_dict->m_dict.Add( w );
			m_dict->m_dict.Sort( SortMLFWords );
		}
		
		
		// here we write the symbols rather than the word, with staff position. Valid only if we 
		// have the imPage and offsets. Maybe would be better to have this in a child class?
		// currently does not use the dictionnary as it should. It just convert the word to lower case
		// See comment above
		if ( m_hmmLevel )
		{
			wxASSERT( offset != -1 );
			wxASSERT( end_point != -1 );
			
			int fact = 100;
		
			wxString hmm;
			wxString hmm_label = word_label;
			hmm_label.MakeLower();
			pos = m_symbols[i].GetPosition() - offset;
			if ( pos < 0 )
				pos = 0;
			
			int next = end_point;
			int next_symbol = next;
			// position du symbol suivant, mais seulement si pas hors portee
			if ( i < (int)m_symbols.GetCount() - 1 )
				next_symbol = m_symbols[i+1].GetPosition() - offset;
			if ( next_symbol < next )
				next = next_symbol;
			
			int width = pos + m_symbols[i].GetWidth();
			// width
			if ( width > next )
			{
				//wxLogDebug( _("Label position %s out of range (symbol truncated)"), subsymbol_label.c_str() );
				width = next;
			}
			if ( pos < width ) // else skip symbol
				hmm << ( pos * fact) << " " << ( width * fact) << " " << hmm_label<< "\n";
			else
				wxLogDebug( _("Label position %s out of range (symbol skipped)"), hmm_label.c_str() );
	
			if ( width + WIN_WIDTH <  next ) // add sp
				hmm << ( width * fact) << " " << ( next * fact) << " {s}\n";
			 
			Write( hmm, hmm.Length() );		
		}
		
		else
		{
			if ( m_pagePosition )  // this is a basic position over the page, without checking overlaps 
									//between symbols nor overflow
			{
				pos = m_symbols[i].GetPosition();
				// really ???? this is not page position anymore.... 
				// I think that offset are used in CmpFile... has to be checked...
				if ( offset != -1 )
					pos -= offset;
				if ( pos < 0 )
					pos = 0;
				
				int width = pos + m_symbols[i].GetWidth();
				word << ( pos ) << " " << ( width ) << " "; // !!!!!!  VERIFIER !!!!!
			}
			word << word_label << "\n";
			Write( word, word.Length() );
		}
	}
	m_symbols.Clear();

	wxString end_label = ".\n";
	Write( end_label, end_label.Length() );
}
	
// copie le portee en convertissant les symbols de la clef courante vers Ut1
// si inPlace, directment dans staff

Layer *MusMLFOutput::GetUt1( Layer *layer )
{

	if ( !layer )
		return NULL;

	int code, oct;
	
	for (int i = 0; i < (int)layer->GetElementCount(); i++ )
	{
		if ( ((LayerElement*)layer->m_children[i])->HasPitchInterface() )
		{
			LayerElement *element = (LayerElement*)layer->m_children[i];
			{
				GetUt1( layer, element, &code, &oct );
                PitchInterface *pitchElement = dynamic_cast<PitchInterface*>(element);
				pitchElement->m_pname = code;
				pitchElement->m_oct = oct;
			}
		}
		else if ( ((LayerElement*)layer->m_children[i])->HasPositionInterface() )
		{
			LayerElement *element = (LayerElement*)layer->m_children[i];
			{
				GetUt1( layer, element, &code, &oct );
                PositionInterface *positionElement = dynamic_cast<PositionInterface*>(element);
				positionElement->m_pname = code;
				positionElement->m_oct = oct;
			}
		}
	}
    return NULL;
}


/* Calcule l'offset de la cle; doit etre separe de clefId pour pouvoir
  etre appele depuis rd_symbol() en cas de changement de definition.
	La valeur attribuee est un facteur de multiplication d'espaces
  (espace[staffSize]) decrivant l'eloignement de la clef par rapport a
  la ligne du bas.
*/

void MusMLFOutput::GetUt1( Layer *layer, LayerElement *pelement, int *code, int *oct)
{

	if (!pelement || !code || !oct) return;

    int offs = layer->GetClefOffset( pelement );
    
	if (dynamic_cast<PitchInterface*>(pelement)) {
        PitchInterface *pitchInterface = dynamic_cast<PitchInterface*>(pelement);
		*code = pitchInterface->m_pname + offs;
        *oct = pitchInterface->m_oct;
    }
	else if (dynamic_cast<PositionInterface*>(pelement)) {
        PositionInterface *positionInterface = dynamic_cast<PositionInterface*>(pelement);
		*code = positionInterface->m_pname + offs;
        *oct = positionInterface->m_oct;
    }
    else {
        return;
    }

	while (*code < 1)
	{
		*code += 7;
		(*oct)--;
	};
	while (*code > 7)
	{
		*code -= 7;
		(*oct)++;
	};
}

bool MusMLFOutput::WritePage( const Page *page, bool write_header )
{
	if ( write_header && m_addHeader )
	{
		Write("#!MLF!#\n",8);
		m_addHeader = false;
	}

    m_layer = NULL;
    for (m_staff_i = 0; m_staff_i < (int)page->GetSystemCount(); m_staff_i++) 
    {
        m_layer = (Layer*)page->m_children[m_staff_i]->m_children[0]->m_children[0];
        WriteLayer( m_layer );
		m_layer = NULL;
    }
    return true;

}

// idem ExportFile() puis WritePage(), mais gere la position des portee de imPage et les portee selon staff numbers
bool MusMLFOutput::WritePage( const Page *page, wxString filename, ImPage *imPage, std::vector<int> *staff_numbers )
{
	wxASSERT_MSG( page, "Page cannot be NULL" );
	wxASSERT_MSG( imPage, "ImPage cannot be NULL" );
	
    m_filename = filename;
	m_shortname = filename;
    //wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	
	if ( m_addHeader )
	{
		Write("#!MLF!#\n",8);
		m_addHeader = false;
	}
	
	int offset;
	int end_point;

    m_layer = NULL;
    for (m_staff_i = 0; m_staff_i < (int)page->GetSystemCount(); m_staff_i++) 
    {
		//if ( staff_numbers && ( staff_numbers->Index( m_staff_i ) == wxNOT_FOUND ) )
		//	continue; // commented in version 2.3.0

        // I think this is wrong, we need to loop through all the staves? maybe not
        m_layer = (Layer*)page->m_children[m_staff_i]->m_children[0]->m_children[0];
		imPage->m_staves[m_staff_i].GetMinMax( &offset, &end_point );
        WriteLayer( m_layer, offset, end_point );
		m_layer = NULL;
    }
    return true;

}


bool MusMLFOutput::WriteLayer( const Layer *layer, int offset,  int end_point )
{

	if (layer->GetElementCount() == 0)
		return true;

    int k;

	StartLabel();

    for (k = 0;k < layer->GetElementCount() ; k++ )
    {
        LayerElement *element = (LayerElement*)layer->m_children[k];
        // we could write all of the in one method, left over from version < 2.0.0
        if ( element->IsNote() || element->IsRest() || element->IsSymbol( SYMBOL_CUSTOS) )
        {
            WriteNote( element );
        }
        else
        {
            WriteSymbol( element );
        }
    }
	EndLabel( offset, end_point );

    return true;
}


bool MusMLFOutput::WriteNote( LayerElement *element )
{
	int  code, oct;
	GetUt1( m_layer, element, &code, &oct);

	MusMLFSymbol *mlfsb = NULL;

	// custos 
	if (element->IsSymbol( SYMBOL_CUSTOS ))
	{
		mlfsb = (MusMLFSymbol*)wxCreateDynamicObject( m_mlf_class_name );
		mlfsb->SetValue( TYPE_CUSTOS, "", element->m_xAbs, 0, _note[code], oct);
		m_symbols.Add( mlfsb );

	}
	// note ou silence
	else 
	{
		mlfsb = (MusMLFSymbol*)wxCreateDynamicObject( m_mlf_class_name );
		int flag = 0;
        if ( element->IsNote() ) {
            Note *note = dynamic_cast<Note*>(element);
            // hampe
            if ((note->m_stemDir != 0) && ((note->m_dur ==  DUR_LG) || (note->m_dur > DUR_1 )))
                flag += NOTE_STEM; // ?? // ax2
            
            // ligature
            if ((note->m_lig == LIG_INITIAL) && (note->m_dur >  DUR_LG) && (note->m_dur < DUR_2 ))
                flag += NOTE_LIGATURE;
                
            // coloration
            if ((note->m_colored == true) && (note->m_dur < DUR_2 ))
                flag += NOTE_COLORATION;
            else if ((note->m_ligObliqua == true) && (note->m_dur > DUR_4 ))
                flag += NOTE_COLORATION;
            
            mlfsb->SetValue( TYPE_NOTE, "", element->m_xAbs, note->m_dur, _note[code],oct, flag);
        }

		//silence
		if (element->IsRest())
		{
            Rest *rest = dynamic_cast<Rest*>(element);
			if ( oct % 2 )
				mlfsb->SetValue( TYPE_REST, "", element->m_xAbs, rest->m_dur, _sil0[code],oct + _oct0[code]);	
			else
				mlfsb->SetValue( TYPE_REST, "", element->m_xAbs, rest->m_dur, _sil1[code],oct + _oct1[code]);
		}

		m_symbols.Add( mlfsb );
	}
	return true;
}


/*
  flag
	B = BARRE
	C = CLE
	A = ALTER
	P = PNT
	I = IND_MES
  autre
  */

bool MusMLFOutput::WriteSymbol( LayerElement *element )
{
	MusMLFSymbol *mlfsb = (MusMLFSymbol*)wxCreateDynamicObject( m_mlf_class_name );

	if (element->IsBarline())
	{
		mlfsb->SetValue( TYPE_SYMBOLE, "B", element->m_xAbs );
		m_symbols.Add( mlfsb );
	}
	else if (element->IsClef())
	{
        Clef *clef = dynamic_cast<Clef*>(element);
		switch(clef->m_clefId)
		{	case SOL2 : mlfsb->SetValue( TYPE_KEY, "S",  element->m_xAbs, 2 ); break;
			case SOL1 : mlfsb->SetValue( TYPE_KEY, "S",  element->m_xAbs, 1 ); break;
			case SOLva : mlfsb->SetValue( TYPE_KEY, "S",  element->m_xAbs, 8 ); break;
			case FA5 : mlfsb->SetValue( TYPE_KEY, "F",  element->m_xAbs, 5 ); break;
			case FA4 : mlfsb->SetValue( TYPE_KEY, "F",  element->m_xAbs, 4 ); break;
			case FA3 : mlfsb->SetValue( TYPE_KEY, "F",  element->m_xAbs, 3 ); break;
			case UT1 : mlfsb->SetValue( TYPE_KEY, "U",  element->m_xAbs, 1 ); break;
			case UT2 : mlfsb->SetValue( TYPE_KEY, "U",  element->m_xAbs, 2 ); break;
			case UT3 : mlfsb->SetValue( TYPE_KEY, "U",  element->m_xAbs, 3 ); break;
			case UT5 : mlfsb->SetValue( TYPE_KEY, "U",  element->m_xAbs, 5); break;
			case UT4 : mlfsb->SetValue( TYPE_KEY, "U",  element->m_xAbs, 4 ); break;
			default: break;
		}
		m_symbols.Add( mlfsb );
	}
	else if (element->IsSymbol( SYMBOL_ACCID ))
	{
        Symbol *symbol = dynamic_cast<Symbol*>(element);
		int  code = 0, oct = 0;
		GetUt1( m_layer, element, &code, &oct);
		if (symbol->m_accid == ACCID_SHARP)
			mlfsb->SetValue( TYPE_ALTERATION, "D",  element->m_xAbs, 0, _note[code], oct );
		else if (symbol->m_accid == ACCID_FLAT)
			mlfsb->SetValue( TYPE_ALTERATION, "B",  element->m_xAbs, 0, _note[code], oct );
		else if (symbol->m_accid == ACCID_NATURAL)
			mlfsb->SetValue( TYPE_ALTERATION, "H",  element->m_xAbs, 0, _note[code], oct );
		else if (symbol->m_accid == ACCID_DOUBLE_SHARP)
			mlfsb->SetValue( TYPE_ALTERATION, "D",  element->m_xAbs, 1, _note[code], oct );
		else if (symbol->m_accid == ACCID_DOUBLE_FLAT)
			mlfsb->SetValue( TYPE_ALTERATION, "B",  element->m_xAbs, 1, _note[code], oct );
		m_symbols.Add( mlfsb );
	}
	else if (element->IsSymbol( SYMBOL_DOT ))
	{
		int  code = 0, oct = 0;
		GetUt1( m_layer, element, &code, &oct);
		mlfsb->SetValue( TYPE_POINT, "", element->m_xAbs, 0, _note[code + ((code+oct) % 2)], ((code + ((code+oct) % 2)) == 8) ? (oct+1) : oct );
		//str += wxString::Format("P_%s_%d\n",
		//	_note[code + ((code+oct) % 2)],((code + ((code+oct) % 2)) == 8) ? (oct+1) : oct,symbol->code,code,oct);
		m_symbols.Add( mlfsb );
	}
	else if (element->IsMensur())
	{
        Mensur *mensur = dynamic_cast<Mensur*>(element);
		// signes standard
		//if ((int)symbol->code & 64) // ax2 ??
        if (mensur->m_meterSymb)
		{
            wxLogWarning( _("Meter symbols not supported") );
            // We should use only figures (CH) and mensur signs - see below
			switch (mensur->m_meterSymb)
			{	
				case METER_SYMB_COMMON: mlfsb->SetValue( TYPE_MESURE, "S_C", element->m_xAbs ); break;
				case METER_SYMB_CUT: mlfsb->SetValue( TYPE_MESURE, "S_CB", element->m_xAbs ); break;
				case METER_SYMB_2: mlfsb->SetValue( TYPE_MESURE, "S_2", element->m_xAbs ); break;
				case METER_SYMB_3: mlfsb->SetValue( TYPE_MESURE, "S_3", element->m_xAbs ); break;
				case METER_SYMB_2_CUT: mlfsb->SetValue( TYPE_MESURE, "S_2B", element->m_xAbs ); break;
				case METER_SYMB_3_CUT: mlfsb->SetValue( TYPE_MESURE, "S_3B", element->m_xAbs ); break;
                default: {};
			}
			m_symbols.Add( mlfsb );
		}
		else if (mensur->m_sign)
		{
			wxString subtype;
			// temps parfait
			if (mensur->m_sign == MENSUR_SIGN_O)
				subtype += wxString::Format("TP");
			// temps imparfait	
			else if (mensur->m_sign == MENSUR_SIGN_C)
				subtype += wxString::Format("TI");
			// temps imparfait double
			else if ((mensur->m_sign == MENSUR_SIGN_C) && mensur->m_reversed)
				subtype += wxString::Format("TID");
			// barre
			if (mensur->m_slash)
				subtype += wxString::Format("_B");
			// prolation parfaite
			if (mensur->m_dot)
				subtype += wxString::Format("_P");

			mlfsb->SetValue( TYPE_MESURE, subtype, element->m_xAbs );
			m_symbols.Add( mlfsb );
		}
		// chiffres
		else
		{
			wxString subtype = wxString::Format("CH_%d_%d", max (mensur->m_num, 1), max(mensur->m_numBase, 1));
			mlfsb->SetValue( TYPE_MESURE, subtype, element->m_xAbs );
			m_symbols.Add( mlfsb );
		}
	}
	//else if ((symbol->flag == CHAINE ) && (symbol->calte == NUMMES))
	//{}
	else
	{
		delete mlfsb;
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------
// MusMLFInput
//----------------------------------------------------------------------------


// in 2.0.0, redesigned as ParseLine
/*
bool MusMLFInput::IsElement( bool *note, wxString *line, int *pos )
{
	if (!note || !line || !pos)
		return false;

	if (line->IsEmpty() || (line->GetChar(0) == '"') || (line->GetChar(0) =='.'))
		return false;

	wxString str;
	wxStringTokenizer tkz( *line , " ");
	if ( !tkz.HasMoreTokens() )
		return false;

	str = tkz.GetNextToken();
	if ( !tkz.HasMoreTokens() ) // no position
	{
		*line = str;
		(*pos) += 45; // defaut step
	}
	else // position is given
	{
		*pos = atoi( str.c_str() );
		str = tkz.GetNextToken(); // skip end position
		if ( !tkz.HasMoreTokens() )
			return false;
		*line = tkz.GetNextToken();
	}
	if ((line->GetChar(0) == 'N') || (line->GetChar(0) =='R')  || (line->GetChar(0) =='C'))
		*note = true;
	else
		*note = false;

	return true;
}
*/

bool MusMLFInput::ParseLine( wxString line, char *element, wxString *elementLine, int *pos )
{
	if (!element || !elementLine || !pos)
		return false;
    
	if (line.IsEmpty() || (line.GetChar(0) == '"') || (line.GetChar(0) =='.'))
		return false;
    
	wxString str;
	wxStringTokenizer tkz( line , " ");
	if ( !tkz.HasMoreTokens() )
		return false;
    
	str = tkz.GetNextToken();
	if ( !tkz.HasMoreTokens() ) // no position
	{
		*elementLine = str;
		(*pos) += 45; // defaut step
	}
	else // position is given
	{
		*pos = atoi( str.c_str() );
		str = tkz.GetNextToken(); // skip end position
		if ( !tkz.HasMoreTokens() )
			return false;
		*elementLine = tkz.GetNextToken();
	}
    *element = elementLine->GetChar(0);
    
	return true;
}



LayerElement *MusMLFInput::ConvertNote( wxString line )
{
	wxStringTokenizer tkz( line , "_");
	if ( !tkz.HasMoreTokens() )
		return NULL;

	wxString str, str1, str2;
	int code, val, flag = 0;
	
	str = tkz.GetNextToken();

	if ( str == "C" )
	{
		if ( !tkz.HasMoreTokens() )
			return NULL;
		str1 = tkz.GetNextToken();
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		str2 = tkz.GetNextToken();
		
        Symbol *custos = new Symbol( SYMBOL_CUSTOS );
        GetPitchWWG( str1.GetChar(0), &code );
        custos->m_pname = code;
        custos->m_oct = atoi( str2.c_str() );
        return custos;
	}
    else
	{
		if ( !tkz.HasMoreTokens() )
			return NULL;
		val = atoi (tkz.GetNextToken().c_str() );

		if ( !tkz.HasMoreTokens() )
			return NULL;
		str1 = tkz.GetNextToken();

		if ( ! tkz.HasMoreTokens() )
			return NULL;
		str2 = tkz.GetNextToken();

		if ( tkz.HasMoreTokens() )
			flag = atoi (tkz.GetNextToken().c_str() );
	}

    if ( str == 'N') {
        Note *note = new Note();
        note->m_dur = val;
        GetPitchWWG( str1.GetChar(0), &code );
        note->m_pname = code;
        note->m_oct = atoi( str2.c_str() );
        if ( flag != 0 )
        {
            // hampe
            if ( (flag & NOTE_STEM) && ((note->m_dur ==  DUR_LG) || (note->m_dur > DUR_1 )) )
                note->m_stemDir = 1; // ?? // ax2
            
            // ligature
            if ( (flag & NOTE_LIGATURE) && (note->m_dur >  DUR_LG) && (note->m_dur < DUR_2 ) )
                note->m_lig = LIG_INITIAL;
			
            // coloration
            if ( (flag & NOTE_COLORATION) && (note->m_dur < DUR_2 ) )
                note->m_colored = true;
            else if ( (flag & NOTE_COLORATION) && (note->m_dur > DUR_4 ) )
                note->m_ligObliqua = true;
        }
        return note;
    }
    else { // rests
        Rest *rest = new Rest();
        rest->m_dur = val;
        GetPitchWWG( str1.GetChar(0), &code );
        rest->m_pname = code;
        rest->m_oct = atoi( str2.c_str() );
        return rest;
    }
}


LayerElement *MusMLFInput::ConvertSymbol( wxString line )
{
	wxStringTokenizer tkz( line , "_");
	if ( !tkz.HasMoreTokens() )
		return NULL;

	int code;

	wxString str = tkz.GetNextToken();
	if ( str == "S" )
	{
		if ( !tkz.HasMoreTokens() )
			return NULL;
		str = tkz.GetNextToken();
		if ( str == "B" )
		{
			Barline *barline = new Barline();
			return barline;
		}
	}
	else if ( str == "K" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		str = tkz.GetNextToken(); 
		Clef *clef = new Clef();
		if ( str == "S2" )
			clef->m_clefId = SOL2;
		else if ( str == "S1" )
			clef->m_clefId = SOL1;
		else if ( str == "S8" )
			clef->m_clefId = SOLva;
		else if ( str == "F4" )
			clef->m_clefId = FA4;
		else if ( str == "F3" )
			clef->m_clefId = FA3;
		else if ( str == "U1" )
			clef->m_clefId = UT1;
		else if ( str == "U2" )
			clef->m_clefId = UT2;
		else if ( str == "U3" )
			clef->m_clefId = UT3;
		else if ( str == "U5" )
			clef->m_clefId = UT5;
		else if ( str == "U4" )
			clef->m_clefId = UT4;
		else
			wxLogWarning( _("Unkown key '%s'"), str.c_str() );

		return clef;
	}
	else if ( str == "A" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		str = tkz.GetNextToken();
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str1 = tkz.GetNextToken();
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str2 = tkz.GetNextToken(); 
		Symbol *symbol = new Symbol( SYMBOL_ACCID );
		GetPitchWWG( str1.GetChar(0), &code );
		symbol->m_pname = code;
		symbol->m_oct = atoi( str2.c_str() );
		if ( str == "D0" )
			symbol->m_accid = ACCID_SHARP;
		else if ( str == "B0" )
			symbol->m_accid = ACCID_FLAT;
		else if ( str == "H0" )
			symbol->m_accid = ACCID_NATURAL;
		else if ( str == "D1" )
			symbol->m_accid = ACCID_DOUBLE_SHARP;
		else if ( str == "B1" )
			symbol->m_accid = ACCID_DOUBLE_FLAT;
		else
			wxLogWarning( _("Unkown alteration") );

		return symbol;
	}
	else if ( str == "P" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str1 = tkz.GetNextToken();
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str2 = tkz.GetNextToken();
		Symbol *symbol = new Symbol( SYMBOL_DOT );
		GetPitchWWG( str1.GetChar(0), &code );
		symbol->m_pname = code;
		symbol->m_oct = atoi( str2.c_str() );

		return symbol;
	}
	else if ( str == "M" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;

		Mensur *mensur = new Mensur();

		wxString str = tkz.GetNextToken();
		wxString option1;
		wxString option2;
		if ( tkz.HasMoreTokens() )
			option1 = tkz.GetNextToken();
		if ( tkz.HasMoreTokens() )
			option2 = tkz.GetNextToken();

		if ( str == "S" )
		{
            if ( option1 == "2" ) {
                // use 2 / 0 for 2 only
                mensur->m_num = 2;
                mensur->m_numBase = 0;
            }
			else if ( option1 == "3" ) {
                mensur->m_num = 3;
                mensur->m_numBase = 0;
            }
            else {
                wxLogWarning( _("Meter symbols not supported") );
                // We should use only figures (CH) and mensur signs - see below
                //symbol->code = 64; ?? // ax2
                if ( option1 == "C" ) 
                    mensur->m_meterSymb = METER_SYMB_COMMON;
                else if ( option1 == "CB" )
                    mensur->m_meterSymb = METER_SYMB_CUT;
                else if ( option1 == "2B" )
                    mensur->m_meterSymb = METER_SYMB_2_CUT;
                else if ( option1 == "3B" )
                    mensur->m_meterSymb = METER_SYMB_3_CUT;
                else
                    wxLogWarning( _("Unkown mesure signe indication") );
            }
		}
		else if ( str == "CH" )
		{
			//symbol->code = 1; ?? // ax2
			mensur->m_num = atoi( option1.c_str() );
			mensur->m_numBase = atoi( option2.c_str() );
		}
		else 
		{
			if ( str == "TP" )
                mensur->m_sign = MENSUR_SIGN_O;
			else if ( str == "TI" )
				mensur->m_sign = MENSUR_SIGN_C;
            else if ( str == "TID" ) {
				mensur->m_sign = MENSUR_SIGN_C;
                mensur->m_reversed = true;
            }
			else {
				wxLogWarning( _("Unkown mesure indication '%s'") , str.c_str() );
            }
			if ( option1 == "B" )
				mensur->m_slash = true;
			if (( option1 == "P" ) || ( option2 == "P" ))
				mensur->m_dot = true;
		}

		return mensur;
	}
	else if ( str != "SP" )
		wxLogWarning( _("Unknown symbol '%s'"), str.c_str() );

	return NULL;
}

MusMLFInput::MusMLFInput( Doc *file, wxString filename ) :
    wxFileInputStream( filename )
{
    m_doc = file;
	m_staff_i = m_staff_label = -1;
}

MusMLFInput::~MusMLFInput()
{
}


// copie le portee en convertissant les symbols de Ut vers Clef courrant
// si inPlace, directment dans staff

void MusMLFInput::GetNotUt1( Layer *layer )
{
	if ( !layer )
		return;

	int code, oct;
	
	for (int i = 0; i < (int)layer->GetElementCount(); i++ )
	{
		if ( ((LayerElement*)layer->m_children[i])->HasPitchInterface() )
		{
			LayerElement *element = (LayerElement*)layer->m_children[i];
			{
				GetNotUt1( layer, element, &code, &oct );
                PitchInterface *pitchElement = dynamic_cast<PitchInterface*>(element);
				pitchElement->m_pname = code;
				pitchElement->m_oct = oct;
			}
		}
		else if ( ((LayerElement*)layer->m_children[i])->HasPositionInterface() )
		{
			LayerElement *element = (LayerElement*)layer->m_children[i];
			{
				GetNotUt1( layer, element, &code, &oct );
                PositionInterface *positionElement = dynamic_cast<PositionInterface*>(element);
				positionElement->m_pname = code;
				positionElement->m_oct = oct;
			}
		}
	}
}


void MusMLFInput::GetNotUt1( Layer *layer, LayerElement *pelement, int *code, int *oct)
{
	if (!layer || !pelement || !code || !oct) return;
    
	int offs;
    
    offs = layer->GetClefOffset( pelement );

	if (dynamic_cast<PitchInterface*>(pelement)) {
        PitchInterface *pitchInterface = dynamic_cast<PitchInterface*>(pelement);
		*code = pitchInterface->m_pname;
        *oct = pitchInterface->m_oct;
    }
	else if (dynamic_cast<PositionInterface*>(pelement)) {
        PositionInterface *positionInterface = dynamic_cast<PositionInterface*>(pelement);
		*code = positionInterface->m_pname;
        *oct = positionInterface->m_oct;
    }
    else {
        return;
    }

	while ( (*code) - offs < 1 )
	{
		(*code) += 7;
		(*oct)--;
	}
	while ( (*code) - offs > 7 )
	{
		(*code) -= 7;
		(*oct)++;
	}
	(*code) -= offs;
}

void MusMLFInput::GetPitchWWG( char code , int *code1 )
{
	if ( !code1 ) return;

	switch (code)
	{
		case 'C': *code1 = 1; break;
		case 'D': *code1 = 2; break;
		case 'E': *code1 = 3; break;
		case 'F': *code1 = 4; break;
		case 'G': *code1 = 5; break;
		case 'A': *code1 = 6; break;
		case 'B': *code1 = 7; break;
	}
}
	
bool MusMLFInput::ReadLine( wxString *line )
{
	if ( !line )
		return false;

	*line = "";
	
	char c = GetC();
	
	if ( !LastRead() )
		return false;

	while ( LastRead() && (c != '\n' ))
	{
		(*line) += c;
		c = GetC();
	}
	return true;
}


bool MusMLFInput::ReadLabelStr( wxString label )
{
	if ( label[0] != '"')
		return false;

	if ( label.Freq( '.' ) < 2 )
		return false;

	if ( label.Freq( '_' ) < 1 )
		return false;

	wxString str = label.BeforeLast('.'); // remove .lab"
	//m_segment_label = atoi( str.AfterLast('.').c_str() );
	str = str.BeforeLast('.'); // remove .seg"
	m_staff_label = atoi ( str.AfterLast('_').c_str() );

	return true;
}

// permet d'importer un fichier par page
// dans ce cas la premiere ligne == #!MLF!#
// Si imPage, ajustera les position en fonction des position x dans imPage (staff)

bool MusMLFInput::ReadPage( Page *page, bool firstLineMLF, ImPage *imPage )
{    
	wxString line;
	if ( firstLineMLF  && ( !ReadLine( &line )  || ( line != "#!MLF!#" )))
		return false;

	Layer *layer = NULL;
	int offset;

	while ( ReadLine( &line ) && ReadLabelStr( line ) )
	{
		if ( m_staff_i < m_staff_label ) // new staff
		{
			if ( layer )
				GetNotUt1( layer ); // convert pitches

            // we have a list of staves in the MLF
            // each staff is a system (so far, needs to be redigned)
            if ( m_staff_label < (int)page->GetSystemCount() )
			{
                // first layer of the first staff of the system
				layer = (Layer*)page->m_children[ m_staff_label ]->m_children[0]->m_children[0];
				m_staff_i = m_staff_label; //m_staff_i++;
			}
			offset = 0;
		}

		if ( layer )
		{
			if ( imPage )
				offset = imPage->m_staves[m_staff_i].m_x1;
			ReadLabel( layer, offset );
		}
	}

	if ( layer )
		GetNotUt1( layer ); // convert pitches

    return true;
}

// offset est la position x relative du label
// normalement donne par imPage si present

bool MusMLFInput::ReadLabel( Layer *layer, int offset )
{
	char elementType;
	int pos = 0;
	wxString line;
    wxString element_line;

	while ( ReadLine( &line ) &&  MusMLFInput::ParseLine( line, &elementType, &element_line, &pos ) )
	{
        LayerElement *layer_element = NULL;
        // we could convert all of the in one method, left over from version < 2.0.0
        if ((elementType == 'N') || (elementType =='R')  || (elementType =='C')) {
			layer_element = MusMLFInput::ConvertNote( element_line );
		}
        else {
			layer_element = MusMLFInput::ConvertSymbol( element_line );
		}
        if ( layer_element ) {
            layer_element->m_xAbs = pos + offset;
            layer->AddElement( layer_element );
        }
	} 
    return true;
}



