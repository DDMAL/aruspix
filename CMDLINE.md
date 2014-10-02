# Aruspix Command-line Set Up #

This document will outline the steps involved in compiling the Command-line version of Aruspix. The library dependencies is indentical as to the ones for the GUI version, with the exception of wxWidgets for which only the wxbase module is necessary. 

## Mac OS X ##

IM Lib (3.6) and Torch are available precompiled:

* [https://github.com/downloads/DDMAL/aruspix/imlib_osx10.6.tar.gz](https://github.com/downloads/DDMAL/aruspix/imlib_osx10.6.tar.gz)
* [https://github.com/downloads/DDMAL/aruspix/torch3_osx10.6.tar.gz](https://github.com/downloads/DDMAL/aruspix/torch3_osx10.6.tar.gz)

### wxWidgets Library (base) ###
	
* Create one directory within the wxWidgets directory: osx-static-base
* In the osx-static-base directory run the following commands:

Configure command:

	cd osx-static-base
	../configure LDFLAGS="-arch i386" OBJCFLAGS="-arch i386" OBJCXXFLAGS="-arch i386" CFLAGS="-arch i386" CXXFLAGS="-arch i386" CPPFLAGS="-arch i386" --disable-shared --with-libjpeg=builtin --with-libpng=builtin --disable-universal-binary --enable-debug --disable-gui --disable-unicode
	make clean (if necessary)
	make

###  Compilation of with XCode ###

For compiling the Command-line tool with XCode, you need to set the environment variables as for the GUI version. The values can refer to the GUI version ones of to other values if necessary. From OS X 10.9, they have to be set in the /etc/launchd.conf. 

Example:

    setenv ARUSPIX_WX_VERSION_CL 3.0
    # setenv ARUSPIX_WX_CL ARUSPIX_WX
    setenv ARUSPIX_WX_CL /Path/to/libs/wx3.0.1
    # setenv ARUSPIX_IMLIB_CL ARUSPIX_IMLIB 
    setenv ARUSPIX_IMLIB_CL /Path/to/libs/imlib3.6_10.6
    # setenv ARUSPIX_TORCH_CL ARUSPIX_TORCH
    setenv ARUSPIX_TORCH_CL /Path/to/libs/Torch3_10.6 

After the modification of the /etc/launchd.conf file, you need to restart your machine. Then open aruspix/osx/aruspix.xcodeproj with Xcode and select the aruspix-cmdline scheme. Aruspix should be ready to be compiled in both Debug and Release mode.

### Compilation with CMake ###

With CMake on OS X, you need to specify the paths to the libraries using -D parameters.

Run:

    cd cmd-line
    cmake . -DwxDir=/Path/to/libs/wx3.0.1/osx-static-base -DimDir=/Path/to/libs/imlib3.6_10.6 -DtorchDir=/Path/to/libs/Torch3_10.6
    make

## Linux (Ubuntu 12.04) Set Up ##

### wxWidgets Library (base) ###

Because Aruspix is not a Unicode application, wxWidgets cannot be installed using apt-get and needs to be compiled. The latest 2.8 version is available from [SourceForge](http://sourceforge.net/projects/wxwindows/files/2.8.12/wxGTK-2.8.12.tar.gz/download). 	

Configure command:

	mkdir gtk-build
	cd gtk-build
	../configure --disable-unicode
	make
	sudo make install

### IM Library ###

IMlib is available from [http://www.tecgraf.puc-rio.br/im/](http://www.tecgraf.puc-rio.br/im/). The version currently used (3.6) is available from [SourceForge](http://downloads.sourceforge.net/project/imtoolkit/3.6.3/Docs%20and%20Sources/im-3.6.3_Sources.tar.gz).


### Torch Library ###

IMlib is available from [http://www.tecgraf.puc-rio.br/im/](http://www.tecgraf.puc-rio.br/im/). The version currently used (3.6) is available from [SourceForge](http://downloads.sourceforge.net/project/imtoolkit/3.6.3/Docs%20and%20Sources/im-3.6.3_Sources.tar.gz).


### Other dependencies ###

Aside from the typical development utilities, a few additional libraries might have to be installed

	sudo apt-get install libgtk2.0-dev libxml2-dev libuuid-dev

### Compilation of Aruspix ###

	mkdir build
	cd build
	cmake ../linux -DwxDir=/usr/local/bin -DimDir=/path/to/imlib
	make

