#***************License************************************************
#
# 
#**********************************************************************
#
#
# UNIX make file 
#
#
#**********************************************************************/

#--------------------------------
# Required Environment Variables
#--------------------------------
SHELL = /bin/sh

PRODUCT_LIB_PREFIX=SB

EL4 = $(shell grep -c "release 4" /etc/redhat-release)
EL5 = $(shell grep -c "release 5" /etc/redhat-release)

BUILDROOT=.
SWISBSDK=$(BUILDROOT)
#XERCESDIR=$(BUILDROOT)/third_party

ifeq ($(EL4),1)
BOOST_LIB=../third_party/boost/lib
endif
ifeq ($(EL5), 1)
BOOST_LIB=../third_party/boost/lib/EL5
endif

BOOST_ROOT=../third_party/boost
BOOST_INC=../third_party/boost/include/boost-1_36/

#--------------------
# Platform macros
#--------------------
PROJ_ROOT = .

CPU = i386
OS = linux
#PLATFORM = $(CPU)-$(OS)

#MAKE_CPU = $(CPU)
#MAKE_OS = $(OS)

PERL = $(shell which perl)
MAKEFILE = Makefile

CC = gcc
CXX = g++

#-----------------------------------------
# Set up macros for different config's
#-----------------------------------------
ifndef CFG
CFG = debug
endif

ifeq ("$(CFG)","debug")
CFG_SUFFIX = D
CFLAGS_CFG = $(CFLAGS_DEBUG)
else
ifeq ("$(CFG)","release")
CFG_SUFFIX =
CFLAGS_CFG = $(CFLAGS_RELEASE)
else
$(error CFG is not a legal type - should be debug or release)
endif
endif

#---------------------
# Directories
#---------------------
PWD := $(shell pwd)
BUILD_ROOT := build
BUILD_ROOT_PLT = $(BUILD_ROOT)/$(PLATFORM)
BUILDDIR = $(CFG)
PLATFORMDIR = $(PROJ_ROOT)/$(PLATFORM)
#SRCPATH = $(PLATFORMDIR)/src

SRCPATH = . 
#
# SWISBSDK Install Directories
#
#bindir = $(SWISBSDK)/bin
#libdir = $(SWISBSDK)/lib
#incdir = $(SWISBSDK)/include

#
# Build Area directories
#
#buildbindir = $(BUILDDIR)/bin
buildlibdir = $(BUILDDIR)

#------------------------
# C/C++ compiler flags
#------------------------
CC = gcc
CXX = g++

CFLAGS_GENERAL = -D_$(CPU)_ \
	         -D_$(OS)_ \
	         -I$(BOOST_INC) \
	         -I./ \
	         -D_SIMPLE_R \
	         -D_REENTRANT \
	         -D_GNU_SOURCE \
	         -D__THREADED \
		 -D_cplusplus

ANSIFLAGS = -pedantic \
	    -Wall \
	    -Wcast-qual \
	    -Wcast-align \
	    -Wmissing-prototypes \
	    -Wstrict-prototypes \
	    -Wtraditional

C_ANSIFLAGS = $(ANSIFLAGS) \
	      -Wid-clash-31

CXX_ANSIFLAGS = $(ANSIFLAGS)

ifdef STRICT
CFLAGS_GENERAL += $(ANSIFLAGS)
endif

# -g     debug symbols
# -O0 	 No Optimization for debug
# -fPIC  Generate Position Independant code
CFLAGS_DEBUG = -g -O0 -fPIC

# -s     strip symbols
# -O2    Optimize for release
# -fPIC  Generate Position Independant code
CFLAGS_RELEASE = -DNDEBUG -s -O2 -fPIC

#----------------------------------------
# Link Flags
#----------------------------------------
#LDFLAGS += -L$(buildlibdir) $(PROJ_LDFLAGS) $($(TARGET)_LDFLAGS) $(LDFLAGS_$(OS)) -L$(BOOST_ROOT)/lib

@echo BOOST_LIB = $(BOOST_LIB)
LDFLAGS += -L$(buildlibdir) $(PROJ_LDFLAGS) $($(TARGET)_LDFLAGS) $(LDFLAGS_$(OS)) -L$(BOOST_LIB) 

LDFLAGS_SHARED = -shared
LDLIBS += $($(TARGET)_LDLIBS)
ifdef STATIC
LDLIBS += $($(TARGET)_LDLIBS_STATIC)
endif

LDLIBS += -lstdc++ -lpthread -ldl -lrt
LDFLAGS += -Wl,-rpath-link=$(SWISBSDK)/lib:$(PROJ_RPATH_LINK)

