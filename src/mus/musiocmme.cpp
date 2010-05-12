/////////////////////////////////////////////////////////////////////////////
// Name:        musiocmme.cpp
// Author:      Laurent Pugin
// Created:     2008
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musiocmme.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"

#include "musiocmme.h"

#include "app/axapp.h"

//----------------------------------------------------------------------------
// MusCmmeOutput
//----------------------------------------------------------------------------

MusCmmeOutput::MusCmmeOutput( MusFile *file, wxString filename ) :
    // This is pretty bad. We open a bad fileoutputstream as we don't use it
	MusFileOutputStream( file, -1 )
{
	m_filename = filename;
    m_xml_root = NULL;
    m_xml_current = NULL;

	// write xml file
	wxASSERT( !m_xml_root );
	m_xml_root = new TiXmlElement("Piece"); // new root
}

MusCmmeOutput::~MusCmmeOutput()
{
    if (m_xml_root)
        delete m_xml_root;

}

bool MusCmmeOutput::ExportFile( )
{
    int i;

	if ( !m_xml_root )
	{
		wxLogMessage(_("Cannot open the file '%s'"), m_filename.c_str() );
		return false;
	}
    WriteFileHeader( &m_file->m_fheader ); // fileheader
    
    int min_voice, max_voice;
    m_file->GetNumberOfVoices( &min_voice, &max_voice );
    
     // general data
	TiXmlElement voice_data("VoiceData");
    // num voices
    TiXmlElement num_voices("NumVoices");
    num_voices.LinkEndChild( new TiXmlText( wxString::Format("%d", max_voice - min_voice + 1 ).c_str() ) );
    voice_data.InsertEndChild( num_voices );
    // voices
    for(i = min_voice; i <= max_voice; i++)
    {
        TiXmlElement voice("Voice");
        TiXmlElement name("Name");
        name.LinkEndChild( new TiXmlText( "[undefined]" ) );
        voice.InsertEndChild( name );
        voice_data.InsertEndChild( voice );
    }
    //
    m_xml_root->InsertEndChild( voice_data ); 
    
     // music section
	TiXmlElement music_section("MusicSection");
    TiXmlElement mensural_music("MensuralMusic");
    // num voices
    mensural_music.InsertEndChild( num_voices );
    // voices
    for(i = min_voice; i <= max_voice; i++)
    {
        TiXmlElement voice("Voice");
        TiXmlElement voice_num("VoiceNum");
        voice_num.LinkEndChild( new TiXmlText( wxString::Format("%d", i + 1 ).c_str() ) );
        voice.InsertEndChild( voice_num );
        TiXmlElement event_list("EventList");
        // get the voice on one staff and write it
        m_xml_current = &event_list;
        MusStaff *staff = m_file->GetVoice( i);
        //wxLogMessage("%d, %d", voices, staff->m_elements.GetCount() );
        WriteStaff( staff );
        delete staff;
        //         
        voice.InsertEndChild( event_list );
        mensural_music.InsertEndChild( voice );
    }
    //
    music_section.InsertEndChild( mensural_music );
    //
    m_xml_root->InsertEndChild( music_section ); 

	TiXmlDocument dom( m_filename.c_str() );
    TiXmlDeclaration declaration( "1.0", "UTF-8", "" );

    dom.InsertEndChild( declaration );
    dom.InsertEndChild( *m_xml_root );
    if ( !dom.SaveFile( ) )
    {
        wxLogMessage(_("Cannot write file '%s'"), m_filename.c_str() );
		return false;
    }
	return true;
    
}

