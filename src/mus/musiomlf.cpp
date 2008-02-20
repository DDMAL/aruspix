/////////////////////////////////////////////////////////////////////////////
// Name:        musiomlf.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////



#ifdef __GNUG__
    #pragma implementation "musiomlf.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"
#include "wx/textfile.h"
#include "wx/tokenzr.h"
#include "wx/txtstrm.h"

#include "musiomlf.h"

#include "im/impage.h"
#include "im/imstaff.h"
#include "im/imstaffsegment.h"

#include "app/axprocess.h"
#include "app/axprogressdlg.h"
#include "app/axapp.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMLFSymboles );

WX_DEFINE_OBJARRAY( ArrayOfRecSymbols );

int SortRecSymbols( RecSymbol **first, RecSymbol **second )
{
    if ( (*first)->m_word < (*second)->m_word )
        return -1;
    else if ( (*first)->m_word > (*second)->m_word )
        return 1;
    else
        return 0;
}

// TINYXML
#if defined (__WXMSW__)
    #include "tinyxml.h"
#else
    #include "tinyxml/tinyxml.h"
#endif

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
		wxLogWarning( _("Uninitialized symbole") );
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
		wxLogWarning( _("Uninitialized symbole") );
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
		wxLogWarning( _("Uninitialized symbole") );
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
		wxLogWarning( _("Uninitialized symbole") );
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
	else if ((m_type == TYPE_MESURE))
	{
		label_width << "_" << m_subtype;
	}
	return label_width;
}


//----------------------------------------------------------------------------
// MusFileOutputStream
//----------------------------------------------------------------------------

MusMLFOutput::MusMLFOutput( MusFile *file, wxString filename, wxString model_symbole_name ) :
    MusFileOutputStream( file, filename )
{
    m_filename = filename;
    wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	m_addPageNo = false;
	m_addHeader = true;
	m_writePosition = false;
	m_staff = NULL;
	m_model_symbole_name = model_symbole_name;
	m_subfile = NULL;
}

//MusMLFOutput::MusMLFOutput( MusFile *file, wxFile *wxfile, wxString filename, wxString model_symbole_name ) :
//    MusFileOutputStream( file, wxfile )
MusMLFOutput::MusMLFOutput( MusFile *file, int fd, wxString filename, wxString model_symbole_name ) :
	MusFileOutputStream( file, fd )
{
    m_filename = filename;
    wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	m_addPageNo = false;
	m_addHeader = false;
	m_writePosition = false;
	m_staff = NULL;
	m_model_symbole_name = model_symbole_name;
	m_subfile = NULL;
}

MusMLFOutput::~MusMLFOutput()
{
	if ( m_subfile )
		delete m_subfile;
}

// specific

MusStaff *MusMLFOutput::SplitSymboles( MusStaff *staff )
{
	unsigned int k;

	MusStaff *nstaff = new MusStaff();
	staff->CopyAttributes( nstaff );
	MusNote *nnote = NULL;
	MusSymbol *nsymbole1 = NULL;
	MusSymbol *nsymbole2 = NULL;

    for (k = 0;k < staff->nblement ; k++ )
    {
		if ( staff->m_elements[k].TYPE == NOTE )
		{
			nnote = new MusNote( *(MusNote*)&staff->m_elements[k] );
			// alteration
			if (nnote->acc != 0)
			{
				nsymbole2 = new MusSymbol();
				nsymbole2->flag = ALTER;
				nsymbole2->calte = nnote->acc;
				nsymbole2->xrel = nnote->xrel - 10*4; // 10 = pas par defaut
				nsymbole2->dec_y = nnote->dec_y;
				nsymbole2->oct = nnote->oct;
				nsymbole2->code = nnote->code;
				nstaff->m_elements.Add( nsymbole2 );
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
					nsymbole2 = new MusSymbol();
					nsymbole2->flag = PNT;
					nsymbole2->point = 0;
					nsymbole2->xrel = nnote->xrel + 30;
					nsymbole2->dec_y = nnote->dec_y;
					nsymbole2->oct = nnote->oct;
					nsymbole2->code = nnote->code;
					nstaff->m_elements.Add( nsymbole2 );
				}
				nnote->point = 0;
			}				
		}
		else
		{
			nsymbole1 = new MusSymbol( *(MusSymbol*)&staff->m_elements[k] );
			nstaff->m_elements.Add( nsymbole1 );
			// barre TODO repetion points
			if (nsymbole1->flag == BARRE)
			{			
				if ( nsymbole1->code == 'D' ) // double barre
				{
					nsymbole2 = new MusSymbol( *nsymbole1 );
					nsymbole2->code = '|';
					nsymbole2->xrel += 10;
					nstaff->m_elements.Add( nsymbole2 );
					nsymbole1->code = '|';
				}
			}
			// indication de mesure - splitter indication avec chiffre ET symbole
			else if (nsymbole1->flag == IND_MES)
			{
				if ((nsymbole1->code & 1) && (nsymbole1->code != 1)) // pas chiffre seuls
				{
					nsymbole2 = new MusSymbol( *nsymbole1 );
					nsymbole2->code = 1;
					nsymbole2->xrel += 10*5; // 10 = pas par defaut
					nstaff->m_elements.Add( nsymbole2 );
					nsymbole1->code -= 1;
				}
			}
		}
    }
	nstaff->CheckIntegrity( );
	return nstaff;
}

