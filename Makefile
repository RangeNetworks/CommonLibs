# Makefile.in generated by automake 1.9.4 from Makefile.am.
# CommonLibs/Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
# 2003, 2004  Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



#
# Copyright 2008, 2009 Free Software Foundation, Inc.
#
# This software is distributed under the terms of the GNU Public License.
# See the COPYING file in the main directory for details.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#
# Copyright 2008 Free Software Foundation, Inc.
#
# This software is distributed under the terms of the GNU Public License.
# See the COPYING file in the main directory for details.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#



SOURCES = $(libcommon_la_SOURCES) $(BitVectorTest_SOURCES) $(ConfigurationTest_SOURCES) $(F16Test_SOURCES) $(InterthreadTest_SOURCES) $(LogTest_SOURCES) $(RegexpTest_SOURCES) $(SocketsTest_SOURCES) $(TimevalTest_SOURCES) $(VectorTest_SOURCES)

srcdir = .
top_srcdir = ..

pkgdatadir = $(datadir)/openbts
pkglibdir = $(libdir)/openbts
pkgincludedir = $(includedir)/openbts
top_builddir = ..
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /opt/local/bin/ginstall -c
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_triplet = i686-apple-darwin10.8.0
host_triplet = i686-apple-darwin10.8.0
target_triplet = i686-apple-darwin10.8.0
DIST_COMMON = $(noinst_HEADERS) $(srcdir)/Makefile.am \
	$(srcdir)/Makefile.in $(top_srcdir)/Makefile.common
noinst_PROGRAMS = BitVectorTest$(EXEEXT) InterthreadTest$(EXEEXT) \
	SocketsTest$(EXEEXT) TimevalTest$(EXEEXT) RegexpTest$(EXEEXT) \
	VectorTest$(EXEEXT) ConfigurationTest$(EXEEXT) \
	LogTest$(EXEEXT) F16Test$(EXEEXT)
subdir = CommonLibs
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
mkinstalldirs = $(install_sh) -d
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
LTLIBRARIES = $(noinst_LTLIBRARIES)
libcommon_la_LIBADD =
am_libcommon_la_OBJECTS = BitVector.lo LinkedLists.lo Sockets.lo \
	Threads.lo Timeval.lo Logger.lo Configuration.lo
libcommon_la_OBJECTS = $(am_libcommon_la_OBJECTS)
PROGRAMS = $(noinst_PROGRAMS)
am_BitVectorTest_OBJECTS = BitVectorTest.$(OBJEXT)
BitVectorTest_OBJECTS = $(am_BitVectorTest_OBJECTS)
BitVectorTest_DEPENDENCIES = libcommon.la
am_ConfigurationTest_OBJECTS = ConfigurationTest.$(OBJEXT)
ConfigurationTest_OBJECTS = $(am_ConfigurationTest_OBJECTS)
am__DEPENDENCIES_1 = $(top_builddir)/sqlite/libsqlite.la
ConfigurationTest_DEPENDENCIES = libcommon.la $(am__DEPENDENCIES_1)
am_F16Test_OBJECTS = F16Test.$(OBJEXT)
F16Test_OBJECTS = $(am_F16Test_OBJECTS)
F16Test_LDADD = $(LDADD)
am_InterthreadTest_OBJECTS = InterthreadTest.$(OBJEXT)
InterthreadTest_OBJECTS = $(am_InterthreadTest_OBJECTS)
InterthreadTest_DEPENDENCIES = libcommon.la
am_LogTest_OBJECTS = LogTest.$(OBJEXT)
LogTest_OBJECTS = $(am_LogTest_OBJECTS)
LogTest_DEPENDENCIES = libcommon.la $(am__DEPENDENCIES_1)
am_RegexpTest_OBJECTS = RegexpTest.$(OBJEXT)
RegexpTest_OBJECTS = $(am_RegexpTest_OBJECTS)
RegexpTest_DEPENDENCIES = libcommon.la
am_SocketsTest_OBJECTS = SocketsTest.$(OBJEXT)
SocketsTest_OBJECTS = $(am_SocketsTest_OBJECTS)
SocketsTest_DEPENDENCIES = libcommon.la
am_TimevalTest_OBJECTS = TimevalTest.$(OBJEXT)
TimevalTest_OBJECTS = $(am_TimevalTest_OBJECTS)
TimevalTest_DEPENDENCIES = libcommon.la
am_VectorTest_OBJECTS = VectorTest.$(OBJEXT)
VectorTest_OBJECTS = $(am_VectorTest_OBJECTS)
VectorTest_DEPENDENCIES = libcommon.la
DEFAULT_INCLUDES = -I. -I$(srcdir) -I$(top_builddir)
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__depfiles_maybe = depfiles
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --tag=CXX --mode=compile $(CXX) $(DEFS) \
	$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(CXXLD) $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
