/////////////////////////////////////////////////////////////////////////////
// Name:        iomlfclass.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.   
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_WG

#ifdef __GNUG__
    #pragma implementation "iomlfclass.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"
#include "wx/textfile.h"
#include "wx/tokenzr.h"

#include "iomlfclass.h"
#include "im/impage.h"
#include "im/imstaff.h"

//----------------------------------------------------------------------------
// MLFClassInterval
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MLFClassInterval, MLFSymbole)

MLFClassInterval::MLFClassInterval( ) :
	MLFSymbole()
{
}

wxString MLFClassInterval::GetLabel( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbole") );
		return "";
	}

	wxString label;

	if ((m_type == TYPE_NOTE) || (m_type == TYPE_CUSTOS))
	{	
		unsigned int pitch = 0;
		switch (m_pitch)
		{
		case ('B'): pitch++;
		case ('A'): pitch++;
		case ('G'): pitch++;
		case ('F'): pitch++;
		case ('E'): pitch++;
		case ('D'): pitch++;
		case ('C'): break;
		}
		//label << (pitch + (m_oct*7)); //<< "_ " << m_pitch << "_" << m_oct;
		pitch += (m_oct*7);
		pitch += (m_value << MLF_INTERVAL_SHIFT);
		label << pitch;
	}
	return label;
}

//----------------------------------------------------------------------------
// MLFClassDuration
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MLFClassDuration, MLFSymbole)

MLFClassDuration::MLFClassDuration( ) :
	MLFSymbole()
{
}

wxString MLFClassDuration::GetLabel( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbole") );
		return "";
	}

	wxString label;

	if (m_type == TYPE_NOTE)
	{	
		label << "N" << "_" << m_value;
	}
	else if (m_type == TYPE_REST)
	{	
		label << "R" << "_" << m_value;
	}
	else if (m_type == TYPE_POINT)
	{
		label << "P"; //??
	}
	return label;
}


//----------------------------------------------------------------------------
// MLFClassPitch
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MLFClassPitch, MLFSymbole)

MLFClassPitch::MLFClassPitch( ) :
	MLFSymbole()
{
}

wxString MLFClassPitch::GetLabel( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbole") );
		return "";
	}

	wxString label;

	if ((m_type == TYPE_NOTE) || (m_type == TYPE_CUSTOS))
	{		
		label << m_pitch << "_" << m_oct;
	}
	/*else if (m_type == TYPE_REST)
	{	
		label << "R" << "_" << m_value;
	}
	else if (m_type == TYPE_POINT)
	{
		label << "P"; //??
	}*/
	return label;
}



//----------------------------------------------------------------------------
// MLFClassSymb
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(MLFClassSymb, MLFSymbole)

MLFClassSymb::MLFClassSymb( ) :
	MLFSymbole()
{
}

wxString MLFClassSymb::GetLabel( )
{
	if ( m_type == 0 )
	{
		wxLogWarning( _("Uninitialized symbole") );
		return "";
	}

	wxString label;

	if (m_type == TYPE_NOTE)
	{	
		label << "N" << "_" << m_value;
	}
	else if (m_type == TYPE_REST)
	{	
		label << "R" << "_" << m_value;
	}
	else if (m_type == TYPE_KEY)
	{
		label << "K";
	}
	else if  (m_type == TYPE_ALTERATION)
	{
		label << "A";
	}
	else if (m_type == TYPE_CUSTOS)
	{
		label << "C";
	}
	else if (m_type == TYPE_POINT)
	{
		label << "P";
	}
	else if (m_type == TYPE_MESURE)
	{
		label << "M";
	}
	else if (m_type == TYPE_SYMBOLE)
	{
		label << m_type << "_" << m_subtype;
	}
	return label;
}

#endif // AX_WG