void MusMLFOutput::LoadSymbolDictionary( wxString filename )
{
	wxTextFile file( filename );
	file.Open();
	if ( !file.IsOpened() )
		return;	
		
		
	m_dictSymbols.Clear();
	m_dict.Clear();
		
	wxString str = file.GetFirstLine();
	wxString word;
	int fuse = 0;
	while( 1 && (fuse < 1000000) ) // don't know why, but I hate while(1) alone....
	{
		fuse++;
		if ( !str.IsEmpty() )
		{
			RecSymbol *s = new RecSymbol();
			//wxLogDebug( str );
			wxStringTokenizer tkz( str , ";");
			if ( tkz.HasMoreTokens() )
				s->m_word = tkz.GetNextToken();
			if ( tkz.HasMoreTokens() )
				s->m_hmm_symbol = tkz.GetNextToken();
			if ( tkz.HasMoreTokens() )
				s->m_states = atoi(tkz.GetNextToken().c_str());
			this->m_dict.Add( s->m_word );
			this->m_dictSymbols.Add( s );
		}
		if ( file.Eof() )
			break;
		str = file.GetNextLine();
	}
}

void MusMLFOutput::WriteSymbolDictionary( wxString filename )
{
	int i;
	wxFileOutputStream stream( filename );
	wxTextOutputStream fdic( stream );
	for( i = 0; i < (int)m_dictSymbols.GetCount(); i++ )
	{
		fdic.WriteString( wxString::Format( "%s;%s;%d\n", 
			m_dictSymbols[i].m_word.c_str(), m_dictSymbols[i].m_hmm_symbol.c_str(), m_dictSymbols[i].m_states ) );
	}
	stream.Close();
}

void MusMLFOutput::CreateSubFile()
{
	wxASSERT( m_subfile == NULL );
	m_subfile = new wxFileOutputStream( m_filename + "_align" );
	wxASSERT( m_subfile->Ok() );
}

void MusMLFOutput::LoadSubFile()
{
	wxASSERT( m_subfile == NULL );
	wxFile subfile( m_filename + "_align", wxFile::write_append );
	m_subfile = new wxFileOutputStream( subfile.fd() );
	subfile.Detach();
	wxASSERT( m_subfile->Ok() );
}

void MusMLFOutput::StartLabel( )
{
	wxString label;
	if ( !m_addPageNo )
		label = wxString::Format("\"*/%s_%d.%d.lab\"\n", m_shortname.c_str(), m_staff_i, m_segment_i);
	else
		label = wxString::Format("\"*/%s_%03d.%d.%d.lab\"\n", m_shortname.c_str(), m_page_i + 1, m_staff_i, m_segment_i);
	Write( label, label.Length() );
	
	if ( m_subfile )
		m_subfile->Write( label.BeforeLast('.') + ".ali\"\n", label.Length() );
	
	m_symboles.Clear();

}

