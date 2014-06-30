//
//  musrc_tuplet.cpp
//  aruspix
//
//  Created by Rodolfo Zitellini on 21/08/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#pragma implementation "musrc.h" 

#include "wx/wxprec.h"

#include "musrc.h"
#include "muslayerelement.h"

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
#define OBLIQUE_OFFSET 0x52 //move to oblique figures

wxString IntToObliqueFigures(unsigned int number) {
    char buf[16];
    unsigned int len;
    
    memset(buf, 0x00, sizeof(buf));
    
    // We do not convert more that FF values
    if (number > 0xFF) number = 0xFF;
    
    sprintf(buf, "%i", number);
    
    len = strlen(buf);
    wxASSERT_MSG((sizeof(buf) - 1) > len, "String conversion overflow.");
    
    for (unsigned int i = 0; i < strlen(buf); i++) {
        buf[i] += OBLIQUE_OFFSET;
    }
    
    return wxString(buf);
}

/**
 * Analyze a tuplet object and figure out if all the notes are in the same beam
 * or not
 */
bool AllNotesBeamed(MusTuplet* tuplet, MusLayer *layer) {
    
    MusBeam *currentBeam, *firstBeam = NULL;
    bool succ = false;
    
    for (int i = 0; i < tuplet->GetNoteCount(); i++) {
        MusNote *mnote = dynamic_cast<MusNote*>(tuplet->m_children[i]);
        MusLayerElement *laidNote = dynamic_cast<MusLayerElement*>(tuplet->m_children[i]);
        
        // First check: if a note is out of a beam
        if (!mnote->IsInBeam( mnote ))
            return false;
        
        currentBeam = dynamic_cast<MusBeam*>(layer->GetFirst(laidNote, BACKWARD, &typeid(MusBeam), &succ));
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
        if (currentBeam->GetChildIndex(tuplet->m_children[0]) == -1)
            return false;
    }
    return true;
}

/**
 * This function gets the tuplet coords for drawing the bracket and number
 * @param tuplet - the tuplet object
 * @param layer - layer obj
 * @param start, end, center - these are the coordinates returned
 * @return the direction of the beam
 *
 * We can divide the tuplets in three types:
 * 1) All notes beamed
 * 2) All notes unbeamed
 * 3) a mixture of the above
 * 
 * The first type are the simplest to calculate, as we just need the 
 * start and end of the beam
 * types 2 and 3 are threaed in the same manner to calculate the points:
 * - if all the stems are in the same direction, the bracket goes from the
 *   first to the last stem and the number is centered. If a stem in the
 *   middle il longher than the first or last, the y positions are offsetted
 *   accordingly to evitate collisions
 * - if stems go in two different directions, the bracket and number are
 *   placed in the side that has more stems in that direction. If the
 *   stems are equal, if goes up. In this case the bracket is orizontal
 *   so we just need the tallnes of the most tall stem. If a notehead
 *   il lower (or upper) than this stem, we compensate that too with an offset
 
 */

