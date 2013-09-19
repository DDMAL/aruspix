//
//  musiodarms.h
//  aruspix
//
//  Created by Rodolfo Zitellini on 02/07/12.
//  Copyright (c) 2012 com.aruspix.www. All rights reserved.
//

#ifndef aruspix_musiodarms_h
#define aruspix_musiodarms_h

#include <string>
#include <vector>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/wfstream.h"

#include "musdoc.h"
#include "muslayer.h"

#include "musbarline.h"
#include "musclef.h"
#include "musmensur.h"
#include "musneume.h"
#include "musnote.h"
#include "musrest.h"
#include "mussymbol.h"
#include "mustie.h"

//----------------------------------------------------------------------------
// MusDarmsInput
//----------------------------------------------------------------------------

#define ASCII_NUMBER_OFFSET 0x30 // for homebrewed char to int

typedef struct _pitchmap {
    unsigned int oct;
    unsigned int pitch; 
} pitchmap;

class MusDarmsInput: public MusFileInputStream
{
public:
    // constructors and destructors
    MusDarmsInput( MusDoc *doc, wxString filename );
    virtual ~MusDarmsInput();
    
    bool ImportFile( );    
    
private:
    int do_Note(int pos, const char* data, bool rest);
    int do_globalSpec(int pos, const char* data);
    int do_Clef(int pos, const char* data);
    int parseMeter(int pos, const char* data);
    void UnrollKeysig(int quantity, char alter);
    
public:
    
private:
    wxString m_filename;

	MusStaff *m_staff;
	MusLayer *m_layer;
    
    MusTie *m_current_tie;
    
    unsigned int m_clef_offset;
    bool m_antique_notation; // support square notation
    
    static pitchmap PitchMap[];
};


#endif