bool MusCmmeOutput::WriteFileHeader( const MusFileHeader *header )
{
    //int i;

    m_xml_root->SetAttribute( "xmlns", "http://www.cmme.org" );
    m_xml_root->SetAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
    m_xml_root->SetAttribute( "xsi:schemaLocation", "http://www.cmme.org cmme.xsd" );
    m_xml_root->SetAttribute( "CMMEversion", "0.897" );
    
    // general data
	TiXmlElement general_data("GeneralData");
    // title
    TiXmlElement title("Title");
    title.LinkEndChild( new TiXmlText( "[undefined]" ) );
    general_data.InsertEndChild( title );
    // composer
    TiXmlElement composer("Composer");
    composer.LinkEndChild( new TiXmlText( "[undefined]" ) );
    general_data.InsertEndChild( composer );
    // editor
    TiXmlElement editor("Editor");
    editor.LinkEndChild( new TiXmlText( "[undefined]" ) );
    general_data.InsertEndChild( editor );
    // notes
    TiXmlElement notes("Notes");
    notes.LinkEndChild( new TiXmlText( 
        wxString::Format("Exported from Aruspix version %s", AxApp::s_version.c_str() ).c_str() ) );
    general_data.InsertEndChild( notes );
    //
    m_xml_root->InsertEndChild( general_data );
    
	return true;
}

bool MusCmmeOutput::WriteStaff( const MusStaff *staff )
{
	unsigned int k;

	for (k = 0;k < staff->nblement ; k++ )
	{
		if ( staff->m_elements[k].IsNote() )
		{
			WriteNote( (MusNote*)&staff->m_elements[k] );
		}
		else if ( staff->m_elements[k].IsSymbol() )
		{
			WriteSymbol( (MusSymbol*)&staff->m_elements[k] );
		}
	}

	return true;
}

bool MusCmmeOutput::WriteNote( const MusNote *note )
{
	int i;
    
    wxASSERT_MSG( m_xml_current, "m_xml_current cannot be NULL");
  
    wxString note_letter;
    switch ( note->code )
    {
        case F1 : note_letter = "B"; break;
        case F2 : note_letter = "C"; break;
        case F3 : note_letter = "D"; break;
        case F4 : note_letter = "E"; break;
        case F5 : note_letter = "F"; break;
        case F6 : note_letter = "G"; break;
        case F7 : note_letter = "A"; break;
        case F8 : note_letter = "B"; break;
        case F9 : note_letter = "C"; break;
        default :
            wxLogWarning("Undefined pitch, note skipped" );
            return true;       
    }
    
    wxString note_type;
    switch ( note->val )
    {
        case LG : note_type = "Longa"; break;
        case BR : note_type = "Brevis"; break;
        case RD : note_type = "Semibrevis"; break;
        case BL : note_type = "Minima"; break;
        case NR : note_type = "Semiminima"; break;
        case CR : note_type = "Fusa"; break;
        case DC : note_type = "Semifusa"; break;
        default :
            wxLogWarning("Undefined note value, note skipped" );
            return true;
    }
    
    if ( note->sil == _NOT)
    {
        TiXmlElement note_element("Note");
        // Length
        TiXmlElement type("Type");
        type.LinkEndChild( new TiXmlText( note_type.c_str() ) );
        note_element.InsertEndChild(type);
        // Letter
        TiXmlElement letter_name("LetterName");
        letter_name.LinkEndChild( new TiXmlText( note_letter.c_str() ) );
        note_element.InsertEndChild(letter_name);
        // Octave
        TiXmlElement octave_num("OctaveNum");
        octave_num.LinkEndChild( new TiXmlText( wxString::Format("%d", note->oct ).c_str() ) );
        note_element.InsertEndChild(octave_num);
        //
        m_xml_current->InsertEndChild(note_element);    
    }
    else // rest
    {
        TiXmlElement rest_element("Rest");
        // Length
        TiXmlElement type("Type");
        type.LinkEndChild( new TiXmlText( note_type.c_str() ) );
        rest_element.InsertEndChild(type);
        m_xml_current->InsertEndChild(rest_element);    
    }
	
	// lyric	
	for ( i = 0; i < (int)note->m_lyrics.GetCount(); i++ ){
		MusSymbol *lyric = &note->m_lyrics[i];
		if ( lyric )
			WriteSymbol( lyric );
	}
	
	return true;
}

bool MusCmmeOutput::WriteSymbol( const MusSymbol *symbol )
{
    // if ( symbol->IsLyric() ) // To be fixed ??
    if ( (symbol->flag == CHAINE) && (symbol->fonte == LYRIC) )
		WriteLyric( symbol );
	
	return true;
}

