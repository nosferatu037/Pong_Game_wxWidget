#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/AppFrame.o \
	${OBJECTDIR}/ball.o \
	${OBJECTDIR}/blocks.o \
	${OBJECTDIR}/character.o \
	${OBJECTDIR}/drawEngine.o \
	${OBJECTDIR}/level.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/pongplayer.o \
	${OBJECTDIR}/sprite.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-H `wx-config --cxxflags` 
CXXFLAGS=-H `wx-config --cxxflags` 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`wx-config --libs`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pong_game_wxwidget

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pong_game_wxwidget: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pong_game_wxwidget ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/AppFrame.o: AppFrame.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/AppFrame.o AppFrame.cpp

${OBJECTDIR}/ball.o: ball.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ball.o ball.cpp

${OBJECTDIR}/blocks.o: blocks.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/blocks.o blocks.cpp

${OBJECTDIR}/character.o: character.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/character.o character.cpp

${OBJECTDIR}/drawEngine.o: drawEngine.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/drawEngine.o drawEngine.cpp

${OBJECTDIR}/level.o: level.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/level.o level.cpp

${OBJECTDIR}/main.o: main.cpp stdwx.h.gch
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/pongplayer.o: pongplayer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/pongplayer.o pongplayer.cpp

${OBJECTDIR}/sprite.o: sprite.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sprite.o sprite.cpp

stdwx.h.gch: stdwx.h 
	@echo Performing Custom Build Step
	g++ `wx-config --cxxflags --libs all` -c -g -Wall -MMD -MP -MF ${OBJECTDIR}/stdwx.h.pch.d -o stdwx.h.gch stdwx.h

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/pong_game_wxwidget
	${RM} stdwx.h.gch

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
