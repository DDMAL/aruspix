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
		else if ( staff->m_elements[k].IsNeume() )
		{
			WriteNeume( (MusNeume*)&staff->m_elements[k] );
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
        /*case LG : note_type = "longa"; break;
        case BR : note_type = "brevis"; break;
		case RD : note_type = "semibrevis"; break;
        case BL : note_type = "minima"; break;
        case NR : note_type = "semiminima"; break;
        case CR : note_type = "fusa"; break;
        case DC : note_type = "semifusa"; break;*/
        case LG : note_type = "long"; break;
        case BR : note_type = "breve"; break;
        case RD : note_type = "1"; break;
        case BL : note_type = "2"; break;
        case NR : note_type = "4"; break;
        case CR : note_type = "8"; break;
        case DC : note_type = "16"; break;
        
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
{
    wxASSERT_MSG( m_xml_current, "m_xml_current cannot be NULL");
	
	// should start with syllables (eventually)
	
	/* punctums all have the same note value, so 
	   first check if it is a multi-note neume */
	wxString neume_type;
	if (neume->n_type == UNEUME) neume_type = "uneume";
	else if (neume->n_type == INEUME) neume_type = "ineume";
	
	wxString neume_form;
	switch (neume->form) 
	{
		case QUIL : neume_form = "quilismatic"; break;
		case RHOM : neume_form = "rhombic"; break;
		case LIQ1 : neume_form = "liquescent1"; break;
		default:
			//NULL
			break;
	}
	
	wxString neume_name;
//	switch (neume->name)
//	{
//		case PUNCT : neume_name = "punctum"; break;
//		case VIRGA : neume_name = "virga"; break;
//		case PRECT : neume_name = "porrectus"; break;
//		case PODAT : neume_name = "pes"; break;
//		case CLVIS : neume_name = "clivis"; break;
//	}
	
	TiXmlElement neume_element(neume_type);
	if (neume_name)
		neume_element.SetAttribute("name", neume_name);
	if (neume_form)
		neume_element.SetAttribute("form", neume_form);
	
	if (neume->n_pitches.size())
		printf("we have a working pitch list\n");
	
	// pitch list
//	wxString note_letter;
//	for (int i = 0; i < neume->n_pitches.size(); i++)
//	{
//		TiXmlElement note_element("note");
//		switch ( neume->n_pitches[i]->code )
//		{
//			case F1 : note_letter = "b"; break;
//			case F2 : note_letter = "c"; break;
//			case F3 : note_letter = "d"; break;
//			case F4 : note_letter = "e"; break;
//			case F5 : note_letter = "f"; break;
//			case F6 : note_letter = "g"; break;
//			case F7 : note_letter = "a"; break;
//			case F8 : note_letter = "b"; break;
//			case F9 : note_letter = "c"; break;
//			default :
//				wxLogWarning("Undefined pitch, note skipped" );
//				return true;
//		}
//        note_element.SetAttribute( "oct", wxString::Format("%d", 
//					neume->n_pitches[i]->oct ).c_str() );
//		m_xml_current->InsertEndChild(note_element);
//	}
	
	m_xml_current->InsertEndChild(neume_element);

	
	return true;
}

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
        // This is not really correct. I don't think standard C and C-cut go in an mensur element
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
        // That's ok. 
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




/*

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
    //
    //    if ( !root ) return;
    //        
    //    if ( root->Attribute( "pre_image_binarization_method" ) )
    //        RecFile::m_pre_image_binarization_method = atoi( root->Attribute( "pre_image_binarization_method" ) );
    //    if ( root->Attribute( "pre_page_binarization_method" ) )
    //        RecFile::m_pre_page_binarization_method = atoi( root->Attribute( "pre_page_binarization_method" ) );
    //    if ( root->Attribute( "pre_page_binarization_method_size" ) )
    //        RecFile::m_pre_page_binarization_method_size = atoi( root->Attribute( "pre_page_binarization_method_size" ) );
    //}

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

*/


//----------------------------------------------------------------------------
// MusMeiInput
//----------------------------------------------------------------------------

MusMeiInput::MusMeiInput( MusFile *file, wxString filename ) :
    // This is pretty bad. We open a bad fileoinputstream as we don't use it
	MusFileInputStream( file, -1 )
{
	m_filename = filename;
    m_currentStaff = NULL;
    m_page = NULL;
}

MusMeiInput::~MusMeiInput()
{
}


bool MusMeiInput::ReadElement(xmlNode * node)
{
    xmlNode *cur_node = NULL;

    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (!strcmp((char*)cur_node->name,"accid")) { if (!mei_accid(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"barline")) { if (!mei_barline(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"clefchange")) { if (!mei_clefchange(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"custos")) { if (!mei_custos(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"dot")) { if (!mei_dot(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"layer")) { if (!mei_layer(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"measure")) { if (!mei_measure(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"mensur")) { if (!mei_mensur(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"note")) { if (!mei_note(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"parts")) { if (!mei_parts(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"rest")) { if (!mei_rest(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"score")) { if (!mei_score(cur_node)) return false; }
            else if (!strcmp((char*)cur_node->name,"staff")) { if (!mei_staff(cur_node)) return false; }
        }
        if (!ReadElement(cur_node->children)) return false ;
    }
    return true;
}

bool MusMeiInput::ReadAttributeBool( xmlNode *node, wxString name, bool *value, bool default_value )
{
    *value = default_value;
    xmlChar *attr_value = xmlGetProp(node, (const xmlChar *)name.c_str() );
    if (!attr_value) {
        return false;
    }
    *value = !strcmp((char *)attr_value,"true");
    xmlFree(attr_value);
    return true;
}

bool MusMeiInput::ReadAttributeInt( xmlNode *node, wxString name, int *value, int default_value )
{
    *value = default_value;
    xmlChar *attr_value = xmlGetProp(node, (const xmlChar *)name.c_str() );
    if (!attr_value) {
        return false;
    }
    *value = atoi((char *)attr_value);
    xmlFree(attr_value);
    return true;
}


bool MusMeiInput::ReadAttributeString( xmlNode *node, wxString name, wxString *value, wxString default_value )
{
    *value = default_value;
    xmlChar *attr_value = xmlGetProp(node, (const xmlChar *)name.c_str() );
    if (!attr_value) {
        return false;
    }
    *value = wxString((char *)attr_value);
    xmlFree(attr_value);
    return true;
}

bool MusMeiInput::ImportFile( )
{

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(m_filename.c_str(), NULL, 0);
    if (doc == NULL) {
		wxLogMessage(_("Cannot read file '%s'"), m_file->m_fname.c_str() );
		return false;
    }
    
    // When importing MEI, the first step is to load one single page and to load everything in one system
    MusPage *page = new MusPage();
    page->defin = 20;
    m_file->m_pages.Add( page );
    // To simplify accessibility, we keep a pointer on that pages
    m_page = &m_file->m_pages[0];

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    if (!ReadElement(root_element)) {
        wxLogError(_("An error occurred while importing '%s'"), m_file->m_fname.c_str() );
    }
    
    m_file->CheckIntegrity();

    // adjust the page size - this is very temporary, we need something better
    // we also have to see what to do if we have a scoredef
    // for now, it just adjust the page size for the content (GetMaxXY)
    wxArrayPtrVoid params; // tableau de pointeurs pour parametres
    MusStaffFunctor getMaxXY( &MusStaff::GetMaxXY );
    wxArrayPtrVoid staffParams; // idem for staff functor
    int max_x = -1;
    int max_y = 0;
    staffParams.Add( &max_x );
    staffParams.Add( &max_y );
    m_page->Process( &getMaxXY, staffParams );
    // change the staff width
    m_page->lrg_lign = max_x / 10;
    // add the space at the bottom - so far in max_y we have the top corner of the last staff
    if (m_page->m_staves.GetCount() > 0 ) {
        max_y += max((&m_page->m_staves.Last())->portNbLine,(&m_page->m_staves.Last())->ecart);
    }
    max_y *= m_page->defin; // transform this into coord.
    // update the page size
    m_file->m_fheader.param.pageFormatHor = max_x / 10 + (2 * m_file->m_fheader.param.MargeGAUCHEIMPAIRE);
    m_file->m_fheader.param.pageFormatVer = max_y / 10 + (2 * m_file->m_fheader.param.MargeSOMMET);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();    
    
    return true;
}

// method for reading mei elements

bool MusMeiInput::mei_accid( xmlNode *node ) {

    MusSymbol *symbol = new MusSymbol();
    symbol->flag = ALTER;
    symbol->oct = 4; // no pitch information in mei. It is randomly placed. This should be fixed...
    symbol->code = F4;
    
    // accid
    wxString accid;
    ReadAttributeString( node, "accid", &accid );
    if (accid == "n") symbol->calte = BECAR;
    else if (accid == "f") symbol->calte = BEMOL;
    else if (accid == "s") symbol->calte = DIESE;
    else if (accid == "ff") symbol->calte = D_BEMOL;
    else if (accid == "ss") symbol->calte = D_DIESE;

    m_currentStaff->Append( symbol );
    return true;
}

bool MusMeiInput::mei_barline( xmlNode *node ) {

    MusSymbol *symbol = new MusSymbol();
    symbol->flag = BARRE;
    symbol->code = '|';
    
    // rend
    wxString rend;
    ReadAttributeString( node, "rend", &rend );
    if (rend == "end") symbol->calte = 1;

    m_currentStaff->Append( symbol, 30 );
    return true;
}

bool MusMeiInput::mei_clefchange( xmlNode *node ) {

    MusSymbol *symbol = new MusSymbol();
    symbol->flag = CLE;
    
    // line
    wxString line;
    ReadAttributeString( node, "line", &line, "1" );
    
    // shape
    wxString shape;
    ReadAttributeString( node, "shape", &shape, "C" );
    
    if ((line == "1") && (shape == "G")) symbol->code = SOL1;
    else if ((line == "2") && (shape == "G")) symbol->code = SOLva;
    else if ((line == "2") && (shape == "C")) symbol->code = UT2;
    else if ((line == "2") && (shape == "G")) symbol->code = SOL2;
    else if ((line == "3") && (shape == "F")) symbol->code = FA3;
    else if ((line == "3") && (shape == "C")) symbol->code = UT3;
    else if ((line == "5") && (shape == "F")) symbol->code = FA5;
    else if ((line == "4") && (shape == "F")) symbol->code = FA4;
    else if ((line == "4") && (shape == "C")) symbol->code = UT4;
    else if ((line == "5") && (shape == "C")) symbol->code = UT5;
    else if ((line == "1") && (shape == "perc")) symbol->code = CLEPERC;

    m_currentStaff->Append( symbol );
    return true;
}

bool MusMeiInput::mei_custos( xmlNode *node ) {


    MusNote *note = new MusNote();
    note->sil = _SIL;
    note->val = CUSTOS;
    
    // pname
    mei_attr_pname(node, &note->code);

    // octave
    ReadAttributeInt( node, "oct", (int*)&note->oct, 3 );

    // insert at the end of the staff with a random step
    m_currentStaff->Append( note );
	return true;
}

bool MusMeiInput::mei_dot( xmlNode *node ) {

    MusSymbol *symbol = new MusSymbol();
    symbol->flag = PNT;
    symbol->oct = 4; // no pitch information in mei. It is randomly placed. This should be fixed...
    symbol->code = F4;

    m_currentStaff->Append( symbol, 25 );
    return true;
}

bool MusMeiInput::mei_layer( xmlNode *node ) {

    //wxLogDebug("mei:layer\n");
    if (m_currentStaff == NULL) { 
        // this might happen if we import a layer without a parent staff
        // in that case we must create a staff
        MusStaff *staff = new MusStaff();
        staff->invisible = true;
        staff->ecart = 1; // when we have just a layer, we decrease the top space
        m_file->m_fheader.param.MargeGAUCHEIMPAIRE = 2; // and the margin as well
        m_page->m_staves.Add( staff );
        m_currentStaff = &m_page->m_staves[0];
    }
    return true;
}

bool MusMeiInput::mei_measure( xmlNode *node ) {

    //wxLogDebug("mei:measure\n");
    return true;
}

bool MusMeiInput::mei_mensur( xmlNode *node ) {

    MusSymbol *symbol = new MusSymbol();
    symbol->flag = IND_MES;
    
    //sign
    wxString sign;
    ReadAttributeString( node, "sign", &sign, "" );
    
    // num
    int num;
    ReadAttributeInt( node, "num", &num, 0 );
    
    // numbase
    int numbase;
    ReadAttributeInt( node, "numbase", &numbase, 0 );
    
    // dot
    bool dot;
    ReadAttributeBool( node, "dot", &dot, false ); 
    
    // slash
    int slash;
    ReadAttributeInt( node, "slash", &slash, 0 );
    
    // slash and num without sign
    if (slash && num && (sign == "")) {
        symbol->code = 64;
        if ((num == 2) && !slash) symbol->calte = 2;
        if ((num == 2) && slash) symbol->calte = 3;
        if ((num == 3) && !slash) symbol->calte = 4;
        if ((num == 3) && slash) symbol->calte = 5;
    } else {
        if (sign == "O") symbol->code = 32;
        else if (sign == "C") symbol->code = 16;
        else if (sign == "C") symbol->code = 8; // this should be C inverse (flipped)
        if (slash) symbol->code += 4;
        if (dot) symbol->code += 2;
    }
    if (num && numbase) {
        symbol->code += 1; 
        symbol->durNum = num;
        symbol->durDen = numbase;
    }

    m_currentStaff->Append( symbol, 40 );
    return true;
}

bool MusMeiInput::mei_note( xmlNode *node ) {

    MusNote *note = new MusNote();
  
    // dur
    mei_attr_dur(node, &note->val);
    
    // pname
    mei_attr_pname(node, &note->code);

    // octave
    ReadAttributeInt( node, "oct", (int*)&note->oct, 4 );
    
    // coloration
    bool coloration;
    ReadAttributeBool( node, "colored", &coloration, false );
    if (coloration && note->val < BL) note->inv_val = true;
    else if (coloration && note->val > NR) note->oblique = true;

    // ligature obliqua
    wxString ligature;
    ReadAttributeString( node, "lig", &ligature, "" );
    if (ligature == "obliqua") note->oblique = true; // we assume the note->ligat is already set
   
    // TODO stem direction;

    ReadAttributeInt( node, "num", (int*)&note->durNum, 1 );
    ReadAttributeInt( node, "numbase", (int*)&note->durDen, 1 );            
   
    // insert at the end of the staff with a random step
    m_currentStaff->Append( note );
	return true;
}


bool MusMeiInput::mei_parts( xmlNode *node ) {

    //wxLogDebug("mei:parts\n");
    return true;
}


bool MusMeiInput::mei_rest( xmlNode *node ) {

    MusNote *note = new MusNote();
    note->sil = _SIL;
    note->oct = 4;
    note->code = F4; // temporary: E in on the staff with C1 clef
            
  
    // dur
    mei_attr_dur(node, &note->val);
    
    // insert at the end of the staff with a random step
    m_currentStaff->Append( note, 25 ); 
	return true;
}
	

bool MusMeiInput::mei_score( xmlNode *node ) {

    //wxLogDebug("mei:score\n");
    return true;
}

bool MusMeiInput::mei_staff( xmlNode *node ) {

    //wxLogDebug("mei:staff\n");
    int n;
    if (ReadAttributeInt( node, "n", &n )) {}
    wxLogDebug("mei:staff %d\n", n);
    if (n == (int)m_page->m_staves.Count() + 1) {
        // we tolerate that the staff has not been created in a scoredef
        // n must not be bigger that the previous id + 1;
        MusStaff *staff = new MusStaff();
        staff->no = n - 1;
        if (n == 1) {
            staff->ecart = 1; // for the first staff, we decrease the top space
            m_file->m_fheader.param.MargeGAUCHEIMPAIRE = 2; // and the margin as well
        }
        m_page->m_staves.Add( staff );
    }
    else if ((n > (int)m_page->m_staves.Count()) || (n < 1)) {
        wxLogError("Reading mei:staff - staff with @n = %d does not exist", n );
        return false;
    }
    m_currentStaff = &m_page->m_staves[n - 1];
    return true;
}

// attributes


void MusMeiInput::mei_attr_pname( xmlNode *node, unsigned short *code ) 
{
    wxString note_letter;
    ReadAttributeString( node, "pname", &note_letter, "c" );
    if (note_letter == "c") *code = F2;
    else if (note_letter == "d") *code = F3;
    else if (note_letter == "e") *code = F4;
    else if (note_letter == "f") *code = F5;
    else if (note_letter == "g") *code = F6;
    else if (note_letter == "a") *code = F7;
    else if (note_letter == "b") *code = F8;
}

 
void MusMeiInput::mei_attr_dur( xmlNode *node, unsigned char *val ) 
{
    wxString note_type;
    ReadAttributeString( node, "dur", &note_type, "" );
    if (note_type == "longa") *val = LG;
    else if (note_type == "brevis") *val = BR;
    else if (note_type == "semibrevis") *val = RD;
    else if (note_type == "minima") *val = BL;
    else if (note_type == "semiminima") *val = NR;
    else if (note_type == "fusa") *val = CR;
    else if (note_type == "semifusa") *val = DC;
    else if (note_type == "long") *val = LG;
    else if (note_type == "breve") *val = BR;
    else if (note_type == "1") *val = RD;
    else if (note_type == "2") *val = BL;
    else if (note_type == "4") *val = NR;
    else if (note_type == "8") *val = CR;
    else if (note_type == "16") *val = DC;
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



