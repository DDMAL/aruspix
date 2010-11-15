/////////////////////////////////////////////////////////////////////////////
// Name:        musmlfdic.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////



#ifdef __GNUG__
    #pragma implementation "musmlfdic.h"
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

#include "musmlfdic.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMLFWords );

int SortMLFWords( MusMLFWord **first, MusMLFWord **second )
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


//----------------------------------------------------------------------------
// MusMLFDictionary
//----------------------------------------------------------------------------

void MusMLFDictionary::Reset()
{
	m_dict.Clear();
}

int MusMLFDictionary::Index( wxString label )
{
	int i;
	for (i = 0; i < (int)m_dict.GetCount(); i++)
		if ( m_dict[i].m_word == label )
			return i;
			
	return wxNOT_FOUND;
}


void MusMLFDictionary::Load( TiXmlElement *file_root )
{
	if ( !file_root )
		return;	
		
    TiXmlElement *elem = NULL;
	TiXmlNode *ndic = NULL;
	TiXmlNode *nentry = NULL;
	TiXmlNode *nhmm = NULL;
	
	ndic = file_root->FirstChild( "dictionary" );
	if ( !ndic )
	{
	  wxLogWarning("No dictionary found in the file");
	  return;
	}
			
	elem = ndic->ToElement();
    if ( !elem ) 
		return;

    // staves
    for( nentry = elem->FirstChild( "entry" ); nentry; nentry = nentry->NextSibling( "entry" ) )
    {
        elem = nentry->ToElement();
        if (!elem) 
			continue;
		
		MusMLFWord word;
		if (elem->Attribute("word"))
			word.m_word = elem->Attribute("word");
			
		for( nhmm = elem->FirstChild( "hmm" ); nhmm; nhmm = nhmm->NextSibling( "hmm" ) )
		{
			elem = nhmm->ToElement();
			if (!elem || !elem->Attribute("name") || !elem->Attribute("states")) 
				continue;
			
			word.m_hmms.Add( elem->Attribute("name") );
			word.m_states.Add( atoi(elem->Attribute("states")) );
		}
        m_dict.Add( word );
    }
}



void MusMLFDictionary::Save( TiXmlElement *file_root )
{
    wxString tmp;
	int i, j;

    TiXmlElement root("dictionary");	

    for( i = 0; i < (int)m_dict.GetCount(); i++ )
    {
        TiXmlElement elem ("entry");
		elem.SetAttribute("word", m_dict[i].m_word.c_str() );
		wxASSERT( m_dict[i].m_hmms.GetCount() == m_dict[i].m_states.GetCount() );
		for( j = 0; j < (int)m_dict[i].m_hmms.GetCount(); j++ )
		{
			TiXmlElement symb ("hmm");
			symb.SetAttribute("name", m_dict[i].m_hmms[j].c_str() );
			symb.SetAttribute("states",  wxString::Format("%d", m_dict[i].m_states[j] ).c_str() );
			elem.InsertEndChild( symb );
		}
		root.InsertEndChild( elem );
    }

	if ( file_root )
	    file_root->InsertEndChild( root );
}



void MusMLFDictionary::WriteDic( wxString filename )
{
	
	int i, j;
	wxFileOutputStream stream( filename );
	wxTextOutputStream fdic( stream );
	for( i = 0; i < (int)m_dict.GetCount(); i++ )
	{
	    fdic.WriteString( m_dict[i].m_word );
		for( j = 0; j < (int)m_dict[i].m_hmms.GetCount(); j++ )
			fdic.WriteString( wxString::Format( " %s", m_dict[i].m_hmms[j].c_str() ) );
		fdic.WriteString( wxString::Format( " {s}\n" ) );
	}
	fdic.WriteString( wxString::Format( "SP_START {s}\n" ) );
	fdic.WriteString( wxString::Format( "SP_END {s}\n" ) );
	stream.Close();
	
}

void MusMLFDictionary::WriteStates( wxString filename )
{
	
	int i, j;
	wxFileOutputStream stream( filename );
	wxTextOutputStream fstates( stream );
	for( i = 0; i < (int)m_dict.GetCount(); i++ )
		for( j = 0; j < (int)m_dict[i].m_states.GetCount(); j++ )
			fstates.WriteString( wxString::Format( "%02d\n", m_dict[i].m_states[j] ) );
	fstates.WriteString( wxString::Format( "%02d\n", 4 ) ); // {s} symbol
	stream.Close();
	
	// provisoire (idem but + 4...)
	wxFileOutputStream stream_3( filename + "3" );
	wxTextOutputStream fstates3( stream_3 );
	for( i = 0; i < (int)m_dict.GetCount(); i++ )
		for( j = 0; j < (int)m_dict[i].m_states.GetCount(); j++ )
			fstates3.WriteString( wxString::Format( "%02d\n", m_dict[i].m_states[j] + 4 ) );
	fstates3.WriteString( wxString::Format( "%02d\n", 4 ) );
	stream_3.Close();
	
	// v
	wxFileOutputStream stream_htk( filename + "_htk" );
	wxTextOutputStream fdic_htk( stream_htk );
	for( i = 0; i < (int)m_dict.GetCount(); i++ )
		for( j = 0; j < (int)m_dict[i].m_hmms.GetCount(); j++ )
			fdic_htk.WriteString( wxString::Format( "%s %02d\n", m_dict[i].m_hmms[j].c_str(), m_dict[i].m_states[j] ) );
	fdic_htk.WriteString( wxString::Format( "{s} %02d\n", 4 ) );
	stream_htk.Close();
	
}


void MusMLFDictionary::WriteHMMs( wxString filename )
{
	
	int i, j;
	wxFileOutputStream stream( filename );
	wxTextOutputStream fdic( stream );
	for( i = 0; i < (int)m_dict.GetCount(); i++ )
		for( j = 0; j < (int)m_dict[i].m_hmms.GetCount(); j++ )
			fdic.WriteString( wxString::Format( "%s\n", m_dict[i].m_hmms[j].c_str() ) );
	fdic.WriteString( wxString::Format( "{s}\n" ) );
	stream.Close();
}




