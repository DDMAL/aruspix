/////////////////////////////////////////////////////////////////////////////
// Name:        musrc_neumes.cpp
// Author:      Laurent Pugin and Chris Niven
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "musrc.h"
#include "muslayout.h"
#include "muslaidoutlayerelement.h"

#include "musneume.h"
#include "musclef.h"
#include "musneumesymbol.h"

//----------------------------------------------------------------------------
// MusRC - MusNeume
//----------------------------------------------------------------------------

void MusRC::DrawNeume( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff)
{
    wxASSERT_MSG( layer, "Pointer to layer cannot be NULL" );
    wxASSERT_MSG( staff, "Pointer to staff cannot be NULL" );
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );
    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);
    
    int oct = neume->m_oct - 4; //for some reason, notes are automatically drawn 4 octaves too high?
    
    element->m_y_abs = CalculateNeumePosY(staff, neume->m_pname, layer->GetClefOffset( element ), oct);
	for (vector<MusNeumeElement>::iterator i = neume->m_pitches.begin(); i != neume->m_pitches.end(); i++) {
		i->m_y_abs = CalculateNeumePosY(staff, neume->m_pname + i->getPitchDifference(), layer->GetClefOffset(element), oct);
	}
    switch (neume->getType()) {
        case (NEUME_TYPE_PUNCTUM): DrawPunctum(dc, element, layer, staff); break;
        case (NEUME_TYPE_PUNCTUM_INCLINATUM): DrawPunctumInclinatum(dc, element, layer, staff); break;
        case (NEUME_TYPE_VIRGA): DrawVirga(dc, element, layer, staff); break;
        //case (NEUME_TYPE_VIRGA_LIQUESCENT): DrawVirgaLiquescent(dc, element, layer, staff); break;
        case (NEUME_TYPE_PODATUS): DrawPodatus(dc, element, layer, staff); break;
        case (NEUME_TYPE_CLIVIS): DrawClivis(dc, element, layer, staff); break;
        case (NEUME_TYPE_PORRECTUS): DrawPorrectus(dc, element, layer, staff); break;
        case (NEUME_TYPE_PORRECTUS_FLEXUS): DrawPorrectusFlexus(dc, element, layer, staff); break;
        case (NEUME_TYPE_SCANDICUS): DrawScandicus(dc, element, layer, staff); break;
        case (NEUME_TYPE_SCANDICUS_FLEXUS): DrawScandicusFlexus(dc, element, layer, staff); break;
        case (NEUME_TYPE_TORCULUS): DrawTorculus(dc, element, layer, staff); break;
        case (NEUME_TYPE_TORCULUS_LIQUESCENT): DrawTorculusLiquescent(dc, element, layer, staff); break;
        case (NEUME_TYPE_TORCULUS_RESUPINUS): DrawTorculusResupinus(dc, element, layer, staff); break;
        case (NEUME_TYPE_COMPOUND): DrawCompound(dc, element, layer, staff); break;
        case (NEUME_TYPE_ANCUS): DrawAncus(dc, element, layer, staff); break;
        case (NEUME_TYPE_CEPHALICUS): DrawCephalicus(dc, element, layer, staff); break;
        case (NEUME_TYPE_EPIPHONUS): DrawEpiphonus(dc, element, layer, staff); break;
        case (NEUME_TYPE_SALICUS): DrawSalicus(dc, element, layer, staff); break;
        case (NEUME_TYPE_CUSTOS): DrawCustos(dc, element, layer, staff); break;
        default: break;
    }
	
	DrawNeumeDots(dc, element, layer, staff);
    
    m_currentColour = AxBLACK;
    return;
}

void MusRC::NeumeLine( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff, int x1, int x2, int y1, int y2)
{
    dc->SetPen( m_currentColour, ToRendererX( m_layout->m_env.m_staffLineWidth ), wxSOLID );
    dc->SetBrush(m_currentColour , wxTRANSPARENT );
    dc->DrawLine( ToRendererX(x1) , ToRendererY (y1) , ToRendererX(x2) , ToRendererY (y2) );
    dc->ResetPen();
    dc->ResetBrush();
}

