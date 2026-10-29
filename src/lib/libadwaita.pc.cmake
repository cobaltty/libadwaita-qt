prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@CMAKE_INSTALL_FULL_INCLUDEDIR@

Name: libadwaita@LIBADWAITA_SUFFIX@
Description: Qt libadwaita Style
Version: @LIBADWAITA_VERSION@
Requires: @PC_REQUIRES@
@PC_REQUIRES_PRIVATE@

Libs: -L${libdir} -llibadwaita@LIBADWAITA_SUFFIX@
Cflags: -I${includedir}/Adwaita
Requires: Qt@QT_VERSION_MAJOR@Core Qt@QT_VERSION_MAJOR@Widgets
