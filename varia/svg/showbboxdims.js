var bb = null;
var getScreenBBox_impl = null;
var svgns = "http://www.w3.org/2000/svg";

function getScreenBBox(e) {
  if(e.getScreenBBox) {
    return e.getScreenBBox();
  } else if (getScreenBBox_impl) {
    return getScreenBBox_impl(e);
  }
}

if(!document.documentElement.getScreenBBox) {
     // load the fallback js implementation if necessary
    var s = document.createElementNS("http://www.w3.org/1999/xhtml", "script");
    s.src = "getscreenbbox.js";
    document.documentElement.appendChild(s);
}

var header = "";
var impl = "\n\
/**\n\
 * This class is used for getting the bounding box of the Leipzig glyphs.\n\
 * The values are used obtained with the ./varia/svg/split.xsl.\n\
 */\n\
class MusLeipzigBBox\n\
{\n\
public:\n\
	MusLeipzigBBox() {\n";

var items = document.documentElement.getElementsByTagName('path');
var i;
for (i = 0; i < items.length; i++) {
	item = items[i];
	label = "LEIPZIG_BBOX_" + item.getAttribute("id").toUpperCase();
	r = item.getBBox();
	
	header += "#define " + label + " " + i + "\n";
	
	impl += "m_bBox[" + label + "].m_x = " + r.x.toFixed(1) + ";\n";
	impl += "m_bBox[" + label + "].m_y = " + r.y.toFixed(1) + ";\n";
	impl += "m_bBox[" + label + "].m_width = " + r.width.toFixed(1) + ";\n";
	impl += "m_bBox[" + label + "].m_height = " + r.height.toFixed(1) + ";\n";
}

impl += "\n\
	};\n\
\n\
	struct BoundingBox\n\
	{\n\
		double m_x;\n\
		double m_y;\n\
		double m_width;\n\
		double m_height;\n\
	} m_bBox[" + i + "];\n\
};\n\
\n";

//alert(header);

var win = window.open( null, "win", "width=800,height=600"); // a window object
var doc = win.document;
doc.open("text/plain", "replace");
doc.write( header.replace(/\n/g, "<br/>") + "<br><br>" + impl.replace(/\n/g, "<br/>"));