void MusRC::DrawAncus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff )
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, '3', staff, neume->m_cueSize);
    festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    xn += CLIVIS_X_DIFF;
	neume->m_pitches.at(1).m_x_abs = xn;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn2,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn2 + 19, nCEPHALICUS, staff, neume->m_cueSize); //notehead : 'K'
    int ynn3 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(2)).getPitchDifference());
	neume->m_pitches.at(2).m_x_abs = xn;
    festa_string( dc, xn, ynn3 + 19, nLIQUES_UP, staff, neume->m_cueSize); //liquescent: 'e'
}

void MusRC::DrawCustos( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff )
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);
    
    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nNEXT_PITCH, staff, neume->m_cueSize);
}

void MusRC::DrawEpiphonus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff )
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    //podatus_ep for first note
    //liques_down for second
    
    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPODATUS_EP, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
	neume->m_pitches.at(1).m_x_abs = xn;
    festa_string( dc, xn, ynn2 + 19, nLIQUES_DOWN, staff, neume->m_cueSize);
}

void MusRC::DrawCephalicus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff )
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    
    //stem: '3'
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, '3', staff, neume->m_cueSize);
    festa_string( dc, xn, ynn + 19, nCEPHALICUS, staff, neume->m_cueSize); //notehead : 'K'
	neume->m_pitches.at(0).m_x_abs = xn;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
	neume->m_pitches.at(1).m_x_abs = xn;
    festa_string( dc, xn, ynn2 + 19, nLIQUES_UP, staff, neume->m_cueSize); //liquescent: 'e'
}

void MusRC::DrawPunctum( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff ) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);
    
    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize); //draw ledger lines
    festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, neume->m_cueSize); //draw punctum
	neume->m_pitches.at(0).m_x_abs = xn;
}

void MusRC::DrawPunctumInclinatum( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff ) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize); //draw ledger lines
    festa_string( dc, xn, ynn + 19, nDIAMOND, staff, neume->m_cueSize); //draw punctum inclinatum
	neume->m_pitches.at(0).m_x_abs = xn;
}

void MusRC::DrawVirga( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff ) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    festa_string( dc, xn + PUNCT_WIDTH, ynn + 19, '#', staff, neume->m_cueSize); //bottom right stem
}

void MusRC::DrawSalicus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff )
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    xn += CLIVIS_X_SAME;
    DrawNeumeLedgerLines( dc, ynn2,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn2 + 19, nPES, staff, neume->m_cueSize);
	neume->m_pitches.at(1).m_x_abs = xn;
    int ynn3 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(2)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn3,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(2).m_x_abs = xn;
    this->NeumeLine( dc, element, layer, staff, xn + 9, xn + 9, ynn2, ynn3);
}

void MusRC::DrawPodatus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff ) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPES, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn2,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(1).m_x_abs = xn;
    this->NeumeLine( dc, element, layer, staff, xn + 9, xn + 9, ynn, ynn2);
}

void MusRC::DrawClivis( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff ) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    int ynn2, dx;
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    festa_string( dc, xn, ynn + 19, '3', staff, neume->m_cueSize);
    ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    if (ynn2 == ynn) {
        dx = CLIVIS_X_SAME;
    } else dx = CLIVIS_X_DIFF;
    DrawNeumeLedgerLines( dc, ynn2,bby,element->m_x_abs + dx - 1,ledge, staffSize);
    festa_string( dc, xn + dx - 1, ynn2 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(1).m_x_abs = xn + dx - 1;
	if (ynn2 != ynn) {
	NeumeLine(dc, element, layer, staff, xn+dx, xn+dx, ynn, ynn2);
	}
}

void MusRC::DrawPorrectus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff ) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, '3', staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    wxString slope; int dx;
    if ((neume->m_pitches.at(1)).getPitchDifference()==-1) {
        slope = nPORRECT_1;
        dx = -5;
    }
    else if ((neume->m_pitches.at(1)).getPitchDifference()==-2) {
        slope = nPORRECT_2;
        dx = -2;
    }
    else if ((neume->m_pitches.at(1)).getPitchDifference()==-3) {
        slope = nPORRECT_3;
        dx = 3;
    }
    else if ((neume->m_pitches.at(1)).getPitchDifference()==-4) {
        slope = nPORRECT_4;
        dx = 3;
    }
    festa_string( dc, xn, ynn + 19, slope, staff, neume->m_cueSize);
    xn += 3*PUNCT_WIDTH + dx;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(2)).getPitchDifference());
	neume->m_pitches.at(2).m_x_abs = xn;
    DrawNeumeLedgerLines( dc, ynn,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, neume->m_cueSize);
    xn += PUNCT_WIDTH - 1;
	//no assignment of x_abs to the second pitch as the bottom of a porrectus never has a dot.
    int ynn3 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    this->NeumeLine( dc, element, layer, staff, xn, xn, ynn2, ynn3);
}

