/////////////////////////////////////////////////////////////////////////////
// Name:        musrc_element.cpp
// Author:      Laurent Pugin and Chris Niven
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "musrc.h"

//----------------------------------------------------------------------------

#include <assert.h>
#include <sstream>
#include <typeinfo>

//----------------------------------------------------------------------------

#include "musapp.h"
#include "musbarline.h"
#include "musbeam.h"
#include "musclef.h"
#include "muskeysig.h"
#include "muslayerelement.h"
#include "musleipzigbbox.h"
#include "musmensur.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"
#include "mustie.h"
#include "mustuplet.h"

//----------------------------------------------------------------------------
// MusRC - MusLayerElement
//----------------------------------------------------------------------------

int MusRC::s_drawingLigX[2], MusRC::s_drawingLigY[2];	// pour garder coord. des ligatures    
bool MusRC::s_drawingLigObliqua = false;	// marque le 1e passage pour une oblique

void MusRC::DrawElement( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    
    int previousColor = m_currentColour;

    if (element == m_currentElement) {
		m_currentColour = AxRED;
    }
    else {
        m_currentColour = AxBLACK;
    }
    
    if (dynamic_cast<MusBarline*>(element)) {
        DrawBarline(dc, element, layer, staff);
    }
    else if (dynamic_cast<MusBeam*>(element)) {
        DrawBeamElement(dc, element, layer, staff);
    }
    else if (dynamic_cast<MusClef*>(element)) {
        DrawClef(dc, element, layer, staff);
    }
    else if (dynamic_cast<MusKeySig*>(element)) {
        DrawKeySig(dc, element, layer, staff);
    }
    else if (dynamic_cast<MusMensur*>(element)) {
        DrawMensur(dc, element, layer, staff);
    }
    else if (dynamic_cast<MusNote*>(element)) {
        DrawDurationElement(dc, element, layer, staff);
    }
    else if (dynamic_cast<MusRest*>(element)) {
        DrawDurationElement(dc, element, layer, staff);
    }
    else if (dynamic_cast<MusSymbol*>(element)) {
        DrawSymbol(dc, element, layer, staff);
    }
    else if (dynamic_cast<MusTie*>(element)) {
        DrawTie(dc, element, layer, staff);
    } 
    else if (dynamic_cast<MusTuplet*>(element)) {
        DrawTupletElement(dc, element, layer, staff);
    }
    else if (dynamic_cast<MusLayerApp*>(element)) {
        DrawLayerApp(dc, element, layer, staff);
    }
    
    m_currentColour = previousColor;
}