#--------------------------------
# General Programs
#--------------------------------
CHMOD = chmod
CHMODFLAGS = go-w,a-x
CHMODFLAGSX = go-w,a+x
INSTALL = cp -pf
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL)
RMDIR = rmdir
MKDIRS = ./mkinstalldirs 
RMCLEAN = rm -rf

#-------------------------------------------
# Final Values
#-------------------------------------------
CFLAGS = $(CFLAGS_CFG) $(CFLAGS_GENERAL) $(MYCFLAGS)
CXXFLAGS = $(CFLAGS_CFG) $(CFLAGS_GENERAL) $(MYCFLAGS)
CPPFLAGS = $(PROJ_CFLAGS) $(CFLAGS_$(OS)) $($(TARGET)_CFLAGS)

PROJ_CFLAGS = \
	-I"$(BOOST_INC)" 

#============================================================================
# Build sources
#============================================================================
PUBLIC_HEADERS = 

# Mask the exports of shared objects by explicitly listing public functions
EXPLICIT_VER_FILES = 1
 
# Set defaults for the library version and product name burnt into
# shared libraries
LIBVER = 1

# Define libaries to be build
LIBS = Mrcp 

# Define library sources
Mrcp_SRC = \
	AMrcpEventHandler.cpp \
        AMrcpObserverSubject.cpp \
        AMrcpResource.cpp \
        AMrcpResourceState.cpp \
	AMrcpSignaling.cpp \
	AMrcpSignalingState.cpp \
	CGenericCommands.cpp \
	CLogger.cpp \
	CMrcpCallbackTask.cpp \
        CMrcpCmdIdMgr.cpp \
	CMrcpEvent.cpp \
	CMrcpGeneric.cpp \
	CMrcpHandleMgr.cpp \
	CMrcpProcessCommandResults.cpp \
	CMrcpProcessSipMsgs.cpp \
        CRtspProcessMessage.cpp \
	CMrcpResourceState.cpp \
	CMrcpSendCommand.cpp \
	CMrcpSendSignal.cpp \
        CMrcpServer.cpp \
        CMrcpServerMgr.cpp \
	CMrcpSession.cpp \
	CMrcpSessionMgr.cpp \
	CMrcpSignalingStates.cpp \
	CMrcpSipSignal.cpp \
	CMrcpRtspSignal.cpp \
	CMrcpSystem.cpp \
	CMrcpSystemCInterface.cpp \
	CMrcpTaskProcessor.cpp \
	CRecognitionCommands.cpp \
	CSpeakCommands.cpp \
	CSpeechRecognizer.cpp \
	CSpeechSynthesizer.cpp \
        CMrcpSrvrWatcher.cpp \
        CWatcherSession.cpp \
	MrcpTasks.cpp \
	MrcpUtils.cpp \
#	Utility.cpp 

# Define any linked libraries
# THIS IS WRONG....SHOULDN"T INCLUDE TRD but is there something??
$(PRODUCT_LIB_PREFIX)Mrcp_LDLIBS = 

# Define version file
#$(PRODUCT_LIB_PREFIX)rec_VER = OSBrec.ver

#============================================================================
# Build Rules
#============================================================================
all_SRC := $(sort $(foreach target, $(PROGS) $(STATIC_LIBS) $(LIBS), $($(target)_SRC)))

libsubdirs := $(sort ./ $(foreach target, $(STATIC_LIBS) $(LIBS), $(dir $($(target)_SRC))))

#-------------------------------------------------
# Header file and Include Directories
#-------------------------------------------------
vpath lib%.a $(patsubst -L%, %, $(filter -L%, $(LDFLAGS))) \
	      $(subst :, , $(LD_LIBRARY_PATH)) $(STD_LIBRARY_PATH)

ifneq ($(SRCPATH),)
vpath %.c $(SRCPATH)
vpath %.cc $(SRCPATH)
vpath %.cpp $(SRCPATH)
vpath %.C $(SRCPATH)
vpath %.def $(SRCPATH)
vpath %.f $(SRCPATH)
vpath %.F $(SRCPATH)
vpath %.l $(SRCPATH)
vpath %.mod $(SRCPATH)
vpath %.p $(SRCPATH)
vpath %.r $(SRCPATH)
vpath %.s $(SRCPATH)
vpath %.sh $(SRCPATH)
vpath %.y $(SRCPATH)
endif

#--------------------------------------------------
# Default all target as all-shared
#--------------------------------------------------
all:  all-static
   @echo buildver = $(BUILDEL4)