void MusMLFOutput::EndLabel( int offsets[], int end_points[] )
{
	//if ( m_writePosition )
	//	wxASSERT_MSG( ( m_types.GetCount() > 0 ), "Not types loaded but write positions wanted" );  

	for (int i = 0; i < (int)m_symboles.GetCount(); i++ )
	{
		wxString symbole;
		wxString symbole_label = m_symboles[i].GetLabel();
		if ( symbole_label == "" ) // skip empty labels
			continue;
		int pos;
		if ( m_writePosition )
		{
			pos = m_symboles[i].GetPosition();
			if ( offsets )
				pos -= offsets[m_segment_i];
				
			int next = pos * 1;
			//if ( m_types.GetCount() > 0 ) // types loaded
			//	next = pos + m_types.GetWidth( &m_symboles[i] ) * 1;
				
				
			symbole << ( pos * 1 ) << " " << ( next ) << " "; // !!!!!!  VERIFIER !!!!!
		}
		// si verifier dans le dictionnaire, tenir jour la liste dans 
		if ( m_dict.Index( symbole_label ) == wxNOT_FOUND )
		{
			m_dict.Add( symbole_label );
			RecSymbol *s = new RecSymbol();
			s->m_word = symbole_label;
			s->m_hmm_symbol = symbole_label;
			s->m_hmm_symbol.MakeLower();
			s->m_states = m_symboles[i].GetNbOfStates();
			this->m_dictSymbols.Add( s );
		}
		symbole << symbole_label << "\n";
		Write( symbole, symbole.Length() );
		// sous symbole avec position
		if ( m_subfile )
		{
			wxASSERT( offsets );
			wxASSERT( end_points );
			
			int fact = 100;
		
			wxString subsymbole;
			wxString subsymbole_label = symbole_label;
			subsymbole_label.MakeLower();
			pos = m_symboles[i].GetPosition() - offsets[m_segment_i];
			if ( pos < 0 )
				pos = 0;
			
			int next = end_points[m_segment_i];
			int next_symbol = next;
			// position du symbole suivant, mais seulement si pas hors portee
			if ( i < (int)m_symboles.GetCount() - 1 )
				next_symbol = m_symboles[i+1].GetPosition() - offsets[m_segment_i];
			if ( next_symbol < next )
				next = next_symbol;
			
			int width = pos + m_symboles[i].GetWidth();
			// width
			if ( width > next )
			{
				//wxLogDebug( _("Label position %s out of range (symbol truncated)"), subsymbole_label.c_str() );
				width = next;
			}
			if ( pos < width ) // else skip symbol
				subsymbole << ( pos * fact) << " " << ( width * fact) << " " << subsymbole_label<< "\n";
			else
				wxLogDebug( _("Label position %s out of range (symbol skipped)"), subsymbole_label.c_str() );
	
			if ( width + WIN_WIDTH <  next ) // add sp
				subsymbole << ( width * fact) << " " << ( next * fact) << " {s}\n";
			 
			m_subfile->Write( subsymbole, subsymbole.Length() );		
		}
	}
	m_symboles.Clear();
	m_segment_i++;

	wxString label = ".\n";
	Write( label, label.Length() );
	
	if ( m_subfile )
		m_subfile->Write( label, label.Length() );	
}

// copie le portee en convertissant les symboles de la clef courante vers Ut1
// si inPlace, directment dans staff

MusStaff *MusMLFOutput::GetUt1( MusStaff *staff , bool inPlace )
{
	if ( !staff )
		return NULL;

	int code, oct;

	MusStaff *nstaff = staff;
	if ( !inPlace )
		nstaff = new MusStaff( *staff );
	
	for (int i = 0; i < (int)nstaff->m_elements.GetCount(); i++ )
	{
		if ( nstaff->m_elements[i].TYPE == NOTE )
		{
			MusNote *note = (MusNote*)&nstaff->m_elements[i];
			{
				GetUt1( nstaff, note, &code, &oct );
				note->code = code;
				note->oct = oct;
			}
		}
		else
		{
			MusSymbol *symbole = (MusSymbol*)&nstaff->m_elements[i];
			if ((symbole->flag == ALTER) || (symbole->flag == PNT))
			{
				GetUt1( nstaff, symbole, &code, &oct );
				symbole->code = code;
				symbole->oct = oct;
			}
		}
	}

	if ( inPlace )
		return NULL;
	else
		return nstaff;
}


/* Calcule l'offset de la cle; doit etre separe de cle_id pour pouvoir
  etre appele depuis rd_symbole() en cas de changement de definition.
	La valeur attribuee est un facteur de multiplication d'espaces
  (espace[pTaille]) decrivant l'eloignement de la clef par rapport a
  la ligne du bas.
*/

