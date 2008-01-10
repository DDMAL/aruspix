/////////////////////////////////////////////////////////////////////////////
// Name:        wgpage.cpp
// Author:      Laurent Pugin
// Created:     2005
// Copyright (c) Laurent Pugin. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef AX_WG

#ifdef __GNUG__
    #pragma implementation "muspage.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


#include "muspage.h"
#include "musstaff.h"
#include "muselement.h"
#include "mussymbol.h"
#include "musnote.h"
#include "muswindow.h"
#include "mus.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfWgStaves );


// WDR: class implementations

//----------------------------------------------------------------------------
// WgPage
//----------------------------------------------------------------------------

WgPage::WgPage() :
	WgObject()
{
	Clear( );
}

WgPage::~WgPage()
{
}

void WgPage::Clear( )
{
	m_staves.Clear( );
	nbrePortees = 0;
	noMasqueFixe = false;
	noMasqueVar = false;
	reserve = 0;
	defin = 20;
	indent = 0;
	indentDroite = 0;
	lrg_lign = 190;
	nbrePortees = 0;
	npage = 0;
}

void WgPage::CheckIntegrity()
{
	this->nbrePortees = (int)this->m_staves.GetCount();

	WgStaff *staff;
	int i;
	int system = 1;
    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
		staff->no = i;
		staff->noLigne = system;
		if ((staff->vertBarre == 0) || (staff->vertBarre == FIN))
			system++;
        staff->CheckIntegrity();
	}
}


void WgPage::ClearStaves( wxDC *dc, WgStaff *start )
{
	wxASSERT_MSG( dc , "DC cannot be NULL");
	if ( !Check() )
		return;

	int j;
	for(j = 0; j < nbrePortees; j++)
	{
		if (start && (start != &this->m_staves[j]))
			continue;
		else
			start = NULL;
		(&this->m_staves[j])->ClearElements( dc );
	}
}

WgStaff *WgPage::GetFirst( )
{
	if ( m_staves.IsEmpty() )
		return NULL;
	return &m_staves[0];
}

WgStaff *WgPage::GetLast( )
{
	if ( m_staves.IsEmpty() )
		return NULL;
	int i = (int)m_staves.GetCount() - 1;
	return &m_staves[i];
}

WgStaff *WgPage::GetNext( WgStaff *staff )
{
	if ( !staff || m_staves.IsEmpty() || ( staff->no >= (int)m_staves.GetCount() - 1 ) )
		return NULL;

	//wxASSERT_MSG( m_staves.Index( *staff) != wxNOT_FOUND, 
	//	"\nLa portee recherchee n'appartient pas à la page" );

	return &m_staves[staff->no + 1];
}

WgStaff *WgPage::GetPrevious( WgStaff *staff  )
{
	if ( !staff || m_staves.IsEmpty() || ( staff->no <= 0 ) )
		return NULL;

	//wxASSERT_MSG( m_staves.Index( *staff) != wxNOT_FOUND, 
	//	"\nLa portee recherchee n'appartient pas à la page" );

	return &m_staves[staff->no - 1];
}


WgStaff *WgPage::GetAtPos( int y )
{
	WgStaff *staff = this->GetFirst();
	if ( !staff )
		return NULL;
	
	while ( this->GetNext(staff) )
	{
		if ( (int)staff->yrel < y )
		{
			// one too much
			if ( this->GetPrevious( staff ) )
				staff = this->GetPrevious( staff );
			return staff;
		}
		staff = this->GetNext( staff );
	}

	if ( ( (int)staff->yrel < y )  && this->GetPrevious( staff ) )
		staff = this->GetPrevious( staff );

	return staff;
}

void WgPage::UpdateStavesPosition( ) 
{
	if ( !Check() )
		return;

	int i, yy, orgx;
	WgStaff *staff = NULL;

	yy = m_w->wymax;
    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
        yy -= staff->ecart * m_w->_interl[ staff->pTaille ];
        m_w->kPos[i].compte = 0;

		// Calcul du TAB initial, s'il y a lieu 
		orgx = staff->indent ? staff->indent*10 : 0;
         
		// calcul du point d'ancrage des curseurs au-dessus de la ligne superieure
		m_w->kPos[i].yp = yy + m_w->_portee[ staff->pTaille ];
		staff->yrel = (int)(m_w->kPos[i].yp);
        // portees à 1 ou 4 lignes
        if (staff->portNbLine == 1)
			m_w->kPos[i].yp  += m_w->_interl[ staff->pTaille ]*2;
        else if (staff->portNbLine == 4)
			m_w->kPos[i].yp  += m_w->_interl[ staff->pTaille ];		
	}
}

void WgPage::DrawPage( wxDC *dc, bool background ) 
{
	wxASSERT_MSG( dc , "DC cannot be NULL");

	if ( !Check() )
		return;

	int i;
    WgStaff *staff;

	if (m_p->orientation)
		i =  m_w->ToZoom( m_w->pageFormatHor ); //+mrgG; marge comprise dans SetDeviceOrigine
	else
		i =  m_w->ToZoom( m_w->pageFormatHor + 50); //+mrgG;

	if ( background )
		dc->DrawRectangle( m_w->mrgG, m_w->ToZoomY(( m_w->pageFormatVer-50) + m_p->MargeSOMMET*10), i,
			m_w->ToZoomY (-50)+m_p->MargeSOMMET*10);


	// position des portees
	UpdateStavesPosition();
	// barres
	DrawBarres( dc );

    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
		staff->Init( m_w );
		//wxLogDebug("staff %d yrel=%d", i, staff->yrel);
		//if (( m_w->drawRect.y > (int)staff->yrel) && ( m_w->drawRect.GetBottom() < (int)staff->yrel) )
			staff->DrawStaffLines( dc , i );		
	}

    for (i = 0; i < nbrePortees; i++) 
	{
		staff = &m_staves[i];
			staff->DrawStaff( dc , i );		
	}
}

// WDR: handler implementations for WgPage

#endif // AX_WG
