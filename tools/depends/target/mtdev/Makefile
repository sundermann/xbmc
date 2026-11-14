include ../../Makefile.include
DEPS =../../Makefile.include Makefile ../../download-files.include

# lib name, version
LIBNAME=mtdev
VERSION=1.1.7
SOURCE=$(LIBNAME)-$(VERSION)
ARCHIVE=$(SOURCE).tar.bz2
SHA512=e6174a38cf67a7f12a3b91e4e27bf74a18d6b40a956950ebb748b0ff87092333daa07e647b26038a5a533f8c48e845d649848e6ba99ea009ab87fd96ed188152
include ../../download-files.include

# configuration settings
CONFIGURE=cp -f $(CONFIG_SUB) $(CONFIG_GUESS) config-aux/; ./configure --prefix=$(PREFIX) --enable-static --disable-shared

LIBDYLIB=$(PLATFORM)/src/.libs/libmtdev.la

all: .installed-$(PLATFORM)


.configured-$(PLATFORM): $(DEPS) | $(TARBALLS_LOCATION)/$(ARCHIVE).$(HASH_TYPE)
	rm -rf $(PLATFORM)/*; mkdir -p $(PLATFORM)
	cd $(PLATFORM); $(ARCHIVE_TOOL) $(ARCHIVE_TOOL_FLAGS) $(TARBALLS_LOCATION)/$(ARCHIVE)
	cd $(PLATFORM); $(CONFIGURE)
	touch $@

$(LIBDYLIB): .configured-$(PLATFORM)
	$(MAKE) -C $(PLATFORM)

.installed-$(PLATFORM): $(LIBDYLIB)
	$(MAKE) -C $(PLATFORM) install
	touch $@

clean:
	$(MAKE) -C $(PLATFORM) clean
	rm -f .installed-$(PLATFORM) .configured-$(PLATFORM)

distclean:
	rm -rf $(PLATFORM) .installed-$(PLATFORM) .configured-$(PLATFORM)
