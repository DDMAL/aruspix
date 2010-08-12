/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei.cpp
// Author:      Laurent Pugin
// Created:     2008
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musiomei.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"
#include "wx/txtstrm.h"

#include "musiomei.h"

#include "app/axapp.h"

//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

MusMeiOutput::MusMeiOutput( MusFile *file, wxString filename ) :
    // This is pretty bad. We open a bad fileoutputstream as we don't use it
	MusFileOutputStream( file, -1 )
{
	m_filename = filename;
    m_xml_root = NULL;
    m_xml_current = NULL;
    m_xml_staves = NULL;

	// write xml file
	wxASSERT( !m_xml_root );
	m_xml_root = new TiXmlElement("mei"); // new root
    m_xml_root->SetAttribute( "meiversion", "1.9b" );
}

MusMeiOutput::~MusMeiOutput()
{
    if (m_xml_root)
        delete m_xml_root;
        
    if (m_xml_staves) {
        wxLogWarning("m_xml_staves should be NULL");
        delete[] m_xml_staves;
    }
}

bool MusMeiOutput::ExportFile( )
{
    
    // temporary header
    // tinyxml does not allow to have other doc declarations than ?xml?
    wxFFile f(m_filename, "w" );
	wxFFileOutputStream stream( f );
	wxTextOutputStream fmei( stream );
	fmei.WriteString( wxString::Format( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" ) );
	//fmei.WriteString( wxString::Format( "<?oxygen SCHSchema=\"http://www.aruspix.net/mei/mei-all.rng\"?>\n" ) );
    //fmei.WriteString( wxString::Format( "<?oxygen RNGSchema=\"http://www.aruspix.net/mei/mei-mensural.rng\" type=\"xml\"?>\n" ) );
	fmei.WriteString( wxString::Format( "<?oxygen SCHSchema=\"/Users/laurent/Documents/mei/branches/mei19.1/ModularizationTesting/mei-all.rng\"?>\n" ) );
    fmei.WriteString( wxString::Format( "<?oxygen RNGSchema=\"/Users/laurent/Documents/mei/branches/mei19.1/ModularizationTesting/mei-all.rng\" type=\"xml\"?>\n" ) );
	//stream.Close();

	if ( !m_xml_root )
	{
		wxLogMessage(_("Cannot open the file '%s'"), m_filename.c_str() );
		return false;
	}
    WriteFileHeader( &m_file->m_fheader ); // fileheader
    
    // music
    TiXmlElement music("music");
    TiXmlElement body("body");
    TiXmlElement mdiv("mdiv");
    
    if ( 1 ) {
        TiXmlElement parts("parts");
        WriteParts( &parts );
        mdiv.InsertEndChild( parts );
    } else { 
        TiXmlElement score("score");
        WriteScore( &score );
        mdiv.InsertEndChild( score );
    }

    body.InsertEndChild( mdiv );
    music.InsertEndChild( body );
    m_xml_root->InsertEndChild( music );
    
    // write the document
	TiXmlDocument dom; // m_filename.c_str() );    
    //TiXmlDeclaration declaration( "1.0", "UTF-8", "" );
    //dom.InsertEndChild( declaration );
    dom.InsertEndChild( *m_xml_root );
    dom.Print( f.fp() );
    /*if ( !dom.SaveFile( ) )
    {
        wxLogMessage(_("Cannot write file '%s'"), m_filename.c_str() );
		return false;
    }*/
    f.Close();
	return true;
    
}


void MusMeiOutput::WriteParts( TiXmlElement *parts ) {

    int min_voice, max_voice;
    m_file->GetNumberOfVoices( &min_voice, &max_voice );

    int i;
    // parts
    for(i = min_voice; i <= max_voice; i++)
    {
        // Get the entire voice on one staff
        MusStaff *mus_staff = m_file->GetVoice( i );
        //
        TiXmlElement part("part");
        part.SetAttribute( "n", wxString::Format("%d", i+1 ).c_str() );
        TiXmlElement section("section");
        
        // scorderf
        TiXmlElement scoredef("scoredef");
        TiXmlElement staffgrp("staffgrp");
        TiXmlElement staffdef("staffdef");
        staffdef.SetAttribute( "n", wxString::Format("%d", i+1 ).c_str() );
        staffdef.SetAttribute( "lines", wxString::Format("%d", mus_staff->portNbLine ).c_str() );
        staffdef.SetAttribute( "clef.line", "1" );
        staffdef.SetAttribute( "clef.shape", "C" );
        staffgrp.InsertEndChild( staffdef );
        scoredef.InsertEndChild( staffgrp );
        section.InsertEndChild( scoredef );
        
        // staff            
        TiXmlElement staff("staff");
        staff.SetAttribute( "n", wxString::Format("%d", i+1 ).c_str() );
        TiXmlElement layer("layer");
        m_xml_current = &layer;
        WriteStaff( mus_staff );
        delete mus_staff;
        staff.InsertEndChild( layer );
        section.InsertEndChild( staff );
        
        part.InsertEndChild( section );
        parts->InsertEndChild( part );
    }
}

void MusMeiOutput::WriteScore( TiXmlElement *score ) {

    int min_voice, max_voice;
    m_file->GetNumberOfVoices( &min_voice, &max_voice );
    
    int i;
    
    m_xml_staves = new TiXmlElement*[max_voice + 1];

    // scorderf
    TiXmlElement scoredef("scoredef");
    TiXmlElement staffgrp("staffgrp");

    for(i = min_voice; i <= max_voice; i++)
    {
        MusStaff *mus_staff = m_file->GetVoice( i );
    
        TiXmlElement staffdef("staffdef");
        staffdef.SetAttribute( "n", wxString::Format("%d", i+1 ).c_str() );
        staffdef.SetAttribute( "lines", wxString::Format("%d", mus_staff->portNbLine ).c_str() );
        staffdef.SetAttribute( "clef.line", "1" );
        staffdef.SetAttribute( "clef.shape", "C" );
        staffgrp.InsertEndChild( staffdef );
        
        TiXmlElement *xml_layer = new TiXmlElement("layer");
        //xml_staff->SetAttribute( "n", wxString::Format("%d", i+1 ).c_str() );
        //TiXmlElement layer("layer");
        m_xml_staves[ i ] = xml_layer;
        
        delete mus_staff;
    }
    scoredef.InsertEndChild( staffgrp );
    score->InsertEndChild( scoredef );
    
    MusPage *page;
    for (i = 0; i < m_file->m_fheader.nbpage; i++ )
	{
		page = &m_file->m_pages.Item(i);
        if (i > 0) {
            for(i = min_voice; i <= max_voice; i++) {
                m_xml_staves[i]->LinkEndChild( new TiXmlElement("pb") );
            }        
        }
		WritePage( page );
    }
    
    TiXmlElement section("section");
    for(i = min_voice; i <= max_voice; i++)
    {
        TiXmlElement staff("staff");
        staff.SetAttribute( "n", wxString::Format("%d", i+1 ).c_str() );
        staff.InsertEndChild( *m_xml_staves[i] );
        delete m_xml_staves[i];
        section.InsertEndChild( staff );
    }
    score->InsertEndChild( section );
    
    delete[] m_xml_staves;
    m_xml_staves = NULL;
}

bool MusMeiOutput::WriteFileHeader( const MusFileHeader *header )
{
    //int i;
 
    // meihead
    TiXmlElement meihead("meihead");
    
    // filedesc
    TiXmlElement filedesc("filedesc");
    // titlestmt
    TiXmlElement titlestmt("titlestmt");
    // title
    TiXmlElement title("title");
    title.LinkEndChild( new TiXmlText( (header->name.Length() > 0) ? header->name  : "[undefined]" ) );
    // pubstmt
    TiXmlElement pubstmt("pubstmt");
    titlestmt.InsertEndChild( title );
    filedesc.InsertEndChild( titlestmt );
    filedesc.InsertEndChild( pubstmt );
    meihead.InsertEndChild( filedesc );
    
    // encodingdesc
    TiXmlElement encodingdesc("encodingdesc");
    // projectdesc
    TiXmlElement projectdesc("projectdesc");
    // p
    TiXmlElement p("p");
    p.LinkEndChild(new TiXmlText( wxString::Format("Encoded with Aruspix version %d.%d.%d", 
            AxApp::s_version_major, AxApp::s_version_minor, AxApp::s_version_revision  ) ) );
    projectdesc.InsertEndChild( p );    
    encodingdesc.InsertEndChild( projectdesc );
    meihead.InsertEndChild( encodingdesc );
    
    m_xml_root->InsertEndChild( meihead );
    
	return true;
}


bool MusMeiOutput::WritePage( const MusPage *page ){

	int j;
    
    TiXmlElement *xml_previous = m_xml_current;   

	MusStaff *staff = NULL;
    for (j = 0; j < page->nbrePortees; j++) 
	{
        staff = &page->m_staves[j];
        m_xml_current = m_xml_staves[staff->voix];
		WriteStaff( staff );
        m_xml_current->LinkEndChild( new TiXmlElement("sb") ); // sb at the end of every single sb
    }
    
    m_xml_current = xml_previous; 
	return true;
}

bool MusMeiOutput::WriteStaff( const MusStaff *staff )
{
	unsigned int k;
    
    TiXmlElement *xml_previous;
    TiXmlElement *ligature = NULL;
    

	for (k = 0;k < staff->nblement ; k++ )
	{
		if ( staff->m_elements[k].IsNote() )
		{
            if ((&staff->m_elements[k])->ligat && !ligature) {
                xml_previous = m_xml_current;
                ligature = new TiXmlElement("ligature");
                m_xml_current = ligature;
            }
			WriteNote( (MusNote*)&staff->m_elements[k] );
            if ((&staff->m_elements[k])->fligat && ligature) {
                m_xml_current = xml_previous;
                m_xml_current->InsertEndChild(*ligature);
                delete ligature;
                ligature = NULL;
            } 
		}
		else if ( staff->m_elements[k].IsSymbol() )
		{
			WriteSymbol( (MusSymbol*)&staff->m_elements[k] );
		}
	}
    
    if (ligature) {
        wxLogWarning("Unclosed ligature" );
        delete ligature;
    }

	return true;
}

bool MusMeiOutput::WriteNote( const MusNote *note )
{
	int i;
    
    wxASSERT_MSG( m_xml_current, "m_xml_current cannot be NULL");
  
    wxString note_letter;
    switch ( note->code )
    {
        case F1 : note_letter = "b"; break;
        case F2 : note_letter = "c"; break;
        case F3 : note_letter = "d"; break;
        case F4 : note_letter = "e"; break;
        case F5 : note_letter = "f"; break;
        case F6 : note_letter = "g"; break;
        case F7 : note_letter = "a"; break;
        case F8 : note_letter = "b"; break;
        case F9 : note_letter = "c"; break;
        default :
            wxLogWarning("Undefined pitch, note skipped" );
            return true;       
    }
    
    wxString note_type;
    switch ( note->val )
    {
        case LG : note_type = "longa"; break;
        case BR : note_type = "brevis"; break;
        case RD : note_type = "semibrevis"; break;
        case BL : note_type = "minima"; break;
        case NR : note_type = "semiminima"; break;
        case CR : note_type = "fusa"; break;
        case DC : note_type = "semifusa"; break;
        /*case LG : note_type = "1"; break;
        case BR : note_type = "1"; break;
        case RD : note_type = "1"; break;
        case BL : note_type = "2"; break;
        case NR : note_type = "4"; break;
        case CR : note_type = "8"; break;
        case DC : note_type = "16"; break;*/
        
        case CUSTOS : break;
        default :
            wxLogWarning("Undefined note value, note skipped" );
            return true;
    }
    
    if (note->sil == _NOT)
    {
        TiXmlElement note_element("note");
        note_element.SetAttribute( "pname", note_letter.c_str() );
        note_element.SetAttribute( "dur", note_type.c_str() );
        note_element.SetAttribute( "oct", wxString::Format("%d", note->oct ).c_str() );
        // coloration
		if (((note->inv_val && (note->val < BL )) || (note->oblique && (note->val > NR )))) {
            note_element.SetAttribute( "colored", "true" );
        }
        // ligature obliqua
        if (note->ligat &&  note->oblique) {
            note_element.SetAttribute( "lig", "obliqua" );
        }
        // TODO stem direction;
        note_element.SetAttribute( "num", wxString::Format("%d", note->durNum ).c_str() );
        note_element.SetAttribute( "numbase", wxString::Format("%d", note->durDen ).c_str() );
        
        m_xml_current->InsertEndChild(note_element);    
    }
    else if ((note->sil == _SIL) && (note->val == CUSTOS))
    {
        TiXmlElement custos_element("custos");
        custos_element.SetAttribute( "pname", note_letter.c_str() );
        custos_element.SetAttribute( "oct", wxString::Format("%d", note->oct ).c_str() );
        m_xml_current->InsertEndChild(custos_element);    
    }
    else // rest
    {
        TiXmlElement rest_element("rest");
        rest_element.SetAttribute( "dur", note_type.c_str() );
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

bool MusMeiOutput::WriteNeume( const MusNeume *neume ) 
{}

bool MusMeiOutput::WriteSymbol( const MusSymbol *symbol )
{

	if (symbol->flag == BARRE) {
        TiXmlElement barline("barline");
        if (symbol->calte == 1) {
            barline.SetAttribute( "rend", "end" );
        }
        m_xml_current->InsertEndChild( barline );
    } else if (symbol->flag == ALTER) {
        TiXmlElement accid("accid");
        switch (symbol->flag)
		{	case BECAR :  accid.SetAttribute( "accid", "n" ); break;
			case D_DIESE : accid.SetAttribute( "accid", "ss" ); break;
			case DIESE : accid.SetAttribute( "accid", "s" ); break;
			case D_BEMOL :  accid.SetAttribute( "accid", "ff" ); break;
			case BEMOL :  accid.SetAttribute( "accid", "f" ); break;
		}
        // 
        // accid.SetAttribute( "loc", wxString::Format("%d", symbol->dec_y));
        m_xml_current->InsertEndChild( accid );
    } else if (symbol->flag == PNT) {
        TiXmlElement dot("dot");
        // dot.SetAttribute( "loc", wxString::Format("%d", symbol->dec_y));
        m_xml_current->InsertEndChild( dot );
    
    } else if (symbol->flag == IND_MES) {
        TiXmlElement mensur("mensur");
    	if (symbol->code & 64)
        {	
            switch (symbol->calte)
            {	
                case 0: 
                    mensur.SetAttribute( "sign", "C" ); 
                    break;
                case 1:
					mensur.SetAttribute( "sign", "C" );
                    mensur.SetAttribute( "slash", "1" ); 
                    break;
                case 2:
                    mensur.SetAttribute( "num", "2" );
                    break;
                case 3:
					mensur.SetAttribute( "num", "3" );
                    break;
                case 4:
					mensur.SetAttribute( "num", "2" );
                    mensur.SetAttribute( "slash", "1" ); 
                    break;
                case 5:
					mensur.SetAttribute( "num", "3" );
                    mensur.SetAttribute( "slash", "1" ); 
                    break;
                }
        } else {
            if (symbol->code & 32) {
            	mensur.SetAttribute( "sign", "O" );
            } else if (symbol->code & 16) {
                mensur.SetAttribute( "sign", "C" );
            } else if (symbol->code & 8) {
                mensur.SetAttribute( "sign", "C" );
            }
            // slash and dot
            if (symbol->code & 4) {	
                mensur.SetAttribute( "slash", "1" );
            }
            if (symbol->code & 2){
                mensur.SetAttribute( "dot", "true" );	
            }
        }
        // figures
        if ( symbol->code & 1) {
            mensur.SetAttribute( "num", wxString::Format("%d", max( symbol->durNum, 1 ) ).c_str() );
            mensur.SetAttribute( "numbase", wxString::Format("%d", max( symbol->durDen, 1 ) ).c_str() );
        }
        m_xml_current->InsertEndChild( mensur );

    } else if (symbol->flag == CLE) {
        TiXmlElement clefchange("clefchange");
        // UT1
        wxString line = "1";
        wxString shape = "C";
        switch(symbol->code)	// cleid
        {	
            case SOL1 : shape = "G"; break;
            case SOLva : shape = "G"; line= "2" ; break; // octava?
            case UT2 : line = "2"; break;
            case SOL2 : shape = "G"; line= "2" ; break;
            case FA3 : shape = "F"; line= "3" ; break;
            case UT3 : line= "3" ; break;
            case FA5 : shape = "F"; line= "5" ; break;
            case FA4 : shape = "F"; line= "4" ; break;
            case UT4 : line= "4" ; break;
            case UT5 : line= "5" ; break;
            case CLEPERC :  shape = "perc"; line= "1" ; break;
            default: break;
        }
        clefchange.SetAttribute( "line", line );
        clefchange.SetAttribute("shape", shape );
        m_xml_current->InsertEndChild( clefchange );
    
    } else if (symbol->flag == CHAINE) {
        if (symbol->fonte == LYRIC) {
            WriteLyric( symbol );
        }
    }
	
	return true;
}

bool MusMeiOutput::WriteElementAttr( const MusElement *element )
{
	
	return true;
}

bool MusMeiOutput::WriteLyric( const MusElement *element )
{
    return true;
}


// WDR: handler implementations for MusMeiOutput


//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

MusMeiInput::MusMeiInput( MusFile *file, wxString filename ) :
    // This is pretty bad. We open a bad fileoinputstream as we don't use it
	MusFileInputStream( file, -1 )
{
	m_filename = filename;
    m_xml_root = NULL;
    m_xml_current = NULL;
}

MusMeiInput::~MusMeiInput()
{
}

TiXmlNode *MusMeiInput::GetFirstChild( TiXmlNode *node, wxString element )
{
    if (!node) {
        return NULL;
    }
    TiXmlNode *next_node = node->FirstChild( element );
    if ( !next_node )
    {
        wxLogMessage(_("Missing element '<%s>' in the xml tree") , element.c_str() );
		return NULL;
    }
    return next_node;
}

bool MusMeiInput::ImportFile( )
{
	int i;
    TiXmlNode *node = NULL;
    
	wxASSERT( !m_xml_root );
    TiXmlDocument dom( (m_filename).c_str() );
	if ( !dom.LoadFile() )
	{
		wxLogMessage(_("Cannot read file '%s'"), m_file->m_fname.c_str() );
		return false;
	}
    
    m_xml_root = dom.RootElement();
    
    ReadFileHeader( &m_file->m_fheader ); // fileheader
    
    // binarization variables	
    node = GetFirstChild( m_xml_root, "music" );
    node = GetFirstChild( node, "body" );
    node = GetFirstChild( node, "mdiv" );
    if ( !node ) {
		return false;
    }
    
    if ( node->FirstChild( "parts" ) ) {
        ReadParts( node->FirstChild( "parts" )->ToElement() );
    } else if ( node->FirstChild( "score" ) ) {
        ReadScore( node->FirstChild( "score" )->ToElement() );
    } else { 
        wxLogMessage(_("Missing element '<parts>' or '<score>' in the xml tree") );
        return false;
    }
    /*
        if ( !root ) return;
            
        if ( root->Attribute( "pre_image_binarization_method" ) )
            RecFile::m_pre_image_binarization_method = atoi( root->Attribute( "pre_image_binarization_method" ) );
        if ( root->Attribute( "pre_page_binarization_method" ) )
            RecFile::m_pre_page_binarization_method = atoi( root->Attribute( "pre_page_binarization_method" ) );
        if ( root->Attribute( "pre_page_binarization_method_size" ) )
            RecFile::m_pre_page_binarization_method_size = atoi( root->Attribute( "pre_page_binarization_method_size" ) );
    }*/

	return true;
}


void MusMeiInput::ReadParts( TiXmlElement *parts ) {

    MusPage *page = new MusPage();
    ReadPage( page );
    m_file->m_pages.Add( page );
}

void MusMeiInput::ReadScore( TiXmlElement *score ) {

    MusPage *page = new MusPage();
    ReadPage( page );
    m_file->m_pages.Add( page );
}

bool MusMeiInput::ReadFileHeader( MusFileHeader *header )
{
	 

	return true;
}


bool MusMeiInput::ReadSeparator( )
{
    return true;
}

bool MusMeiInput::ReadPage( MusPage *page )
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
bool MusMeiInput::ReadStaff( MusStaff *staff )
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
		else if ( c == NEUME ) 
		{
			MusNeume *neume = new MusNeume();
			neume->no = k;
			ReadNeume( neume );
			
			staff->m_elements.Add( neume );
		}
	}
			
	return true;
}

bool MusMeiInput::ReadNote( MusNote *note )
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

bool MusMeiInput::ReadNeume( MusNeume *neume )
{
	char count;
	
	for ( int i = 0; i < count; i++ ) {
		MusSymbol *lyric = new MusSymbol();
		Read( &lyric->TYPE, 1 );
		ReadSymbol( lyric );
//		lyric->m_neume_ptr = neume;				 		
//		neume->m_lyrics.Add( lyric );
	}
	
	return true;
}

bool MusMeiInput::ReadSymbol( MusSymbol *symbol )
{
	if ( symbol->IsLyric() )
        ReadLyric( symbol );
     
	return true;
}

bool MusMeiInput::ReadElementAttr( MusElement *element )
{
	return true;
}

bool MusMeiInput::ReadLyric( MusElement *element )
{

	return true;
}
bool MusMeiInput::ReadPagination( MusPagination *pagination )
{

	return true;
}

bool MusMeiInput::ReadHeaderFooter( MusHeaderFooter *headerfooter)
{
	return true;
}


// WDR: handler implementations for MusMeiInput


