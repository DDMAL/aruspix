/////////////////////////////////////////////////////////////////////////////
// Name:        muslayout.cpp
// Author:      Laurent Pugin
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "muslayout.h"
#include "muspage.h"
#include "mussystem.h"
#include "muslaidoutstaff.h"
#include "muslaidoutlayer.h"
#include "muslaidoutlayerelement.h"

#include "musdiv.h"
#include "mussection.h"
#include "musmeasure.h"
#include "musstaff.h"
#include "muslayer.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY( ArrayOfMusLayouts );

//----------------------------------------------------------------------------
// MusLayout
//----------------------------------------------------------------------------

MusLayout::MusLayout() :
	MusLayoutObject()
{
    Clear();
}

MusLayout::~MusLayout()
{
}

void MusLayout::Clear( )
{
	m_pages.Clear( );
}


void MusLayout::AddPage( MusPage *page )
{
	page->SetLayout( this );
	m_pages.Add( page );
}

void MusLayout::CheckIntegrity( )
{
	wxASSERT_MSG( m_doc, "MusDoc parent cannot be NULL");
	
	MusPage *page;
	int i;
    for (i = 0; i < this->GetPageCount(); i++) 
	{
		page = &m_pages[i];
        page->CheckIntegrity();
	}
	
}


void MusLayout::Realize( MusScore *score )
{
	Clear();
	
	wxASSERT( score );
	
	MusPage *page = new MusPage( );
	MusSystem *system = new MusSystem();
	
	int i, j, k, l, m;
	for (i = 0; i < (int)score->m_sections.GetCount(); i++) {
		MusSection *section = &score->m_sections[i];
		for (j = 0; j < (int)section->m_sectionElements.GetCount(); j++) {
			MusSectionInterface *sectionElement = &section->m_sectionElements[j];
			if (dynamic_cast<MusMeasure*> (sectionElement)) {
				MusMeasure* measure = dynamic_cast<MusMeasure*>(sectionElement);
				for (k = 0; k < (int)measure->m_measureElements.GetCount(); k++) {
					if (dynamic_cast<MusStaff*> (&measure->m_measureElements[k])) {
						MusStaff *staff = dynamic_cast<MusStaff*> (&measure->m_measureElements[k]);
						MusLaidOutStaff *laidOutStaff;
						if (k >= (int)system->m_staves.GetCount()) {
							system->AddStaff( new MusLaidOutStaff());
						}
						laidOutStaff = &system->m_staves[k];
						for (l = 0; l < (int)staff->m_staffElements.GetCount(); l++) {
							if (dynamic_cast<MusLayer*> (&staff->m_staffElements[l])) {
								MusLayer *layer = dynamic_cast<MusLayer*> (&staff->m_staffElements[l]);
								MusLaidOutLayer *laidOutLayer;
								if (l >= laidOutStaff->GetLayerCount()) {
									laidOutStaff->AddLayer( new MusLaidOutLayer());
								}
								laidOutLayer = &laidOutStaff->m_layers[l];
								for (m = 0; m < (int)layer->m_layerElements.GetCount(); m++) {
									MusLaidOutLayerElement *element = new MusLaidOutLayerElement( &layer->m_layerElements[m]);
									laidOutLayer->AddElement( element );
								}
							}	
						}			
					}
				}
			}
		}
	}

	page->AddSystem( system );
	this->AddPage( page );
	this->CheckIntegrity();
}

// functors for MusLayout

void MusLayout::Process(MusLayoutFunctor *functor, wxArrayPtrVoid params )
{
    MusPageFunctor *pageFunctor = dynamic_cast<MusPageFunctor*>(functor);
    MusPage *page;
	int i;
    for (i = 0; i < (int)m_pages.GetCount(); i++) 
	{
        page = &m_pages[i];
        if (pageFunctor) { // is is a MusSystemFunctor, call it
            functor->Call( page, params );
        }
        else { // process it further
            page->Process( functor, params );
        }
	}
}