bool MusCmmeOutput::WriteElementAttr( const MusElement *element )
{
	
	return true;
}

bool MusCmmeOutput::WriteLyric( const MusElement *element )
{
    return true;
}


// WDR: handler implementations for MusCmmeOutput


//----------------------------------------------------------------------------
// MusCmmeInput
//----------------------------------------------------------------------------

MusCmmeInput::MusCmmeInput( MusFile *file, wxString filename ) :
	MusFileInputStream( file, filename )
{
	m_vmaj = m_vmin = m_vrev = 10000; // arbitrary version, we assume we will never reach version 10000...
}

MusCmmeInput::~MusCmmeInput()
{
}

bool MusCmmeInput::ImportFile( )
{
	int i;

	if ( !IsOk() )
	{
		wxLogMessage(_("Cannot read file '%s'"), m_file->m_fname.c_str() );
		return false;
	}

    ReadFileHeader( &m_file->m_fheader ); // fileheader
    
	m_file->m_pages.Clear();		
    for (i = 0; i < m_file->m_fheader.nbpage; i++ )
	{
		MusPage *page = new MusPage();
		ReadPage( page );
		m_file->m_pages.Add( page );
    }
    if ( !ReadSeparator() ) 
		return false;
	if ( m_file->m_fheader.param.entetePied & PAGINATION )
		ReadPagination( &m_file->m_pagination );
	if ( m_file->m_fheader.param.entetePied & ENTETE )
		ReadHeaderFooter( &m_file->m_header );
	if ( m_file->m_fheader.param.entetePied & PIEDDEPAGE )
		ReadHeaderFooter( &m_file->m_footer );

	//wxLogMessage("OK %d", m_file->m_pages.GetCount() );

	return true;
}

bool MusCmmeInput::ReadFileHeader( MusFileHeader *header )
{
	 

	return true;
}


bool MusCmmeInput::ReadSeparator( )
{
    return true;
}

bool MusCmmeInput::ReadPage( MusPage *page )
{
	int j;

    if ( !ReadSeparator() )
		return false;

    for (j = 0; j < page->nbrePortees; j++) 
	{
		MusStaff *staff = new MusStaff();
		ReadStaff( staff );
		page->m_staves.Add( staff );
	}

	return true;

}
bool MusCmmeInput::ReadStaff( MusStaff *staff )
{
	unsigned int k;

	unsigned char c;
	for ( k = 0; k < staff->nblement; k++ )
	{
		Read( &c, 1 );
		if ( c == NOTE )
		{
			MusNote *note = new MusNote();
			note->no = k;
			ReadNote( note );
				 
			staff->m_elements.Add( note );
		}
		else if ( c == SYMB )
		{
			MusSymbol *symbol = new MusSymbol();
			symbol->no = k;
			ReadSymbol( symbol );
			staff->m_elements.Add( symbol );
		}
	}
			
	return true;
}

bool MusCmmeInput::ReadNote( MusNote *note )
{
	char count;
	
	for ( int i = 0; i < count; i++ ) {
		MusSymbol *lyric = new MusSymbol();
		Read( &lyric->TYPE, 1 );
		ReadSymbol( lyric );
		lyric->m_note_ptr = note;				 		
		note->m_lyrics.Add( lyric );
	}

	return true;
}

bool MusCmmeInput::ReadSymbol( MusSymbol *symbol )
{
	if ( symbol->IsLyric() )
        ReadLyric( symbol );
     
	return true;
}

bool MusCmmeInput::ReadElementAttr( MusElement *element )
{
	return true;
}

bool MusCmmeInput::ReadLyric( MusElement *element )
{

	return true;
}
bool MusCmmeInput::ReadPagination( MusPagination *pagination )
{

	return true;
}

bool MusCmmeInput::ReadHeaderFooter( MusHeaderFooter *headerfooter)
{
	return true;
}


// WDR: handler implementations for MusCmmeInput