void MusRC::DrawDurationElement( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"

    MusDurationInterface *durElement = dynamic_cast<MusDurationInterface*>(element);
	if ( !durElement )
		return;
        
	
    if (dynamic_cast<MusNote*>(element)) 
    {
        MusNote *note = dynamic_cast<MusNote*>(element);
        int oct = note->m_oct - 4;
        
        //if ( !m_lyricMode && BelongsToTheNote( m_currentElement ) ) // the current element is a lyric that belongs to the note we are drawing
        //    m_currentColour = AxCYAN;
            
        dc->StartGraphic( element, "note", Mus::StringFormat("s_%d_%d_%d", staff->GetId(), layer->voix, element->GetId() ) );
        element->m_y_drawing = CalculatePitchPosY( staff, note->m_pname, layer->GetClefOffset( element ), oct );
        
        if (!note->m_chord) // && (!pelement->ElemInvisible || illumine))
        {	
            DrawNote(dc, element, layer, staff);
        //else 
        //{
            //	note->accord(dc, staff);
        }
        dc->EndGraphic(element, this ); //RZ
	} 
    else if (dynamic_cast<MusRest*>(element)) {
        MusRest *rest = dynamic_cast<MusRest*>(element);
        int oct = rest->m_oct - 4;

        dc->StartGraphic( element, "rest", Mus::StringFormat("s_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
        //if (!transp_sil)
		//	pnote->code = getSilencePitch (pelement);
        
        // Automatically calculate rest position, if so requested
        if (rest->m_pname == REST_AUTO)
            element->m_y_drawing = CalculateRestPosY( staff, rest->m_dur);
        else
            element->m_y_drawing = CalculatePitchPosY( staff, rest->m_pname, layer->GetClefOffset( element ), oct);
		
        DrawRest( dc, element, layer, staff );
        dc->EndGraphic(element, this ); //RZ
	}
    
	/* 
	for ( int i = 0; i < (int)m_lyrics.GetCount(); i++ ){
		MusSymbol1 *lyric = &m_lyrics[i];
		
		if ( lyric != NULL )
		{
            if ( lyric == m_currentElement )
                m_currentColour = AxRED;
            else if ( (this == m_currentElement) || BelongsToTheNote( m_currentElement ) )
                m_currentColour = AxCYAN;

			putlyric(dc, lyric->m_x_abs + staff->m_x_abs, staff->m_y_drawing + lyric->dec_y , 
						  lyric->m_debord_str, staff->staffSize, ( lyric == m_currentElement && m_inputLyric ) );
		}		
	}
    */ // ax2 lyrics
	
	return;
}

void MusRC::DrawBeamElement(MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff) {
    
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    
    MusBeam *beam = dynamic_cast<MusBeam*>(element);

    dc->StartGraphic( element, "beam", Mus::StringFormat("s_%d_%d_%d", staff->GetId(), layer->voix, element->GetId() ) );
    
    for (unsigned int i = 0; i < beam->m_children.size(); i++) {
        if ( dynamic_cast<MusLayerElement*>(beam->m_children[i]) ) {
            MusLayerElement *element = dynamic_cast<MusLayerElement*>(beam->m_children[i]);
            DrawElement(dc, element, layer, staff);
        }
    }
    
    // BEAM!
    DrawBeam( dc, layer, beam, staff );
    
    dc->EndGraphic(element, this ); //RZ
}

void MusRC::DrawTupletElement(MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff) {
    
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    
    MusTuplet *tuplet = dynamic_cast<MusTuplet*>(element);
    
    dc->StartGraphic( element, "tuplet", Mus::StringFormat("s_%d_%d_%d", staff->GetId(), layer->voix, element->GetId() ) );
    
    // Draw the inner elements
    for (unsigned int i = 0; i < tuplet->m_children.size(); i++) {
        if ( dynamic_cast<MusLayerElement*>(tuplet->m_children[i]) ) {
            MusLayerElement *element = dynamic_cast<MusLayerElement*>(tuplet->m_children[i]);
            DrawElement(dc, element, layer, staff);
        }
    }
    
    // Tupletize the elements!
    DrawTuplet( dc, tuplet, layer, staff );
    
    dc->EndGraphic(element, this ); //RZ
}

// dessine la note en a,b+by. Calcule et dessine lignes addit. avec by=m_y_drawing
// b = decalage en fonction oct., clef, a partir du curseur; by = pos. curs.
// Accords: note doit connaitre le x non modifie par accord(), la fin de 
// l'accord (ptr_n->fchord), la valeur y extreme opposee au sommet de la
// queue: le ptr *testchord extern peut garder le x et l'y.


void MusRC::DrawNote ( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff)
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(dynamic_cast<MusNote*>(element)); // Element must be a MusNote"
    
    MusNote *note = dynamic_cast<MusNote*>(element);
    
    bool inBeam = (note->GetFirstParent( &typeid( MusBeam ) ) != NULL );
    
	int staffSize = staff->staffSize;

	//	int horphyspoint=h_pnt;
	int b = element->m_y_drawing;
	int up=0, i, valdec, fontNo, ledge, queueCentre;
	int x1, x2, y2, espac7, decval, vertical;
	int formval = 0;	// pour permettre dessiner colorations avec dÇcalage de val
	int rayon, milieu = 0;

	int xn = element->m_x_abs, xl = element->m_x_abs;
	int bby = staff->m_y_drawing - m_doc->m_staffSize[staffSize];  // bby= y sommet portee
	int ynn = element->m_y_drawing + staff->m_y_drawing; 
	static int ynn_chrd;

	xn += note->m_hOffset;
	//val=note->m_dur;
	formval = (note->m_colored && note->m_dur > DUR_1) ? (note->m_dur+1) : note->m_dur;
	queueCentre = 0;


	rayon = m_doc->m_noteRadius[staffSize][note->m_cueSize];

	if (note->m_dur > DUR_1 || (note->m_dur == DUR_1 && staff->notAnc))	// annuler provisoirement la modif. des lignes addit.
		ledge = m_doc->m_ledgerLine[staffSize][note->m_cueSize];
	else
	{	
        ledge= m_doc->m_ledgerLine[staffSize][2];
		rayon += rayon/3;
	}

	/*NEW: diminuer le rayon d'une quantitÇ paramÇtrable*/
	//if (val>DUR_1)
	//	rayon -= ((rayon * m_doc->m_parameters.m_stemCorrection) / 20);
	x1 = xn-rayon;	// position d'appel du caractäre et de la queue gauche
    xl = xn;

	// permettre d'inverser le cotÇ de la tete de note avec flag lat
	/*if (this->lat && !this->chord)
	{	if (this->queue)
		{	x1 = xn + rayon;
			xl = xn + rayon * 2;
		}
		else
		{	x1 = xn - rayon * 3;
			xl = xn - rayon * 2;
		}
	}*/ // ax2 - not support of lat

	DrawLedgerLines( dc, ynn, bby, xl, ledge, staffSize);	// dessin lignes additionnelles

	if (note->m_dur == DUR_LG || note->m_dur == DUR_BR || ((note->m_lig) && note->m_dur == DUR_1))	// dessin carrees
	{
		DrawLigature ( dc, ynn, element, layer, staff);
 	}
	else if (note->m_dur==DUR_1)
	{	
        if ( is_in (note->m_headshape, LOSANGEVIDE, OPTIONLIBRE))
			fontNo = LEIPZIG_OFFSET_NOTE_HEAD+note->m_headshape;
		else if (note->m_colored) // && !note->m_ligObliqua) // in WG, use of obliq for coloration? 
			fontNo = LEIPZIG_HEAD_WHOLE_FILLED;
		else
			fontNo = LEIPZIG_HEAD_WHOLE;

		DrawLeipzigFont( dc, x1, ynn, fontNo, staff, note->m_cueSize );
		decval = ynn;
	}
	else
	{	
        if ( is_in (note->m_headshape, LOSANGEVIDE, OPTIONLIBRE))
			fontNo = LEIPZIG_OFFSET_NOTE_HEAD+note->m_headshape;

		else if (note->m_colored || formval == DUR_2)
			fontNo = LEIPZIG_HEAD_HALF;
		else
			fontNo = LEIPZIG_HEAD_QUARTER;

		DrawLeipzigFont( dc,x1, ynn, fontNo, staff, note->m_cueSize );

		milieu = bby - m_doc->m_interl[staffSize]*2;

// test ligne mediane pour direction queues: notation mesuree, milieu queue haut
		if (staff->notAnc)
			milieu +=  m_doc->m_halfInterl[staffSize];

		if (note->m_chord) { /*** && this == testchord)***/
			ynn_chrd = ynn;
        }
		if (inBeam && formval > DUR_4)
        {
            // no stem
		}
		else if (note->m_headshape != SANSQUEUE && (!note->m_chord || (note->m_chord==CHORD_TERMINAL))) {	
            if (note->m_chord==CHORD_TERMINAL) {	
				/***up = testchord->obj.not.haste;
				xn = testchord->m_x_abs;***/
			}
			else {
				//***up = this->q_auto ? ((ynn < milieu)? ON :OFF):this->queue;
				// ENZ
				up = (ynn < milieu) ? ON : OFF;
            }
			
			// ENZ
			if ( note->m_stemDir != 0 ) {
				up = (up == ON) ? OFF : ON;
            }
			
			espac7 = note->m_cueSize ? ( m_doc->m_halfInterl[staffSize]*5) : ( m_doc->m_halfInterl[staffSize]*7);
			vertical = note->m_cueSize ?  m_doc->m_halfInterl[staffSize] :  m_doc->m_interl[staffSize];
			decval = vertical * (valdec = formval-DUR_8);
			
			/***if (this->existDebord)	// queue longueur manuelle
				traiteQueue (&espac7, this);***/

			// diminuer le rayon de la moitie de l'epaisseur du trait de queue
			rayon -= (m_doc->m_env.m_stemWidth) / 2;

			if (!up) {	// si queue vers le bas (a gauche)
				espac7 = -espac7;
				decval = -decval;
				rayon = -rayon;
			}

			y2 = ((formval>DUR_8) ? (ynn + espac7 + decval) : (ynn + espac7));

			if ((note->m_chord==CHORD_INITIAL) || (note->m_chord==CHORD_MEDIAL)) {
				ynn = ynn_chrd;
            }

			/***if (this->q_auto)
			{	this->queue = (up > 0);
				if ( (y2 >= milieu && ynn > milieu) || (y2 <= milieu && ynn < milieu) )
				// note et queue du meme cote par rapport au centre de la portee
				{	y2 = milieu;
					queueCentre = 1;
				}
			}***/

			if (staff->notAnc)
				rayon = 0;
			x2 = xn + rayon;

			if (up)
			{
				if (formval > DUR_8 && !queueCentre)
				// Le 24 Septembre 1993. Correction esthetique pour rapprocher tailles 
				//   des DUR_8 et DUR_16 (longeur de queues trop inegales).
					y2 -= m_doc->m_halfInterl[staffSize];
				decval = y2;
				if (staff->notAnc)
					v_bline ( dc,y2,(int)(ynn + m_doc->m_halfInterl[staffSize]),x2, m_doc->m_env.m_stemWidth );//queue en descendant
				else
					v_bline ( dc,y2,(int)(ynn+ m_doc->m_verticalUnit2[staffSize]),x2 - (m_doc->m_env.m_stemWidth / 2), m_doc->m_env.m_stemWidth );//queue en descendant
                
                element->m_stem_start.x = element->m_stem_end.x = x2 - (m_doc->m_env.m_stemWidth / 2);
                element->m_stem_end.y = y2;
                element->m_stem_start.y = (int)(ynn+ m_doc->m_verticalUnit2[staffSize]);
                element->m_drawn_stem_dir = true;
                
				if (formval > DUR_4)
				{
                    y2 += m_doc->m_env.m_stemWidth / 2; // ENZO correction empirique...
					DrawLeipzigFont( dc,x2,y2,LEIPZIG_STEM_FLAG_UP, staff, note->m_cueSize );
					for (i=0; i < valdec; i++)
						DrawLeipzigFont( dc,x2,y2-=vertical,LEIPZIG_STEM_FLAG_UP, staff, note->m_cueSize );
				}
			}
			else
			{                
				if (formval > DUR_8 && !queueCentre)
				// Le 24 Septembre 1993. Correction esthetique pour rapprocher tailles 
				//   des DUR_8 et DUR_16 (longeur de queues trop inegales).
					y2 += m_doc->m_halfInterl[staffSize];
				decval = y2;

				if (staff->notAnc)
					v_bline ( dc,y2,ynn- m_doc->m_halfInterl[staffSize],x2 - (m_doc->m_env.m_stemWidth / 2), m_doc->m_env.m_stemWidth );//queue en descendant
				else
					v_bline ( dc,y2,(int)(ynn- m_doc->m_verticalUnit2[staffSize]),x2 - (m_doc->m_env.m_stemWidth / 2), m_doc->m_env.m_stemWidth );	// queue en montant

                element->m_stem_start.x = element->m_stem_end.x = x2 - (m_doc->m_env.m_stemWidth / 2);
                element->m_stem_start.y = (int)(ynn- m_doc->m_verticalUnit2[staffSize]);
                element->m_stem_end.y = y2;
                element->m_drawn_stem_dir = false;
                
				// ENZ
				// decalage du crochet vers la gauche
				// changement dans la fonte Leipzig 4.3 ‡ cause de problemes d'affichage
				// en deÁ‡ de 0 avec la notation ancienne
				// dans la fonte les crochets ont ete decales de 164 vers la droite
				int cr_offset = m_doc->m_noteRadius[staffSize][note->m_cueSize]  + (m_doc->m_env.m_stemWidth / 2);
				if (formval > DUR_4)
				{
                    y2 -= m_doc->m_env.m_stemWidth / 2; // ENZO correction empirique...
					DrawLeipzigFont( dc,x2 - cr_offset,y2,LEIPZIG_STEM_FLAG_DOWN , staff, note->m_cueSize );
					for (i=0; i < valdec; i++)
						DrawLeipzigFont( dc,x2  - cr_offset,y2+=vertical,LEIPZIG_STEM_FLAG_DOWN, staff, 
									 note->m_cueSize );
				}
			}
            if (note->m_cueSize && note->m_acciaccatura)
                DrawAcciaccaturaSlash(dc, element);
		}	// fin de dessin queues et crochets

	}
    
	DrawLedgerLines( dc, ynn, bby, xl, ledge, staffSize);

	if (note->m_slur[0])
	{	
        if (note->m_slur[0]==SLUR_TERMINAL)
		{	/***liaison = OFF;	// pour tests de beam...
			liais_note(hdc, this);***/
		}
		/***if (this->dliai && !attente)	premierLie = this;***/
	}
    
	if (note->m_accid) // && !this->accInvis) // ax2 no support invisible accidental yet
	{
		if (note->m_chord)
            {}/***x1 = x_acc_chrd (this,0);***/
		else
			x1 -= 1.5 * m_doc->m_accidWidth[staffSize][note->m_cueSize];
		MusSymbol accid( SYMBOL_ACCID );
		//symb.Init( m_r );
        accid.m_oct = note->m_oct;
        accid.m_pname = note->m_pname;
		accid.m_accid = note->m_accid;
        accid.m_x_abs = x1;
        DrawSymbol( dc, &accid, layer, staff, element ); // ax2
	}
	if (note->m_chord)
	{	
        /***x2 = testchord->m_x_abs + m_doc->m_step2;
		if (this->haste)
		{	if (this->lat || (this->ptr_fe && this->ptr_fe->type==NOTE && this->ptr_fe->obj.not.lat)
				|| (this->ptr_pe && element->m_x_abs==this->ptr_pe->m_x_abs && this->ptr_pe->type==NOTE && this->ptr_pe->obj.not.lat
					&& this->dec_y - this->ptr_pe->dec_y < m_doc->m_interl[staffSize]
					&& 0 != ((int)b % (int)m_doc->m_interl[staffSize]))
				)
				x2 += m_doc->m_noteRadius[staffSize][dimin] * 2;
		}*///
	}
	else
	{	if (note->m_dur < DUR_2 || (note->m_dur > DUR_8 && !inBeam && up))
			x2 = xn + m_doc->m_step1*7/2;
		else
			x2 = xn + m_doc->m_step1*5/2;

		//if (this->lat) // ax2 - no support of note head flip
        //    x2 += rayon*2;
	}

	if (note->m_dots) // && (!this->pointInvisible)) // ax2 - no support of invisible dots yet
	{
		DrawDots( dc,x2,b, 0, note->m_dots, staff );
	}
/*
	if (this->stacc && (!this->rel || !this->queue_lig))
	{
		if (!this->chord || ((!this->queue_lig && this == testchord) || (this->queue_lig && this->fchord )))
		{
			if (val > DUR_BR)
			{	if  (!this->queue_lig)
				{	if ((this->queue && !this->chord) || (this->chord && this->haste))
					{	b -= m_doc->m_interl[staffSize];
						decval = -m_doc->m_interl[staffSize];
					}
					else
					{	b += m_doc->m_interl[staffSize];
						decval = 0;
					}

				}
				else	// tous les cas inversÇs par queue_lig
				{	b = decval-staff->m_y_drawing;
	
					if ((!this->queue && !this->chord) || (this->chord && !this->haste))
					{	b -= m_doc->m_interl[staffSize];
						decval = -1;
						if (val <= DUR_1)
							decval = -m_doc->m_interl[staffSize];

					}
					else
					{	b += m_doc->m_halfInterl[staffSize];
						decval = 0;
						if (val <= DUR_1)
							b += m_doc->m_halfInterl[staffSize];
					}
				}

			}
			else
				return;
			putStacc (hdc,xn,b,decval,this->typStac);
		}
	}
*/
	
	//temp debug code
//	m_currentColour = wxCYAN;
//	rect_plein2(dc, element->m_x_abs - 3, ynn - 3, element->m_x_abs + 3, ynn + 3);
//	printf("\nxrel: %d, ynn: %d\n\n", element->m_x_abs, ynn);
//	m_currentColour = wxBLACK;
	//temp debug code
	

    if (note->m_fermata)
        DrawFermata(dc, element, staff);

    if (note->m_embellishment == EMB_TRILL)
        DrawTrill(dc, element, staff);
    
	return;
}


void MusRC::DrawRest ( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{	
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(dynamic_cast<MusRest*>(element)); // Element must be a MusRest"
        
    MusRest *rest = dynamic_cast<MusRest*>(element);

	int formval = rest->m_dur;
	int a = element->m_x_abs + rest->m_hOffset;
    int b = element->m_y_drawing;

	//unsigned char dot = this->point;
	/*if (inv_val && (!this->oblique && formval > DUR_1 || this->oblique && formval > DUR_2))
		formval += 1;*/ // ax2 - no support of inv_val for rest - need to use @dur.ges

	if (rest->m_dur == VALSilSpec) // WG multi-rest? 
    {
		formval = DUR_1;
    }
	else if (formval > DUR_2)
    {
		a -= m_doc->m_noteRadius[staff->staffSize][rest->m_cueSize];
    }

	if (formval == DUR_BR || formval == DUR_2) 
    {
		b -= 0; //m_doc->m_interl[staff->staffSize]; // LP position des silences
    }

	if (formval == DUR_1)
	{	
        if (staff->portNbLine == 1) {
		// silences sur portee a une seule ligne
			b += m_doc->m_interl[staff->staffSize];
		}
        else
        {
			//b += m_doc->m_interl[staff->staffSize]*2; 
			b -= 0; //m_doc->m_interl[staff->staffSize]*2;// LP positions des silences
        }
	}

	if (rest->m_dur == VALSilSpec && rest->m_multimeasure_dur > 1) // LP: not sure what is actually does...
    {
		DrawSpecialRest( dc, a, element, staff);
    }
	else
	{	
        switch (formval)
		{	
            case DUR_LG: DrawLongRest ( dc, a, b, staff); break;
			case DUR_BR: DrawBreveRest( dc, a, b, staff); break;
			case DUR_1:
			case DUR_2: DrawWholeRest ( dc, a, b, formval, rest->m_dots, rest->m_cueSize, staff); break;
			//case CUSTOS: s_nr ( dc, a, b - m_doc->m_halfInterl[staff->staffSize] + 1, '#' - LEIPZIG_REST_QUARTER + DUR_4 , staff); break; // Now in MusSymbol
			default: DrawQuarterRest( dc, a, b, formval, rest->m_dots, rest->m_cueSize, staff);
		}
	}
    
    if(rest->m_fermata)
        DrawFermata(dc, element, staff);
    
	return;
}


void MusRC::DrawLedgerLines( MusDC *dc, int y_n, int y_p, int xn, unsigned int smaller, int staffSize)
{
	int yn, ynt, yh, yb, test, v_decal = m_doc->m_interl[staffSize];
	int dist, xng, xnd;
	register int i;


	yh = y_p + m_doc->m_halfInterl[staffSize]; yb = y_p- m_doc->m_staffSize[staffSize]- m_doc->m_halfInterl[staffSize];

	if (!is_in(y_n,yh,yb))                           // note hors-portee?
	{
		xng = xn - smaller;
		xnd = xn + smaller;

		dist = ((y_n > yh) ? (y_n - y_p) : y_p - m_doc->m_staffSize[staffSize] - y_n);
  		ynt = ((dist % m_doc->m_interl[staffSize] > 0) ? (dist - m_doc->m_halfInterl[staffSize]) : dist);
		test = ynt/ m_doc->m_interl[staffSize];
		if (y_n > yh)
		{	yn = ynt + y_p;
			v_decal = - m_doc->m_interl[staffSize];
		}
		else
			yn = y_p - m_doc->m_staffSize[staffSize] - ynt;

		//hPen = (HPEN)SelectObject (hdc, CreatePen (PS_SOLID, _param.EpLignesPORTEE+1, workColor2));
		//xng = toZoom(xng);
		//xnd = toZoom(xnd);

        dc->SetPen( m_currentColour, ToRendererX( m_doc->m_env.m_staffLineWidth ), AxSOLID );
        dc->SetBrush(m_currentColour , AxTRANSPARENT );

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

/** This function draws multi-measure rests
 **/
#define NUMBER_REDUCTION 5
void MusRC::DrawSpecialRest ( MusDC *dc, int a, MusLayerElement *element, MusStaff *staff)
{	
    //MusLeipzigBBox *bbox = new MusLeipzigBBox();
    int x, x2, y, y2, lenght;

    MusRest *rest = dynamic_cast<MusRest*>(element);
    
    // We do not support more than three chars
    if (rest->m_multimeasure_dur > 999)
        rest->m_multimeasure_dur = 999;
    
    // This is 1/2 the lenght of th black rectangle
	lenght = (m_doc->m_step1 * 5);
    
    // Position centered in third line
    // it would be m_interl * 6.5, or m_interl / 2 * 13
	y = staff->m_y_drawing - (m_doc->m_interl[staff->staffSize] / 2) * 13;
    y2 = y + m_doc->m_interl[staff->staffSize];
	
    // a is the central point, claculate x and x2
    x = a - lenght; 
    x2 = a + lenght;
    
    // Draw the base rect
    // make it 8 pixels smaller than the interline space
    // these are the two 4 substracted and added
	rect_plein2(dc, x, y2 - 4, x2, y + 4);
    
    //Draw the to lines at beginning and end
    // make it 8 pixesl longers, and 4 pixels width
    v_bline(dc, y - 4, y2 + 4, x, 4);
    v_bline(dc, y - 4, y2 + 4, x2, 4);
    
    // Draw the text above
    int w, h;
    unsigned int start_offset = 0; // offset from x to center text
    
    // convert to string
    std::stringstream text;
    text << rest->m_multimeasure_dur;
    
    dc->GetTextExtent( text.str(), &w, &h);
    start_offset = (x2 - x - w) / 2; // calculate offset to center text
    
    putstring(dc, x + start_offset, staff->m_y_drawing - m_doc->m_staffSize[staff->staffSize] + 5, text.str(), false);
    
    return;

}

void MusRC::DrawLongRest ( MusDC *dc, int a, int b, MusStaff *staff)

{	int x, x2, y = b + staff->m_y_drawing, y2;

	x = a; //- m_doc->m_step1/3; 
	x2 = a+ (m_doc->m_step1 *2 / 3); // LP
	if (b % m_doc->m_interl[staff->staffSize])
		y -= m_doc->m_halfInterl[staff->staffSize];
	y2 = y + m_doc->m_interl[staff->staffSize]*2;
	rect_plein2( dc,x,y2,x2,y);
	return;
}


void MusRC::DrawBreveRest ( MusDC *dc, int a, int b, MusStaff *staff)

{	int x, x2, y = b + staff->m_y_drawing, y2;

	x = a; //- m_doc->m_step1/3; 
	x2 = a+ (m_doc->m_step1 *2 / 3); // LP

	if (b % m_doc->m_interl[staff->staffSize])
		y -= m_doc->m_halfInterl[staff->staffSize];
	y2 = y + m_doc->m_interl[staff->staffSize];
	rect_plein2 ( dc,x,y2,x2,y);
	x = a - m_doc->m_step1; x2 = a + m_doc->m_step1;

	h_bline ( dc, x,x2,y2,1);
	h_bline ( dc, x,x2,y, 1);
	return;
}

void MusRC::DrawWholeRest ( MusDC *dc, int a, int b, int valeur, unsigned char dots, unsigned int smaller, MusStaff *staff)

{	int x, x2, y = b + staff->m_y_drawing, y2, vertic = m_doc->m_halfInterl[staff->staffSize];
	int off;
	float foff;

	if (staff->notAnc)
		foff = (m_doc->m_step1 *1 / 3);
	else
		foff = (m_doc->m_ledgerLine[staff->staffSize][2] * 2) / 3; // i.e., la moitie de la ronde

	if (smaller)
		foff *= (int)( (float)m_doc->m_graceRatio[0] / (float)m_doc->m_graceRatio[1] );
	off = (int)foff;

	x = a - off;
	x2 = a + off;

	if (valeur == DUR_1)
		vertic = -vertic;

	if (b % m_doc->m_interl[staff->staffSize])
	{
		if (valeur == DUR_2)
			y -= vertic;
		else
			y += vertic;
	}

	y2 = y + vertic;
	rect_plein2 ( dc, x,y,x2,y2);

	off /= 2;
	x -= off;
	x2 += off;

	if (y > (int)staff->m_y_drawing- m_doc->m_staffSize[staff->staffSize] || y < (int)staff->m_y_drawing-2* m_doc->m_staffSize[staff->staffSize])
		h_bline ( dc, x, x2, y, m_doc->m_env.m_staffLineWidth);

	if (dots)
		DrawDots ( dc,(x2 + m_doc->m_step1), y2, -(int)staff->m_y_drawing, dots, staff);
}


void MusRC::DrawQuarterRest ( MusDC *dc, int a, int b, int valeur, unsigned char dots, unsigned int smaller, MusStaff *staff)
{
	int _intrl = m_doc->m_interl[staff->staffSize];

	DrawLeipzigFont( dc, a, (b + staff->m_y_drawing), LEIPZIG_REST_QUARTER + (valeur-DUR_4), staff, smaller );

	//DrawLeipzigFont( dc, a, (b + staff->m_y_drawing - m_doc->m_halfInterl[staff->staffSize]), '#', staff, note->m_cueSize);

	if (dots)
	{	if (valeur >= DUR_16)
			_intrl = 0;
		DrawDots ( dc, (a+ m_doc->m_step2), b, _intrl, dots, staff);
	}
	return;
}


void MusRC::DrawDots ( MusDC *dc, int x1, int y1, int offy, unsigned char dots, MusStaff *staff )

{
	y1 += offy;
	int i;
	for (i = 0; i < dots; i++) {
		DrawDot ( dc, x1, y1, 0, staff);
		x1 += std::max (6, m_doc->m_step1);
	}
	return;
}



void MusRC::CalculateLigaturePosX ( MusLayerElement *element, MusLayer *layer, MusStaff *staff)
{
	if (element == NULL) 
    {
    	return;
    }
    MusLayerElement *previous = layer->GetPrevious(element);
	if (previous == NULL || !previous->IsNote()) 
    {
        return;
    }
    MusNote *previousNote = dynamic_cast<MusNote*>(previous);
    if (previousNote->m_lig==LIG_TERMINAL)
    {
        return;
    } 
	if (previousNote->m_lig && previousNote->m_dur <= DUR_1)
	{	
        element->m_x_abs = previous->m_x_abs + m_doc->m_brevisWidth[staff->staffSize] * 2;
	}
    return;
}

void MusRC::DrawLigature ( MusDC *dc, int y, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{	
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(dynamic_cast<MusNote*>(element)); // Element must be a MusNote"
    
    MusNote *note = dynamic_cast<MusNote*>(element);
    

	int xn, x1, x2, yy2, y1, y2, y3, y4, y5;
	int milieu, up, epaisseur;

	epaisseur = std::max (2, m_doc->m_env.m_beamWidth/2);
	xn = element->m_x_abs;
	
	if ((note->m_lig==LIG_MEDIAL) || (note->m_lig==LIG_TERMINAL))
    {
		CalculateLigaturePosX ( element, layer, staff );
    }
	else {
		xn = element->m_x_abs + note->m_hOffset;
    }


	// calcul des dimensions du rectangle
	x1 = xn - m_doc->m_brevisWidth[staff->staffSize]; x2 = xn +  m_doc->m_brevisWidth[staff->staffSize];
	y1 = y + m_doc->m_halfInterl[staff->staffSize]; 
	y2 = y - m_doc->m_halfInterl[staff->staffSize]; 
	y3 = (int)(y1 + m_doc->m_verticalUnit1[staff->staffSize]);	// partie d'encadrement qui depasse
	y4 = (int)(y2 - m_doc->m_verticalUnit1[staff->staffSize]);	

	if (!note->m_ligObliqua && (!MusRC::s_drawingLigObliqua))	// notes rectangulaires, y c. en ligature
	{
		if ( !note->m_colored)
		{				//	double base des carrees
			hGrosseligne ( dc, x1,  y1,  x2,  y1, -epaisseur );
			hGrosseligne ( dc, x1,  y2,  x2,  y2, epaisseur );
		}
		else
			rect_plein2( dc,x1,y1,x2,y2);	// dessine val carree pleine // ENZ correction de x2

		v_bline ( dc, y3, y4, x1, m_doc->m_env.m_stemWidth );	// corset lateral
		v_bline ( dc, y3, y4, x2, m_doc->m_env.m_stemWidth );
	}
	else			// traitement des obliques
	{
		if (!MusRC::s_drawingLigObliqua)	// 1e passage: ligne verticale initiale
		{
			v_bline (dc,y3,y4,x1, m_doc->m_env.m_stemWidth );
			MusRC::s_drawingLigObliqua = true;
			//oblique = OFF;
//			if (val == DUR_1)	// queue gauche haut si DUR_1
//				queue_lig = ON;
		}
		else	// 2e passage: lignes obl. et verticale finale
		{
			x1 -=  m_doc->m_brevisWidth[staff->staffSize]*2;	// avance auto

			y1 = *MusRC::s_drawingLigY - m_doc->m_halfInterl[staff->staffSize];	// ligat_y contient y original
			yy2 = y2;
			y5 = y1+ m_doc->m_interl[staff->staffSize]; y2 += m_doc->m_interl[staff->staffSize];	// on monte d'un INTERL

			if (note->m_colored)
				hGrosseligne ( dc,  x1,  y1,  x2,  yy2, m_doc->m_interl[staff->staffSize]);
			else
			{	hGrosseligne ( dc,  x1,  y1,  x2,  yy2, 5);
				hGrosseligne ( dc,  x1,  y5,  x2,  y2, -5);
			}
			v_bline ( dc,y3,y4,x2,m_doc->m_env.m_stemWidth);	//cloture verticale

			MusRC::s_drawingLigObliqua = false;
//			queue_lig = OFF;	//desamorce alg.queue DUR_BR

		}
	}

	if (note->m_lig)	// memoriser positions d'une note a l'autre; relier notes par barres
	{	
        *(MusRC::s_drawingLigX+1) = x2; *(MusRC::s_drawingLigY+1) = y;	// relie notes ligaturees par barres verticales
		//if (in(x1,(*MusRC::s_drawingLigX)-2,(*MusRC::s_drawingLigX)+2) || (this->fligat && this->lat && !MusNote1::marq_obl))
			// les dernieres conditions pour permettre ligature verticale ancienne
		//	v_bline (dc, *ligat_y, y1, (this->fligat && this->lat) ? x2: x1, m_doc->m_parameters.m_stemWidth); // ax2 - drawing vertical lines missing
		*MusRC::s_drawingLigX = *(MusRC::s_drawingLigX + 1);
		*MusRC::s_drawingLigY = *(MusRC::s_drawingLigY + 1);
	}

	
	y3 = y2 - m_doc->m_halfInterl[staff->staffSize]*6;

	if (note->m_lig)
	{	
        if (note->m_dur == DUR_BR) //  && this->queue_lig)	// queue gauche bas: DUR_BR initiale descendante // ax2 - no support of queue_lig (see WG corrigeLigature)
		{
            v_bline ( dc, y2, y3, x1, m_doc->m_env.m_stemWidth );
        }
		else if (note->m_dur == DUR_LG) // && !this->queue_lig) // DUR_LG en ligature, queue droite bas // ax2 - no support of queue_lig
		{
            v_bline (dc, y2, y3, x2, m_doc->m_env.m_stemWidth );
        }
		else if (note->m_dur == DUR_1) // && this->queue_lig )	// queue gauche haut // ax2 - no support of queue_lig
		{	
            y2 = y1 + m_doc->m_halfInterl[staff->staffSize]*6;
			v_bline ( dc, y1, y2, x1, m_doc->m_env.m_stemWidth );
		} 
	}
	else if (note->m_dur == DUR_LG)		// DUR_LG isolee: queue comme notes normales
	{	
		milieu = staff->m_y_drawing - m_doc->m_interl[staff->staffSize]*6;
		//***up = this->q_auto ? ((y < milieu)? ON :OFF):this->queue;
		// ENZ
		up = (y < milieu) ? ON : OFF;
		// ENZ
		if ( !note->m_stemDir == 0 )
			up = (up == ON) ? OFF : ON;
			
		if (up)
		{	
            y3 = y1 + m_doc->m_halfInterl[staff->staffSize]*6;
			y2 = y1;
		}
		v_bline ( dc, y2,y3,x2, m_doc->m_env.m_stemWidth );
	}

	return;
}

void MusRC::DrawBarline( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(staff->m_parent); // Pointer to system cannot be NULL"
    assert(dynamic_cast<MusBarline*>(element)); // Element must be a MusBarline"
    
    MusBarline *barline = dynamic_cast<MusBarline*>(element);
    int x = element->m_x_abs + barline->m_hOffset;

    dc->StartGraphic( element, "barline", Mus::StringFormat("s_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
    
    if (barline->m_barlineType==BARLINE_SINGLE)			
    {	
        DrawBarline( dc, (MusSystem*)staff->m_parent, x,  m_doc->m_env.m_barlineWidth, barline->m_onStaffOnly, staff);
    }
    else if (barline->m_partialBarline)
    {
        DrawPartialBarline ( dc, (MusSystem*)staff->m_parent, x, staff);
    }
    else
    {
        DrawSpecialBarline( dc, (MusSystem*)staff->m_parent, x, barline->m_barlineType, barline->m_onStaffOnly, staff);
    }
    
    dc->EndGraphic(element, this ); //RZ
}

void MusRC::DrawClef( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(dynamic_cast<MusClef*>(element)); // Element must be a MusClef"
    
    MusClef *clef = dynamic_cast<MusClef*>(element);

    dc->StartGraphic( element, "clef", Mus::StringFormat("s_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
	
	int b = (staff->m_y_drawing- m_doc->m_staffSize[ staff->staffSize ]);
	int a = element->m_x_abs;
    int sym = LEIPZIG_CLEF_G;	//sSOL, position d'ordre des cles sol fa ut in fonts

	if (staff->portNbLine > 5)
		b -= ((staff->portNbLine - 5) * 2) *m_doc->m_halfInterl[ staff->staffSize ]; // LP: I am not sure it works with any number of lines

    /*  poser sym=no de position sSOL dans la fonte
     *	au depart; ne faire operation sur b qu'une fois pour cas semblables,
     *  et au palier commun superieur, incrementer sym, sans break.
     */
	switch(clef->m_clefId)
	{
		case UT1 : 
            sym += 2;
		case SOL1 : 
            b -= m_doc->m_staffSize[ staff->staffSize ]; 
            break;
		case SOLva : 
            sym += 1;
		case UT2 : 
            sym += 2;
		case SOL2 : 
            b -= m_doc->m_interl[ staff->staffSize ]*3; 
            break;
		case FA3 : 
            sym--;
		case UT3 : 
            b -= m_doc->m_interl[ staff->staffSize ]*2; 
            sym += 2; 
            break;
		case FA5 : 
            sym++; 
            break;
		case FA4 : 
            sym--;
		case UT4 : 
            b -= m_doc->m_interl[ staff->staffSize ];
		case UT5 :  
            sym += 2; 
            break;
		case CLEPERC :  
            b -= m_doc->m_interl[ staff->staffSize ]*2;
            sym = LEIPZIG_CLEF_PERC; 
            break;
		default: 
            break;
	}

	a -= m_doc->m_step1*2;
	if (clef->m_cueSize)
		a+= m_doc->m_step1;

	DrawLeipzigFont ( dc, a, b, sym, staff, clef->m_cueSize  );
   
    dc->EndGraphic(element, this ); //RZ
}

void MusRC::DrawMensur( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(dynamic_cast<MusMensur*>(element)); // Element must be a MusMensur"

    MusMensur *mensur = dynamic_cast<MusMensur*>(element);
 
    dc->StartGraphic( element, "mensur", Mus::StringFormat("s_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
	
	int x, yp;
	float mDen=1.0, mNum=1.0;

	if (mensur->m_meterSymb)
	{	
		yp = staff->m_y_drawing - (m_doc->m_staffSize[ staff->staffSize ]+ m_doc->m_halfInterl[ staff->staffSize ]*6);
		
		unsigned char fontChar = LEIPZIG_METER_SYMB_COMMON;
		switch (mensur->m_meterSymb)
		{	
            case METER_SYMB_COMMON:
				fontChar = LEIPZIG_METER_SYMB_COMMON;
				mNum = 4; mDen = 4; break;
			case METER_SYMB_CUT:
				fontChar = LEIPZIG_METER_SYMB_CUT;
				mNum = 2; mDen = 2; break;
			case METER_SYMB_2:
				fontChar = LEIPZIG_METER_SYMB_2;
				mNum = 2; mDen = 2; break;
			case METER_SYMB_3:
				fontChar = LEIPZIG_METER_SYMB_3;
				mNum = 3; mDen = 2; break;
			case METER_SYMB_2_CUT:
				fontChar = LEIPZIG_METER_SYMB_2_CUT;	
				mNum = 4; mDen = 2; break;
			case METER_SYMB_3_CUT:
				fontChar = LEIPZIG_METER_SYMB_3_CUT;	
				mNum = 6; mDen = 2; break;
            default:
                break;
		}
		if ( dc )
		{	
			DrawLeipzigFont( dc, element->m_x_abs, yp, fontChar, staff, staff->staffSize);
		}
	}
	else
	{
		if (mensur->m_sign==MENSUR_SIGN_O)
		{	
            mNum = 2; mDen = 2;
			DrawMensurCircle ( dc, element->m_x_abs, staff->m_y_drawing, staff);
		}
		else if (mensur->m_sign==MENSUR_SIGN_C && !mensur->m_reversed)
		{	
            mNum = 2; mDen = 2;
			DrawMensurHalfCircle ( dc, element->m_x_abs, staff->m_y_drawing, staff);
		}
		else if (mensur->m_sign==MENSUR_SIGN_C && mensur->m_reversed)
		{	
            mNum = 4; mDen = 2;
			DrawMensurReversedHalfCircle ( dc, element->m_x_abs, staff->m_y_drawing, staff);
		}
		if (mensur->m_slash) // we handle only one single slash
		{	
            DrawMensurSlash ( dc, element->m_x_abs, staff->m_y_drawing, staff);
			mDen = 1;
		}
		if (mensur->m_dot) // we handle only one single dot
		{	
            DrawMensurDot (dc, element->m_x_abs, staff->m_y_drawing, staff);
			mNum = 9; mDen = 4;
		}
	}


	if (mensur->m_num)
	{	
        x = element->m_x_abs;
		if (mensur->m_sign || mensur->m_meterSymb) 
        {
			x += m_doc->m_step1*5; // step forward because we have a sign or a meter symbol
        }
		DrawMensurFigures ( dc, x, staff->m_y_drawing, mensur->m_num, mensur->m_numBase, staff);
		//mDen = max ( this->durDen, (unsigned short)1); // ax2
		//mNum = max ( this->durNum, (unsigned short)1); // ax2
	}
    /*
	MesVal = mNum / mDen;
	mesureNum = (int)mNum;
	mesureDen = (int)mDen;
    */ // ax2 
    
    dc->EndGraphic(element, this ); //RZ

}


void MusRC::DrawMensurCircle( MusDC *dc, int x, int yy, MusStaff *staff )
{
	assert( dc ); // DC cannot be NULL
	
	int y =  ToRendererY (yy - m_doc->m_interl[ staff->staffSize ] * 6);
	int r = ToRendererX( m_doc->m_interl[ staff->staffSize ]);

	int w = std::max( ToRendererX(4), 2 );

    dc->SetPen( m_currentColour, w, AxSOLID );
    dc->SetBrush( m_currentColour, AxTRANSPARENT );

	dc->DrawCircle( ToRendererX(x), y, r );

    dc->ResetPen();
    dc->ResetBrush();
}	

void MusRC::DrawMensurHalfCircle( MusDC *dc, int x, int yy, MusStaff *staff )
{
	assert( dc ); // DC cannot be NULL

	int w = std::max( ToRendererX(4), 2 );
    dc->SetPen( m_currentColour, w, AxSOLID );
    dc->SetBrush( m_currentColour, AxTRANSPARENT );

	int y =  ToRendererY (yy - m_doc->m_interl[ staff->staffSize ] * 5);
	int r = ToRendererX( m_doc->m_interl[ staff->staffSize ]);

	x = ToRendererX (x);
	x -= 3*r/3;

	dc->DrawEllipticArc( x, y, 2*r, 2*r, 70, 290 );
		
    dc->ResetPen();
    dc->ResetBrush();

	return;
}	

void MusRC::DrawMensurReversedHalfCircle( MusDC *dc, int x, int yy, MusStaff *staff )
{	
	assert( dc ); // DC cannot be NULL

	int w = std::max( ToRendererX(4), 2 );
    dc->SetPen( m_currentColour, w, AxSOLID );
    dc->SetBrush( m_currentColour, AxTRANSPARENT );

	int y =  ToRendererY (yy - m_doc->m_interl[ staff->staffSize ] * 5);
	int r = ToRendererX( m_doc->m_interl[ staff->staffSize ] );

	x = ToRendererX (x);
	x -= 4*r/3;

	dc->DrawEllipticArc( x, y, 2*r, 2*r, 250, 110 );
    
    dc->ResetPen();
    dc->ResetBrush();

	return;
}	

void MusRC::DrawMensurDot ( MusDC *dc, int x, int yy, MusStaff *staff )
{
	assert( dc ); // DC cannot be NULL

	int y =  ToRendererY (yy - m_doc->m_interl[ staff->staffSize ] * 6);
	int r = std::max( ToRendererX(4), 2 );
	
    dc->SetPen( m_currentColour, 1, AxSOLID );
    dc->SetBrush( m_currentColour, AxSOLID );

	dc->DrawCircle( ToRendererX(x) -r/2 , y, r );
		
    dc->ResetPen();
    dc->ResetBrush();

	return;
}	


void MusRC::DrawMensurSlash ( MusDC *dc, int a, int yy, MusStaff *staff )
{	
	assert( dc ); // DC cannot be NULL
	
	int y1 = yy - m_doc->m_staffSize[ staff->staffSize ];
	int y2 = y1 - m_doc->m_staffSize[ staff->staffSize ];
	
	v_bline2 ( dc, y1, y2, a, 3);
	return;
}	


void MusRC::DrawMensurFigures( MusDC *dc, int x, int y, int num, int numBase, MusStaff *staff)
{
	assert( dc ); // DC cannot be NULL	
    	
	int ynum, yden;
	std::string s;

	if (numBase)
	{	
		ynum = y - (m_doc->m_staffSize[staff->staffSize]+ m_doc->m_halfInterl[staff->staffSize]*4);
		yden = ynum - (m_doc->m_interl[staff->staffSize]*2);
	}
	else
		ynum = y - (m_doc->m_staffSize[staff->staffSize]+ m_doc->m_halfInterl[staff->staffSize]*6);

	if (numBase > 9 || num > 9)	// avancer
		x += m_doc->m_step1*2;

	s = Mus::StringFormat("%u",num);
	putstring ( dc, x, ynum, s, 1, staff->staffSize);	// '1' = centrer

	if (numBase)
	{
        s = Mus::StringFormat("%u",numBase);
		putstring ( dc, x, yden, s, 1, staff->staffSize);	// '1' = centrer
	}
	return;
}


void MusRC::DrawSymbol( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff, MusLayerElement *parent )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(dynamic_cast<MusSymbol*>(element)); // Element must be a MusSymbol"
    
    // This is used when we add dynamically an element (eg. accidentals before notes)
    // So we can get the clef without adding the new elem in the list
    MusLayerElement *list_elem = element;
    if (parent) list_elem = parent;
    
    MusSymbol *symbol = dynamic_cast<MusSymbol*>(element);
    int oct = symbol->m_oct - 4;
    element->m_y_drawing = CalculatePitchPosY( staff, symbol->m_pname, layer->GetClefOffset( list_elem ), oct);
    
    if (symbol->m_type==SYMBOL_ACCID) {
        DrawSymbolAccid(dc, element, layer, staff);
    }
    else if (symbol->m_type==SYMBOL_CUSTOS) {
        DrawSymbolCustos(dc, element, layer, staff);
    }
    else if (symbol->m_type==SYMBOL_DOT) {
        DrawSymbolDot(dc, element, layer, staff);
    }
    
}


void MusRC::DrawSymbolAccid( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(dynamic_cast<MusSymbol*>(element)); // Element must be a MusSymbol"
    
    MusSymbol *accid = dynamic_cast<MusSymbol*>(element);
    dc->StartGraphic( element, "accid", Mus::StringFormat("accid_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
    
    int x = element->m_x_abs + accid->m_hOffset;
    int y = element->m_y_drawing + staff->m_y_drawing;
    
    int symc;
    switch (accid->m_accid)
    {	case ACCID_NATURAL :  symc = LEIPZIG_ACCID_NATURAL; break;
        //case ACCID_DOUBLE_SHARP : symc = LEIPZIG_ACCID_DOUBLE_SHARP; DrawLeipzigFont ( dc, x, y, symc, staff, accid->m_cueSize );
        // so far, double sharp (and flat) have been used for key signature. This is poor design and should be fixed
        case ACCID_DOUBLE_SHARP : symc = LEIPZIG_ACCID_SHARP; DrawLeipzigFont ( dc, x, y, symc, staff, accid->m_cueSize );    
                    y += 7*m_doc->m_halfInterl[staff->staffSize]; // LP
        case ACCID_SHARP : symc = LEIPZIG_ACCID_SHARP; break;
        case ACCID_DOUBLE_FLAT :  symc = LEIPZIG_ACCID_FLAT; DrawLeipzigFont ( dc, x, y, symc, staff, accid->m_cueSize );
                    y += 7*m_doc->m_halfInterl[staff->staffSize]; // LP
        case ACCID_FLAT :  symc = LEIPZIG_ACCID_FLAT; break;
        case ACCID_QUARTER_SHARP : symc = LEIPZIG_ACCID_QUARTER_SHARP; break;
        case ACCID_QUARTER_FLAT :  symc= LEIPZIG_ACCID_QUARTER_FLAT; break;
    }
    DrawLeipzigFont ( dc, x, y, symc, staff, accid->m_cueSize );

    
    dc->EndGraphic(element, this ); //RZ

}


void MusRC::DrawSymbolCustos( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(dynamic_cast<MusSymbol*>(element)); // Element must be a MusSymbol"
    
    MusSymbol *custos = dynamic_cast<MusSymbol*>(element);
    dc->StartGraphic( element, "custos", Mus::StringFormat("custos_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );

    int x = element->m_x_abs + custos->m_hOffset;
    int y = element->m_y_drawing + staff->m_y_drawing;
    y -= m_doc->m_halfInterl[staff->staffSize] - m_doc->m_verticalUnit2[staff->staffSize];  // LP - correction in 2.0.0
    
    DrawLeipzigFont( dc, x, y, 35, staff, custos->m_cueSize );
    
    dc->EndGraphic(element, this ); //RZ

}

void MusRC::DrawSymbolDot( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(dynamic_cast<MusSymbol*>(element)); // Element must be a MusSymbol"
    
    MusSymbol *dot = dynamic_cast<MusSymbol*>(element);
    dc->StartGraphic( element, "dot", Mus::StringFormat("dot_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
    
    int x = element->m_x_abs + dot->m_hOffset;
    int y = element->m_y_drawing;

    switch (dot->m_dot)
    {	
        case 1 : DrawDot( dc,x,y,0,staff); x += std::max (6, m_doc->m_step1);
        case 0 : DrawDot ( dc,x,y,0,staff);
    }
    
    dc->EndGraphic(element, this ); //RZ

}

void MusRC::DrawKeySig( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff )
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"

    MusKeySig *ks = dynamic_cast<MusKeySig*>(element);
    int symb;
    int x, y;
    
    MusClef *c = layer->GetClef(element);
    if (!c) {
        return;
    }
    
    dc->StartGraphic( element, "keysig", Mus::StringFormat("keysig_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
    
    for (int i = 0; i < ks->m_num_alter; i++) {
        
        element->m_y_drawing = CalculatePitchPosY( staff, ks->GetAlterationAt(i), layer->GetClefOffset( element ), ks->GetOctave(ks->GetAlterationAt(i), c->m_clefId));
        
        x = element->m_x_abs + (m_doc->m_accidWidth[staff->staffSize][0] * i);
        y = element->m_y_drawing + staff->m_y_drawing;
        
        if (ks->m_alteration == ACCID_FLAT)
            symb = LEIPZIG_ACCID_FLAT;
        else
            symb = LEIPZIG_ACCID_SHARP;
        
        DrawLeipzigFont ( dc, x, y, symb, staff, false );
    }
    
    dc->EndGraphic(element, this ); //RZ
    
}

void MusRC::DrawTie( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff)
{
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    
    bool up = false;
    
    MusTie *t = dynamic_cast<MusTie*>(element);
    MusLayerElement *note1 = t->m_first;
    MusLayerElement *note2 = t->m_second;
    
    if (note1 == NULL || note2 == NULL)
        return;
    
    // Copied from DrawNote
    // We could use the stamDir information
    // but then we have to take in account (1) beams (2) stemmed and non stemmed notes tied together
    int ynn = note1->m_y_drawing + staff->m_y_drawing;
    int bby = staff->m_y_drawing - m_doc->m_staffSize[staff->staffSize];
    int milieu = bby - m_doc->m_interl[staff->staffSize] * 2;
    
    up = (ynn < milieu) ? true : false;
    
    dc->StartGraphic( element, "tie", Mus::StringFormat("tie_%d_%d_%d", staff->GetId(), layer->voix, element->GetId()) );
    
    // FIXME, take in account elements that can be netween notes, eg keys time etc
    
    if (up)
        DrawTieBezier(dc, ToRendererX(note1->m_x_abs), ToRendererY(note1->m_y_drawing + staff->m_y_drawing - 14), ToRendererX(note2->m_x_abs), 20, 6, true);
    else
        DrawTieBezier(dc, ToRendererX(note1->m_x_abs), ToRendererY(note1->m_y_drawing + staff->m_y_drawing + 14), ToRendererX(note2->m_x_abs), 20, 6, false);

    dc->EndGraphic(element, this ); //RZ
    
}

void MusRC::DrawAcciaccaturaSlash(MusDC *dc, MusLayerElement *element) {
    
    MusNote *note = dynamic_cast<MusNote*>(element);
    
    if (note->m_dur < DUR_8)
        return;
    
    dc->SetPen(AxBLACK, 2, AxSOLID);
    dc->SetBrush( AxBLACK, AxSOLID );
    
    if (element->m_drawn_stem_dir)
        dc->DrawLine(element->m_stem_start.x - 10, ToRendererY(element->m_stem_start.y + 10), element->m_stem_start.x + 20, ToRendererY(element->m_stem_start.y + 40));
    else
        dc->DrawLine(element->m_stem_start.x - 10, ToRendererY(element->m_stem_start.y - 10), element->m_stem_start.x + 20, ToRendererY(element->m_stem_start.y - 40));
    
    dc->ResetPen();
    dc->ResetBrush();
}

/** Draws a fermata
 rest - the fermata is always above the stavv
 note - for breves and semibreves, only above the staff
      - for flagged notes, the fermata is on the side of the notehead
 */
void MusRC::DrawFermata(MusDC *dc, MusLayerElement *element, MusStaff *staff) {
    int x, y;
    int emb_offset = 0; // if there is and embellishment, offset the note up
    
    // We position the fermata in the same horizontal pos. of th eobject
    x = element->m_x_abs;
    
    // First case, notes
    if (dynamic_cast<MusNote*>(element)) {
        MusNote *note = dynamic_cast<MusNote*>(element);
        
        // stem down or semibreve/longa, fermata up!
        if (!element->m_drawn_stem_dir && (note->m_dur != DUR_1 || note->m_dur != DUR_BR)) {
            
            // only for up-fermatas, if there is a trill on the same note
            // add a 35 pixel space so they do not collide
            if (note->m_embellishment)
                emb_offset = 35;
            
            // check that the notehead is in the staff.
            if ((element->m_y_drawing + staff->m_y_drawing) < staff->m_y_abs)
                // in the staff, set the fermata 20 pixels above the last line (+ embellishment offset)
                y = staff->m_y_abs + 20 + emb_offset;
            else
                // out of the staff, place the trill 20 px above the notehead
                y = (element->m_y_drawing + staff->m_y_drawing) + 20 + emb_offset;
            
            // draw the up-fermata
            DrawLeipzigFont ( dc, element->m_x_abs, y, LEIPZIG_FERMATA_UP, staff, false );
        } else { // stem up fermata down
            
            // This works as above, only we check that the note head is not
            // UNDER the staff
            if ((element->m_y_drawing + staff->m_y_drawing) > (staff->m_y_abs - m_doc->m_staffSize[staff->staffSize]))
                // notehead in staff, set at 20 px under
                y = staff->m_y_abs - m_doc->m_staffSize[staff->staffSize] - 20;
            else
                // notehead under staff, set 20 px under notehead
                y = (element->m_y_drawing + staff->m_y_drawing) - 20;
            
            DrawLeipzigFont ( dc, element->m_x_abs, y, LEIPZIG_FERMATA_DOWN, staff, false );
        }
    } else if (dynamic_cast<MusRest*>(element)) {
        // this is a rest
        // rests for the moment are always in the staff
        // so just place the fermata above the staff + 20 px
        y = staff->m_y_abs + 20;
        DrawLeipzigFont ( dc, element->m_x_abs, y, LEIPZIG_FERMATA_UP, staff, false );
    }
}

// Draw a trill above the notehead
// This function works as the up-fermata portion of DrawFermata
// if there are many symbols to draw we could make a generalized function
void MusRC::DrawTrill(MusDC *dc, MusLayerElement *element, MusStaff *staff) {
    int x, y;    
    x = element->m_x_abs;

    if ((element->m_y_drawing + staff->m_y_drawing) < staff->m_y_abs)
        y = staff->m_y_abs + 30;
    else
        y = (element->m_y_drawing + staff->m_y_drawing) + 30;
    
    DrawLeipzigFont ( dc, element->m_x_abs, y, LEIPZIG_EMB_TRILL, staff, false );
}


void MusRC::DrawLayerApp( MusDC *dc, MusLayerElement *element, MusLayer *layer, MusStaff *staff ){
    
    assert(layer); // Pointer to layer cannot be NULL"
    assert(staff); // Pointer to staff cannot be NULL"
    assert(staff->m_parent); // Pointer to staff system cannot be NULL"
    
    MusLayerApp *app = dynamic_cast<MusLayerApp*>(element);    
    int i;
    for (i = 0; i < app->GetRdgCount(); i++ )
    {
        MusLayer *rdg = (MusLayer*)app->m_children[i];
        int j;
        for (j = 0; j < rdg->GetElementCount(); j++ ) {
            MusLayerElement *lelem = (MusLayerElement*)rdg->m_children[j];
            if (i == 0) {
                m_currentColour = AxGREEN;
            }
            else {
                m_currentColour = AxBLUE;
            }
            DrawElement(dc, lelem, layer, staff );
            /*
            MusLayerElement rdgElement(&rdg->m_elements[j] );
            rdgElement.m_layer = element->m_layer;
            rdgElement.SetLayout( m_doc );
            rdgElement.m_x_abs = element->m_x_abs;
            DrawElement(dc, &rdgElement, layer, staff );
            rdgElement.m_layer = NULL;
            */
        }
        
        /*
        MusStaff *appStaff = new MusStaff( staff->m_logStaffNb );
        appStaff->m_y_drawing = staff->m_y_drawing + m_doc->m_staffSize[staff->staffSize];
        appStaff->m_system = staff->m_system;
        appStaff->SetLayout( m_doc );
        DrawStaff(dc, appStaff, staff->m_system );
        delete appStaff;
        */
    }
}

/*
 * Lyric code not refactored in ax2
    
void MusNote1::DeleteLyricFromNote( MusSymbol1 *lyric )
{
	for ( int i = 0; i < (int)this->m_lyrics.GetCount(); i++ ){
		MusSymbol1 *tmp = &this->m_lyrics[i];
		if ( lyric == tmp ){
			this->m_lyrics.RemoveAt( i );
			break;
		}
	}
}

void MusNote1::CheckLyricIntegrity( )
{
	this->m_lyrics.Sort( SortElements );
	int num = this->m_lyrics.GetCount();
	int i = 0;
	
	// Check for empty lyric elements
	while ( i < num ){
		MusSymbol1 *symbol = &this->m_lyrics[i];		
		if ( symbol->IsLyricEmpty() ){
			this->m_lyrics.RemoveAt( i );
			delete symbol;
			num = this->m_lyrics.GetCount();
		}
		else i++;
	}
	
	// Ensure elements have correct numbering within array
	num = this->m_lyrics.GetCount();
	for ( i = 0; i < num; i++ ){
		MusElement *element = &this->m_lyrics[i];
		element->no = i;
	}
}

MusSymbol1 *MusNote1::GetFirstLyric( )
{
	if ( this->m_lyrics.GetCount() == 0 ) 
		return NULL;
	else
		return &this->m_lyrics[0];
}

MusSymbol1 *MusNote1::GetLastLyric( )
{
	int num = this->m_lyrics.GetCount(); 
	if ( num == 0 ) 
		return NULL;
	else 
		return &this->m_lyrics[num-1];
}

MusSymbol1 *MusNote1::GetLyricNo( int no )
{
	int num = this->m_lyrics.GetCount(); 
	if ( (no < 0) || (num <= no) ) 
		return NULL;
	else 
		return &this->m_lyrics[no];
}

*/
