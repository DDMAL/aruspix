// Fake method stubs needed for tests to link

#include "wx/wx.h"
#include "mus/musrc.h"
#include "mus/musstaff.h"
#include "mus/musfile.h"
#include "mus/muspage.h"
#include "mus/mus.h"
#include "mus/musnote.h"

#include <exception>

class AxDC;

//-------------
// MusStaff
//-------------

int MusStaff::testcle(int x) {
    throw "fakemus stub";
}
int MusStaff::y_neume(int note, int dec_clef, int oct) {
    throw "fakemus stub";
}
int MusStaff::getOctCl ( MusElement *test, char *cle_id, int mlf ) {
    throw "fakemus stub";
}
void MusStaff::updat_pscle (int i, MusElement *chk) {
    throw "fakemus stub";
}



MusStaff::MusStaff() { throw "fakemus stub"; }
void MusStaff::Append(MusElement *element, int i) { throw "fakemus stub"; }
void MusStaff::GetMaxXY( wxArrayPtrVoid params ) { throw "fakemus stub"; }
void MusStaff::CheckIntegrity() { throw "fakemus stub"; }
MusStaff::~MusStaff() { throw "fakemus stub"; }
void MusStaff::CopyElements(wxArrayPtrVoid p) { throw "fakemus stub"; }


//-------------
// MusRC
//-------------

void MusRC::festa_string( AxDC *dc, int x, int y, const wxString str,
                       MusStaff *staff, int dimin ) { throw "fakemus stub"; }

void MusRC::box(AxDC *dc, int x1, int y1, int x2, int y2) { throw "fakemus stub"; }

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
// MusSymbol
//-------------

MusSymbol::MusSymbol() { throw "fakemus stub"; }
MusSymbol::~MusSymbol() { throw "fakemus stub"; }
bool MusSymbol::IsLyric() { throw "fakemus stub"; }
void MusSymbol::SetPitch(int p, int o) { throw "fakemus stub"; }
void MusSymbol::Draw(AxDC *dc, MusStaff *staff) { throw "fakemus stub"; }
void MusSymbol::SetValue(int i, MusStaff *m, int j) { throw "fakemus stub"; }

//-------------
// MusNote
//-------------

MusNote::MusNote() { throw "fakemus stub"; }
MusNote::~MusNote() { throw "fakemus stub"; }
void MusNote::SetPitch(int p, int o) { throw "fakemus stub"; }
void MusNote::Draw(AxDC *dc, MusStaff *staff) { throw "fakemus stub"; }
void MusNote::SetValue(int i, MusStaff *m, int j) { throw "fakemus stub"; }
void MusNote::ChangeStem(MusStaff *s) { throw "fakemus stub"; }
void MusNote::SetLigature(MusStaff *s) { throw "fakemus stub"; }
void MusNote::ChangeColoration(MusStaff *staff) { throw "fakemus stub"; }

//-------------
// Other
//-------------

ArrayOfMusStaves::~ArrayOfMusStaves() { throw "fakemus stub"; }
ArrayOfMusElements::~ArrayOfMusElements() { throw "fakemus stub"; }
ArrayOfMusSymbols::~ArrayOfMusSymbols() { throw "fakemus stub"; }
ArrayOfMusStaves::ArrayOfMusStaves(const ArrayOfMusStaves &other) { throw "fakemus stub"; }

MusHeaderFooter::MusHeaderFooter() { throw "fakemus stub"; }
MusHeaderFooter::~MusHeaderFooter() { throw "fakemus stub"; }

MusPagination::MusPagination() { throw "fakemus stub"; }
MusPagination::~MusPagination() { throw "fakemus stub"; }

MusFileHeader::MusFileHeader() { throw "fakemus stub"; }
MusFileHeader::~MusFileHeader() { throw "fakemus stub"; }

MusParameters::MusParameters() { throw "fakemus stub"; }
MusParameters::~MusParameters() { throw "fakemus stub"; }

