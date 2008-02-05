/////////////////////////////////////////////////////////////////////////////
// Name:        cmpmlf.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_RECOGNITION
		#ifdef AX_COMPARISON

#ifdef __GNUG__
    #pragma implementation "cmpmlf.h"
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

#include "cmpmlf.h"

#include "im/impage.h"
#include "im/imstaff.h"
#include "im/imstaffsegment.h"

//----------------------------------------------------------------------------
// CmpMLFSymb
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(CmpMLFSymb, MusMLFSymbol)

CmpMLFSymb::CmpMLFSymb( ) :
	MusMLFSymbol()
{
}

void CmpMLFSymb::SetValue( char type, wxString subtype, int position, int value, char pitch, int oct, int flag)
{
	if ( flag & NOTE_STEM )
		flag -= NOTE_STEM;
	MusMLFSymbol::SetValue( type, subtype, position, value, pitch, oct, flag );
}


wxString CmpMLFSymb::GetLabel( )
{
	if ( !m_cache.IsEmpty() )
		return m_cache;
		
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbole") );
		return "";
	}

	wxString label, label_width;
	label << m_type;

	if ((m_type == TYPE_NOTE) || (m_type == TYPE_REST))
	{	
		label << "_" << m_value; 
		if (m_type == TYPE_NOTE) // ignore pitch and oct with rest for comparison 	
			label << "_" << m_pitch << "_" << m_oct;
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
	
	m_cache = label;
	return label;
}

//----------------------------------------------------------------------------
// CmpMLFOutput
//----------------------------------------------------------------------------

CmpMLFOutput::CmpMLFOutput( MusFile *file, wxString filename, wxString model_symbole_name ) :
    MusMLFOutput(  file, filename, model_symbole_name  )
{
	// temporary, cannot be modified, but should be...
	m_ignore_clefs = true;
	m_ignore_custos = true;
	m_ignore_keys = true;
}

//CmpMLFOutput::CmpMLFOutput( MusFile *file, wxFile *wxfile, wxString filename, wxString model_symbole_name ) :
//    MusFileOutputStream( file, wxfile )
CmpMLFOutput::CmpMLFOutput( MusFile *file, int fd, wxString filename, wxString model_symbole_name ) :
	MusMLFOutput( file, fd, filename, model_symbole_name )
{
	// idem previous constructor
	m_ignore_clefs = true;
	m_ignore_custos = true;
	m_ignore_keys = true;
}

CmpMLFOutput::~CmpMLFOutput()
{
}

void CmpMLFOutput::StartLabel( )
{
	m_symboles.Clear();
}

void CmpMLFOutput::EndLabel( )
{
	int pos;

	for (int i = 0; i < (int)m_symboles.GetCount(); i++ )
	{
		CmpMLFSymb *current = (CmpMLFSymb*)&m_symboles[i];
		wxString symbole;
		wxString symbole_label = current->GetLabel();
		if ( symbole_label == "" ) // skip empty labels
			continue;
		pos = current->GetPosition();
		symbole << pos  << " ";
		symbole << symbole_label << " " <<  current->m_im_staff << " " <<  current->m_im_staff_segment <<  " " << current->m_im_pos << " " << current->m_im_filename << "\n";
		Write( symbole, symbole.Length() );
	}
	//m_symboles.Clear();	
}

