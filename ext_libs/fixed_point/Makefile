#******************************************************************************
#
# Makefile - Rules for building the project flyx5.
#
# Edited by Juan I Carrano
#

PROGRAM_NAME=libfxp

# Defines the part type that this project uses.
#
PART=TM4C1237H6PM

## Make a thin library
#
MAKE_LIBRARY=true
THIN_ARCHIVE=true

CPPFLAGS += -Dgcc

# The base directory
#
ROOT=../..
TOOLSPATH=../../tools
OUT_DIR=.

# Where to find header files that do not live in the source directory.
#
IPATH=-Iinclude

# Include the common make definitions.
#
include ${TOOLSPATH}/Makefile.tiva
