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
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=
AS=as.exe

# Macros
CND_PLATFORM=Cygwin_4.x-Windows
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/873296555/jidctint.o \
	${OBJECTDIR}/_ext/873296555/jdapimin.o \
	${OBJECTDIR}/_ext/873296555/jaricom.o \
	${OBJECTDIR}/_ext/1982019137/exf1api.o \
	${OBJECTDIR}/_ext/873296555/jdatadst.o \
	${OBJECTDIR}/_ext/873296555/jdapistd.o \
	${OBJECTDIR}/_ext/873296555/jdatasrc.o \
	${OBJECTDIR}/_ext/873296555/jdhuff.o \
	${OBJECTDIR}/_ext/873296555/jfdctint.o \
	${OBJECTDIR}/_ext/873296555/jdsample.o \
	${OBJECTDIR}/_ext/873296555/jdmaster.o \
	${OBJECTDIR}/_ext/873296555/jctrans.o \
	${OBJECTDIR}/_ext/873296555/jdarith.o \
	${OBJECTDIR}/_ext/873296555/jcsample.o \
	${OBJECTDIR}/_ext/873296555/jcmainct.o \
	${OBJECTDIR}/_ext/1982019137/libexf1.o \
	${OBJECTDIR}/_ext/873296555/jutils.o \
	${OBJECTDIR}/_ext/873296555/jdcolor.o \
	${OBJECTDIR}/_ext/873296555/jcprepct.o \
	${OBJECTDIR}/_ext/873296555/jdmainct.o \
	${OBJECTDIR}/_ext/873296555/jdcoefct.o \
	${OBJECTDIR}/_ext/873296555/jdmarker.o \
	${OBJECTDIR}/_ext/873296555/jcapistd.o \
	${OBJECTDIR}/_ext/873296555/jcmaster.o \
	${OBJECTDIR}/_ext/873296555/jquant2.o \
	${OBJECTDIR}/_ext/873296555/jddctmgr.o \
	${OBJECTDIR}/_ext/873296555/jcapimin.o \
	${OBJECTDIR}/_ext/873296555/jcinit.o \
	${OBJECTDIR}/_ext/873296555/jdpostct.o \
	${OBJECTDIR}/_ext/873296555/jdmerge.o \
	${OBJECTDIR}/_ext/873296555/jchuff.o \
	${OBJECTDIR}/_ext/873296555/jcomapi.o \
	${OBJECTDIR}/_ext/873296555/jfdctflt.o \
	${OBJECTDIR}/_ext/1982019137/exf1ctrl.o \
	${OBJECTDIR}/_ext/873296555/jquant1.o \
	${OBJECTDIR}/_ext/873296555/jmemmgr.o \
	${OBJECTDIR}/_ext/873296555/jdinput.o \
	${OBJECTDIR}/_ext/873296555/jdtrans.o \
	${OBJECTDIR}/_ext/873296555/jccoefct.o \
	${OBJECTDIR}/_ext/873296555/jcmarker.o \
	${OBJECTDIR}/_ext/873296555/jmemansi.o \
	${OBJECTDIR}/_ext/873296555/jccolor.o \
	${OBJECTDIR}/_ext/873296555/jfdctfst.o \
	${OBJECTDIR}/_ext/873296555/jcparam.o \
	${OBJECTDIR}/_ext/873296555/jcarith.o \
	${OBJECTDIR}/_ext/873296555/jidctflt.o \
	${OBJECTDIR}/_ext/873296555/jerror.o \
	${OBJECTDIR}/_ext/873296555/jidctfst.o \
	${OBJECTDIR}/_ext/873296555/jcdctmgr.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../../lib -lusb

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-Debug.mk ../../../bin/exf1Ctrl.exe

../../../bin/exf1Ctrl.exe: ${OBJECTFILES}
	${MKDIR} -p ../../../bin
	${LINK.cc} -o ../../../bin/exf1Ctrl ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/873296555/jidctint.o: ../../../src/jpeg-8b/jidctint.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jidctint.o ../../../src/jpeg-8b/jidctint.c

${OBJECTDIR}/_ext/873296555/jdapimin.o: ../../../src/jpeg-8b/jdapimin.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdapimin.o ../../../src/jpeg-8b/jdapimin.c

${OBJECTDIR}/_ext/873296555/jaricom.o: ../../../src/jpeg-8b/jaricom.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jaricom.o ../../../src/jpeg-8b/jaricom.c

${OBJECTDIR}/_ext/1982019137/exf1api.o: /cygdrive/C/Documents\ and\ Settings/a0866631/My\ Documents/Privat/exf1ctrl/src/exf1api.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1982019137
	${RM} $@.d
	$(COMPILE.cc) -g -I../../../include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1982019137/exf1api.o /cygdrive/C/Documents\ and\ Settings/a0866631/My\ Documents/Privat/exf1ctrl/src/exf1api.cpp