#--------------------------------------------------
# .PHONY target declares all possible rules
#--------------------------------------------------
.PHONY: $(PROGS) $(STATIC_LIBS) $(LIBS) \
	$(addsuffix -shared, $(STATIC_LIBS) $(LIBS)) \
	$(addsuffix -static, $(STATIC_LIBS) $(LIBS)) \
	$(addsuffix -install, $(STATIC_LIBS) $(LIBS) $(PROGS)) \
	$(addsuffix -install-static, $(STATIC_LIBS) $(LIBS)) \
	$(addsuffix -install-shared, $(STATIC_LIBS) $(LIBS)) \
	$(addsuffix -install-progs, $(PROGS)) \
	$(buildbindir)/installprogsubdirs $(buildlibdir)/installlibsubdirs \
	installbuildbindir installbuildlibdir \
	installbindir installlibdir \
	all progs libs-static libs-shared \
	all-libs all-static all-shared \
	all-install headers clean

all-static: $(addsuffix -install-static, $(STATIC_LIBS) $(LIBS)) \
	    $(addsuffix -install, $(PROGS))

all-shared: $(addsuffix -install-static, $(STATIC_LIBS)) \
	    $(addsuffix -install-shared, $(LIBS)) \
	    $(addsuffix -install, $(PROGS))

progs: $(addsuffix -progs, $(PROGS))

libs: libs-static libs-shared

libs-static: $(addsuffix -static, $(STATIC_LIBS) $(LIBS))

libs-shared: $(addsuffix -static, $(STATIC_LIBS)) $(addsuffix -shared, $(LIBS))

#---------------------------------------------------------------------------
# headers copies all the public headers and scripts to the build area
#         (normally automatically done when a library, DLL, or program is 
#         successfully build)
#---------------------------------------------------------------------------

#---------------------------------------------------------------------
# clean deletes the entire directory tree for the current build config
#---------------------------------------------------------------------
clean: $(PROJ_CLEAN)
	$(RMCLEAN) $(BUILDDIR)
	@echo Cleaning up $(BUILDDIR)

#--------------------------------------------
# Rules to create (install) build subdirs
#--------------------------------------------
installbuildlibdir:
	$(MKDIRS) $(buildlibdir)
	-$(CHMOD) $(CHMODFLAGSX) $(buildlibdir)

#-----------------------------------------------
# Targets 
#-----------------------------------------------
$(addsuffix -install, $(PROGS)): %-install: installbindir %-progs
	$(RM) $(bindir)/$*$(CFG_SUFFIX)
#	$(INSTALL_PROGRAM) $(buildbindir)/$*$(CFG_SUFFIX) $(bindir)

$(addsuffix -install, $(STATIC_LIBS) $(LIBS)): %-install: %-install-static %-install-shared

$(addsuffix -install-static, $(STATIC_LIBS) $(LIBS)): %-install-static: installlibdir \
	                               %-static headers
	$(RM) $(libdir)/lib$*$(CFG_SUFFIX).a
#	$(INSTALL_DATA) $(buildlibdir)/lib$*$(CFG_SUFFIX).a $(libdir)

$(addsuffix -install-shared, $(LIBS)): %-install-shared: installlibdir \
	                               %-shared headers
	$(RM) $(libdir)/lib$*$(CFG_SUFFIX).so.$(LIBVER)
#	$(INSTALL_DATA) $(buildlibdir)/lib$*$(CFG_SUFFIX).so.$(LIBVER) $(libdir)
	$(RM) $(libdir)/lib$*$(CFG_SUFFIX).so
#	ln -s lib$*$(CFG_SUFFIX).so.$(LIBVER) $(libdir)/lib$*$(CFG_SUFFIX).so

#-------------------------------------------
# Program Targets
#--------------------------------------------
$(PROGS): %: %-progs

$(addsuffix -progs, $(PROGS)): %-progs: installbuildbindir \
	            $(buildbindir)/installbinsubdirs
	$(MAKE) -f $(MAKEFILE) $(buildbindir)/$*$(CFG_SUFFIX) TARGET=$* \
	DEPS=$*_SRC

#--------------------------------------------
# Library Targets
#--------------------------------------------
$(STATIC_LIBS): %: %-static %-shared

$(LIBS): %: %-static %-shared

$(addsuffix -static, $(STATIC_LIBS) $(LIBS)): %-static: installbuildlibdir \
		     $(buildlibdir)/installlibsubdirs
	$(MAKE) -f $(MAKEFILE) $(buildlibdir)/lib$*$(CFG_SUFFIX).a TARGET=$* \
	DEPS=$*_SRC

LDEXPORTS =
ifdef EXPLICIT_VER_FILES
LDEXPORTS = --version-script $($(TARGET)_VER)
endif
ifndef BUILDING_STUB_SO
LDFOLLOWLINKS = -Wl,-h,$(notdir $@).$(LIBVER)
endif
LDFLAGS_SHARED +=  $(LDEXPORTS) $(LDFOLLOWLINKS)

