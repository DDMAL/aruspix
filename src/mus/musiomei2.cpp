/////////////////////////////////////////////////////////////////////////////
// Name:        musiomei2.cpp
// Author:      Laurent Pugin, Alastair Porter
// Created:     2008, 2011
// Copyright (c) Laurent Pugin and Alastair Porter All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "musiomei2.h"
#endif

#include <algorithm>
using std::min;
using std::max;

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wx.h"

#include <libxml/tree.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include "wx/filename.h"
#include "wx/txtstrm.h"

#include "musneume.h"
#include "musneumesymbol.h"
#include "musiomei2.h"

// A Simple MEI input/output that can only read neume documents.
// Documents can only be written if they have first been imported - no support is
// given for creating them from scratch.

//----------------------------------------------------------------------------
// MusMeiOutput
//----------------------------------------------------------------------------

MusMeiOutput::MusMeiOutput( MusFile *file, wxString filename ) :
    // This is pretty bad. We open a bad fileoutputstream as we don't use it
	MusFileOutputStream( file, -1 )
{
	m_filename = filename;
}

MusMeiOutput::~MusMeiOutput()
{
}

bool MusMeiOutput::ExportFile( )
{
    if (m_file->GetMeiDocument() == NULL) {
        wxLogError("Unable to export a new document to MEI. Please import first");
        return false;
    }
    MeiDocument *doc = m_file->GetMeiDocument();
    string fname = m_filename.mb_str();
    doc->setDocName(fname);
    doc->WriteToXml(doc);
	return true;
}


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

MeiElement* FacsTable::GetZone(std::string key)
{
	MeiElement* result = NULL;
	for (vector<FacsEntry>::iterator i = entries.begin(); i != entries.end(); i++) {
		if (i->key == key) {
			result = i->zone;
		}
	}
	return result;
}

void FacsTable::add(std::string key, MeiElement *zone)
{
	FacsEntry entry;
	entry.key = key;
	entry.zone = zone;
	entries.push_back(entry);
}

void FacsTable::replace(std::string key, MeiElement *zone)
{
	vector<FacsEntry>::iterator i = entries.begin();
	while (i != entries.end()) {
		if (i->zone == zone) {
			i->key = key;
			break;
		} else {
			i++;
		}
	}
}

void MusMeiInput::ReadFacsTable(MeiElement *element, FacsTable *table)
{
	if (element->getChildren().size() > 0) {
		for (vector<MeiElement*>::iterator i = element->getChildren().begin(); i != element->getChildren().end(); i++) {
			MeiElement* e = *i;
			if (e->getName() == "zone") {
				std::string id;
				if (e->getId() != "") {
					id = e->getId();
				} else {
					throw "missing xml:id attribute on zone element";
				}
				table->add(id, e);
			} else if (e->getName() == "system") {
				std::string facs;
				if (e->getFacs() != NULL) {
					facs = e->getFacs()->getValue();
				} else {
					throw "missing facs on a system element";
				}
				std::string id;
				if (e->getId() != "") {
					id = e->getId();
				} else {
					throw "missing xml:id on a system element";
				}
				MeiElement* zone = table->GetZone(facs); //this will have already been assigned by now.
				if (zone != NULL) {
					table->replace(id,zone);
				} else {
					throw "zone not found in FacsTable";
				}
			}
			ReadFacsTable(*i,table);
		}
	}
}

void MusMeiInput::ReadElement(MeiElement *element, FacsTable *table)
{
	int insertx;
	if (element->getFacs() != NULL && element->getName() != "system") { //if it has a facs, there must be a corresponding zone for the MEI to validate...?
		std::string facs = element->getFacs()->getValue();
		MeiElement *zone = table->GetZone(facs);
		if (zone != NULL) {
			element->setZone(zone);
		} else {
			throw "no zone for facs!";
		}
		if (m_currentStaff) {
			if (m_currentStaff->GetLast()) {
				insertx = atoi(zone->getAttribute("ulx")->getValue().c_str())/2 - m_currentStaff->GetLast()->xrel;
			} else {
				insertx = atoi(zone->getAttribute("ulx")->getValue().c_str())/2;
			}
		}
	}
	if (element->getName() == "neume" || element->getName() == "custos") {
        if (element->getName() == "custos") {
			MeiAttribute *o = element->getAttribute("oct");
			//faking the oct attribute for the time being.
			if (o == NULL) {
				element->addAttribute(MeiAttribute("oct","4"));
			}
        }
        MusNeume *neume = new MusNeume(*element);
		neume->setMeiStaffZone(m_currentStaff->m_meiref->getZone());
        m_currentStaff->Append(neume, insertx);
    } else if (element->getName() == "division" || element->getName() == "clef" || element->getName() == "accid") {
        if (element->getName() == "accid") {
            MeiAttribute *o = element->getAttribute("oct");
            //faking oct attribute for the time being.
            if (o == NULL) {
                element->addAttribute(MeiAttribute("oct","4"));
            }
        }
        MusNeumeSymbol *neume_symbol = new MusNeumeSymbol(*element);
		neume_symbol->setMeiStaffZone(m_currentStaff->m_meiref->getZone());
        m_currentStaff->Append(neume_symbol, insertx);
    } else if (element->getName() == "sb") {
		std::string systemref = element->getAttribute("systemref")->getValue();
		MeiElement* zone = table->GetZone(systemref);
		element->setZone(zone);
        mei_staff(element, table);
		//} else if (element->getName() == "layer") {
		//    mei_layer(element);
    }
    for (vector<MeiElement*>::iterator i = element->getChildren().begin(); i != element->getChildren().end(); i++) {
        ReadElement(*i, table);
    }
}

