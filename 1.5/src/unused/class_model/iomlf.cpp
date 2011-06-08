/////////////////////////////////////////////////////////////////////////////
// Name:        iomlf.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_WG

#ifdef __GNUG__
    #pragma implementation "iomlf.h"
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

#include "iomlf.h"
#include "iomlfclass.h"
#include "im/impage.h"
#include "im/imstaff.h"
#include "im/imstaffsegment.h"

#include "app/axprocess.h"
#include "app/progressdlg.h"

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

IMPLEMENT_DYNAMIC_CLASS(MLFSymbole, wxObject)

//----------------------------------------------------------------------------
// MLFSymbole
//----------------------------------------------------------------------------

MLFSymbole::MLFSymbole( ) :
	wxObject()
{
	m_type = 0;
	m_value = 0;
	m_pitch = 0;
	m_oct = 0;
	m_flag = 0;
	m_position = 0;
}

void MLFSymbole::SetValue( char type, wxString subtype, int position, int value, char pitch, int oct, int flag )
{
	m_type = type;
	m_subtype = subtype;
	m_position = position;
	m_value = value;
	m_pitch = pitch;
	m_oct = oct;
	m_flag = flag;
}


wxString MLFSymbole::GetLabel( )
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


int MLFSymbole::GetNbOfStates( )
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

int MLFSymbole::GetWidth( )
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

wxString MLFSymbole::GetLabelType( )
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
// WgFileOutputStream
//----------------------------------------------------------------------------

MLFOutput::MLFOutput( WgFile *file, wxString filename, wxString model_symbole_name ) :
    WgFileOutputStream( file, filename )
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

MLFOutput::~MLFOutput()
{
}

// specific