void MusRC::DrawPorrectusFlexus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff)
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;    
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    festa_string( dc, xn, ynn + 19, '3', staff, neume->m_cueSize);
    wxString slope; int dx;
    if ((neume->m_pitches.at(1)).getPitchDifference()==-1) {
        slope = nPORRECT_1;
        dx = -5;
    }
    else if ((neume->m_pitches.at(1)).getPitchDifference()==-2) {
        slope = nPORRECT_2;
        dx = -2;
    }
    else if ((neume->m_pitches.at(1)).getPitchDifference()==-3) {
        slope = nPORRECT_3;
        dx = 3;
    }
    else if ((neume->m_pitches.at(1)).getPitchDifference()==-4) {
        slope = nPORRECT_4;
        dx = 3;
    }
    festa_string( dc, xn, ynn + 19, slope, staff, neume->m_cueSize);
    xn += 4*PUNCT_WIDTH + dx;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(2)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn2,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(2).m_x_abs = xn;
    xn += PUNCT_WIDTH - 1;
    int ynn3 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(3)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn3,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(3).m_x_abs = xn;
    this->NeumeLine( dc, element, layer, staff, xn + 1, xn, ynn2, ynn3);
}

void MusRC::DrawScandicus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff ) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    xn += PUNCT_WIDTH;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn2,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(1).m_x_abs = xn;
    xn += PUNCT_WIDTH;
    int ynn3 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(2)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn3,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(2).m_x_abs = xn;
}

void MusRC::DrawScandicusFlexus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff )
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPES, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn2,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(1).m_x_abs = xn;
    this->NeumeLine( dc, element, layer, staff, xn + 9, xn + 9, ynn, ynn2);
    xn += CLIVIS_X_SAME;
    int ynn3 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(2)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn3,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn3 + 19, '3', staff, neume->m_cueSize);
    festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(2).m_x_abs = xn;
    xn += PUNCT_WIDTH - 2;
    int ynn4 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(3)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn4,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn4 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(3).m_x_abs = xn;
    this->NeumeLine( dc, element, layer, staff, xn + 1, xn + 1, ynn3, ynn4);
}

void MusRC::DrawTorculus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff ) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    this->NeumeLine( dc, element, layer, staff, xn + 9, xn + 9, ynn, ynn2);
    xn += PUNCT_WIDTH - 2;
    DrawNeumeLedgerLines( dc, ynn2,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn2 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(1).m_x_abs = xn;
    int ynn3 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(2)).getPitchDifference());
    xn += PUNCT_WIDTH - 2;
    this->NeumeLine( dc, element, layer, staff, xn + 1, xn + 1, ynn2, ynn3);
    DrawNeumeLedgerLines( dc, ynn3,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(2).m_x_abs = xn;
}

void MusRC::DrawTorculusLiquescent( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff )
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    this->NeumeLine( dc, element, layer, staff, xn + 9, xn + 9, ynn, ynn2);
    xn += PUNCT_WIDTH - 2;
    DrawNeumeLedgerLines( dc, ynn2,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn2 + 19, nCEPHALICUS, staff, neume->m_cueSize); //notehead : 'K'
	neume->m_pitches.at(1).m_x_abs = xn;
    int ynn3 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(2)).getPitchDifference());
    festa_string( dc, xn, ynn3 + 19, nLIQUES_UP, staff, neume->m_cueSize); //liquescent: 'e'
	neume->m_pitches.at(2).m_x_abs = xn;
}