SOURCES = $(libcommon_la_SOURCES) $(BitVectorTest_SOURCES) \
	$(ConfigurationTest_SOURCES) $(F16Test_SOURCES) \
	$(InterthreadTest_SOURCES) $(LogTest_SOURCES) \
	$(RegexpTest_SOURCES) $(SocketsTest_SOURCES) \
	$(TimevalTest_SOURCES) $(VectorTest_SOURCES)
DIST_SOURCES = $(libcommon_la_SOURCES) $(BitVectorTest_SOURCES) \
	$(ConfigurationTest_SOURCES) $(F16Test_SOURCES) \
	$(InterthreadTest_SOURCES) $(LogTest_SOURCES) \
	$(RegexpTest_SOURCES) $(SocketsTest_SOURCES) \
	$(TimevalTest_SOURCES) $(VectorTest_SOURCES)
HEADERS = $(noinst_HEADERS)
ETAGS = etags
CTAGS = ctags
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
ACLOCAL = ${SHELL} /Users/dburgess/OpenBTSProject/range/software/private/openbts/trunk-public-staging/missing --run aclocal-1.9
AMDEP_FALSE = #
AMDEP_TRUE = 
AMTAR = ${SHELL} /Users/dburgess/OpenBTSProject/range/software/private/openbts/trunk-public-staging/missing --run tar
AR = ar
AS = as
AUTOCONF = ${SHELL} /Users/dburgess/OpenBTSProject/range/software/private/openbts/trunk-public-staging/missing --run autoconf
AUTOHEADER = ${SHELL} /Users/dburgess/OpenBTSProject/range/software/private/openbts/trunk-public-staging/missing --run autoheader
AUTOMAKE = ${SHELL} /Users/dburgess/OpenBTSProject/range/software/private/openbts/trunk-public-staging/missing --run automake-1.9
AWK = gawk
CC = gcc
CCAS = gcc
CCASFLAGS = -g -O2
CCDEPMODE = depmode=gcc3
CFLAGS = -g -O2
CPP = gcc -E
CPPFLAGS = 
CXX = g++
CXXCPP = g++ -E
CXXDEPMODE = depmode=gcc3
CXXFLAGS = -g -O2
CYGPATH_W = echo
DEFS = -DHAVE_CONFIG_H
DEPDIR = .deps
DLLTOOL = dlltool
ECHO = /bin/echo
ECHO_C = \c
ECHO_N = 
ECHO_T = 
EGREP = /usr/bin/grep -E
EXEEXT = 
F77 = gfortran
FFLAGS = -g -O2
GREP = /usr/bin/grep
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
LDFLAGS = 
LIBOBJS = 
LIBS = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
LTLIBOBJS = 
MAKEINFO = ${SHELL} /Users/dburgess/OpenBTSProject/range/software/private/openbts/trunk-public-staging/missing --run makeinfo
OBJDUMP = objdump
OBJEXT = o
ORTP_CFLAGS = -D_REENTRANT -DORTP_INET6 -I/usr/local/include  
ORTP_LIBS = -L/usr/local/lib -lortp -lpthread -lssl -lcrypto  
OSIP_CFLAGS = -DOSIP_MT -I/usr/local/include  
OSIP_LIBS = -L/usr/local/lib -losipparser2 -losip2  
PACKAGE = openbts
PACKAGE_BUGREPORT = 
PACKAGE_NAME = openbts
PACKAGE_STRING = openbts P2.8TRUNK
PACKAGE_TARNAME = openbts
PACKAGE_VERSION = P2.8TRUNK
PATH_SEPARATOR = :
PKG_CONFIG = /sw/bin/pkg-config
RANLIB = ranlib
RM_PROG = /bin/rm
SET_MAKE = 
SHELL = /bin/sh
STRIP = strip
USRP_CFLAGS = @USRP_CFLAGS@
USRP_LIBS = @USRP_LIBS@
VERSION = P2.8TRUNK
ac_ct_CC = gcc
ac_ct_CXX = g++
ac_ct_F77 = gfortran
am__fastdepCC_FALSE = #
am__fastdepCC_TRUE = 
am__fastdepCXX_FALSE = #
am__fastdepCXX_TRUE = 
am__include = include
am__leading_dot = .
am__quote = 
am__tar = ${AMTAR} chof - "$$tardir"
am__untar = ${AMTAR} xf -
bindir = ${exec_prefix}/bin
build = i686-apple-darwin10.8.0
build_alias = 
build_cpu = i686
build_os = darwin10.8.0
build_vendor = apple
datadir = ${datarootdir}
datarootdir = ${prefix}/share
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
dvidir = ${docdir}
exec_prefix = ${prefix}
host = i686-apple-darwin10.8.0
host_alias = 
host_cpu = i686
host_os = darwin10.8.0
host_vendor = apple
htmldir = ${docdir}
includedir = ${prefix}/include
infodir = ${datarootdir}/info
install_sh = /Users/dburgess/OpenBTSProject/range/software/private/openbts/trunk-public-staging/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localedir = ${datarootdir}/locale
localstatedir = ${prefix}/var
mandir = ${datarootdir}/man
mkdir_p = $(install_sh) -d
oldincludedir = /usr/include
pdfdir = ${docdir}
prefix = /usr/local
program_transform_name = s,x,x,
psdir = ${docdir}
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
sysconfdir = ${prefix}/etc
target = i686-apple-darwin10.8.0
target_alias = 
target_cpu = i686
target_os = darwin10.8.0
target_vendor = apple
COMMON_INCLUDEDIR = $(top_srcdir)/CommonLibs
CONTROL_INCLUDEDIR = $(top_srcdir)/Control
GSM_INCLUDEDIR = $(top_srcdir)/GSM
SIP_INCLUDEDIR = $(top_srcdir)/SIP
SMS_INCLUDEDIR = $(top_srcdir)/SMS
TRX_INCLUDEDIR = $(top_srcdir)/TRXManager
GLOBALS_INCLUDEDIR = $(top_srcdir)/Globals
CLI_INCLUDEDIR = $(top_srcdir)/CLI
SQLITE_INCLUDEDIR = $(top_srcdir)/sqlite
HLR_INCLUDEDIR = $(top_srcdir)/HLR
STD_DEFINES_AND_INCLUDES = \
	-I$(COMMON_INCLUDEDIR) \
	-I$(CONTROL_INCLUDEDIR) \
	-I$(GSM_INCLUDEDIR) \
	-I$(SIP_INCLUDEDIR) \
	-I$(SMS_INCLUDEDIR) \
	-I$(TRX_INCLUDEDIR) \
	-I$(GLOBALS_INCLUDEDIR) \
	-I$(CLI_INCLUDEDIR) \
	-I$(HLR_INCLUDEDIR) \
	-I$(SQLITE_INCLUDEDIR)