${OBJECTDIR}/_ext/873296555/jdatadst.o: ../../../src/jpeg-8b/jdatadst.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdatadst.o ../../../src/jpeg-8b/jdatadst.c

${OBJECTDIR}/_ext/873296555/jdapistd.o: ../../../src/jpeg-8b/jdapistd.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdapistd.o ../../../src/jpeg-8b/jdapistd.c

${OBJECTDIR}/_ext/873296555/jdatasrc.o: ../../../src/jpeg-8b/jdatasrc.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdatasrc.o ../../../src/jpeg-8b/jdatasrc.c

${OBJECTDIR}/_ext/873296555/jdhuff.o: ../../../src/jpeg-8b/jdhuff.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdhuff.o ../../../src/jpeg-8b/jdhuff.c

${OBJECTDIR}/_ext/873296555/jfdctint.o: ../../../src/jpeg-8b/jfdctint.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jfdctint.o ../../../src/jpeg-8b/jfdctint.c

${OBJECTDIR}/_ext/873296555/jdsample.o: ../../../src/jpeg-8b/jdsample.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdsample.o ../../../src/jpeg-8b/jdsample.c

${OBJECTDIR}/_ext/873296555/jdmaster.o: ../../../src/jpeg-8b/jdmaster.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdmaster.o ../../../src/jpeg-8b/jdmaster.c

${OBJECTDIR}/_ext/873296555/jctrans.o: ../../../src/jpeg-8b/jctrans.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jctrans.o ../../../src/jpeg-8b/jctrans.c

${OBJECTDIR}/_ext/873296555/jdarith.o: ../../../src/jpeg-8b/jdarith.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdarith.o ../../../src/jpeg-8b/jdarith.c

${OBJECTDIR}/_ext/873296555/jcsample.o: ../../../src/jpeg-8b/jcsample.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcsample.o ../../../src/jpeg-8b/jcsample.c

${OBJECTDIR}/_ext/873296555/jcmainct.o: ../../../src/jpeg-8b/jcmainct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcmainct.o ../../../src/jpeg-8b/jcmainct.c

${OBJECTDIR}/_ext/1982019137/libexf1.o: /cygdrive/C/Documents\ and\ Settings/a0866631/My\ Documents/Privat/exf1ctrl/src/libexf1.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1982019137
	${RM} $@.d
	$(COMPILE.cc) -g -I../../../include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1982019137/libexf1.o /cygdrive/C/Documents\ and\ Settings/a0866631/My\ Documents/Privat/exf1ctrl/src/libexf1.cpp

${OBJECTDIR}/_ext/873296555/jutils.o: ../../../src/jpeg-8b/jutils.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jutils.o ../../../src/jpeg-8b/jutils.c

${OBJECTDIR}/_ext/873296555/jdcolor.o: ../../../src/jpeg-8b/jdcolor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdcolor.o ../../../src/jpeg-8b/jdcolor.c

${OBJECTDIR}/_ext/873296555/jcprepct.o: ../../../src/jpeg-8b/jcprepct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcprepct.o ../../../src/jpeg-8b/jcprepct.c

${OBJECTDIR}/_ext/873296555/jdmainct.o: ../../../src/jpeg-8b/jdmainct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdmainct.o ../../../src/jpeg-8b/jdmainct.c

${OBJECTDIR}/_ext/873296555/jdcoefct.o: ../../../src/jpeg-8b/jdcoefct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdcoefct.o ../../../src/jpeg-8b/jdcoefct.c

${OBJECTDIR}/_ext/873296555/jdmarker.o: ../../../src/jpeg-8b/jdmarker.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdmarker.o ../../../src/jpeg-8b/jdmarker.c

${OBJECTDIR}/_ext/873296555/jcapistd.o: ../../../src/jpeg-8b/jcapistd.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcapistd.o ../../../src/jpeg-8b/jcapistd.c

${OBJECTDIR}/_ext/873296555/jcmaster.o: ../../../src/jpeg-8b/jcmaster.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcmaster.o ../../../src/jpeg-8b/jcmaster.c

${OBJECTDIR}/_ext/873296555/jquant2.o: ../../../src/jpeg-8b/jquant2.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jquant2.o ../../../src/jpeg-8b/jquant2.c

${OBJECTDIR}/_ext/873296555/jddctmgr.o: ../../../src/jpeg-8b/jddctmgr.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jddctmgr.o ../../../src/jpeg-8b/jddctmgr.c