bool GetTupletCoordinates(MusTuplet* tuplet, MusLayer *layer, MusPoint* start, MusPoint* end, MusPoint *center) {
    MusPoint first, last;
    int x, y;
    bool direction = true; //true = up, false = down

    MusLayerElement *firstNote = dynamic_cast<MusLayerElement*>(tuplet->m_children[0]);
    MusLayerElement *lastNote = dynamic_cast<MusLayerElement*>(tuplet->m_children[tuplet->GetNoteCount() - 1]);
    
    // AllNotesBeamed tries to figure out if all the notes are in the same beam
    if (AllNotesBeamed(tuplet, layer)) {
        // yes they are in a beam
        // get the x position centered from the STEM so it looks better
        // NOTE start and end are left to 0, this is the signal that no bracket has to be drawn
        x = firstNote->m_stem_start.x + (lastNote->m_stem_start.x - firstNote->m_stem_start.x) / 2;
        
        // align the center point at the exact center of the first an last stem
        // TUPLET_OFFSET is summed so it does not collide with the stem
        if (firstNote->m_drawn_stem_dir)
            y = lastNote->m_stem_end.y + (firstNote->m_stem_end.y - lastNote->m_stem_end.y) / 2 + TUPLET_OFFSET;
        else 
            y = lastNote->m_stem_end.y + (firstNote->m_stem_end.y - lastNote->m_stem_end.y) / 2 - TUPLET_OFFSET;
        
        // Copy the generated coordinates
        center->x = x;
        center->y = y;
        direction =  firstNote->m_drawn_stem_dir; // stem direction is same for all notes
    } else {
        // There are unbeamed notes of two different beams
        // treat all the notes as unbeames
        int ups = 0, downs = 0; // quantity of up- and down-stems
        
        // In this case use the center of the notehead to calculate the exact center
        // as it looks better
        x = firstNote->m_x_abs + (lastNote->m_x_abs - firstNote->m_x_abs) / 2;
        
        // Return the start and end position for the brackes
        // starting from the first edge and last of the BBoxes
        start->x = firstNote->m_selfBB_x1;
        end->x = lastNote->m_selfBB_x2;
        
        // THe first step is to calculate all the stem directions
        // cycle into the elements and count the up and down dirs
        for (int i = 0; i < tuplet->GetNoteCount(); i++) {
            MusLayerElement *currentNote = dynamic_cast<MusLayerElement*>(tuplet->m_children[i]);
            
            if (currentNote->m_drawn_stem_dir == true)
                ups++;
            else
                downs++;
        }
        // true means up
        direction = ups > downs ? true : false;
        
        // if ups or downs is 0, it means all the stems go in the same direction
        if (ups == 0 || downs == 0) {
            
            // Calculate the average between the first and last stem
            // set center, start and end too.
            if (direction) { // up
                y = lastNote->m_stem_end.y + (firstNote->m_stem_end.y - lastNote->m_stem_end.y) / 2 + TUPLET_OFFSET;
                start->y = firstNote->m_stem_end.y + TUPLET_OFFSET;
                end->y = lastNote->m_stem_end.y + TUPLET_OFFSET;
            } else {
                y = lastNote->m_stem_end.y + (firstNote->m_stem_end.y - lastNote->m_stem_end.y) / 2 - TUPLET_OFFSET;
                start->y = firstNote->m_stem_end.y - TUPLET_OFFSET;
                end->y = lastNote->m_stem_end.y - TUPLET_OFFSET;
            }
            
            // Now we cycle again in all the intermediate notes (i.e. we start from the second note
            // and stop at last -1)
            // We will see if the position of the note is more (or less for down stems) of the calculated
            // average. In this case we offset down or up all the points
            for (int i = 1; i < tuplet->GetNoteCount() - 1 ; i++) {
                 MusLayerElement *currentNote = dynamic_cast<MusLayerElement*>(tuplet->m_children[i]);
                
                if (direction) {
                    // The note is more than the avg, adjust to y the difference
                    // from this note to the avg
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
            // this case is similar to the above, but the bracket is only orizontal
            // y is 0 because the final y pos is above the tallest stem
            y = 0;
            
            // Find the tallest stem and set y to it (with the offset distance)
            for (int i = 0; i < tuplet->GetNoteCount(); i++) {
                MusLayerElement *currentNote = dynamic_cast<MusLayerElement*>(tuplet->m_children[i]);
                
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
                    // but if a notehead with a reversed stem is taller that the last
                    // calculated y, we need to offset
                }
            }
            
            // end and start are on the same line (and so il center when set later)
            end->y = start->y = y;
        }
    }
        
    
    center->x = x;
    center->y = y;
    return direction;
}


void MusRC::DrawTuplet( MusDC *dc, MusTuplet *tuplet, MusLayer *layer, MusStaff *staff)
{
    wxASSERT_MSG( layer, "Pointer to layer cannot be NULL" );
    wxASSERT_MSG( staff, "Pointer to staff cannot be NULL" );
    
    int txt_lenght, txt_height;
    
    // rz MusTuplet *tuplet = dynamic_cast<MusTuplet*>(element);
    //char notes = tuplet->m_notes.GetCount();
    
    wxString notes = IntToObliqueFigures((unsigned int)tuplet->GetNoteCount());
    
    // WORKS ONLY FOR ONE CHAR!
    //char_position = notes + 1; // in the bbox array, '0' char is at pos 1
    
    /*
    switch (notes) {
        case 0: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_0; break;
        case 1: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_1; break;
        case 2: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_2; break;
        case 3: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_3; break;
        case 4: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_4; break;
        case 5: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_5; break;
        case 6: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_6; break;
        case 7: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_7; break;
        case 8: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_8; break;
        case 9: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_9; break;
        default: char_position = LEIPZIG_BBOX_OBLIQUE_FIGURE_0; break;
    }*/
    
    //txt_lenght = bbox->m_bBox[char_position].m_width * ((double)(m_layout->m_fontSize[0][0]) / LEIPZIG_UNITS_PER_EM) + 1;
    
    dc->GetTextExtent(notes, &txt_lenght, &txt_height);
    
    MusPoint start, end, center;
    bool direction = GetTupletCoordinates(tuplet, layer, &start, &end, &center);
    
    //rz dc->StartGraphic( element, "tuplet", wxString::Format("tuplet_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
    
    // Calculate position for number 0x82
    // since the number is slanted, move the center left
    // by 4 pixels so it seems more centered to the eye
    int txt_x = center.x - (txt_lenght / 2) - 4;
    //DrawLeipzigFont ( dc, txt_x,  center.y, notes + 0x82, staff, false);
    
    putstring(dc, txt_x, center.y, notes, 0);
    
    //dc->SetPen(AxBLACK);
    dc->SetPen(AxBLACK, 2, wxSOLID);
    
    // Start is 0 when no line is necessary (i.e. beamed notes)
    if (start.x > 0) {
        // Draw the bracket, interrupt where the number is
        
        // get the slope
        double m = (double)(start.y - end.y) / (double)(start.x - end.x);
        
        // x = 10 pixels before the number
        double x = txt_x - 4;
        // xa = just after, the number is abundant so I do not add anything
        double xa = txt_x + txt_lenght + 2;
        
        // calculate the y coords in the slope
        double y1 = (double)start.y + m * (x - (double)start.x);
        double y2 = (double)start.y + m * (xa - (double)start.x);
        
        // first line
        dc->DrawLine(start.x, ToRendererY(start.y), (int)x, ToRendererY((int)y1));
        // second line after gap
        dc->DrawLine((int)xa, ToRendererY((int)y2), end.x, ToRendererY(end.y));
        
        // vertical bracket lines
        if (direction) {
            dc->DrawLine(start.x, ToRendererY(start.y), start.x, ToRendererY(start.y - 10));
            dc->DrawLine(end.x, ToRendererY(end.y), end.x, ToRendererY(end.y - 10));
        } else {
            dc->DrawLine(start.x, ToRendererY(start.y), start.x, ToRendererY(start.y + 10));
            dc->DrawLine(end.x, ToRendererY(end.y), end.x, ToRendererY(end.y + 10));
        }
                
    }
    
    //rz dc->EndGraphic(element, this );
}