void MusRC::DrawTorculusResupinus( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff )
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPES, staff, neume->m_cueSize);
	neume->m_pitches.at(0).m_x_abs = xn;
    
    xn += PUNCT_WIDTH;
    int ynn2 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(1)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn2,bby,element->m_x_abs,ledge, staffSize);
    wxString slope; int dx;
    if (((neume->m_pitches.at(1)).getPitchDifference())-((neume->m_pitches.at(2)).getPitchDifference())==1) {
        slope = nPORRECT_1;
        dx = -5;
    }
    else if (((neume->m_pitches.at(1)).getPitchDifference())-((neume->m_pitches.at(2)).getPitchDifference())==2) {
        slope = nPORRECT_2;
        dx = -2;
    }
    else if (((neume->m_pitches.at(1)).getPitchDifference())-((neume->m_pitches.at(2)).getPitchDifference())==3) {
        slope = nPORRECT_3;
        dx = 3;
    }
    else if (((neume->m_pitches.at(1)).getPitchDifference())-((neume->m_pitches.at(2)).getPitchDifference())==4) {
        slope = nPORRECT_4;
        dx = 3;
    }
    festa_string( dc, xn, ynn2 + 19, slope, staff, neume->m_cueSize);
    xn += 3*PUNCT_WIDTH + dx;
	neume->m_pitches.at(1).m_x_abs = xn;
    int ynn3 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(3)).getPitchDifference());
    DrawNeumeLedgerLines( dc, ynn3,bby,xn,ledge, staffSize);
    festa_string( dc, xn, ynn3 + 19, nPUNCTUM, staff, neume->m_cueSize);
	neume->m_pitches.at(3).m_x_abs = xn;
	neume->m_pitches.at(2).m_x_abs = xn;
    xn += PUNCT_WIDTH - 1;
    int ynn4 = ynn + (m_layout->m_halfInterl[staffSize])*((neume->m_pitches.at(2)).getPitchDifference());
    this->NeumeLine( dc, element, layer, staff, xn, xn, ynn3, ynn4);
}

void MusRC::DrawCompound( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff ) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);

    // magic happens here
    int staffSize = staff->staffSize;
    
    int xn = element->m_x_abs;
    //int xl = element->m_x_abs;
    int bby = staff->m_y_abs - m_layout->m_staffSize[staffSize];
    int ynn = element->m_y_abs + staff->m_y_abs;
    //printf("closed ynn value: %d\nclosed m_y_abs: %d\nclosed m_y_abs: %d\n", 
    //     ynn, element->m_y_abs, staff->m_y_abs );
    
    xn += neume->m_hOffset;
    
    int ledge = m_layout->m_ledgerLine[staffSize][2];
    DrawNeumeLedgerLines( dc, ynn,bby,element->m_x_abs,ledge, staffSize);
    festa_string( dc, xn, ynn + 19, nPUNCTUM, staff, neume->m_cueSize);
    int dx = (PUNCT_WIDTH + 1)*(neume->m_pitches.size());
    int y1 = 0, y2 = 0;
    for (vector<MusNeumeElement>::iterator i = neume->m_pitches.begin() + 1; i != neume->m_pitches.end(); i++) {
        MusNeumeElement e = *i;
        if (e.getPitchDifference() > y1)
            y1 = e.getPitchDifference();
        if (e.getPitchDifference() < y2)
            y2 = e.getPitchDifference();
    }
    y1 = ynn + (m_layout->m_halfInterl[staffSize])*y1;
    y2 = ynn + (m_layout->m_halfInterl[staffSize])*y2;
    box( dc, xn, y1, xn + dx, y2 );
}

void MusRC::DrawNeumeDots(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff) 
{
    wxASSERT_MSG( dynamic_cast<MusNeume*>(element->m_layerElement), "Element must be a MusNeume" );    
    MusNeume *neume = dynamic_cast<MusNeume*>(element->m_layerElement);
    
	for (vector<MusNeumeElement>::iterator i = neume->m_pitches.begin(); i != neume->m_pitches.end(); i++) {
		if (i->getOrnament() == DOT) {
			bool onspace = (i->m_y_abs/(m_layout->m_halfInterl[staff->staffSize]))%2;
			int y = staff->m_y_abs + i->m_y_abs + m_layout->m_interl[staff->staffSize];
			y += (!onspace) ? (m_layout->m_halfInterl[staff->staffSize]) : 0;
			bool noteabove = false;
			for (vector<MusNeumeElement>::iterator it = neume->m_pitches.begin(); it != neume->m_pitches.end(); it++) {
				if (!onspace && (it->getPitchDifference() - i->getPitchDifference() == 1)) {
					noteabove = true;
				}
			}
			y -= (noteabove) ? (m_layout->m_interl[staff->staffSize]) : 0;
			festa_string( dc, neume->m_pitches.back().m_x_abs + PUNCT_WIDTH, y + 19, nDOT, staff, neume->m_cueSize);
		}
	}
}