void MusMLFOutput::GetUt1( MusStaff *staff, MusElement *pelement, int *code, int *oct)
{
	if (!pelement || !code || !oct) return;

	char valeur = 0;
	int offs;

	if (!staff || (staff->getOctCl(pelement,&valeur, 1 )==0))
		valeur = 5;
	MusSymbol::calcoffs(&offs, valeur);

	*oct = pelement->oct;
	*code = 0;
	if (pelement->TYPE == SYMB)
		*code = ((MusSymbol*)pelement)->code + offs;
	else
		*code = ((MusNote*)pelement)->code + offs;

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

bool MusMLFOutput::ExportFile( MusFile *file, wxString filename )
{
	wxASSERT_MSG( file, "File cannot be NULL" );

    m_filename = filename;
    wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	this->m_file = file;
	return ExportFile();
}

bool MusMLFOutput::ExportFile( )
{
	wxASSERT_MSG( m_file, "File cannot be NULL" );

	if ( m_addHeader )
	{
		Write("#!MLF!#\n",8);
		m_addHeader = false;
	}

    MusPage *page = NULL;

    for (m_page_i = 0; m_page_i < m_file->m_fheader.nbpage; m_page_i++ )
    {
        page = &m_file->m_pages.Item(m_page_i);
        WritePage( page );
    }

    //wxMessageBox("stop");
    //wxLogMessage("OK %d", m_file->m_pages.GetCount() );
	
	m_dictSymbols.Sort( SortRecSymbols );

    return true;
}


bool MusMLFOutput::WritePage( const MusPage *page, bool write_header )
{
	if ( write_header && m_addHeader )
	{
		Write("#!MLF!#\n",8);
		m_addHeader = false;
	}

    m_staff = NULL;
    for (m_staff_i = 0; m_staff_i < page->nbrePortees; m_staff_i++) 
    {
		m_segment_i = 0;
        MusStaff *staff = &page->m_staves[m_staff_i];
		m_staff = MusMLFOutput::SplitSymboles( staff );
        WriteStaff( m_staff );
		delete m_staff;
		m_staff = NULL;
    }


    return true;

}


// idem ExportFile() puis WritePage(), mais gere les segment de portee au moyen de imPage et les portee selon staff numbers
bool MusMLFOutput::WritePage( const MusPage *page, wxString filename, ImPage *imPage, wxArrayInt *staff_numbers )
{
	wxASSERT_MSG( page, "MusPage cannot be NULL" );
	wxASSERT_MSG( imPage, "ImPage cannot be NULL" );
	
    m_filename = filename;
	m_shortname = filename;
    //wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	
	if ( m_addHeader ) // ????????????
	{
		Write("#!MLF!#\n",8);
		m_addHeader = false;
	}
	
	int offsets[256]; // maximum 256 segment per staff, seems ok... but not secure
	int split_points[256]; // maximum 256 segment per staff, seems ok... but not secure
	int end_points[256];

    m_staff = NULL;
    for (m_staff_i = 0; m_staff_i < page->nbrePortees; m_staff_i++) 
    {
		if ( staff_numbers && ( staff_numbers->Index( m_staff_i ) == wxNOT_FOUND ) )
			continue;
	
		m_segment_i = 0;
        MusStaff *staff = &page->m_staves[m_staff_i];
		imPage->GetStaffSegmentOffsets( m_staff_i, offsets, split_points, end_points);
		m_staff = MusMLFOutput::SplitSymboles( staff );
        WriteStaff( m_staff, offsets, split_points, end_points );
		delete m_staff;
		m_staff = NULL;
    }
	m_dictSymbols.Sort( SortRecSymbols );

    return true;

}


bool MusMLFOutput::WriteStaff( const MusStaff *staff, int offsets[], int split_points[], int end_points[] )
{
	if (staff->nblement == 0)
		return true;

    unsigned int k;

	StartLabel();

    for (k = 0;k < staff->nblement ; k++ )
    {
		if ( offsets && (split_points[m_segment_i] != -1) ) // gestion des segments, et pas dernier segment
		{
			if ( staff->m_elements[k].xrel > (unsigned int)split_points[m_segment_i] ) // next segment
			{
					EndLabel( offsets, end_points );
					StartLabel();
			}
		}
        if ( staff->m_elements[k].TYPE == NOTE )
        {
            WriteNote( (MusNote*)&staff->m_elements[k] );
        }
        else
        {
            WriteSymbole( (MusSymbol*)&staff->m_elements[k] );
        }
    }
	EndLabel( offsets, end_points );

    return true;
}


bool MusMLFOutput::WriteNote(  MusNote *note )
{
	int  code, oct;
	GetUt1( m_staff, note, &code, &oct);

	MusMLFSymbol *mlfsb = NULL;

	// custos 
	if (note->val == CUSTOS)
	{
		mlfsb = (MusMLFSymbol*)wxCreateDynamicObject( m_model_symbole_name );
		mlfsb->SetValue( TYPE_CUSTOS, "", note->xrel, 0, _note[code],oct);
		m_symboles.Add( mlfsb );

	}
	// note ou silence
	else 
	{
		mlfsb = (MusMLFSymbol*)wxCreateDynamicObject( m_model_symbole_name );
		int flag = 0;
		// hampe
		if ((note->q_auto == false) && (note->sil != _SIL) && ((note->val ==  LG) || (note->val > RD )))
			flag += NOTE_STEM;
		
		// ligature
		if ((note->ligat == true) && (note->sil != _SIL) && (note->val >  LG) && (note->val < BL ))
			flag += NOTE_LIGATURE;
			
		// coloration
		if ((note->inv_val == true) && (note->sil != _SIL) && (note->val < BL ))
			flag += NOTE_COLORATION;
		else if ((note->oblique == true) && (note->sil != _SIL) && (note->val > NR ))
			flag += NOTE_COLORATION;

		//silence
		if (note->sil == _SIL)
		{
			if ( oct % 2 )
				mlfsb->SetValue( TYPE_REST, "", note->xrel, note->val, _sil0[code],oct + _oct0[code]);	
			else
				mlfsb->SetValue( TYPE_REST, "", note->xrel, note->val, _sil1[code],oct + _oct1[code]);
		}
		else
			mlfsb->SetValue( TYPE_NOTE, "", note->xrel, note->val, _note[code],oct, flag);
		m_symboles.Add( mlfsb );
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

bool MusMLFOutput::WriteSymbole(  MusSymbol *symbole )
{
	// gestion des segment de portees (pas actif ?????)
	if ((symbole->flag == BARRE) && (symbole->code == 'I'))
	{	
		wxASSERT_MSG( false, "Should not happen..." );
		EndLabel();
		StartLabel();
		return true;
	}

	MusMLFSymbol *mlfsb = (MusMLFSymbol*)wxCreateDynamicObject( m_model_symbole_name );

	if (symbole->flag == BARRE)
	{
		mlfsb->SetValue( TYPE_SYMBOLE, "B", symbole->xrel );
		m_symboles.Add( mlfsb );
	}
	else if (symbole->flag == CLE)
	{
		switch(symbole->code)
		{	case SOL2 : mlfsb->SetValue( TYPE_KEY, "S",  symbole->xrel, 2 ); break;
			case SOL1 : mlfsb->SetValue( TYPE_KEY, "S",  symbole->xrel, 1 ); break;
			case SOLva : mlfsb->SetValue( TYPE_KEY, "S",  symbole->xrel, 8 ); break;
			case FA5 : mlfsb->SetValue( TYPE_KEY, "F",  symbole->xrel, 5 ); break;
			case FA4 : mlfsb->SetValue( TYPE_KEY, "F",  symbole->xrel, 4 ); break;
			case FA3 : mlfsb->SetValue( TYPE_KEY, "F",  symbole->xrel, 3 ); break;
			case UT1 : mlfsb->SetValue( TYPE_KEY, "U",  symbole->xrel, 1 ); break;
			case UT2 : mlfsb->SetValue( TYPE_KEY, "U",  symbole->xrel, 2 ); break;
			case UT3 : mlfsb->SetValue( TYPE_KEY, "U",  symbole->xrel, 3 ); break;
			case UT5 : mlfsb->SetValue( TYPE_KEY, "U",  symbole->xrel, 5); break;
			case UT4 : mlfsb->SetValue( TYPE_KEY, "U",  symbole->xrel, 4 ); break;
			default: break;
		}
		m_symboles.Add( mlfsb );
	}
	else if (symbole->flag == ALTER)
	{
		int  code = 0, oct = 0;
		GetUt1( m_staff, symbole, &code, &oct);
		if (symbole->calte == DIESE)
			mlfsb->SetValue( TYPE_ALTERATION, "D",  symbole->xrel, 0, _note[code], oct );
		else if (symbole->calte == BEMOL)
			mlfsb->SetValue( TYPE_ALTERATION, "B",  symbole->xrel, 0, _note[code], oct );
		else if (symbole->calte == BECAR)
			mlfsb->SetValue( TYPE_ALTERATION, "H",  symbole->xrel, 0, _note[code], oct );
		else if (symbole->calte == D_DIESE)
			mlfsb->SetValue( TYPE_ALTERATION, "D",  symbole->xrel, 1, _note[code], oct );
		else if (symbole->calte == D_BEMOL)
			mlfsb->SetValue( TYPE_ALTERATION, "B",  symbole->xrel, 1, _note[code], oct );
		m_symboles.Add( mlfsb );
	}
	else if (symbole->flag == PNT)
	{
		int  code = 0, oct = 0;
		GetUt1( m_staff, symbole, &code, &oct);
		mlfsb->SetValue( TYPE_POINT, "", symbole->xrel, 0, _note[code + ((code+oct) % 2)], ((code + ((code+oct) % 2)) == 8) ? (oct+1) : oct );
		//str += wxString::Format("P_%s_%d\n",
		//	_note[code + ((code+oct) % 2)],((code + ((code+oct) % 2)) == 8) ? (oct+1) : oct,symbole->code,code,oct);
		m_symboles.Add( mlfsb );
	}
	else if (symbole->flag == IND_MES)
	{
		// signes standard
		if ((int)symbole->code & 64)
		{
			switch (symbole->calte)
			{	
				case 0: mlfsb->SetValue( TYPE_MESURE, "S_C", symbole->xrel ); break;
				case 1: mlfsb->SetValue( TYPE_MESURE, "S_CB", symbole->xrel ); break;
				case 2: mlfsb->SetValue( TYPE_MESURE, "S_2", symbole->xrel ); break;
				case 3: mlfsb->SetValue( TYPE_MESURE, "S_3", symbole->xrel ); break;
				case 4: mlfsb->SetValue( TYPE_MESURE, "S_2B", symbole->xrel ); break;
				case 5: mlfsb->SetValue( TYPE_MESURE, "S_3B", symbole->xrel ); break;
			}
			m_symboles.Add( mlfsb );
		}
		else if (symbole->code != 1)
		{
			wxString subtype;
			// temps parfait
			if ((int)symbole->code & 32)
				subtype += wxString::Format("TP");
			// temps imparfait	
			else if ((int)symbole->code & 16)
				subtype += wxString::Format("TI");
			// temps imparfait double
			else if ((int)symbole->code & 8)
				subtype += wxString::Format("TID");
			// barre
			if ((int)symbole->code & 4)
				subtype += wxString::Format("_B");
			// prolation parfaite
			if ((int)symbole->code & 2)
				subtype += wxString::Format("_P");

			mlfsb->SetValue( TYPE_MESURE, subtype, symbole->xrel );
			m_symboles.Add( mlfsb );
		}
		// chiffres
		else
		{
			wxString subtype = wxString::Format("CH_%d_%d",max (symbole->durNum, 1),max (symbole->durDen, 1));
			mlfsb->SetValue( TYPE_MESURE, subtype, symbole->xrel );
			m_symboles.Add( mlfsb );
		}
	}
	//else if ((symbole->flag == CHAINE ) && (symbole->calte == NUMMES))
	//{}
	else
	{
		delete mlfsb;
		return false;
	}

	return true;
}

void MusMLFOutput::WriteDictionary( wxString filename )
{
	int i;
	wxFileOutputStream stream( filename );
	wxTextOutputStream fdic( stream );
	for( i = 0; i < (int)m_dictSymbols.GetCount(); i++ )
	{
	    fdic.WriteString( m_dictSymbols[i].m_word );
		fdic.WriteString( wxString::Format( " %s {s}\n", m_dictSymbols[i].m_hmm_symbol.c_str() ) );
	}
	fdic.WriteString( wxString::Format( "SP_START {s}\n" ) );
	fdic.WriteString( wxString::Format( "SP_END {s}\n" ) );
	stream.Close();
}
	
void MusMLFOutput::WriteStatesPerSymbol( wxString filename )
{
	int i;
	wxFileOutputStream stream( filename );
	wxTextOutputStream fstates( stream );
	for( i = 0; i < (int)m_dictSymbols.GetCount(); i++ )
	{
		fstates.WriteString( wxString::Format( "%02d\n", m_dictSymbols[i].m_states ) );
	}
	fstates.WriteString( wxString::Format( "%02d\n", 4 ) );
	stream.Close();
	
	// provisoire
	wxFileOutputStream stream_3( filename + "3" );
	wxTextOutputStream fstates3( stream_3 );
	for( i = 0; i < (int)m_dictSymbols.GetCount(); i++ )
	{
		fstates3.WriteString( wxString::Format( "%02d\n", m_dictSymbols[i].m_states + 4 ) );
	}
	fstates3.WriteString( wxString::Format( "%02d\n", 4 ) );
	stream_3.Close();
	
	// v
	wxFileOutputStream stream_htk( filename + "_htk" );
	wxTextOutputStream fdic_htk( stream_htk );
	for( i = 0; i < (int)m_dictSymbols.GetCount(); i++ )
	{
		fdic_htk.WriteString( wxString::Format( "%s %02d\n", m_dictSymbols[i].m_hmm_symbol.c_str(), m_dictSymbols[i].m_states ) );
	}
	fdic_htk.WriteString( wxString::Format( "{s} %02d\n", 4 ) );
	stream_htk.Close();
}

void MusMLFOutput::WriteHMMSymbols( wxString filename )
{
	int i;
	wxFileOutputStream stream( filename );
	wxTextOutputStream fdic( stream );
	for( i = 0; i < (int)m_dictSymbols.GetCount(); i++ )
	{
		fdic.WriteString( wxString::Format( "%s\n", m_dictSymbols[i].m_hmm_symbol.c_str() ) );
	}
	fdic.WriteString( wxString::Format( "{s}\n" ) );
	stream.Close();
}

// WDR: handler implementations for MusMLFOutput


//----------------------------------------------------------------------------
// MusMLFInput
//----------------------------------------------------------------------------


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



MusNote *MusMLFInput::ConvertNote( wxString line )
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
		
		val = CUSTOS;
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

	MusNote *note = new MusNote();
	note->val = val;
	GetPitchWWG( str1.GetChar(0), &code );
	note->code = code;
	note->oct = atoi( str2.c_str() );
	if ( str != "N" ) // silence AND CUSTOS
		note->sil = _SIL;
	if ( flag != 0 )
	{
		// hampe
		if ( (flag & NOTE_STEM) && (note->sil != _SIL) && ((note->val ==  LG) || (note->val > RD )) )
			note->q_auto = false;
		
		// ligature
		if ( (flag & NOTE_LIGATURE)  && (note->sil != _SIL) && (note->val >  LG) && (note->val < BL ) )
			note->ligat = true;
			
		// coloration
		if (  (flag & NOTE_COLORATION) && (note->sil != _SIL) && (note->val < BL ) )
			note->inv_val = true;
		else if ( (flag & NOTE_COLORATION) && (note->sil != _SIL) && (note->val > NR ) )
			note->oblique = true;
	}
	
	return note;
}

MusSymbol *MusMLFInput::ConvertSymbole( wxString line )
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
			MusSymbol *symbole = new MusSymbol();
			symbole->flag = BARRE;
			symbole->code = '|';

			return symbole;
		}
	}
	else if ( str == "K" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		str = tkz.GetNextToken(); 
		MusSymbol *symbole = new MusSymbol();
		symbole->flag = CLE;
		if ( str == "S2" )
			symbole->code = SOL2;
		else if ( str == "S1" )
			symbole->code = SOL1;
		else if ( str == "S8" )
			symbole->code = SOLva;
		else if ( str == "F4" )
			symbole->code = FA4;
		else if ( str == "F3" )
			symbole->code = FA3;
		else if ( str == "U1" )
			symbole->code = UT1;
		else if ( str == "U2" )
			symbole->code = UT2;
		else if ( str == "U3" )
			symbole->code = UT3;
		else if ( str == "U5" )
			symbole->code = UT5;
		else if ( str == "U4" )
			symbole->code = UT4;
		else
			wxLogWarning( _("Unkown key") );

		return symbole;
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
		MusSymbol *symbole = new MusSymbol();
		symbole->flag = ALTER;
		GetPitchWWG( str1.GetChar(0), &code );
		symbole->code = code;
		symbole->oct = atoi( str2.c_str() );
		if ( str == "D0" )
			symbole->calte = DIESE;
		else if ( str == "B0" )
			symbole->calte = BEMOL;
		else if ( str == "H0" )
			symbole->calte = BECAR;
		else if ( str == "D1" )
			symbole->calte = D_DIESE;
		else if ( str == "B1" )
			symbole->calte = D_BEMOL;
		else
			wxLogWarning( _("Unkown alteration") );

		return symbole;
	}
	else if ( str == "P" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str1 = tkz.GetNextToken();
		if ( ! tkz.HasMoreTokens() )
			return NULL;
		wxString str2 = tkz.GetNextToken();
		MusSymbol *symbole = new MusSymbol();
		symbole->flag = PNT;
		GetPitchWWG( str1.GetChar(0), &code );
		symbole->code = code;
		symbole->oct = atoi( str2.c_str() );

		return symbole;
	}
	else if ( str == "M" )
	{
		if ( ! tkz.HasMoreTokens() )
			return NULL;

		MusSymbol *symbole = new MusSymbol();
		symbole->flag = IND_MES;

		wxString str = tkz.GetNextToken();
		wxString option1;
		wxString option2;
		if ( tkz.HasMoreTokens() )
			option1 = tkz.GetNextToken();
		if ( tkz.HasMoreTokens() )
			option2 = tkz.GetNextToken();

		if ( str == "S" )
		{
			symbole->code = 64;
			if ( option1 == "C" )
				symbole->calte = 0;
			else if ( option1 == "CB" )
				symbole->calte = 1;
			else if ( option1 == "2" )
				symbole->calte = 2;
			else if ( option1 == "3" )
				symbole->calte = 3;
			else if ( option1 == "2B" )
				symbole->calte = 4;
			else if ( option1 == "3B" )
				symbole->calte = 5;
			else if ( option1 == "C" )
				symbole->calte = 0;
			else
				wxLogWarning( _("Unkown mesure signe indication") );
		}
		else if ( str == "CH" )
		{
			symbole->code = 1;
			symbole->durNum = atoi( option1.c_str() );
			symbole->durDen = atoi( option2.c_str() );
		}
		else 
		{
			if ( str == "TP" )
				symbole->code = 32;
			else if ( str == "TI" )
				symbole->code = 16;
			else if ( str == "TID" )
				symbole->code = 8;
			else
				wxLogWarning( _("Unkown mesure indication") );
			if ( option1 == "B" )
				symbole->code += 4;
			if (( option1 == "P" ) || ( option2 == "P" ))
				symbole->code += 2;
		}

		return symbole;
	}
	else if ( str != "SP" )
		wxLogWarning( _("Unknown symbole '%s'"), str.c_str() );

	return NULL;
}

