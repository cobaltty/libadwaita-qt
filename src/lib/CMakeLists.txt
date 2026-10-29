
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

configure_file(config-adwaita.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-adwaita.h )
configure_file(libadwaita.pc.cmake "${CMAKE_CURRENT_BINARY_DIR}/libadwaita${LIBADWAITA_SUFFIX}.pc" @ONLY)

set(adwaita6priv_SRCS
    # Animations
    animations/adwaitaanimation.cpp
    animations/adwaitaanimations.cpp
    animations/adwaitaanimationdata.cpp
    animations/adwaitabaseengine.cpp
    animations/adwaitabusyindicatordata.cpp
    animations/adwaitabusyindicatorengine.cpp
    animations/adwaitadialdata.cpp
    animations/adwaitadialengine.cpp
    animations/adwaitaenabledata.cpp
    animations/adwaitagenericdata.cpp
    animations/adwaitaheaderviewdata.cpp
    animations/adwaitaheaderviewengine.cpp
    animations/adwaitascrollbardata.cpp
    animations/adwaitascrollbarengine.cpp
    animations/adwaitaspinboxengine.cpp
    animations/adwaitaspinboxdata.cpp
    animations/adwaitastackedwidgetdata.cpp
    animations/adwaitastackedwidgetengine.cpp
    animations/adwaitatabbarengine.cpp
    animations/adwaitatabbardata.cpp
    animations/adwaitatoolboxengine.cpp
    animations/adwaitatransitiondata.cpp
    animations/adwaitatransitionwidget.cpp
    animations/adwaitawidgetstateengine.cpp
    animations/adwaitawidgetstatedata.cpp

    # Others
    adwaitaaddeventfilter.cpp
    adwaitamnemonics.cpp
    adwaitasplitterproxy.cpp
    adwaitawidgetexplorer.cpp
    adwaitawindowmanager.cpp
)

set(adwaita_SRCS
    adwaita.cpp
    adwaitacolors.cpp
    adwaitarenderer.cpp
)

set(adwaita_HEADERS
    adwaita.h
    adwaitacolors.h
    adwaitarenderer.h
    libadwaita_export.h
)

# Target name "adwaita6" → CMake auto-adds "lib" prefix → libadwaita6.so
add_library("adwaita${LIBADWAITA_SUFFIX}priv" SHARED ${adwaita6priv_SRCS})
target_link_libraries("adwaita${LIBADWAITA_SUFFIX}priv"
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
add_library("adwaita${LIBADWAITA_SUFFIX}" SHARED ${adwaita_SRCS})

target_link_libraries("adwaita${LIBADWAITA_SUFFIX}"
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE
    "adwaita${LIBADWAITA_SUFFIX}priv"
)

set_target_properties("adwaita${LIBADWAITA_SUFFIX}priv" PROPERTIES VERSION ${LIBADWAITA_VERSION} SOVERSION ${LIBADWAITA_VERSION_MAJOR})
if(MINGW AND BUILD_SHARED_LIBS)
    get_target_property(ADWAITA_SOVERSION "adwaita${LIBADWAITA_SUFFIX}priv" SOVERSION)
    set_target_properties("adwaita${LIBADWAITA_SUFFIX}priv" PROPERTIES SUFFIX "-${ADWAITA_SOVERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

set_target_properties("adwaita${LIBADWAITA_SUFFIX}" PROPERTIES VERSION ${LIBADWAITA_VERSION} SOVERSION ${LIBADWAITA_VERSION_MAJOR})
if(MINGW AND BUILD_SHARED_LIBS)
    get_target_property(ADWAITA_SOVERSION "adwaita${LIBADWAITA_SUFFIX}" SOVERSION)
    set_target_properties("adwaita${LIBADWAITA_SUFFIX}" PROPERTIES SUFFIX "-${ADWAITA_SOVERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

if (USE_QT6)
    target_include_directories("adwaita${LIBADWAITA_SUFFIX}" PUBLIC ${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${Qt6Widgets_INCLUDE_DIRS})
else()
    target_include_directories("adwaita${LIBADWAITA_SUFFIX}" PUBLIC ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
endif()

install(TARGETS "adwaita${LIBADWAITA_SUFFIX}priv" EXPORT "LibAdwaita${LIBADWAITA_SUFFIX}Targets" RUNTIME DESTINATION bin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS "adwaita${LIBADWAITA_SUFFIX}" EXPORT "LibAdwaita${LIBADWAITA_SUFFIX}Targets" RUNTIME DESTINATION bin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${adwaita_HEADERS} DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/Adwaita")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libadwaita${LIBADWAITA_SUFFIX}.pc" DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/cmake/AdwaitaQt${LIBADWAITA_SUFFIX}")

include(CMakePackageConfigHelpers)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/AdwaitaQtConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/AdwaitaQt${LIBADWAITA_SUFFIX}Config.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/AdwaitaQt${LIBADWAITA_SUFFIX}ConfigVersion.cmake"
  VERSION ${LIBADWAITA_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/AdwaitaQt${LIBADWAITA_SUFFIX}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/AdwaitaQt${LIBADWAITA_SUFFIX}ConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT "LibAdwaita${LIBADWAITA_SUFFIX}Targets" DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE "LibAdwaita${LIBADWAITA_SUFFIX}Targets.cmake")