bool CmpMLFOutput::WriteStaff( const MusStaff *staff )
{
	if (staff->nblement == 0)
		return true;

    unsigned int k;
	bool added;

	StartLabel();
	
	MusSymbol *clef = NULL;
	bool last_is_clef = false;

    for (k = 0;k < staff->nblement ; k++ )
    {
		added = false;
        if ( staff->m_elements[k].TYPE == NOTE )
		{
			last_is_clef = false;

			bool ignore = false;
			MusNote *cur = (MusNote*)&staff->m_elements[k];
			if (cur->val == CUSTOS)			
			{
				if ( m_ignore_custos ) // not safe because will ignore any alteration after a clef, even if not part of a key ...
					ignore = true;
			}
			
			if (!ignore)
				added = WriteNote( (MusNote*)&staff->m_elements[k] );
			else
				cur->m_cmp_flag = CMP_MATCH;
        }
		else
		{
			bool ignore = false;
			MusSymbol *cur = (MusSymbol*)&staff->m_elements[k];
			if (cur->flag == CLE)
			{
				last_is_clef = true;
				if (!clef || clef->code != cur->code) // first clef or new clef, we keep it
					clef = cur;
				else if (m_ignore_clefs)
					ignore = true;
			}
			else if (cur->flag == ALTER)
			{
				if ( last_is_clef && m_ignore_keys ) // not safe because will ignore any alteration after a clef, even if not part of a key ...
					ignore = true;
			}
			else
				last_is_clef = false;
			
			if (!ignore)
				added = WriteSymbole( (MusSymbol*)&staff->m_elements[k] );
			else
				cur->m_cmp_flag = CMP_MATCH;
		}
		if ( added )
		{	
			((CmpMLFSymb*)&m_symboles.Last())->m_im_filename = (&staff->m_elements[k])->m_im_filename;
			((CmpMLFSymb*)&m_symboles.Last())->m_im_staff = (&staff->m_elements[k])->m_im_staff;
			((CmpMLFSymb*)&m_symboles.Last())->m_im_staff_segment = (&staff->m_elements[k])->m_im_staff_segment;
			((CmpMLFSymb*)&m_symboles.Last())->m_im_pos = (&staff->m_elements[k])->m_im_pos;
			((CmpMLFSymb*)&m_symboles.Last())->m_index = k;
		}
    }
	//EndLabel( );

    return true;
}

// WDR: handler implementations for CmpMLFOutput


//----------------------------------------------------------------------------
// CmpMLFInput
//----------------------------------------------------------------------------

CmpMLFInput::CmpMLFInput( MusFile *file, wxString filename ) :
    MusMLFInput( file, filename )
{
}

CmpMLFInput::~CmpMLFInput()
{
}
	
bool CmpMLFInput::ReadLabelStr( wxString label )
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
	
	// filename
	str = str.BeforeLast('_');
	m_cmp_page_label = str.AfterLast('/');

	return true;
}


MusStaff* CmpMLFInput::ImportFileInStaff( )
{
	wxString line;
	if ( !ReadLine( &line )  || (line != "#!MLF!#" ))
		return NULL ;
		
	m_cmp_pos = 50;

	MusStaff *staff = new MusStaff();
	while ( ReadLine( &line ) && ReadLabelStr( line ) )
	{
		ReadLabel( staff );
	}
	MusStaff *staff_ut1 = GetNotUt1( staff );
	delete staff;
	staff_ut1->CheckIntegrity();
	return staff_ut1;
}


// offset est la position x relative du label (p ex segment)
// normalement donne par imPage si present

bool CmpMLFInput::ReadLabel( MusStaff *staff )
{
	bool is_note;
	int pos = 0;
	wxString line;

	while ( ReadLine( &line ) &&  CmpMLFInput::IsElement( &is_note, &line, &pos ) )
	{
		MusElement *e = NULL;
		if ( !is_note )
		{
			MusSymbol *s = CmpMLFInput::ConvertSymbole( line );
			if ( s )
			{
				e = s;
				s->xrel = m_cmp_pos;
				staff->m_elements.Add( s );
			}
		}
		else
		{
			MusNote *n = CmpMLFInput::ConvertNote( line );
			if ( n )
			{
				e = n;
				n->xrel = m_cmp_pos;
				staff->m_elements.Add( n );
			}
		}
		
		if ( e )
		{
			e->m_im_filename = m_cmp_page_label;
			e->m_im_staff = m_staff_label;
			e->m_im_staff_segment = m_segment_label;
			e->m_im_pos = pos;
		}
		m_cmp_pos += 45; // default step;
	}       
    return true;
}

// WDR: handler implementations for CmpMLFInput

		#endif // AX_COMPARISON
#endif // AX_RECOGNITION
