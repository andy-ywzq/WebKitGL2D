set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Use -femit-struct-debug-baseonly to reduce the size of WebCore static library
    set(CMAKE_CXX_FLAGS_DEBUG "-g -femit-struct-debug-baseonly" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
endif ()

set(SHARED_CORE 0)
set(ENABLE_WEBKIT 0)
set(ENABLE_WEBKIT2 1)

set(WebKit2_OUTPUT_NAME WebKitNix)

add_definitions(-DBUILDING_NIX__=1)

# We set this because we now use Source/Platform/chromium
add_definitions(-DWEBKIT_IMPLEMENTATION=1)

find_package(Cairo 1.12.8 REQUIRED)
find_package(Fontconfig 2.8.0 REQUIRED)
find_package(Freetype REQUIRED)
find_package(GLIB 2.36.0 REQUIRED COMPONENTS gio gobject gmodule gthread)
find_package(HarfBuzz)
find_package(ICU REQUIRED)
find_package(JPEG REQUIRED)
find_package(LibXml2 2.6 REQUIRED)
find_package(LibXslt 1.1.7 REQUIRED)
find_package(PNG REQUIRED)
find_package(Sqlite REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

# Variable that must exists on CMake space
# to keep common CMake files working as desired for us
set(WTF_USE_ICU_UNICODE ON)
set(WTF_USE_LEVELDB ON)
set(ENABLE_API_TESTS ON)

WEBKIT_OPTION_DEFAULTS("NIX")

if (WTF_USE_CURL)
    find_package(CURL REQUIRED)
    set(REQUIRED_NETWORK libcurl)
else ()
    find_package(LibSoup 2.42.0 REQUIRED)
    set(REQUIRED_NETWORK libsoup-2.4)
endif ()

if (WTF_USE_OPENGL_ES_2)
    find_package(EGL REQUIRED)
    find_package(OpenGLES2 REQUIRED)
    add_definitions(-DWTF_USE_OPENGL_ES_2=1)
else ()
    find_package(X11 REQUIRED)
    find_package(OpenGL REQUIRED)
    add_definitions(-DHAVE_GLX=1)
endif ()

if (NOT ENABLE_SVG)
    set(ENABLE_SVG_FONTS OFF)
endif ()

WEBKIT_OPTION_DEFINE(WTF_USE_OPENGL_ES_2 "Use EGL + OpenGLES2" OFF)
WEBKIT_OPTION_DEFINE(WTF_USE_CURL "Use libCurl as network backend" OFF)
PROCESS_WEBKIT_OPTIONS()



