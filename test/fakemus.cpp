// Fake method stubs needed for tests to link

#include "wx/wx.h"
#include "mus/musrc.h"
#include "mus/musstaff.h"
#include "mus/musdoc.h"
#include "mus/muspage.h"
#include "mus/muswwg.h"
#include "mus/musnote.h"

#include <exception>

class MusDC;

//-------------
// MusLaidOutStaff
//-------------

int MusLaidOutStaff::GetClefOffset(int x) {
    throw "fakemus stub";
}
int MusLaidOutStaff::CalculateNeumePosY(int note, int dec_clef, int oct) {
    throw "fakemus stub";
}
int MusLaidOutStaff::GetClef ( MusElement *test, char *clefId, int mlf ) {
    throw "fakemus stub";
}
void MusLaidOutStaff::updat_pscle (int i, MusElement *chk) {
    throw "fakemus stub";
}



MusLaidOutStaff::MusLaidOutStaff() { throw "fakemus stub"; }
void MusLaidOutStaff::Append(MusElement *element, int i) { throw "fakemus stub"; }
void MusLaidOutStaff::GetMaxXY( wxArrayPtrVoid params ) { throw "fakemus stub"; }
void MusLaidOutStaff::CheckIntegrity() { throw "fakemus stub"; }
MusLaidOutStaff::~MusStaff() { throw "fakemus stub"; }
void MusLaidOutStaff::CopyElements(wxArrayPtrVoid p) { throw "fakemus stub"; }


//-------------
// MusRC
//-------------

void MusRC::festa_string( MusDC *dc, int x, int y, const wxString str,
                       MusLaidOutStaff *staff, int dimin ) { throw "fakemus stub"; }

void MusRC::box(MusDC *dc, int x1, int y1, int x2, int y2) { throw "fakemus stub"; }

int MusRC::ToRendererX(int) { throw "fakemus stub"; }
int MusRC::ToRendererY(int) { throw "fakemus stub"; }


//-------------
// MusPage
//-------------
MusPage::MusPage() { throw "fakemus stub"; }
MusPage::~MusPage() { throw "fakemus stub"; }
void MusPage::CheckIntegrity() { throw "fakemus stub"; }
void MusPage::Process(MusStaffFunctor *func, wxArrayPtrVoid p) { throw "fakemus stub"; }
void MusPage::CountVoices( wxArrayPtrVoid params ) { throw "fakemus stub"; }
void MusPage::ProcessVoices( wxArrayPtrVoid params ) { throw "fakemus stub"; }
//-------------
// MusSymbol1
//-------------

MusSymbol1::MusSymbol1() { throw "fakemus stub"; }
MusSymbol1::~MusSymbol() { throw "fakemus stub"; }
bool MusSymbol1::IsLyric() { throw "fakemus stub"; }
void MusSymbol1::SetPitch(int p, int o) { throw "fakemus stub"; }
void MusSymbol1::Draw(MusDC *dc, MusLaidOutStaff *staff) { throw "fakemus stub"; }
void MusSymbol1::SetValue(int i, MusLaidOutStaff *m, int j) { throw "fakemus stub"; }

//-------------
// MusNote1
//-------------

MusNote1::MusNote1() { throw "fakemus stub"; }
MusNote1::~MusNote() { throw "fakemus stub"; }
void MusNote1::SetPitch(int p, int o) { throw "fakemus stub"; }
void MusNote1::Draw(MusDC *dc, MusLaidOutStaff *staff) { throw "fakemus stub"; }
void MusNote1::SetValue(int i, MusLaidOutStaff *m, int j) { throw "fakemus stub"; }
void MusNote1::ChangeStem(MusLaidOutStaff *s) { throw "fakemus stub"; }
void MusNote1::SetLigature(MusLaidOutStaff *s) { throw "fakemus stub"; }
void MusNote1::ChangeColoration(MusLaidOutStaff *staff) { throw "fakemus stub"; }

//-------------
// Other
//-------------

ArrayOfMusStaves::~ArrayOfMusStaves() { throw "fakemus stub"; }
ArrayOfMusElements::~ArrayOfMusElements() { throw "fakemus stub"; }
ArrayOfMusSymbols::~ArrayOfMusSymbols() { throw "fakemus stub"; }
ArrayOfMusStaves::ArrayOfMusStaves(const ArrayOfMusStaves &other) { throw "fakemus stub"; }

MusWWGData::MusWWGData() { throw "fakemus stub"; }
MusWWGData::~MusHeaderFooter() { throw "fakemus stub"; }

MusPagination::MusPagination() { throw "fakemus stub"; }
MusPagination::~MusPagination() { throw "fakemus stub"; }

MusFileHeader::MusFileHeader() { throw "fakemus stub"; }
MusFileHeader::~MusFileHeader() { throw "fakemus stub"; }

MusParameters::MusParameters() { throw "fakemus stub"; }
MusParameters::~MusParameters() { throw "fakemus stub"; }