MusMLFInput::MusMLFInput( MusFile *file, wxString filename ) :
    MusFileInputStream( file, filename )
{
	m_staff_i = m_staff_label = -1;
	m_segment_i = m_segment_label = 0;
}

MusMLFInput::~MusMLFInput()
{
}


// copie le portee en convertissant les symboles de Ut vers Clef courrant
// si inPlace, directment dans staff

MusStaff *MusMLFInput::GetNotUt1( MusStaff *staff , bool inPlace )
{
	if ( !staff )
		return NULL;

	int code, oct;

	MusStaff *nstaff = staff;
	if ( !inPlace )
		nstaff = new MusStaff( *staff );
	
	for (int i = 0; i < (int)nstaff->m_elements.GetCount(); i++ )
	{
		if ( nstaff->m_elements[i].TYPE == NOTE )
		{
			MusNote *note = (MusNote*)&nstaff->m_elements[i];
			{
				GetNotUt1( nstaff, note, &code, &oct );
				note->code = code;
				note->oct = oct;
			}
		}
		else
		{
			MusSymbol *symbole = (MusSymbol*)&nstaff->m_elements[i];
			if ((symbole->flag == ALTER) || (symbole->flag == PNT))
			{
				GetNotUt1( nstaff, symbole, &code, &oct );
				symbole->code = code;
				symbole->oct = oct;
			}
		}
	}

	if ( inPlace )
		return NULL;
	else
		return nstaff;
}