bool MusMeiInput::ReadElement(MeiElement *element)
{
    FacsTable table;
	ReadFacsTable(element, &table);
    for (vector<MeiElement*>::iterator i = element->getChildren().begin(); i != element->getChildren().end(); i++) {
        ReadElement(*i, &table);
    }
    return true;
}

bool MusMeiInput::ImportFile( )
{
    string encoding = "utf-8";
    string fname = m_filename.mb_str();
    MeiDocument *doc = MeiDocument::ReadFromXml(fname, encoding);
    m_file->SetMeiDocument(doc);
    
    // When importing MEI, the first step is to load one single page and to load everything in one system
    MusPage *page = new MusPage();
    page->defin = 20;
    m_file->m_pages.Add( page );
    // To simplify accessibility, we keep a pointer on that pages
    m_page = &m_file->m_pages[0];

    MeiElement *root_element = doc->getRootElement();

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
    double max_y = 0;
    staffParams.Add( &max_x );
    staffParams.Add( &max_y );
    m_page->Process( &getMaxXY, staffParams );
    // change the staff width
    m_page->lrg_lign = max_x / 10;
    // add the space at the bottom - so far in max_y we have the top corner of the last staff
    if (m_page->m_staves.GetCount() > 0 ) {
        max_y += max((unsigned short)(&m_page->m_staves.Last())->portNbLine,(unsigned short)(&m_page->m_staves.Last())->ecart);
    }
    max_y *= m_page->defin; // transform this into coord.
    // update the page size
    m_file->m_fheader.param.pageFormatHor = max_x / 10 + (2 * m_file->m_fheader.param.MargeGAUCHEIMPAIRE);
    //m_file->m_fheader.param.pageFormatVer = max_y / 10 + (2 * m_file->m_fheader.param.MargeSOMMET);

    return true;
}

bool MusMeiInput::ReadAttributeBool( xmlNode *node, wxString name, bool *value, bool default_value) {
    return true;
}
bool MusMeiInput::ReadAttributeInt( xmlNode *node, wxString name, int *value, int default_value) {
    return true;
}
bool MusMeiInput::ReadAttributeString( xmlNode *node, wxString name, wxString *value, wxString default_value) {
    return true;
}

// method for reading mei elements

bool MusMeiInput::mei_accid( xmlNode *node ) {

    //MusNeumeSymbol *symbol = MusNeumeSymbol(node);
    //m_currentStaff->Append( symbol );
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

bool MusMeiInput::mei_layer(MeiElement *element) {

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

bool MusMeiInput::mei_staff(MeiElement *element, FacsTable *table) {
	
	//wxLogDebug("mei:staff\n");
    int n = atoi((element->getAttribute("n")->getValue()).c_str());
	int image = m_file->m_fheader.param.pageFormatVer * 10;
    //if (ReadAttributeInt( element, "n", &n )) {}
    wxLogDebug("mei:staff %d\n", n);
    if (n == (int)m_page->m_staves.Count() + 1) {
        // we tolerate that the staff has not been created in a scoredef
        // n must not be bigger that the previous id + 1;
        MusStaff *staff = new MusStaff();
        staff->portNbLine = 4; //added as experiment --Jamie
        staff->no = n - 1;
		staff->m_meiref = element;
		MeiElement *zone = staff->m_meiref->getZone();
		double ymid = (atoi(zone->getAttribute("lry")->getValue().c_str()) + atoi(zone->getAttribute("uly")->getValue().c_str()))/2.0;
        if (n == 1) {
            staff->ecart = (ymid*(1 + 20.0/image) - 30.0)/20.0 - 1.0; // for the first staff, we decrease the top space to account for the "border" on the MusFile
            m_file->m_fheader.param.MargeGAUCHEIMPAIRE = (int)((double)(atoi(zone->getAttribute("ulx")->getValue().c_str()))/10.0); // and the margin as well
        } else {
			MeiElement* zone2 = table->GetZone(m_page->m_staves[staff->no - 1].m_meiref->getAttribute("systemref")->getValue());
			double ymid2 = (atoi(zone2->getAttribute("lry")->getValue().c_str()) + atoi(zone2->getAttribute("uly")->getValue().c_str()))/2.0;
			staff->ecart = ((ymid - ymid2)*(1 + 20.0/image) - 60)/20.0 + 3.0;
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



