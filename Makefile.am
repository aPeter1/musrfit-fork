## Process this file with automake to create Makefile.in

SUBDIRS = src

EXTRA_DIST = autogen.sh acinclude.m4

# The following is to install the documentation
# It is a bit lengthy because one cannot simply install whole directories (except copying them recursively -- which also takes the .svn directories...)
DOCDIRS = examples \
          examples/data \
          examples/ASlibs \
          examples/ASlibs/data \
          examples/ASlibs/profiles \
          examples/BMWlibs \
          examples/BMWlibs/data \
          examples/BMWlibs/profiles \
          html/ \
          html/_downloads \
          html/_images \
          html/_sources \
          html/_static 
          #html/technical

install-data-hook:
	@$(NORMAL_INSTALL)
	for DIR in $(DOCDIRS); do $(MKDIR_P) $(DOCDIR)/$$DIR; $(INSTALL_DATA) doc/$$DIR/*.* $(DOCDIR)/$$DIR; done
	$(MKDIR_P) $(DOCDIR)/latex_images
	$(INSTALL_DATA) src/musredit/latex_images/*.png $(DOCDIR)/latex_images

uninstall-hook:
	@$(NORMAL_UNINSTALL)
	rm -rf $(DOCDIR)