$(addsuffix -shared, $(LIBS)): %-shared: installbuildlibdir \
		     $(buildlibdir)/installlibsubdirs 
	$(MAKE) -f $(MAKEFILE) $(buildlibdir)/lib$*$(CFG_SUFFIX).so TARGET=$* \
	DEPS=$*_SRC

#-----------------------------------------
# Recursive rules
#------------------------------------------

$(addprefix $(buildlibdir)/, $(patsubst %, lib%$(CFG_SUFFIX).a, $(STATIC_LIBS) $(LIBS))): \
	    $(addprefix $(buildlibdir)/, \
	      $(patsubst %.s, %$(CFG_SUFFIX).o, $(filter %.s, $($(DEPS)))) \
	      $(patsubst %.c, %$(CFG_SUFFIX).o, $(filter %.c, $($(DEPS)))) \
	      $(patsubst %.cc, %$(CFG_SUFFIX).o, $(filter %.cc, $($(DEPS)))) \
	      $(patsubst %.cpp, %$(CFG_SUFFIX).o, $(filter %.cpp, $($(DEPS)))) \
	      $(patsubst %.C, %$(CFG_SUFFIX).o, $(filter %.C, $($(DEPS)))))
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $^
	-$(CHMOD) $(CHMODFLAGS) $@

$(addprefix $(buildlibdir)/, $(patsubst %, lib%$(CFG_SUFFIX).so, $(LIBS))): \
	    $(addprefix $(buildlibdir)/, \
	      $(patsubst %.s, %$(CFG_SUFFIX).o, $(filter %.s, $($(DEPS)))) \
	      $(patsubst %.c, %$(CFG_SUFFIX).o, $(filter %.c, $($(DEPS)))) \
	      $(patsubst %.cc, %$(CFG_SUFFIX).o, $(filter %.cc, $($(DEPS)))) \
	      $(patsubst %.cpp, %$(CFG_SUFFIX).o, $(filter %.cpp, $($(DEPS)))) \
	      $(patsubst %.C, %$(CFG_SUFFIX).o, $(filter %.C, $($(DEPS)))))

	$(LINK.cc) $(LDFLAGS_SHARED) $(filter %$(CFG_SUFFIX).o, $^) -o $@.$(LIBVER) $(LDLIBS)
	-$(CHMOD) $(CHMODFLAGSX) $@.$(LIBVER)
	$(RM) $@
	ln -s $(patsubst $(buildlibdir)/%,%, $@).$(LIBVER) $@

#---------------------------------------
# Object File compilation Rules
#----------------------------------------

$(buildbindir)/%$(CFG_SUFFIX).o: %.c
	$(COMPILE.c)  $(addprefix ${PWD}/,$<) -o $@
	-$(CHMOD) $(CHMODFLAGS) $@

$(buildbindir)/%$(CFG_SUFFIX).o: %.cc
	$(COMPILE.cc)  $(addprefix ${PWD}/, $<) -o $@
	-$(CHMOD) $(CHMODFLAGS) $@

$(buildbindir)/%$(CFG_SUFFIX).o: %.cpp
	$(COMPILE.cc)  $(addprefix ${PWD}/, $<) -o $@
	-$(CHMOD) $(CHMODFLAGS) $@

$(buildbindir)/%$(CFG_SUFFIX).o: %.C
	$(COMPILE.C)  $(addprefix ${PWD}/, $<) -o $@
	-$(CHMOD) $(CHMODFLAGS) $@

$(buildlibdir)/%$(CFG_SUFFIX).o: %.c
	$(COMPILE.c)  $(addprefix ${PWD}/, $<) -o $@ 
	-$(CHMOD) $(CHMODFLAGS) $@

$(buildlibdir)/%$(CFG_SUFFIX).o: %.cc
	$(COMPILE.cc) $(addprefix ${PWD}/, $<) -o $@
	-$(CHMOD) $(CHMODFLAGS) $@

$(buildlibdir)/%$(CFG_SUFFIX).o: %.cpp
	$(COMPILE.cc) $(addprefix ${PWD}/, $<) -o $@
	-$(CHMOD) $(CHMODFLAGS) $@

$(buildlibdir)/%$(CFG_SUFFIX).o: %.C
	$(COMPILE.C)  $(addprefix ${PWD}/, $<) -o $@
	-$(CHMOD) $(CHMODFLAGS) $@

$(buildlibdir)/%$(CFG_SUFFIX).o: %.s
	$(AS) $(ASFLAGS) -o $@ $<
	-$(CHMOD) $(CHMODFLAGS) $@
