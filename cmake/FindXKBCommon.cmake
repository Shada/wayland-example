# - Try to find xkbcommon using pkg-config
#
# Sets the following variables:
#  XKB_COMMON_FOUND
#  XKB_COMMON_LIBRARIES
#  XKB_COMMON_INCLUDE_DIRS

find_package(PkgConfig REQUIRED)
pkg_check_modules(XKB_COMMON REQUIRED xkbcommon)

set(XKB_COMMON_FOUND ${XKB_COMMON_FOUND})
set(XKB_COMMON_LIBRARIES ${XKB_COMMON_LIBRARIES})
set(XKB_COMMON_INCLUDE_DIRS ${XKB_COMMON_INCLUDE_DIRS})