//Also same as MusNote1. Could use an update, since it fails to draw ledger lines immediately below the staff.
void MusRC::DrawNeumeLedgerLines( MusDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int staffSize)
{
    int yn, ynt, yh, yb, test, v_decal = m_layout->m_interl[staffSize];
    int dist, xng, xnd;
    register int i;
    
    
    yh = y_p + m_layout->m_halfInterl[staffSize]; yb = y_p- m_layout->m_staffSize[staffSize]- m_layout->m_halfInterl[staffSize];
    
    if (!in(y_n,yh,yb))                           // note hors-portee?
    {
        xng = xn - smaller;
        xnd = xn + smaller;
        
        dist = ((y_n > yh) ? (y_n - y_p) : y_p - m_layout->m_staffSize[staffSize] - y_n);
        ynt = ((dist % m_layout->m_interl[staffSize] > 0) ? (dist - m_layout->m_halfInterl[staffSize]) : dist);
        test = ynt/ m_layout->m_interl[staffSize];
        if (y_n > yh)
        {   yn = ynt + y_p;
            v_decal = - m_layout->m_interl[staffSize];
        }
        else
            yn = y_p - m_layout->m_staffSize[staffSize] - ynt;
        
        //hPen = (HPEN)SelectObject (hdc, CreatePen (PS_SOLID, _param.EpLignesPORTEE+1, workColor2));
        //xng = toZoom(xng);
        //xnd = toZoom(xnd);
        
        dc->SetPen( m_currentColour, ToRendererX( m_layout->m_env.m_staffLineWidth ), wxSOLID );
        dc->SetBrush(m_currentColour , wxTRANSPARENT );
        
        for (i = 0; i < test; i++)
        {
            dc->DrawLine( ToRendererX(xng) , ToRendererY ( yn ) , ToRendererX(xnd) , ToRendererY ( yn ) );
            //h_line ( dc, xng, xnd, yn, _param.EpLignesPORTEE);
            //yh =  toZoom(yn);
            //MoveToEx (hdc, xng, yh, NULL);
            //LineTo (hdc, xnd, yh);
            
            yn += v_decal;
        }
        
        dc->ResetPen();
        dc->ResetBrush();
    }
    return;
}


//----------------------------------------------------------------------------
// MusRC - MusNeumeSymbol
//----------------------------------------------------------------------------

void MusRC::DrawNeumeSymbol( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff)
{
    wxASSERT_MSG( layer, "Pointer to layer cannot be NULL" );
    wxASSERT_MSG( staff, "Pointer to staff cannot be NULL" );
    wxASSERT_MSG( dynamic_cast<MusNeumeSymbol*>(element->m_layerElement), "Element must be a MusNeumeSymbol" );	
    MusNeumeSymbol *symbol = dynamic_cast<MusNeumeSymbol*>(element->m_layerElement);   
	
    dc->StartGraphic( element, "neumeSymbol", wxString::Format("neumeSymbol_%d_%d_%d", staff->GetId(), layer->voix, element->GetId() ) );
	
	int x = element->m_x_abs + symbol->m_hOffset;
	
	int oct = symbol->m_oct - 4;
	
	if ((symbol->getType() == NEUME_SYMB_FLAT) || (symbol->getType() == NEUME_SYMB_NATURAL)) {
		element->m_y_abs = CalculateNeumePosY(staff, symbol->m_pname, layer->GetClefOffset(element), oct) + m_layout->m_interl[staff->staffSize];
	}
	else if ((symbol->getType() == NEUME_SYMB_COMMA) || (symbol->getType() == NEUME_SYMB_DIVISION_FINAL) || (symbol->getType() == NEUME_SYMB_DIVISION_MAJOR)
			 || (symbol->getType() == NEUME_SYMB_DIVISION_MINOR) || (symbol->getType() == NEUME_SYMB_DIVISION_SMALL))
	{
		element->m_y_abs = -m_layout->m_staffSize[staff->staffSize] - m_layout->m_interl[staff->staffSize]*2;
	}
	switch (symbol->getType())
	{
		case NEUME_SYMB_CLEF_C:
		case NEUME_SYMB_CLEF_F:
		symbol->calcoffs (&x,(int)symbol->getValue());
		element->m_y_abs = x;
		DrawNeumeClef( dc, element, layer, staff); 
		break;
		case NEUME_SYMB_COMMA: DrawComma(dc, element, staff, symbol->m_cueSize); break;
		case NEUME_SYMB_FLAT: DrawFlat(dc, element, staff, symbol->m_cueSize); break;
		case NEUME_SYMB_NATURAL: DrawNatural(dc, element, staff, symbol->m_cueSize); break;
		case NEUME_SYMB_DIVISION_FINAL: DrawDivFinal(dc, element, staff, symbol->m_cueSize); break;
		case NEUME_SYMB_DIVISION_MAJOR: DrawDivMajor(dc, element, staff, symbol->m_cueSize); break;
		case NEUME_SYMB_DIVISION_MINOR: DrawDivMinor(dc, element, staff, symbol->m_cueSize); break;
		case NEUME_SYMB_DIVISION_SMALL: DrawDivSmall(dc, element, staff, symbol->m_cueSize); break;
	}
	
    m_currentColour = AxBLACK;
	
    dc->EndGraphic(element);//RZ
	
	return;
}

