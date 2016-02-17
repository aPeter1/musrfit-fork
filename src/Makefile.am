## Process this file with automake to create Makefile.in

BUILT_SOURCES = git-revision.h
git-revision.h : Makefile
	echo \#define GIT_REVISION \"`git log -n 1 --pretty=format:"%ad - %h"`\" > include/git-revision.h

if PNEXUS_ENABLED
  PNEXUSDIRS = external/nexus
endif

if BUILD_MUSRGUI
  EDITORDIR = musrgui
endif

if BUILD_MUSREDIT
  EDITORDIR = musredit
endif

if BUILD_MUSREDIT_QT5
  EDITORDIR = musredit_qt5
endif

SUBDIRS = external/MusrRoot \
          external/TLemRunHeader \
          external/MuSR_software \
          external/mud \
          $(PNEXUSDIRS) \
          classes \
          external \
          musredit_qt5 \
          musredit \
          musrgui 

if BUILD_MUSRGUI
#SUBDIRS += $(EDITORDIR)
EXTRA_DIST = $(EDITORDIR)/Makefile
endif

if BUILD_MUSREDIT
#SUBDIRS += $(EDITORDIR)
EXTRA_DIST = $(EDITORDIR)/Makefile
endif

if BUILD_MUSREDIT_QT5
EXTRA_DIST = $(EDITORDIR)/Makefile
endif

bin_PROGRAMS = musrfit musrview musrt0 musrFT msr2msr msr2data any2many
bin_PROGRAMS += write_musrRoot_runHeader musrRootValidation
bin_PROGRAMS += dump_header

musrfit_SOURCES = musrfit.cpp
musrview_SOURCES = musrview.cpp
musrt0_SOURCES = musrt0.cpp
musrFT_SOURCES = musrFT.cpp
msr2msr_SOURCES = msr2msr.cpp
msr2data_SOURCES = msr2data.cpp
any2many_SOURCES = any2many.cpp
write_musrRoot_runHeader_SOURCES = write_musrRoot_runHeader.cpp
musrRootValidation_SOURCES = musrRootValidation.cpp
dump_header_SOURCES = dump_header.cpp

xmldir = $(HOME)/.musrfit
xml_DATA = musrfit_startup.xml

LIBADD = $(PMUSR_LIBS) $(MUSR_ROOT_LIBS) $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS) $(PNEXUS_LIBS)

AM_CXXFLAGS = $(LOCAL_BIN_CXXFLAGS)
AM_LDFLAGS = $(LOCAL_BIN_LDFLAGS)
AM_CPPFLAGS = $(MUSR_ROOT_CFLAGS) $(LEM_CFLAGS) $(MUD_CFLAGS) $(PSIBIN_CFLAGS) $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) $(GSL_CFLAGS) $(BOOST_CFLAGS) $(ROOT_CFLAGS) \
           $(LIBXML2_CFLAGS)
if PNEXUS_ENABLED
AM_CPPFLAGS += $(HDF5_CFLAGS) $(NEXUS_CFLAGS) $(PNEXUS_CXXFLAGS)
endif

LIBS = $(PMUSR_LIBS) $(USERFCN_LIBS) $(MUSR_ROOT_LIBS) $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS) $(PNEXUS_LIBS) \
       $(FFTW3_LIBS) $(GSL_LIBS) $(ROOT_LIBS) $(LIBXML2_LIBS)

install-xmlDATA: $(xml_DATA)
	test -z "$(xmldir)" || $(mkdir_p) "$(DESTDIR)$(xmldir)"
	@if test -e "$(DESTDIR)$(xmldir)/$(xml_DATA)"; then \
	  DIFF="$$(diff "$(DESTDIR)$(xmldir)/$(xml_DATA)" "$(xml_DATA)" 2>&1)"; \
	  if test "x$$DIFF" != "x"; then \
	    echo "   " && \
	    echo "   musrfit_startup.xml in $(xmldir)" && \
	    echo "   is different from the distribution's version." && \
	    echo "   Do you want to overwrite it? [y/N]" && \
	    read OVERWRITE && \
	    if test "$$OVERWRITE" = "y" || test "$$OVERWRITE" = "Y"; then \
	      echo "$(INSTALL_DATA) '$(xml_DATA)' '$(DESTDIR)$(xmldir)'" && \
	      $(INSTALL_DATA) '$(xml_DATA)' '$(DESTDIR)$(xmldir)'; \
	    fi; \
	  fi; \
	else \
	  echo "$(INSTALL_DATA) '$(xml_DATA)' '$(DESTDIR)$(xmldir)'" && \
	  $(INSTALL_DATA) '$(xml_DATA)' '$(DESTDIR)$(xmldir)'; \
	fi

uninstall-xmlDATA:
	@if test -e "$(DESTDIR)$(xmldir)/$(xml_DATA)"; then \
	  DIFF="$$(diff "$(DESTDIR)$(xmldir)/$(xml_DATA)" "$(xml_DATA)" 2>&1)"; \
	  if test "x$$DIFF" != "x"; then \
	    echo "   " && \
	    echo "   musrfit_startup.xml in $(xmldir)" && \
	    echo "   is different from the distribution's version." && \
	    echo "   Do you want to remove it? [y/N]" && \
	    read REMOVE && \
	    if test "$$REMOVE" = "y" || test "$$REMOVE" = "Y"; then \
	      echo "$(RM) '$(DESTDIR)$(xmldir)/$(xml_DATA)'" && \
	      $(RM) "$(DESTDIR)$(xmldir)/$(xml_DATA)"; \
	    fi; \
	  else \
	    echo "$(RM) '$(DESTDIR)$(xmldir)/$(xml_DATA)'" && \
	    $(RM) "$(DESTDIR)$(xmldir)/$(xml_DATA)"; \
	  fi; \
	fi
