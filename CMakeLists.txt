# ------------------------------------------------------------------------------
# LibCPPTools
#
# A library with helper functions and classes to use in C++ projects.
#
# Author: Javier Campón Pichardo
# Date: 30/07/2014
# Copyright 2014 Javier Campón Pichardo
#
# Distributeed under the terms of the GNU General Public License version 3 or
# later.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software. If not, see <http://www.gnu.org/licenses/>.
# ------------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
cmake_policy(VERSION 2.8)

# ---------------------
# Project configuration
# ---------------------
project("LibCPPTools")
# Include custom modules path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake modules")
# Install prefix
set(CMAKE_INSTALL_PREFIX "/usr")
find_package(ConfigPackage REQUIRED)
ConfigPackage("cpptools" "1.0.0.0" "org.kazires" "C++ Library with miscellaneous tools.")

# Colours for messages.
find_package(Colours)

# ----------------------
# Compiler configuration
# ----------------------
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# -----------------------
# Libraries configuration
# -----------------------
find_package(PkgConfig REQUIRED)

pkg_check_modules(GDKMM REQUIRED "gdkmm-3.0 >= 3.10.1")

include_directories(${GDKMM_INCLUDE_DIRS})
link_directories   (${GDKMM_LIBRARY_DIRS})
add_definitions    (${GDKMM_CFLAGS_OTHER})

## -----------
# Source code
# -----------
set(SOURCES
    src/htmltools.cpp
    src/resourcemanager.cpp)

set(HEADERS
    src/config.h
    src/htmltools.hpp
    src/resourcemanager.hpp)

add_library("${PACKAGE}" SHARED ${SOURCES} ${HEADERS})
add_executable(test src/main.cpp)
target_link_libraries("${PACKAGE}" ${GDKMM_LIBRARIES})
target_link_libraries(test "${PACKAGE}")

# ---------------------
# Doxygen documentation
# ---------------------
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOCUMENTATION_LANGUAGE English) # Documentation language.
    configure_file (${PROJECT_TEMPLATE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)

    # Use 'make doc' to generate the documentation.
    add_custom_target(doc ALL
                      ${DOXYGEN_EXECUTABLE} Doxyfile
                      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
                      COMMENT "Generating documentation with Doxygen" VERBATIM)
endif ()

# ------------
# Translations
# ------------
# - Creates a custom target named pot to create the PO Template file.
# - Creates custom targets for every existing translation. This targets will be
#   built within the ALL target.
#   The PO files with the translations must be at translations directory on the
#   project source directory.
#   The MO files will be created at locale directory in the project binary
#   directory.
#   If BUILD_TYPE value is RELEASE the mo files will be installed on install
#   target.
# -----------------------------------------------------------------------------

find_package(XGettext REQUIRED)

set(TRANSLATIONS_DIR "${PROJECT_SOURCE_DIR}/translations")

if(MSGFMT_FOUND)
    file(GLOB PO_FILES "${TRANSLATIONS_DIR}/*.po")

    foreach(po_file IN LISTS PO_FILES)
        get_filename_component(language "${po_file}" NAME_WE)

        add_custom_target("po-${language}" ALL
                          mkdir -p "${PROJECT_BINARY_DIR}/locale/${language}/LC_MESSAGES"
                          COMMAND "${MSGFMT_EXECUTABLE}"
                          "--output=${PROJECT_BINARY_DIR}/locale/${language}/LC_MESSAGES/${PACKAGE}.mo" "${po_file}"
                          WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                          COMMENT "Processing PO file for locale '${language}'..." VERBATIM)
    endforeach()
endif()

if(XGETTEXT_FOUND)
    set(COPYRIGHT_HOLDER "Javier Campón Pichardo")
    set(PACKAGE_BUG_REPORT javiercampon@gmail.com)

    add_custom_target(pot
                      mkdir -p "${TRANSLATIONS_DIR}"
                      COMMAND "${XGETTEXT_EXECUTABLE}"
                      "--default-domain=${PACKAGE}" "--output-dir=${TRANSLATIONS_DIR}" "--output=${PACKAGE}.pot"
                      --from-code=UTF-8 --language=C++ --keyword=_ --indent --sort-by-file
                      "--package-name=${PACKAGE}"
                      "--package-version=${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}"
                      "--copyright-holder=${COPYRIGHT_HOLDER}" "--msgid-bugs-address=${PACKAGE_BUG_REPORT}"
                      ${SOURCES} ${HEADERS}
                      COMMENT "Creating POT file at '${TRANSLATIONS_DIR}'...")

    message(STATUS "${Green}Use '${Blue}make pot${Green}' to create the PO Template file.${ColourReset}")
endif()

# --------------
# Install target
# --------------

# Install and uninstall targets will be available only in RELEASE build type.
if(CMAKE_BUILD_TYPE STREQUAL "RELEASE")
    set(GSCHEMA_DIR "${CMAKE_INSTALL_PREFIX}/share/glib-2.0/schemas")

    install(TARGETS ${PACKAGE}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)

    install(DIRECTORY "${PROJECT_SOURCE_DIR}/src/" DESTINATION "include/${PACKAGE}"
            FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "config.h" EXCLUDE)

    set_target_properties(${PACKAGE} PROPERTIES
                          VERSION   "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
                          SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

    if(EXISTS "${PROJECT_BINARY_DIR}/locale/")
        install(DIRECTORY "${PROJECT_BINARY_DIR}/locale/" DESTINATION "${LOCALE_DIR}"
                FILES_MATCHING PATTERN "*.mo")
    endif()

    if(EXISTS "${PROJECT_BINARY_DIR}/schemas/")
        install(DIRECTORY "${PROJECT_BINARY_DIR}/schemas/" DESTINATION "${GSCHEMA_DIR}" # Installing gschemas.
                FILES_MATCHING PATTERN "*.xml")
        install(CODE "execute_process(COMMAND glib-compile-schemas \"${GSCHEMA_DIR}\")") # Compiling gschemas.
    endif()

    if(EXISTS "${PROJECT_BINARY_DIR}/${PACKAGE}.desktop")
        install(FILES "${PROJECT_BINARY_DIR}/${PACKAGE}.desktop" DESTINATION "share/applications") # Desktop file for system menus/dash.
    endif()

    if(EXISTS "${PROJECT_BINARY_DIR}/lib${PACKAGE}.pc")
        install(FILES "${PROJECT_BINARY_DIR}/lib${PACKAGE}.pc" DESTINATION "lib/pkgconfig")
    endif()

    message(STATUS "${Green}Use '${Blue}make install${Green}' to install the application.${ColourReset}")

    # ----------------
    # Uninstall target
    # ----------------
    configure_file("${PROJECT_TEMPLATE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" @ONLY)
    add_custom_target(uninstall
                      COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
                      COMMENT "Deleting installed files...")

    message(STATUS "${Green}Use '${Blue}make uninstall${Green}' to uninstall the application.${ColourReset}")
endif()
