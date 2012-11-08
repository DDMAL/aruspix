# Aruspix Development Set Up #

## Introduction ##
Aruspix uses four libraries that have to be installed on your system for it to compile. 
They are the following:

* IM 3.x ([http://www.tecgraf.puc-rio.br/im/](http://www.tecgraf.puc-rio.br/im/)) 
* Torch 3.0 ([http://www.torch.ch/](http://www.torch.ch/))
* wxWidgets 2.8.x ([http://www.wxwidgets.org/](http://www.wxwidgets.org/))
* TinyXML ([http://www.grinninglizard.com/tinyxml/index.html/](http://www.grinninglizard.com/tinyxml/index.html/))
* LibMEI ([http://github.com/DDMAL/libmei](http://github.com/DDMAL/libmei))

This document will outline the steps involved in compiling these libraries 
for MacOS X and Windows XP operating systems.

For MacOS X, TinyXML is include in the project and does not need to be installed and compiled separately.

## Mac OS 10 Set Up (Using Xcode) ##

IM Lib (3.6) and Torch are available precompiled:

* [https://github.com/downloads/DDMAL/aruspix/imlib_osx10.6.tar.gz](https://github.com/downloads/DDMAL/aruspix/imlib_osx10.6.tar.gz)
* [https://github.com/downloads/DDMAL/aruspix/torch3_osx10.6.tar.gz](https://github.com/downloads/DDMAL/aruspix/torch3_osx10.6.tar.gz)

### wxWidgets Compilation ##
	
* Create two directories within the wxWidgets directory: osx-static, osx-static-debug.
* In the osx-static-debug directory run the following commands to compile for debug mode:

NOTE: this was tested with xcode 4.3, with command line tools installed (or wx will not compile).

Configure command:

	cd osx-static-debug
	../configure CFLAGS=-arch i386 CXXFLAGS=-arch i386 CPPFLAGS=-arch \
		i386 LDFLAGS=-arch i386 OBJCFLAGS=-arch i386 OBJCXXFLAGS=-arch i386 \
		--disable-shared --with-libjpeg=builtin --with-libpng=builtin --disable-universal-binary --enable-debug \
		--with-macosx-sdk=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk
	make clean (if necessary)
	make

In the osx-static directory, run the same commands but with:

	--disable-debug


### libmei ###

It currently uses a forked version of the official LibMEI that includes a layout customization and that can be linked statically. You get it from [https://github.com/lpugin/libmei](https://github.com/lpugin/libmei), and compile the "static" target in xcode (For the moment you need the "static" branch).
Once compiled, the ARUSPIX_MEI variable below needs to point to the mei compilation dir:

	$MEI_BASE_DIR/build/Debug/

### Setting Aruspix Environment Variables ###

In the aruspix/osx directory you will find an XML file named sample_enviroment.plist. This
file is used by Xcode to set the following environment variables for linking purposes:
	

* ARUSPIX_IMLIB: Location of IM library.
* ARUSPIX_TORCH: Location of Torch library.
* ARUSPIX_MEI: Location of libmei framework.
* ARUSPIX_WX: Location of wxWidgets library.
* ARUSPIX_WX_VERSION: wxWidgets library version number.


environment.plist prototype:
	
		<?xml version="1.0" encoding="UTF-8"?>
		<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
		<plist version="1.0">
		<dict>
		    <key>ARUSPIX_IMLIB</key>
		    <string>/Users/puginl/libs/imlib</string>
		    <key>ARUSPIX_TORCH</key>
		    <string>/Users/puginl/libs/Torch3</string>
		    <key>ARUSPIX_MEI</key>
		    <string>/Users/puginl/libs/libmei/build/Debug</string>
		    <key>ARUSPIX_WX</key>
		    <string>/Users/puginl/libs/wx2.8.11</string>
		    <key>ARUSPIX_WX_VERSION</key>
		    <string>2.8</string>
		</dict>
		</plist>


* You must modify the paths within this file to match your directory structure and rename it environment.plist.
* In your home directory create the following hidden directory: .MacOSX.
* Now copy the modified environment.plist file into the .MacOSX directory.
* You must now log out and log back in.
	
### Compilation of Machine Learning Executables used by Aruspix ###

* Open aruspix/osx/torch.xcodeproj with Xcode.
* Compile the following executables in both release and debug mode: adapt, decoder, ngram.

### Compilation of Aruspix ###

Open aruspix/osx/aruspix.xcodeproj with Xcode.
Aruspix should be ready to be compiled in both Debug and Release mode.


## Aruspix command line version on FreeBSD ##

In this case, only libmei and wxWidgets are needed. The aruspix_headless is an amiable target on XCode, so this procedure is needed only if installing on FreeBSD or Linux (not tested)

### wxWidgets ###
Download wxX11 version 2.8.x (any minor revision should do), and compile it:

	$ ./configure --disable-gui --enable-static --disable-shared --enable-monolithic
	$ make
	$ sudo make install
	
### libmei ###
Download libmei from GitHub as usual and compile it (it is not compiled static in this case):

	mkdir build; cd build
	cmake ..
	make
	sudo make install

### Aruspix ###
Get aruspix from GitHub, and compile using cmake as for libmei:

	mkdir build; cd build
	cmake ..
	make
	sudo make install


## Windows XP Set Up (Using Microsoft Visual C++ 6) ##

Note: Microsoft Visual C++ 6 has been the main Windows development tool up to 
date. However the project has also been compiled and run successfully using
Microsoft Visual C++ 8 following the same set up guide. Some of the project
properties may need to be adjusted as they are not always maintained when
upgrading a project. Refer to the trouble shooting section if problems arise.


### wxWidgets ###
To compile wxWidgets open

	wxWidgets-2.8.7\build\msw\wx.dsw
	
using VC 6. Select Batch Build from the Build drop down menu. Now select the
following three configurations from each section and build:

* Win32 DLL Debug
* Win32 Release
* Win32 Debug

### IM Library ###

This library is available pre-compiled. Just ensure that the library you've
downloaded has a file structure that matches the following example:

	\imlib
		\include : should contain all source files
		\lib : should contain all library files

### TinyXML ###

To compile this library simply open \begin{verbatim} tinyxml\tinyxml.dsw \end{verbatim}
with VC 6. Select Batch Build from the Build drop down menu and the build the
whole project. Now ensure that the file structure matches the following example:


	\tinyxml : Contains the tinyxml source files.
		\Debug : Contains the debug tinyxml lib file.
		\Release : Contains the release tinyxml lib file.
	
### Torch ###

After downloading Torch, follow the instructions provided on the Torch website
for compiling the library on Windows. Be sure to name the project "Torch3".
[http://www.torch.ch/matos/w_install.pdf](http://www.torch.ch/matos/w_install.pdf)
After compilation, make sure the Torch library folder has a file structure
that matches the following example:

	\Torch3
    	\Debug : Contains the Torch3.lib (Debug) file
    	\Release : Contains the Torch3.lib (Release) file
    	\src : Contains the Torch source files

Next you need to add the following environment variable to your system: 

* TORCH : Path to the Torch folder. 

To do this, right click on the My Computer icon and select properties. Then
select Environment Variables from the Advanced tab. Add the variable to the
"User variables" section.
Now open aruspix\win32\torch.dsw with VC 6.
Select batch build and build all the project configurations.
  
### Aruspix ###

To compile Aruspix the following environment variables must added to operating
system's configuration:

* IMLIB : Path of the IMlib folder
* TINYXML : Path of the TinyXML folder
* WXWIN : Path of the wxWidgets folder
* WXWIN_VERSION : wxWidgets version number (format ex. for 2.8.7: 28)

Now simply open aruspix/win32/Aruspix.dsw and you should be ready to compile Aruspix.

### Troubleshooting ###

If linking errors occur at any stage in the setup it is likely due to the code
generation settings of the projects. To fix the problem ensure that all the
libraries and projects are compiled using the same code generation settings
which can be found in: 
	
	project settings/properties -> C/C++ -> Code Generation -> Use run-time library

We have been using "Multithreaded DLL" for release versions and "Multithreaded
Debug DLL" for debug versions.


## Linux Set Up ##

### Dependencies ###

	sudo apt-get install libtorch3-dev libwxgtk2.8-dev wx-common libimlib2-dev libxml2-dev cmake

 IMlib is available from [http://www.tecgraf.puc-rio.br/im/](http://www.tecgraf.puc-rio.br/im/)

### Compilation ###


	mkdir build
	cd build
	cmake ../linux
	make


## Aruspix Source Code Organization ##

This file describe the source code organization of the project, with the directory tree.
A first group of directories contains files that are common to the application, as the
other directories are specific to the different workspace environment. Every workspace
environment is contained into a single directory that can be excluded from the application
by changing the preprocessor definitions.\\

### Common files ###

/app : This directory contains the files that constitute the core of the application.
The are prefixed with "ax". Most of the a related to GUI classes.\\\\
/im : This directory contains the files related to image processing. Classes and
function that extend some functionalities of the IMLIB image processing library,
such as advanced binarization methods, are to be find here.\\\\
/ml : This directory contains the files that relate to machine learning.\\\\
/mus : This directory contains the files that implement the music editor and handle the music files.

### Workspace environment files ###

Workspace environments can be enabled or disabled at compilation by changing
preprocessor definitions. The files generated with wxDesigner cannot be disabled
by changing the preprocessor definitions and have to be removed by hand (this
has to be fixed).
/empty : This directory contains a workspace template (May be out of date).
/superimposition : This directory contains the files that implement the superimposition
of digital music on to the original images.
/recognition : This directory contains the files that deal with the music recognition.
/comparison :