void MusMLFInput::GetNotUt1( MusStaff *staff, MusElement *pelement, int *code, int *oct)
{
	if (!pelement || !code || !oct) return;

	char valeur = 0;
	int offs;

	if (!staff || (staff->getOctCl(pelement,&valeur, 1 )==0))
		valeur = 5;
	MusSymbol::calcoffs(&offs, valeur);

	*oct = pelement->oct;
	*code = 0;

	if (pelement->TYPE == SYMB)
		*code = ((MusSymbol*)pelement)->code;
	else
		*code = ((MusNote*)pelement)->code;

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
	m_segment_label = atoi( str.AfterLast('.').c_str() );
	str = str.BeforeLast('.'); // remove .seg"
	m_staff_label = atoi ( str.AfterLast('_').c_str() );

	return true;
}


bool MusMLFInput::ImportFile( int staff_per_page )
{
	m_file->m_pages.Clear();

	wxString line;
	if ( !ReadLine( &line )  || (line != "#!MLF!#" ))
		return false;

	MusPage *page = NULL;
	MusStaff *staff = NULL;
	int offset;

	if ( staff_per_page != -1 ) 
	// staff per page is given
	// just fill page one after the othe
	{
		int n_staff = 0;
		while ( ReadLine( &line ) && ReadLabelStr( line ) )
		{
			if ( !page ) // new page
			{
				page = new MusPage();
				n_staff = 0;
				offset = 0;
			}
			staff = new MusStaff();
			ReadLabel( staff, offset );
			MusStaff *staff2 = GetNotUt1( staff );
			delete staff;
			staff = staff2;
			page->m_staves.Add( staff );
			page->nbrePortees++;
			staff = NULL;
			n_staff++;
			if ( n_staff == staff_per_page )
			{
				m_file->m_pages.Add( page );
				m_file->m_fheader.nbpage++;
				page = NULL;
			}
		}

	}
	else
	{
		wxASSERT_MSG( false, "Staff per page should be specified in MFLInput ImportFile" );
	}

	if ( page && staff )
	{
		GetNotUt1( staff, true );
		page->m_staves.Add( staff );
	}
	if ( page )
		m_file->m_pages.Add( page );
	m_file->CheckIntegrity();

    return true;
}