${OBJECTDIR}/_ext/873296555/jcapimin.o: ../../../src/jpeg-8b/jcapimin.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcapimin.o ../../../src/jpeg-8b/jcapimin.c

${OBJECTDIR}/_ext/873296555/jcinit.o: ../../../src/jpeg-8b/jcinit.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcinit.o ../../../src/jpeg-8b/jcinit.c

${OBJECTDIR}/_ext/873296555/jdpostct.o: ../../../src/jpeg-8b/jdpostct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdpostct.o ../../../src/jpeg-8b/jdpostct.c

${OBJECTDIR}/_ext/873296555/jdmerge.o: ../../../src/jpeg-8b/jdmerge.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdmerge.o ../../../src/jpeg-8b/jdmerge.c

${OBJECTDIR}/_ext/873296555/jchuff.o: ../../../src/jpeg-8b/jchuff.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jchuff.o ../../../src/jpeg-8b/jchuff.c

${OBJECTDIR}/_ext/873296555/jcomapi.o: ../../../src/jpeg-8b/jcomapi.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcomapi.o ../../../src/jpeg-8b/jcomapi.c

${OBJECTDIR}/_ext/873296555/jfdctflt.o: ../../../src/jpeg-8b/jfdctflt.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jfdctflt.o ../../../src/jpeg-8b/jfdctflt.c

${OBJECTDIR}/_ext/1982019137/exf1ctrl.o: /cygdrive/C/Documents\ and\ Settings/a0866631/My\ Documents/Privat/exf1ctrl/src/exf1ctrl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1982019137
	${RM} $@.d
	$(COMPILE.cc) -g -I../../../include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1982019137/exf1ctrl.o /cygdrive/C/Documents\ and\ Settings/a0866631/My\ Documents/Privat/exf1ctrl/src/exf1ctrl.cpp

${OBJECTDIR}/_ext/873296555/jquant1.o: ../../../src/jpeg-8b/jquant1.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jquant1.o ../../../src/jpeg-8b/jquant1.c

${OBJECTDIR}/_ext/873296555/jmemmgr.o: ../../../src/jpeg-8b/jmemmgr.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jmemmgr.o ../../../src/jpeg-8b/jmemmgr.c

${OBJECTDIR}/_ext/873296555/jdinput.o: ../../../src/jpeg-8b/jdinput.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdinput.o ../../../src/jpeg-8b/jdinput.c

${OBJECTDIR}/_ext/873296555/jdtrans.o: ../../../src/jpeg-8b/jdtrans.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jdtrans.o ../../../src/jpeg-8b/jdtrans.c

${OBJECTDIR}/_ext/873296555/jccoefct.o: ../../../src/jpeg-8b/jccoefct.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jccoefct.o ../../../src/jpeg-8b/jccoefct.c

${OBJECTDIR}/_ext/873296555/jcmarker.o: ../../../src/jpeg-8b/jcmarker.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcmarker.o ../../../src/jpeg-8b/jcmarker.c

${OBJECTDIR}/_ext/873296555/jmemansi.o: ../../../src/jpeg-8b/jmemansi.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jmemansi.o ../../../src/jpeg-8b/jmemansi.c

${OBJECTDIR}/_ext/873296555/jccolor.o: ../../../src/jpeg-8b/jccolor.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jccolor.o ../../../src/jpeg-8b/jccolor.c

${OBJECTDIR}/_ext/873296555/jfdctfst.o: ../../../src/jpeg-8b/jfdctfst.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jfdctfst.o ../../../src/jpeg-8b/jfdctfst.c

${OBJECTDIR}/_ext/873296555/jcparam.o: ../../../src/jpeg-8b/jcparam.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcparam.o ../../../src/jpeg-8b/jcparam.c

${OBJECTDIR}/_ext/873296555/jcarith.o: ../../../src/jpeg-8b/jcarith.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcarith.o ../../../src/jpeg-8b/jcarith.c

${OBJECTDIR}/_ext/873296555/jidctflt.o: ../../../src/jpeg-8b/jidctflt.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jidctflt.o ../../../src/jpeg-8b/jidctflt.c

${OBJECTDIR}/_ext/873296555/jerror.o: ../../../src/jpeg-8b/jerror.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jerror.o ../../../src/jpeg-8b/jerror.c

${OBJECTDIR}/_ext/873296555/jidctfst.o: ../../../src/jpeg-8b/jidctfst.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jidctfst.o ../../../src/jpeg-8b/jidctfst.c

${OBJECTDIR}/_ext/873296555/jcdctmgr.o: ../../../src/jpeg-8b/jcdctmgr.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/873296555
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/873296555/jcdctmgr.o ../../../src/jpeg-8b/jcdctmgr.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} ../../../bin/exf1Ctrl.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
