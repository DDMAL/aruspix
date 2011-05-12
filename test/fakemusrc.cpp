// Fake method stubs needed for musneume_test to compile

#include "wx/wx.h"
#include "mus/musrc.h"
#include "mus/musstaff.h"

class AxDC;

int MusStaff::testcle(int x) {
    return 0;
}

int MusStaff::y_neume(int note, int dec_clef, int oct) {
    return 0;
}

void MusRC::festa_string( AxDC *dc, int x, int y, const wxString str,
                       MusStaff *staff, int dimin ) {

}

void MusRC::box(AxDC *dc, int x1, int y1, int x2, int y2) {

}
