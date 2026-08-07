cmake_minimum_required(VERSION 3.21)
project(aruspix CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies are provided by Conan; see conanfile.py + recipes/im/.
# Configure with `cmake --preset conan-release` after `conan install`.
find_package(im REQUIRED)
find_package(wxWidgets REQUIRED)
find_package(LibXml2 REQUIRED)

set(aruspixSources
    src/app/axabout_wdr.cpp
    src/app/axapp.cpp
    src/app/axapp_wdr.cpp
    src/app/axctrl.cpp
    src/app/axenv.cpp
    src/app/axfile.cpp
    src/app/axframe.cpp
    src/app/axgotodlg.cpp
    src/app/aximagecontroller.cpp
    src/app/aximage.cpp
    src/app/aximcontrol_wdr.cpp
    src/app/axoptionsdlg.cpp
    src/app/axoptions_wdr.cpp
    src/app/axprocess.cpp
    src/app/axprogressdlg.cpp
    src/app/axscrolledwindow.cpp
    src/app/axundo.cpp
    src/app/wxprec.cpp

    src/comparison/cmp.cpp
    src/comparison/cmpctrl.cpp
    src/comparison/cmpfile.cpp
    src/comparison/cmpmus.cpp
    src/comparison/cmp_wdr.cpp

    src/edition/edt.cpp
    src/edition/edtfile.cpp
    src/edition/edt_wdr_inc.cpp

    src/im/imbrink3classes.cpp
    src/im/imbrink.cpp
    src/im/imext.cpp
    src/im/imkmeans.cpp
    src/im/imoperator.cpp
    src/im/impage.cpp
    src/im/imregister.cpp
    src/im/imstaff.cpp
    src/im/imstaffsegment.cpp

    src/mus/musapp.cpp
    src/mus/musbarline.cpp
    src/mus/musbboxdc.cpp
    src/mus/musbeam.cpp
    src/mus/musclef.cpp
    src/mus/musdc.cpp
    src/mus/musdoc.cpp
    src/mus/musdurationinterface.cpp
    src/mus/musiobin.cpp
    src/mus/musio.cpp
    src/mus/musiodarms.cpp
    src/mus/musiomei.cpp
    src/mus/musiomlf.cpp
    src/mus/musiomusxml.cpp
    src/mus/musiopae.cpp
    src/mus/musiowwg.cpp
    src/mus/muskeysig.cpp
    src/mus/muslayer.cpp
    src/mus/muslayerelement.cpp
    src/mus/musleipzigbbox.cpp
    src/mus/musmensur.cpp
    src/mus/musneume.cpp
    src/mus/musneumesymbol.cpp
    src/mus/musnote.cpp
    src/mus/musmlfdic.cpp
    src/mus/musobject.cpp
    src/mus/muspage.cpp
    src/mus/muspitchinterface.cpp
    src/mus/muspositioninterface.cpp
    src/mus/musrc.cpp
    src/mus/musrc_beam.cpp
    src/mus/musrc_bezier.cpp
    src/mus/musrc_element.cpp
    src/mus/musrc_graph.cpp
    src/mus/musrc_page.cpp
    src/mus/musrc_neume.cpp
    src/mus/musrc_tuplet.cpp
    src/mus/musrest.cpp
    src/mus/musslur.cpp
    src/mus/musstaff.cpp
    src/mus/mussvgdc.cpp
    src/mus/mussymbol.cpp
    src/mus/mussystem.cpp
    src/mus/mustie.cpp
    src/mus/mustoolpanel.cpp
    src/mus/mustuplet.cpp
    src/mus/muswindow.cpp
    src/mus/muswwg.cpp
    src/mus/muswxdc.cpp

    src/recognition/recbookctrl.cpp
    src/recognition/recbookfile.cpp
    src/recognition/rec.cpp
    src/recognition/recfile.cpp
    src/recognition/recim.cpp
    src/recognition/recmlfbmp.cpp
    src/recognition/recmodels.cpp
    src/recognition/recmus.cpp
    src/recognition/rec_wdr.cpp

    src/superimposition/supbookctrl.cpp
    src/superimposition/supbookfile.cpp
    src/superimposition/sup.cpp
    src/superimposition/supfile.cpp
    src/superimposition/supim.cpp
    src/superimposition/sup_wdr.cpp

    src/tinyxml/tinystr.cpp
    src/tinyxml/tinyxml.cpp
    src/tinyxml/tinyxmlerror.cpp
    src/tinyxml/tinyxmlparser.cpp
)

if(WIN32)
    list(APPEND aruspixSources src/aruspix_uuid_win.cpp)
endif()

add_executable(aruspix ${aruspixSources})

# wxWidgets' wxIMPLEMENT_APP macro emits WinMain, not main, so the
# executable needs the GUI subsystem on Windows. WIN32_EXECUTABLE also
# wires up the right entry-point shim in the C runtime.
set_target_properties(aruspix PROPERTIES WIN32_EXECUTABLE TRUE)

target_include_directories(aruspix PRIVATE
    src
    src/app
    src/comparison
    src/recognition
    src/rtmidi
    src/edition
    src/superimposition
    src/tinyxml
    src/im
    src/ml
    src/mus
)

target_compile_definitions(aruspix PRIVATE
    AX_SUPERIMPOSITION
    AX_COMPARISON
    AX_EDT
    AX_RECOGNITION
    __AXDEBUG__
    _FILE_OFFSET_BITS=64
    _LARGE_FILES
)

if(MSVC)
    target_compile_options(aruspix PRIVATE /W3)
else()
    target_compile_options(aruspix PRIVATE -g -Wno-write-strings -Wno-format-security)
endif()

target_link_libraries(aruspix PRIVATE
    im::im
    wxWidgets::wxWidgets
    LibXml2::LibXml2
)

if(UNIX AND NOT APPLE)
    target_link_libraries(aruspix PRIVATE pthread uuid)
elseif(APPLE)
    target_link_libraries(aruspix PRIVATE pthread)
elseif(WIN32)
    # rpcrt4 provides UuidCreate.
    target_link_libraries(aruspix PRIVATE rpcrt4)
    # IM's upstream prebuilt im.lib statically embeds libtiff (and
    # libpng/libjpeg/zlib) — those symbols then collide with the
    # tiff/png/jpeg/zlib that wxWidgets pulls in transitively from
    # Conan. /FORCE:MULTIPLE lets the linker pick the first definition;
    # /IGNORE:4006 silences the resulting warning. The duplicates are
    # the same upstream code (libtiff 4.x etc.) so binding to either
    # copy is safe.
    target_link_options(aruspix PRIVATE /FORCE:MULTIPLE /IGNORE:4006 /IGNORE:4088)

    # fftw is built as a DLL on Windows (see conanfile.py) so IM's
    # im_fftw3.lib imports resolve. Copy every runtime DLL the aruspix
    # target depends on (fftw3f.dll, and anything else Conan exposes as
    # SHARED) into the output directory so the exe can find them at
    # launch without needing PATH tweaks.
    add_custom_command(TARGET aruspix POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:aruspix> $<TARGET_FILE_DIR:aruspix>
        COMMAND_EXPAND_LISTS
    )
endif()