COMMON_LA = $(top_builddir)/CommonLibs/libcommon.la
GSM_LA = $(top_builddir)/GSM/libGSM.la
SIP_LA = $(top_builddir)/SIP/libSIP.la
SMS_LA = $(top_builddir)/SMS/libSMS.la
TRX_LA = $(top_builddir)/TRXManager/libtrxmanager.la
CONTROL_LA = $(top_builddir)/Control/libcontrol.la
GLOBALS_LA = $(top_builddir)/Globals/libglobals.la
CLI_LA = $(top_builddir)/CLI/libcli.la
HLR_LA = $(top_builddir)/HLR/libHLR.la
SQLITE_LA = $(top_builddir)/sqlite/libsqlite.la
MOSTLYCLEANFILES = *~ testSource testDestination
AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES)
AM_CXXFLAGS = -Wall -O3 -g -lpthread
EXTRA_DIST = \
	example.config \
	README.common

noinst_LTLIBRARIES = libcommon.la
libcommon_la_SOURCES = \
	BitVector.cpp \
	LinkedLists.cpp \
	Sockets.cpp \
	Threads.cpp \
	Timeval.cpp \
	Logger.cpp \
	Configuration.cpp

noinst_HEADERS = \
	BitVector.h \
	Interthread.h \
	LinkedLists.h \
	Sockets.h \
	Threads.h \
	Timeval.h \
	Regexp.h \
	Vector.h \
	Configuration.h \
	F16.h \
	Logger.h

