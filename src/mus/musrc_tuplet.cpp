//
//  musrc_tuplet.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 21/08/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#include "wx/wxprec.h"

#include "musrc.h"
#include "muslayout.h"
#include "muslaidoutlayerelement.h"

#include "musbarline.h"
#include "musleipzigbbox.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"
#include "muskeysig.h"
#include "musbeam.h"
#include "mustie.h"
#include "mustuplet.h"

#include <typeinfo>

#define TUPLET_OFFSET 20

bool AllNotesBeamed(MusTuplet* tuplet, MusLaidOutLayer *layer) {
    MusBeam *currentBeam, *firstBeam = NULL;
    bool succ = false;
    
    for (unsigned int i = 0; i < tuplet->m_notes.GetCount(); i++) {
        MusNote *mnote = dynamic_cast<MusNote*>(&tuplet->m_notes[i]);
        MusLaidOutLayerElement *laidNote = layer->GetFromMusLayerElement(&tuplet->m_notes[i]);
        
        // First check: if a note is out of a beam
        if (mnote->m_beam[0] == 0)
            return false;
        
        currentBeam = dynamic_cast<MusBeam*>(layer->GetFirst(laidNote, BACKWARD, &typeid(MusBeam), &succ)->m_layerElement);
        // no MusBeam found before this note, but the note is set as beamed. mah!
        // return false, we have no beaming
        if (!succ)
            return false;
        
        // The first time we find a beam, set the previous one
        if (firstBeam == NULL)
            firstBeam = currentBeam;
        
        // First beam has to be the same always
        // if it is a different one, it means the tuplet
        // is broken into two beams
        if (firstBeam != currentBeam)
            return false;
     
        
        // So the beam is the same, check that the note
        // is actually into this beam, if not return false
        if (currentBeam->m_notes.Index(tuplet->m_notes[0]) == wxNOT_FOUND)
            return false;
    }
    
    return true;
}

bool GetTupletCoordinates(MusTuplet* tuplet, MusLaidOutLayer *layer, MusPoint* start, MusPoint* end, MusPoint *center) {
    MusPoint first, last;
    int x, y;
    bool direction = true; //true = up, false = down

    MusLaidOutLayerElement *firstNote = layer->GetFromMusLayerElement(&tuplet->m_notes[0]);
    MusLaidOutLayerElement *lastNote = layer->GetFromMusLayerElement(&tuplet->m_notes[tuplet->m_notes.GetCount() - 1]);
    
    // We found a beam
    if (AllNotesBeamed(tuplet, layer)) {
        
        x = firstNote->m_stem_start.x + (lastNote->m_stem_start.x - firstNote->m_stem_start.x) / 2;
        
        if (firstNote->m_drawn_stem_dir)
            y = lastNote->m_stem_end.y + (firstNote->m_stem_end.y - lastNote->m_stem_end.y) / 2 + TUPLET_OFFSET;
        else 
            y = lastNote->m_stem_end.y + (firstNote->m_stem_end.y - lastNote->m_stem_end.y) / 2 - TUPLET_OFFSET;
        
        center->x = x;
        center->y = y;
        direction =  firstNote->m_drawn_stem_dir;
    } else {
        int ups = 0, downs = 0;
        
        x = firstNote->m_x_abs + (lastNote->m_x_abs - firstNote->m_x_abs) / 2;
        
        start->x = firstNote->m_selfBB_x1;
        end->x = lastNote->m_selfBB_x2;
        
        for (unsigned int i = 0; i < tuplet->m_notes.GetCount(); i++) {
            MusLaidOutLayerElement *currentNote = layer->GetFromMusLayerElement(&tuplet->m_notes[i]);
            
            if (currentNote->m_drawn_stem_dir == true)
                ups++;
            else
                downs++;
        }
        
        direction = ups > downs ? true : false;
        
        // Beam goes only in one direction
        if (ups == 0 || downs == 0) {
            
            if (direction) { // up
                y = lastNote->m_stem_end.y + (firstNote->m_stem_end.y - lastNote->m_stem_end.y) / 2 + TUPLET_OFFSET;
                start->y = firstNote->m_stem_end.y + TUPLET_OFFSET;
                end->y = lastNote->m_stem_end.y + TUPLET_OFFSET;
            } else {
                y = lastNote->m_stem_end.y + (firstNote->m_stem_end.y - lastNote->m_stem_end.y) / 2 - TUPLET_OFFSET;
                start->y = firstNote->m_stem_end.y - TUPLET_OFFSET;
                end->y = lastNote->m_stem_end.y - TUPLET_OFFSET;
            }
            
            for (unsigned int i = 1; i < tuplet->m_notes.GetCount() - 1 ; i++) {
                 MusLaidOutLayerElement *currentNote = layer->GetFromMusLayerElement(&tuplet->m_notes[i]);
                
                if (direction) {
                    if (currentNote->m_stem_end.y + TUPLET_OFFSET > y) {
                        int offset = y - (currentNote->m_stem_end.y + TUPLET_OFFSET);
                        y -= offset;
                        end->y -= offset;
                        start->y -= offset;
                    }
                } else {
                    if (currentNote->m_stem_end.y - TUPLET_OFFSET < y) {
                        int offset = y - (currentNote->m_stem_end.y - TUPLET_OFFSET);
                        y -= offset;
                        end->y -= offset;
                        start->y -= offset;
                    }
                }
                
            }
            
            
        } else { // two directional beams
            y = 0;
            
            for (unsigned int i = 0; i < tuplet->m_notes.GetCount(); i++) {
                MusLaidOutLayerElement *currentNote = layer->GetFromMusLayerElement(&tuplet->m_notes[i]);
                
                if (currentNote->m_drawn_stem_dir == direction) {
                                        
                    if (direction) {
                        if (y == 0 || currentNote->m_stem_end.y + TUPLET_OFFSET >= y)
                            y = currentNote->m_stem_end.y + TUPLET_OFFSET;
                    } else {
                        if (y == 0 || currentNote->m_stem_end.y - TUPLET_OFFSET <= y)
                            y = currentNote->m_stem_end.y - TUPLET_OFFSET;
                    }
                        
                } else {
                    // do none for now
                }
            }
            
            end->y = start->y = y;

        }
    }
        
    
    center->x = x;
    center->y = y;
    return direction;
}