void MusRC::DrawNeumeClef( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff)
{
    wxASSERT_MSG( dynamic_cast<MusNeumeSymbol*>(element->m_layerElement), "Element must be a MusNeumeSymbol" );	
    MusNeumeSymbol *clef = dynamic_cast<MusNeumeSymbol*>(element->m_layerElement);   
	
	int x = element->m_x_abs;
	int y = staff->m_y_abs - m_layout->m_staffSize[staff->staffSize] - element->m_y_abs + m_layout->m_halfInterl[staff->staffSize]; //with a fudge factor?
	wxString shape = nF_CLEF;
	
	switch (clef->getValue())
	{
		case nC1: shape = nC_CLEF;
		case nF1: y -= m_layout->m_interl[staff->staffSize]*3; break;
		case nC2: shape = nC_CLEF;
		case nF2: y -= m_layout->m_interl[staff->staffSize]*2; break;
		case nC3: shape = nC_CLEF; y -= m_layout->m_interl[staff->staffSize] - 1; break; 
		case nF3: y -= m_layout->m_interl[staff->staffSize] - 6; break;
		case nC4: shape = nC_CLEF; y += 4; break;
		case nF4: y += 8; break;
		default: break;
	}
	
	festa_string(dc, x, y, shape, staff, clef->m_cueSize);
}

void MusRC::DrawComma(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize )
{
	int x = element->m_x_abs;
	int y = staff->m_y_abs + element->m_y_abs;
	festa_string(dc, x, y, nCOMMA, staff, cueSize);
}

void MusRC::DrawFlat(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize )
{
	int x = element->m_x_abs;
	int y = staff->m_y_abs + element->m_y_abs;
	festa_string(dc, x, y, nB_FLAT, staff, cueSize);
}

void MusRC::DrawNatural(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize )
{
	int x = element->m_x_abs;
	int y = staff->m_y_abs + element->m_y_abs - m_layout->m_halfInterl[staff->staffSize]/2;
	festa_string(dc, x, y, nNATURAL, staff, cueSize);
}

void MusRC::DrawDivMinor(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize )
{
	int x = element->m_x_abs;
	int y = staff->m_y_abs + element->m_y_abs + m_layout->m_halfInterl[staff->staffSize] - 3;
	festa_string(dc, x, y, nDIV_MINOR, staff, cueSize);
}

void MusRC::DrawDivMajor(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize)
{
	int x = element->m_x_abs;
	int y = staff->m_y_abs + element->m_y_abs + 6;
	festa_string(dc, x, y, nDIV_MAJOR, staff, cueSize);
}

void MusRC::DrawDivFinal(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize )
{
	int x = element->m_x_abs;
	int y = staff->m_y_abs + element->m_y_abs + m_layout->m_halfInterl[staff->staffSize] - 2;
	festa_string(dc, x, y, nDIV_FINAL, staff, cueSize);
}

void MusRC::DrawDivSmall(MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutStaff *staff, bool cueSize )
{
	int x = element->m_x_abs;
	int y = staff->m_y_abs + element->m_y_abs;
	festa_string(dc, x, y, nDIV_SMALL, staff, cueSize);
}