BitVectorTest_SOURCES = BitVectorTest.cpp
BitVectorTest_LDADD = libcommon.la
InterthreadTest_SOURCES = InterthreadTest.cpp
InterthreadTest_LDADD = libcommon.la
InterthreadTest_LDFLAGS = -lpthread
SocketsTest_SOURCES = SocketsTest.cpp
SocketsTest_LDADD = libcommon.la
SocketsTest_LDFLAGS = -lpthread
TimevalTest_SOURCES = TimevalTest.cpp
TimevalTest_LDADD = libcommon.la
VectorTest_SOURCES = VectorTest.cpp
VectorTest_LDADD = libcommon.la
RegexpTest_SOURCES = RegexpTest.cpp
RegexpTest_LDADD = libcommon.la
ConfigurationTest_SOURCES = ConfigurationTest.cpp
ConfigurationTest_LDADD = libcommon.la 	$(SQLITE_LA)
LogTest_SOURCES = LogTest.cpp
LogTest_LDADD = libcommon.la $(SQLITE_LA)
F16Test_SOURCES = F16Test.cpp
all: all-am

.SUFFIXES:
.SUFFIXES: .cpp .lo .o .obj
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am $(top_srcdir)/Makefile.common $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh \
		&& exit 0; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --gnu  CommonLibs/Makefile'; \
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  CommonLibs/Makefile
.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)'; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe);; \
	esac;

$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