void MusRC::DrawTuplet( MusDC *dc, MusLaidOutLayerElement *element, MusLaidOutLayer *layer, MusLaidOutStaff *staff)
{
    wxASSERT_MSG( layer, "Pointer to layer cannot be NULL" );
    wxASSERT_MSG( staff, "Pointer to staff cannot be NULL" );
    
    int txt_lenght, char_position;
    MusLeipzigBBox *bbox = new MusLeipzigBBox;
    
    MusTuplet *tuplet = dynamic_cast<MusTuplet*>(element->m_layerElement);
    char notes = tuplet->m_notes.GetCount();
    
    // WORKS ONLY FOR ONE CHAR!
    char_position = notes + 1; // in the bbox array, '0' char is at pos 1
    
    txt_lenght = bbox->m_bBox[char_position].m_width * ((double)(m_layout->m_fontSize[0][0]) / LEIPZIG_UNITS_PER_EM) + 1;
    
    MusPoint start, end, center;
    bool direction = GetTupletCoordinates(tuplet, layer, &start, &end, &center);
    
    dc->StartGraphic( element, "tuplet", wxString::Format("tuplet_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
    
    // Calculate position for number
    int txt_x = center.x - (txt_lenght / 2);
    DrawLeipzigFont ( dc, txt_x,  center.y, notes + 0x82, staff, false);
    
    dc->SetPen(AxBLACK);
    
    // Start is 0 when no line is necessary (i.e. beamed notes)
    if (start.x > 0) {
        //dc->DrawLine(start.x, ToRendererY(start.y), txt_x, ToRendererY(center.y));
        //dc->DrawLine(txt_x + txt_lenght, ToRendererY(center.y), end.x, ToRendererY(end.y));
        dc->DrawLine(start.x, ToRendererY(start.y), end.x, ToRendererY(end.y));
        
        if (direction) {
            dc->DrawLine(start.x, ToRendererY(start.y), start.x, ToRendererY(start.y - 10));
            dc->DrawLine(end.x, ToRendererY(end.y), end.x, ToRendererY(end.y - 10));
        } else {
            dc->DrawLine(start.x, ToRendererY(start.y), start.x, ToRendererY(start.y + 10));
            dc->DrawLine(end.x, ToRendererY(end.y), end.x, ToRendererY(end.y + 10));
        }
                
    }
    
    dc->EndGraphic(element, this );
}