// permet d'importer un fichier par page
// dans ce cas la premiere ligne == #!MLF!#
// Si imPage, ajustera les position en fonction des position x dans imPage (staff et segment)

bool MusMLFInput::ReadPage( MusPage *page , bool firstLineMLF, ImPage *imPage )
{
	wxString line;
	if ( firstLineMLF  && (!ReadLine( &line )  || (line != "#!MLF!#" )))
		return false;

	MusStaff *staff = NULL;
	int offset;

	while ( ReadLine( &line ) && ReadLabelStr( line ) )
	{
		if ( m_staff_i < m_staff_label ) // new staff
		{
			if ( staff )
				GetNotUt1( staff, true ); // convert pitches

			if (m_staff_label < (int)page->m_staves.GetCount())
			{
				staff = &page->m_staves[ m_staff_label ];
				m_staff_i = m_staff_label; //m_staff_i++;
			}
			m_segment_i = 0;
			offset = 0;
			
		}
		else
			m_segment_i++;

		if ( staff )
		{
			if ( imPage )
				//offset = imPage->m_staves[m_staff_i].m_segments[m_segment_i].m_x1 - m_file->m_fheader.param.MargeGAUCHEIMPAIRE * 10;
				offset = imPage->m_staves[m_staff_i].m_segments[m_segment_i].m_x1;
			ReadLabel( staff, offset );
		}
	}

	if ( staff )
		GetNotUt1( staff, true ); // convert pitches

	page->CheckIntegrity();

    return true;

}

// offset est la position x relative du label (p ex segment)
// normalement donne par imPage si present

bool MusMLFInput::ReadLabel( MusStaff *staff, int offset )
{
	bool is_note;
	int pos = 0;
	wxString line;

	while ( ReadLine( &line ) &&  MusMLFInput::IsElement( &is_note, &line, &pos ) )
	{
		if ( !is_note )
		{
			MusSymbol *s = MusMLFInput::ConvertSymbole( line );
			if ( s )
			{
				s->xrel = pos + offset;
				staff->m_elements.Add( s );
			}
		}
		else
		{
			MusNote *n = MusMLFInput::ConvertNote( line );
			if ( n )
			{
				n->xrel = pos + offset;
				staff->m_elements.Add( n );
			}
		}
	}       
    return true;
}

// WDR: handler implementations for MusMLFInput


