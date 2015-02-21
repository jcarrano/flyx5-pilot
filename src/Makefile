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
IPATH=-I${ROOT}/ext_libs -I${ROOT}/ext_libs/fixed_point/include

CPPFLAGS=-DFXP_C99_INLINE

# libraries
LDPATH=-L${ROOT}/ext_libs/driverlib -L${ROOT}/ext_libs/fixed_point
LIBRARIES=-ldrivers -lfxp

DEBUG_FLAGS=-dA -g3

#OFLAGS=-O2

# Include the common make definitions.
#
include ${TOOLSPATH}/Makefile.tiva
