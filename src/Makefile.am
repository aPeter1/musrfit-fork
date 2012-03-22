## Process this file with automake to create Makefile.in

if PNEXUS_ENABLED
  PNEXUSDIRS = external/nexus
endif

if BUILD_MUSRGUI
  EDITORDIR = musrgui
endif

if BUILD_MUSREDIT
  EDITORDIR = musredit
endif

SUBDIRS = external/MusrRoot \
          external/TLemRunHeader \
          external/MuSR_software \
          external/mud \
          $(PNEXUSDIRS) \
          classes \
          external

if BUILD_MUSRGUI
SUBDIRS += $(EDITORDIR)
EXTRA_DIST = $(EDITORDIR)/Makefile
endif

if BUILD_MUSREDIT
SUBDIRS += $(EDITORDIR)
EXTRA_DIST = $(EDITORDIR)/Makefile
endif

bin_PROGRAMS = musrfit musrview musrt0 msr2msr msr2data any2many
if PNEXUS_ENABLED
bin_PROGRAMS += nexus_dump
endif
bin_PROGRAMS += read_musrRoot_runHeader write_musrRoot_runHeader

musrfit_SOURCES = musrfit.cpp
musrview_SOURCES = musrview.cpp
musrt0_SOURCES = musrt0.cpp
msr2msr_SOURCES = msr2msr.cpp
msr2data_SOURCES = msr2data.cpp
any2many_SOURCES = any2many.cpp
if PNEXUS_ENABLED
nexus_dump_SOURCES = nexus_dump.cpp
endif
read_musrRoot_runHeader_SOURCES = read_musrRoot_runHeader.cpp
write_musrRoot_runHeader_SOURCES = write_musrRoot_runHeader.cpp

xmldir = $(bindir)
xml_DATA = musrfit_startup.xml

LIBADD = $(PMUSR_LIBS) $(MUSR_ROOT_LIBS) $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS) $(PNEXUS_LIBS)

AM_CXXFLAGS = $(LOCAL_BIN_CXXFLAGS)
AM_LDFLAGS = $(LOCAL_BIN_LDFLAGS)
INCLUDES = $(MUSR_ROOT_CFLAGS) $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) $(GSL_CFLAGS) $(BOOST_CFLAGS) $(ROOT_CFLAGS)
if PNEXUS_ENABLED
INCLUDES += $(HDF5_CFLAGS) $(NEXUS_CFLAGS) $(PNEXUS_CXXFLAGS)
endif

LIBS = $(PMUSR_LIBS) $(USERFCN_LIBS) $(MUSR_ROOT_LIBS) $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS) $(PNEXUS_LIBS) \
       $(FFTW3_LIBS) $(GSL_LIBS) $(ROOT_LIBS)

install-xmlDATA: $(xml_DATA)
	test -z "$(xmldir)" || $(mkdir_p) "$(DESTDIR)$(xmldir)"
	@if test -e "$(DESTDIR)$(xmldir)/$(xml_DATA)"; then \
	  DIFF="$$(diff "$(DESTDIR)$(xmldir)/$(xml_DATA)" "$(xml_DATA)" 2>&1)"; \
	  if test "x$$DIFF" != "x"; then \
	    echo "   " && \
	    echo "   musrfit_startup.xml in $(bindir)" && \
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
	    echo "   musrfit_startup.xml in $(bindir)" && \
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