$(top_srcdir)/configure:  $(am__configure_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(ACLOCAL_M4):  $(am__aclocal_m4_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

clean-noinstLTLIBRARIES:
	-test -z "$(noinst_LTLIBRARIES)" || rm -f $(noinst_LTLIBRARIES)
	@list='$(noinst_LTLIBRARIES)'; for p in $$list; do \
	  dir="`echo $$p | sed -e 's|/[^/]*$$||'`"; \
	  test "$$dir" != "$$p" || dir=.; \
	  echo "rm -f \"$${dir}/so_locations\""; \
	  rm -f "$${dir}/so_locations"; \
	done
libcommon.la: $(libcommon_la_OBJECTS) $(libcommon_la_DEPENDENCIES) 
	$(CXXLINK)  $(libcommon_la_LDFLAGS) $(libcommon_la_OBJECTS) $(libcommon_la_LIBADD) $(LIBS)

clean-noinstPROGRAMS:
	@list='$(noinst_PROGRAMS)'; for p in $$list; do \
	  f=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  echo " rm -f $$p $$f"; \
	  rm -f $$p $$f ; \
	done
BitVectorTest$(EXEEXT): $(BitVectorTest_OBJECTS) $(BitVectorTest_DEPENDENCIES) 
	@rm -f BitVectorTest$(EXEEXT)
	$(CXXLINK) $(BitVectorTest_LDFLAGS) $(BitVectorTest_OBJECTS) $(BitVectorTest_LDADD) $(LIBS)
ConfigurationTest$(EXEEXT): $(ConfigurationTest_OBJECTS) $(ConfigurationTest_DEPENDENCIES) 
	@rm -f ConfigurationTest$(EXEEXT)
	$(CXXLINK) $(ConfigurationTest_LDFLAGS) $(ConfigurationTest_OBJECTS) $(ConfigurationTest_LDADD) $(LIBS)
F16Test$(EXEEXT): $(F16Test_OBJECTS) $(F16Test_DEPENDENCIES) 
	@rm -f F16Test$(EXEEXT)
	$(CXXLINK) $(F16Test_LDFLAGS) $(F16Test_OBJECTS) $(F16Test_LDADD) $(LIBS)
InterthreadTest$(EXEEXT): $(InterthreadTest_OBJECTS) $(InterthreadTest_DEPENDENCIES) 
	@rm -f InterthreadTest$(EXEEXT)
	$(CXXLINK) $(InterthreadTest_LDFLAGS) $(InterthreadTest_OBJECTS) $(InterthreadTest_LDADD) $(LIBS)
LogTest$(EXEEXT): $(LogTest_OBJECTS) $(LogTest_DEPENDENCIES) 
	@rm -f LogTest$(EXEEXT)
	$(CXXLINK) $(LogTest_LDFLAGS) $(LogTest_OBJECTS) $(LogTest_LDADD) $(LIBS)
RegexpTest$(EXEEXT): $(RegexpTest_OBJECTS) $(RegexpTest_DEPENDENCIES) 
	@rm -f RegexpTest$(EXEEXT)
	$(CXXLINK) $(RegexpTest_LDFLAGS) $(RegexpTest_OBJECTS) $(RegexpTest_LDADD) $(LIBS)
SocketsTest$(EXEEXT): $(SocketsTest_OBJECTS) $(SocketsTest_DEPENDENCIES) 
	@rm -f SocketsTest$(EXEEXT)
	$(CXXLINK) $(SocketsTest_LDFLAGS) $(SocketsTest_OBJECTS) $(SocketsTest_LDADD) $(LIBS)
TimevalTest$(EXEEXT): $(TimevalTest_OBJECTS) $(TimevalTest_DEPENDENCIES) 
	@rm -f TimevalTest$(EXEEXT)
	$(CXXLINK) $(TimevalTest_LDFLAGS) $(TimevalTest_OBJECTS) $(TimevalTest_LDADD) $(LIBS)
VectorTest$(EXEEXT): $(VectorTest_OBJECTS) $(VectorTest_DEPENDENCIES) 
	@rm -f VectorTest$(EXEEXT)
	$(CXXLINK) $(VectorTest_LDFLAGS) $(VectorTest_OBJECTS) $(VectorTest_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT)

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/BitVector.Plo
include ./$(DEPDIR)/BitVectorTest.Po
include ./$(DEPDIR)/Configuration.Plo
include ./$(DEPDIR)/ConfigurationTest.Po
include ./$(DEPDIR)/F16Test.Po
include ./$(DEPDIR)/InterthreadTest.Po
include ./$(DEPDIR)/LinkedLists.Plo
include ./$(DEPDIR)/LogTest.Po
include ./$(DEPDIR)/Logger.Plo
include ./$(DEPDIR)/RegexpTest.Po
include ./$(DEPDIR)/Sockets.Plo
include ./$(DEPDIR)/SocketsTest.Po
include ./$(DEPDIR)/Threads.Plo
include ./$(DEPDIR)/Timeval.Plo
include ./$(DEPDIR)/TimevalTest.Po
include ./$(DEPDIR)/VectorTest.Po

.cpp.o:
	if $(CXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ $<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ $<

.cpp.obj:
	if $(CXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ `$(CYGPATH_W) '$<'`; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ `$(CYGPATH_W) '$<'`

.cpp.lo:
	if $(LTCXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ $<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Plo"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
#	source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE) -c -o $@ $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
uninstall-info-am:

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique
tags: TAGS

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)  $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	if test -z "$(ETAGS_ARGS)$$tags$$unique"; then :; else \
	  test -n "$$unique" || unique=$$empty_fix; \
	  $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	    $$tags $$unique; \
	fi
ctags: CTAGS
CTAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)  $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(CTAGS_ARGS)$$tags$$unique" \
	  || $(CTAGS) $(CTAGSFLAGS) $(AM_CTAGSFLAGS) $(CTAGS_ARGS) \
	     $$tags $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH tags

distdir: $(DISTFILES)
	$(mkdir_p) $(distdir)/..
	@srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's|.|.|g'`; \
	list='$(DISTFILES)'; for file in $$list; do \
	  case $$file in \
	    $(srcdir)/*) file=`echo "$$file" | sed "s|^$$srcdirstrip/||"`;; \
	    $(top_srcdir)/*) file=`echo "$$file" | sed "s|^$$topsrcdirstrip/|$(top_builddir)/|"`;; \
	  esac; \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkdir_p) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(LTLIBRARIES) $(PROGRAMS) $(HEADERS)
installdirs:
install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:
	-test -z "$(MOSTLYCLEANFILES)" || rm -f $(MOSTLYCLEANFILES)

clean-generic:

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-generic clean-libtool clean-noinstLTLIBRARIES \
	clean-noinstPROGRAMS mostlyclean-am

distclean: distclean-am
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-libtool distclean-tags

dvi: dvi-am

dvi-am:

html: html-am

info: info-am

info-am:

install-data-am:

install-exec-am:

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am: uninstall-info-am

.PHONY: CTAGS GTAGS all all-am check check-am clean clean-generic \
	clean-libtool clean-noinstLTLIBRARIES clean-noinstPROGRAMS \
	ctags distclean distclean-compile distclean-generic \
	distclean-libtool distclean-tags distdir dvi dvi-am html \
	html-am info info-am install install-am install-data \
	install-data-am install-exec install-exec-am install-info \
	install-info-am install-man install-strip installcheck \
	installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool pdf pdf-am ps ps-am \
	tags uninstall uninstall-am uninstall-info-am

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