WgStaff *MLFOutput::SplitSymboles( WgStaff *staff )
{
	unsigned int k;

	WgStaff *nstaff = new WgStaff();
	staff->CopyAttributes( nstaff );
	WgNote *nnote = NULL;
	WgSymbole *nsymbole1 = NULL;
	WgSymbole *nsymbole2 = NULL;

    for (k = 0;k < staff->nblement ; k++ )
    {
		if ( staff->m_elements[k].TYPE == NOTE )
		{
			nnote = new WgNote( *(WgNote*)&staff->m_elements[k] );
			// alteration
			if (nnote->acc != 0)
			{
				nsymbole2 = new WgSymbole();
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
					nsymbole2 = new WgSymbole();
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
			nsymbole1 = new WgSymbole( *(WgSymbole*)&staff->m_elements[k] );
			nstaff->m_elements.Add( nsymbole1 );
			// barre TODO repetion points
			if (nsymbole1->flag == BARRE)
			{			
				if ( nsymbole1->code == 'D' ) // double barre
				{
					nsymbole2 = new WgSymbole( *nsymbole1 );
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
					nsymbole2 = new WgSymbole( *nsymbole1 );
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

/*
void MLFOutput::PitchMLFtoIntervalMLF( wxString base_filename )
{

	wxString mlf_in = base_filename + ".notes.mlf";
	wxString mlf_out = base_filename + ".mlf";

	//if ( mlf_in == mlf_out )
	//	return;

	// input mlf file
	wxTextFile fmlf_in( mlf_in );
	if ( !fmlf_in.Exists() )
	{
		wxLogError( "File '%s' doesn't exists", mlf_in.c_str() );
		return;
	} 
	fmlf_in.Open();
	if ( !fmlf_in.IsOpened() )
	{
		wxLogError( _("Unable to open file '%s'"), mlf_in.c_str() );
		return;
	}

	// output mlf file
	wxTextFile fmlf_out( mlf_out );
	if ( !fmlf_out.Exists() )
		fmlf_out.Create();
	fmlf_out.Open();
	if ( !fmlf_out.IsOpened() )
	{
		fmlf_in.Close();
		wxLogError( _("Unable to open file '%s'"), mlf_out.c_str() );
		return;
	}
	fmlf_out.Clear();

	// parcourir le fichier
	int previous_pitch = -1;
	int previous_value;
	int value, pitch;
	long note;
	wxString str;
	for ( str = fmlf_in.GetFirstLine(); !fmlf_in.Eof(); str = fmlf_in.GetNextLine() )
	{
		//if ( str.ToLong( &note ) ) // numeric value, it's a note
		//{
		//	if ( previous_note != -1 )
		//		fmlf_out.AddLine( wxString::Format("%d", (int)note - previous_note ) );
		//	previous_note = (int)note;
		//}
		//else // copy line and reset previous note
		//{
		//	fmlf_out.AddLine( str );
		//	previous_note = -1;
		//}
		/if ( str.ToLong( &note ) ) // numeric value, it's a note
		//{
		//	pitch = note & MLF_PITCH; 
		//	value = note & MLF_INTERVAL_V1;
		//	value = (value >> MLF_INTERVAL_SHIFT);
		//	if ( previous_note != -1 )
		//	{
		//		int code = pitch - previous_pitch  + 
		//				( previous_value << MLF_INTERVAL_SHIFT ) + 
		//				( value << MLF_INTERVAL_SHIFT_V );
		//		fmlf_out.AddLine( wxString::Format("%d", code ) );
		//	}
		//	previous_pitch = pitch;
		//	previous_value = value;
		//}
		if ( str.ToLong( &note ) ) // numeric value, it's a note
		{
			pitch = note & MLF_INTERVAL; 
			value = note & MLF_INTERVAL_V1;
			value = (value >> MLF_INTERVAL_SHIFT);
			if ( previous_pitch != -1 )
			{
				int code = pitch - previous_pitch  + 
						( value << MLF_INTERVAL_SHIFT );
				fmlf_out.AddLine( wxString::Format("%d", code ) );
			}
			previous_pitch = pitch;
			previous_value = value;
		}
		else // copy line and reset previous note
		{
			fmlf_out.AddLine( str );
			previous_pitch = -1;
		}
	}
	fmlf_out.AddLine( str ); // copy last line ("." or empty)

	fmlf_in.Close();
	fmlf_out.Write();
	fmlf_out.Close();
}
*/

/*
void MLFOutput::GenerateMapping( wxString base_filename, wxString base_filename2, bool is_interval )
{
	int i, j;

	wxString mapfile;
	wxString vocfile;
	wxString mlf_in = base_filename2 + ".mlf";

	if ( !is_interval )
	{
		mapfile = base_filename + ".map";
		vocfile = base_filename + ".dic";
	}
	else
	{
		mapfile = base_filename + ".notes.map";
		vocfile = base_filename + ".notes.dic";
	}
	

	wxArrayString map_dict; // dictionnary of the mapped values
	wxArrayString map_lines; // mapping between the symboles (ex: N_0 N \n N_1 N ...)
	int pos;
	bool is_note;
	int max_pitch = 0, min_pitch = 60; // six octaves
	int max_value = 0, min_value = 8; // quadruple croche
	wxArrayInt map_lines_dict; // mapping des index entre map_lines et map_dict
								// used to calculate p(w|c)

	for(i = 0; i < (int)m_loadedDict.GetCount(); i++)
	{
		if ( MLFInput::IsElement( &is_note, &m_loadedDict[i], &pos ) )
		{
			bool ok = false;
			m_symboles.Clear();
			if ( !is_note )
			{
				WgSymbole *s = MLFInput::ConvertSymbole( m_loadedDict[i] );
				if ( s )
					ok = this->WriteSymbole( s );
			}
			else
			{
				WgNote *n = MLFInput::ConvertNote( m_loadedDict[i] );
				if ( n )
					ok = this->WriteNote( n );
			}
			wxString map;
			if ( ok )
				map = m_symboles[0].GetLabel();
			if (map.IsEmpty())
			{
				if (m_loadedDict[i] == SP_START)
					map = "SP_START";
				else if (m_loadedDict[i] == SP_END)
					map = SP_END;
				else
					map = SP_WORD;
				map_lines_dict.Add( -1 );

			}
			else if ( map_dict.Index( map ) == wxNOT_FOUND )
			{
				
				//
				//long val;
				//if ( is_interval && map.ToLong( &val ) )
				//{
				//	if ( min_pitch > (int)val )
				//		min_pitch = (int)val;
				//	if ( max_pitch < (int)val )
				//		max_pitch = (int)val;
				//}
				long val;
				if ( is_interval && map.ToLong( &val ) )
				{
					// get pitch
					int pitch = val & MLF_INTERVAL; 
					if ( min_pitch > pitch )
						min_pitch = pitch;
					if ( max_pitch < pitch )
						max_pitch = pitch;
					// get note value
					val = val & MLF_INTERVAL_V1;
					val = (val >> MLF_INTERVAL_SHIFT);
					if ( min_value > val )
						min_value = val;
					if ( max_value < val )
						max_value = val;
				}							
				map_dict.Add( map );
				map_lines_dict.Add( (int)map_dict.GetCount() - 1 );

			}
			else
				map_lines_dict.Add( map_dict.Index( map ) );

			map_lines.Add( wxString::Format("%s %s", m_loadedDict[i].c_str(), map.c_str() ) );
		}
	}
  

	// ******
	// calculate classes statistics P(w|c) from an mlf file
	// principe : on parcours les deux fichier mlf (original et classe) et on compte les stats
	int *map_lines_count = new int[ (int)m_loadedDict.GetCount() ];
	memset( map_lines_count, 0, (int)m_loadedDict.GetCount()  * sizeof(int) );
	int *map_dict_count = new int[ (int)map_dict.GetCount() ];
	memset( map_dict_count, 0, (int)map_dict.GetCount()  * sizeof(int) );
	// input mlf file
	wxTextFile fmlf_in( mlf_in );
	if ( !fmlf_in.Exists() )
	{
		wxLogError( "File '%s' doesn't exists", mlf_in.c_str() );
		delete[] map_dict_count;
		delete[] map_lines_count;
		return;
	} 
	fmlf_in.Open();
	if ( !fmlf_in.IsOpened() )
	{
		wxLogError( _("Unable to open file '%s'"), mlf_in.c_str() );
		delete[] map_dict_count;
		delete[] map_lines_count;
		return;
	}

	// parcourir le fichier
	wxString str;
	for ( str = fmlf_in.GetFirstLine(); !fmlf_in.Eof(); str = fmlf_in.GetNextLine() )
	{
		if ( str.IsSameAs("#!MLF!#") )
			continue;

		i = m_loadedDict.Index( str );
		if (i == wxNOT_FOUND )
			continue;

		map_lines_count[i]=map_lines_count[i]+1;
		int dict_i = map_lines_dict[i];
		if ( dict_i != -1 )
			map_dict_count[dict_i]=map_dict_count[dict_i]+1;
	}
	fmlf_in.Close();


	// ***** output files
	// write mapfile
	wxTextFile fmap( mapfile );
	if ( !fmap.Exists() )
		fmap.Create(); 
	fmap.Open();
	if ( !fmap.IsOpened() )
	{
		wxLogError( _("Unable to open file '%s'"), mapfile.c_str() );
		delete[] map_dict_count;
		delete[] map_lines_count;
		return;
	}
	fmap.Clear();
	for(i = 0; i < (int)map_lines.GetCount(); i++) 
	{
		wxString line;
		int dict_i = map_lines_dict[i];
		if ( dict_i != -1 )
		{
			if ((map_dict_count[dict_i] != 0 ) && (map_lines_count[i] != 0 ))
				line = wxString::Format("%s %f", map_lines[i].c_str(), 
					log((double)map_lines_count[i]/(double)map_dict_count[dict_i]) );
			else if (map_dict_count[dict_i] == 0 )
				line = wxString::Format("%s %f", map_lines[i].c_str(), 
					0.0 );
			else
				line = wxString::Format("%s %f", map_lines[i].c_str(), 
					-99.999999 );
		}
		else
			line = wxString::Format("%s %f", map_lines[i].c_str(), 0.0 );
		fmap.AddLine( line );
	}
	fmap.Write();
	fmap.Close();

	// write voc file
	wxTextFile fvoc( vocfile );
	if ( !fvoc.Exists() )
		fvoc.Create();
	fvoc.Open();
	if ( !fvoc.IsOpened() )
	{
		wxLogError( _("Unable to open file '%s'"), vocfile.c_str() );
		delete[] map_dict_count;
		delete[] map_lines_count;
		return;
	}
	fvoc.Clear();
	// all words
	for(i = 0; i < (int)map_dict.GetCount(); i++) 
		fvoc.AddLine( map_dict[i] );
	// END and START
	fvoc.AddLine(SP_START);
	fvoc.AddLine(SP_END);
	fvoc.Write();
	fvoc.Close();


	delete[] map_lines_count;
	delete[] map_dict_count;

	if ( is_interval ) 
		// convert pitches to interval dictionnary
		// convertion will also be done before ngram generation from mlf and
		// in decoding in ClassModel if class is_interval
	{
		vocfile = base_filename + ".dic"; 
		map_dict.Clear();
		//for(i = min_pitch; i < max_pitch; i++)
		//{
		//	map_dict.Add( wxString::Format("%d", max_pitch - i ) );
		//	map_dict.Add( wxString::Format("%d", - (max_pitch - i) ) );
		//}
		// unisson
		//map_dict.Add( wxString::Format("%d", 0) );
		int val;
		//for(i = min_pitch; i < max_pitch; i++)
		//	for(j = min_value; j <= max_value; j++)
		//		for(k = min_value; k <= max_value; k++)
		//		{
		//			val = 
		//				max_pitch - i + 
		//				( j << MLF_INTERVAL_SHIFT ) + 
		//				( k << MLF_INTERVAL_SHIFT_V );
		//			map_dict.Add( wxString::Format("%d", val ) );
		//			val = 
		//				- (max_pitch - i) + 
		//				( j << MLF_INTERVAL_SHIFT ) + 
		//				( k << MLF_INTERVAL_SHIFT_V );
		//			map_dict.Add( wxString::Format("%d", val ) );
		//		}
		// unisson
		//for(j = min_value; j <= max_value; j++)
		//	for(k = min_value; k <= max_value; k++)
		//	{
		//		val = ( j << MLF_INTERVAL_SHIFT ) + ( k << MLF_INTERVAL_SHIFT_V );
		//		map_dict.Add( wxString::Format("%d", val ) );
		//	}
		for(i = min_pitch; i < max_pitch; i++)
			for(j = min_value; j <= max_value; j++)
			{
				val = 
					max_pitch - i + 
					( j << MLF_INTERVAL_SHIFT );
				map_dict.Add( wxString::Format("%d", val ) );
				val = 
						- (max_pitch - i) + 
						( j << MLF_INTERVAL_SHIFT );
				map_dict.Add( wxString::Format("%d", val ) );
			}
		// unisson
		for(j = min_value; j <= max_value; j++)
		{
			val = ( j << MLF_INTERVAL_SHIFT );
			map_dict.Add( wxString::Format("%d", val ) );
		}

		// write voc file
		wxTextFile fvoc( vocfile );
		if ( !fvoc.Exists() )
			fvoc.Create();
		fvoc.Open();
		if ( !fvoc.IsOpened() )
		{
			wxLogError( _("Unable to open file '%s'"), vocfile.c_str() );
			return;
		}
		fvoc.Clear();
		// all words
		for(i = 0; i < (int)map_dict.GetCount(); i++) 
			fvoc.AddLine( map_dict[i] );
		// END and START
		fvoc.AddLine(SP_START);
		fvoc.AddLine(SP_END);
		fvoc.Write();
		fvoc.Close();
	}
}
*/

bool MLFOutput::GenerateNGram( wxString base_filename, int order, ProgressDlg *dlg )
{
	wxString mlffile = base_filename + ".mlf";
	wxString vocfile = base_filename + ".dic";
	wxString outputfile = base_filename + ".gram";

#ifdef __WXMSW__
	#if defined(_DEBUG)
		wxString cmd = "Ngram.exe";
	#else
		wxString cmd = "Ngram.exe";
	#endif   
#elif __WXGTK__
	#if defined(_DEBUG)
		wxString cmd = "ngramd";
	#else
		wxString cmd = "ngram";
	#endif   
#elif __WXMAC__
	#if defined(__AXDEBUG__)
		wxString cmd = "ngramd";
	#else
		wxString cmd = "ngram";
	#endif   
#endif

	wxString args = " ";

	args << " -output_fname " << outputfile.c_str();
	args << " " << mlffile.c_str();
	args << " " << vocfile.c_str();
	args << " " << order;

	wxLogDebug(args);

	AxProcess *process = new AxProcess( cmd, args, NULL );
	if ( process->Start() )
	{
		process->Detach();
		int pid = process->GetPid();
		process->m_deleteOnTerminate = false;
		if ( dlg ) // suspend only if dlg
		{
			while ( process->GetPid() == pid )
			{
				wxMilliSleep( 500 );
				//wxSafeYield();
				if ( dlg && !dlg->IncTimerOperation() )
				{
					process->m_deleteOnTerminate = true;
					wxKill( pid, wxSIGKILL ); 
					return false;
				
				}
			}
		}	
	}
	if ( process->m_status != 0 )
	{
		delete process;
		return false;
	}
	
	delete process;
	return true;
}

/*
void MLFOutput::LoadTypes( wxString filename )
{
	m_types.Load( filename );
}
*/

void MLFOutput::LoadDictionary( wxString filename )
{
	wxTextFile file( filename );
	file.Open();
	if ( !file.IsOpened() )
		return;	

	wxString str = file.GetFirstLine();
	wxString word;
	while(1)
	{
		if ( !str.IsEmpty() )
		{
			wxStringTokenizer tkz( str , " ");
			//wxStringTokenizer tkz( str , "\t");
			if ( tkz.HasMoreTokens() )
			{
				wxString s = tkz.GetNextToken();
				m_loadedDict.Add( s );
			}
		}
		if ( file.Eof() )
			break;
		str = file.GetNextLine();
	}

	/*if ( writePosition )
	{
		
	}*/

	/*
	for (int i = 0; i < (int)m_loadedDict.GetCount(); i++)
	{
		bool is_note;
		int pos;
		wxString line = m_loadedDict[i];
		if (MLFInput::IsElement( &is_note, &line, &pos ) )
		{
			if ( !is_note )
			{
				WgSymbole *s= MLFInput::ConvertSymbole( line );
				if ( s )
				{
					WriteSymbole( s );
					if ( writePosition  && !m_widths.GetWidth( &m_symboles.Last() ) )
						wxLogWarning( "Unknown width for %s", line.c_str() );
					delete s;
					
				}
			}
			else
			{
				WgNote *n= MLFInput::ConvertNote( line );
				if ( n )
				{
					WriteNote( n );
					if ( writePosition  && !m_widths.GetWidth( &m_symboles.Last() ) )
						wxLogWarning( "Unknown width for %s", line.c_str() );
					delete n;
					
				}
			}
		}
	}
	EndLabel();
	*/
}


void MLFOutput::CreateSubFile()
{
	wxASSERT( m_subfile == NULL );
	m_subfile = new wxFileOutputStream( m_filename + "_align" );
	wxASSERT( m_subfile->Ok() );
}

void MLFOutput::StartLabel( )
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

void MLFOutput::EndLabel( int offsets[], int end_points[] )
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

WgStaff *MLFOutput::GetUt1( WgStaff *staff , bool inPlace )
{
	if ( !staff )
		return NULL;

	int code, oct;

	WgStaff *nstaff = staff;
	if ( !inPlace )
		nstaff = new WgStaff( *staff );
	
	for (int i = 0; i < (int)nstaff->m_elements.GetCount(); i++ )
	{
		if ( nstaff->m_elements[i].TYPE == NOTE )
		{
			WgNote *note = (WgNote*)&nstaff->m_elements[i];
			{
				GetUt1( nstaff, note, &code, &oct );
				note->code = code;
				note->oct = oct;
			}
		}
		else
		{
			WgSymbole *symbole = (WgSymbole*)&nstaff->m_elements[i];
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

void MLFOutput::GetUt1( WgStaff *staff, WgElement *pelement, int *code, int *oct)
{
	if (!pelement || !code || !oct) return;

	char valeur = 0;
	int offs;

	if (!staff || (staff->getOctCl(pelement,&valeur, 1 )==0))
		valeur = 5;
	WgSymbole::calcoffs(&offs, valeur);

	*oct = pelement->oct;
	*code = 0;
	if (pelement->TYPE == SYMB)
		*code = ((WgSymbole*)pelement)->code + offs;
	else
		*code = ((WgNote*)pelement)->code + offs;

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

bool MLFOutput::ExportFile( WgFile *file, wxString filename )
{
	wxASSERT_MSG( file, "File cannot be NULL" );

    m_filename = filename;
    wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	this->m_file = file;
	return ExportFile();
}

bool MLFOutput::ExportFile( )
{
	wxASSERT_MSG( m_file, "File cannot be NULL" );

	if ( m_addHeader )
	{
		Write("#!MLF!#\n",8);
		m_addHeader = false;
	}

    WgPage *page = NULL;

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


bool MLFOutput::WritePage( const WgPage *page, bool write_header )
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
        WgStaff *staff = &page->m_staves[m_staff_i];
		m_staff = MLFOutput::SplitSymboles( staff );
        WriteStaff( m_staff );
		delete m_staff;
		m_staff = NULL;
    }


    return true;

}


// idem ExportFile() puis WritePage(), mais gere les segment de portee au moyen de imPage
bool MLFOutput::WritePage( const WgPage *page, wxString filename, ImPage *imPage, bool write_header )
{
	wxASSERT_MSG( page, "WgPage cannot be NULL" );
	wxASSERT_MSG( imPage, "ImPage cannot be NULL" );
	
	if ( write_header && m_addHeader )
	{
		Write("#!MLF!#\n",8);
		m_addHeader = false;
	}
	
    m_filename = filename;
	m_shortname = filename;
    //wxFileName::SplitPath( m_filename, NULL, &m_shortname, NULL );
	
	if ( m_addHeader )
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
		m_segment_i = 0;
        WgStaff *staff = &page->m_staves[m_staff_i];
		imPage->GetStaffSegmentOffsets( m_staff_i, offsets, split_points, end_points);
		m_staff = MLFOutput::SplitSymboles( staff );
        WriteStaff( m_staff, offsets, split_points, end_points );
		delete m_staff;
		m_staff = NULL;
    }
	m_dictSymbols.Sort( SortRecSymbols );

    return true;

}


bool MLFOutput::WriteStaff( const WgStaff *staff, int offsets[], int split_points[], int end_points[] )
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
            WriteNote( (WgNote*)&staff->m_elements[k] );
        }
        else
        {
            WriteSymbole( (WgSymbole*)&staff->m_elements[k] );
        }
    }
	EndLabel( offsets, end_points );

    return true;
}


bool MLFOutput::WriteNote(  WgNote *note )
{
	int  code, oct;
	GetUt1( m_staff, note, &code, &oct);

	MLFSymbole *mlfsb = NULL;

	// custos 
	if (note->val == CUSTOS)
	{
		mlfsb = (MLFSymbole*)wxCreateDynamicObject( m_model_symbole_name );
		mlfsb->SetValue( TYPE_CUSTOS, "", note->xrel, 0, _note[code],oct);
		m_symboles.Add( mlfsb );

	}
	// note ou silence
	else 
	{
		mlfsb = (MLFSymbole*)wxCreateDynamicObject( m_model_symbole_name );
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

bool MLFOutput::WriteSymbole(  WgSymbole *symbole )
{
	// gestion des segment de portees (pas actif ?????)
	if ((symbole->flag == BARRE) && (symbole->code == 'I'))
	{	
		wxASSERT_MSG( false, "Should not happen..." );
		EndLabel();
		StartLabel();
		return true;
	}

	MLFSymbole *mlfsb = (MLFSymbole*)wxCreateDynamicObject( m_model_symbole_name );

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

void MLFOutput::WriteDictionary( wxString filename )
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
	
void MLFOutput::WriteStatesPerSymbol( wxString filename )
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

void MLFOutput::WriteHMMSymbols( wxString filename )
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

// WDR: handler implementations for MLFOutput


//----------------------------------------------------------------------------
// MLFInput
//----------------------------------------------------------------------------


bool MLFInput::IsElement( bool *note, wxString *line, int *pos )
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



WgNote *MLFInput::ConvertNote( wxString line )
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

	WgNote *note = new WgNote();
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

WgSymbole *MLFInput::ConvertSymbole( wxString line )
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
			WgSymbole *symbole = new WgSymbole();
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
		WgSymbole *symbole = new WgSymbole();
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
		WgSymbole *symbole = new WgSymbole();
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
		WgSymbole *symbole = new WgSymbole();
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

		WgSymbole *symbole = new WgSymbole();
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

MLFInput::MLFInput( WgFile *file, wxString filename ) :
    WgFileInputStream( file, filename )
{
	m_page_i  = m_page_label = 0;
	m_staff_i = m_staff_label = -1;
	m_segment_i = m_segment_label = 0;
}

MLFInput::~MLFInput()
{
}


// copie le portee en convertissant les symboles de Ut vers Clef courrant
// si inPlace, directment dans staff

WgStaff *MLFInput::GetNotUt1( WgStaff *staff , bool inPlace )
{
	if ( !staff )
		return NULL;

	int code, oct;

	WgStaff *nstaff = staff;
	if ( !inPlace )
		nstaff = new WgStaff( *staff );
	
	for (int i = 0; i < (int)nstaff->m_elements.GetCount(); i++ )
	{
		if ( nstaff->m_elements[i].TYPE == NOTE )
		{
			WgNote *note = (WgNote*)&nstaff->m_elements[i];
			{
				GetNotUt1( nstaff, note, &code, &oct );
				note->code = code;
				note->oct = oct;
			}
		}
		else
		{
			WgSymbole *symbole = (WgSymbole*)&nstaff->m_elements[i];
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


void MLFInput::GetNotUt1( WgStaff *staff, WgElement *pelement, int *code, int *oct)
{
	if (!pelement || !code || !oct) return;

	char valeur = 0;
	int offs;

	if (!staff || (staff->getOctCl(pelement,&valeur, 1 )==0))
		valeur = 5;
	WgSymbole::calcoffs(&offs, valeur);

	*oct = pelement->oct;
	*code = 0;

	if (pelement->TYPE == SYMB)
		*code = ((WgSymbole*)pelement)->code;
	else
		*code = ((WgNote*)pelement)->code;

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

void MLFInput::GetPitchWWG( char code , int *code1 )
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
	
bool MLFInput::ReadLine( wxString *line )
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


bool MLFInput::ReadLabelStr( wxString label )
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
	str = str.BeforeLast('_'); // remove _staff"
	//m_page_label = atoi( str.AfterLast('_').c_str() );
	m_page_label = 0; // Desactivated

	return true;
}


bool MLFInput::ImportFile( int staff_per_page )
{
	m_file->m_pages.Clear();

	wxString line;
	if ( !ReadLine( &line )  || (line != "#!MLF!#" ))
		return false;

	WgPage *page = NULL;
	WgStaff *staff = NULL;
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
				page = new WgPage();
				n_staff = 0;
				offset = 0;
			}
			staff = new WgStaff();
			ReadLabel( staff, offset );
			WgStaff *staff2 = GetNotUt1( staff );
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
	// fill pages following file name indexes
	// Desactivated !!!
	/*{
		while ( ReadLine( &line ) && ReadLabelStr( line ) )
		{
			if ( m_page_i < m_page_label ) // new page
			{
				if ( page && staff ) // add last staff
				{
					WgStaff *staff2 = GetNotUt1( staff );
					delete staff;
					staff = staff2;
					page->m_staves.Add( staff );
					page->nbrePortees++;
					staff = NULL;
				}
				if ( page ) // add last page
				{
					m_file->m_pages.Add( page );
					m_file->m_fheader.nbpage++;
					page = NULL;
				}
				page = new WgPage();
				m_page_i++;
				m_staff_i = -1;
				m_segment_i = 0;
				offset = 0;
			}
			if ( page && ( m_staff_i < m_staff_label )) // new staff
			{
				if ( staff ) // add last staff
				{
					GetNotUt1( staff, true );
					page->m_staves.Add( staff );
					page->nbrePortees++;
					staff = NULL;
				}
				staff = new WgStaff();
				m_staff_i = ++
				m_segment_i = 0;
				offset = 0;
			}
			if ( staff )
				ReadLabel( staff, offset );
		}
	}
	*/

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

bool MLFInput::ReadPage( WgPage *page , bool firstLineMLF, ImPage *imPage )
{
	wxString line;
	if ( firstLineMLF  && (!ReadLine( &line )  || (line != "#!MLF!#" )))
		return false;

	WgStaff *staff = NULL;
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

bool MLFInput::ReadLabel( WgStaff *staff, int offset )
{
	bool is_note;
	int pos = 0;
	wxString line;

	while ( ReadLine( &line ) &&  MLFInput::IsElement( &is_note, &line, &pos ) )
	{
		if ( !is_note )
		{
			WgSymbole *s = MLFInput::ConvertSymbole( line );
			if ( s )
			{
				s->xrel = pos + offset;
				staff->m_elements.Add( s );
			}
		}
		else
		{
			WgNote *n = MLFInput::ConvertNote( line );
			if ( n )
			{
				n->xrel = pos + offset;
				staff->m_elements.Add( n );
			}
		}
	}       
    return true;
}

// WDR: handler implementations for MLFInput

#endif // AX_WG
