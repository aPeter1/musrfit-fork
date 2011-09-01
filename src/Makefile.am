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

SUBDIRS = external/TLemRunHeader \
          external/MuSR_software \
          external/mud \
          $(PNEXUSDIRS) \
          classes \
          external \
          $(EDITORDIR)

EXTRA_DIST = $(EDITORDIR)/Makefile

bin_PROGRAMS = musrfit musrview musrt0 musrparam msr2msr msr2data any2many
musrfit_SOURCES = musrfit.cpp
musrview_SOURCES = musrview.cpp
musrt0_SOURCES = musrt0.cpp
musrparam_SOURCES = musrparam.cpp
msr2msr_SOURCES = msr2msr.cpp
msr2data_SOURCES = msr2data.cpp
any2many_SOURCES = any2many.cpp

xmldir = $(bindir)
xml_DATA = musrfit_startup.xml

LIBADD = $(PMUSR_LIBS) $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS) $(PNEXUS_LIBS)

AM_CXXFLAGS = $(LOCAL_BIN_CXXFLAGS)
AM_LDFLAGS = $(LOCAL_BIN_LDFLAGS)
INCLUDES = $(PMUSR_CFLAGS) $(FFTW3_CFLAGS) $(GSL_CFLAGS) $(BOOST_CFLAGS) $(ROOT_CFLAGS)
LIBS = $(PMUSR_LIBS) $(USERFCN_LIBS) $(LEM_LIBS) $(PSIBIN_LIBS) $(MUD_LIBS) $(PNEXUS_LIBS) $(FFTW3_LIBS) $(GSL_LIBS) $(ROOT_LIBS)

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
	      $(RM) "$(DESTDIR)$(xmldir)/$(xml_DATA)" && \
	      rmdir "$(DESTDIR)$(xmldir)"; \
	    fi; \
	  else \
	    echo "$(RM) '$(DESTDIR)$(xmldir)/$(xml_DATA)'" && \
	    $(RM) "$(DESTDIR)$(xmldir)/$(xml_DATA)" && \
	    rmdir "$(DESTDIR)$(xmldir)"; \
	  fi; \
	fi
