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

var header = "/*\n\
 * This list is used for getting the bounding box of the Leipzig glyphs.\n\
 * The values were obtained with ./varia/svg/split.xsl and boundingbox.svg.\n\
 * It should not be modified by hand.\n\
 */\n";
var impl = header;

var items = document.documentElement.getElementsByTagName('path');
var i;
for (i = 0; i < items.length; i++) {
	item = items[i];
	label = "LEIPZIG_BBOX_" + item.getAttribute("id").toUpperCase();
	r = item.getBBox();
	
	// add the define to the header
	header += "#define " + label + " " + i + "\n";
	
	// add the bb value to the implementation
	impl += "m_bBox[" + label + "].m_x = " + r.x.toFixed(1) + ";\n";
	impl += "m_bBox[" + label + "].m_y = " + r.y.toFixed(1) + ";\n";
	impl += "m_bBox[" + label + "].m_width = " + r.width.toFixed(1) + ";\n";
	impl += "m_bBox[" + label + "].m_height = " + r.height.toFixed(1) + ";\n";
}

header += "#define LEIPZIG_GLYPHS " + i + "\n";
header += "/* end of the generated data */\n\n";
impl += "/* end of the generated data */\n\n";

// ouptut the content to be copied to the header file
console.log(header);
// output the content to be copied to the implementation file
console.log(impl);

/*
// open a new window for the output
var win = window.open( null, "win", "width=800,height=600"); // a window object
var doc = win.document;
doc.open("text/plain", "replace");
doc.write( header.replace(/\n/g, "<br/>") + "<br><br>" + impl.replace(/\n/g, "<br/>"));
*/
