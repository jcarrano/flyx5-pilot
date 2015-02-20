#******************************************************************************
#
# Makefile - Rules for building the project flyx5.
#
# Edited by Juan I Carrano
#

PROGRAM_NAME=flx5pilot

# Defines the part type that this project uses.
#
PART=TM4C1237H6PM	#for quad
#PART=TM4C123GH6PM	# for dev

# The base directory
#
ROOT=../
TOOLSPATH=../tools

# Where to find header files that do not live in the source directory.
#
IPATH=-I${ROOT}/ext_libs

# libraries
LDPATH=-L${ROOT}/ext_libs/driverlib
LIBRARIES=-ldrivers

DEBUG_FLAGS=-dA -g3

#OFLAGS=-O2

# Include the common make definitions.
#
include ${TOOLSPATH}/Makefile.tiva
