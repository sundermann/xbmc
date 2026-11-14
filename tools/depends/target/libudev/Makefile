include ../../Makefile.include
DEPS =../../Makefile.include Makefile ../../download-files.include

# lib name, version
LIBNAME=libudev
VERSION=3.3.0
SOURCE=$(LIBNAME)-$(VERSION)
ARCHIVE=$(SOURCE).tar.gz
SHA512=f36529fe9579ce23fed7565640afeed1577c8aaeaa7df84cdd7e63ef153d737364fcd3f4b304c0609103cd73203914c050df311df5a2b7d8c04f69cf40c24d3a
include ../../download-files.include

# configuration settings
CONFIGURE=./configure --prefix=$(PREFIX) --disable-shared --enable-static --enable-introspection=no --disable-programs --disable-blkid --disable-selinux --disable-manpages --disable-kmod --disable-mtd_probe

LIBDYLIB=$(PLATFORM)/src/libudev/.libs/libudev.la

all: .installed-$(PLATFORM)


.configured-$(PLATFORM): $(DEPS) | $(TARBALLS_LOCATION)/$(ARCHIVE).$(HASH_TYPE)
	rm -rf $(PLATFORM)/*; mkdir -p $(PLATFORM)
	cd $(PLATFORM); $(ARCHIVE_TOOL) $(ARCHIVE_TOOL_FLAGS) $(TARBALLS_LOCATION)/$(ARCHIVE)
	cd $(PLATFORM); $(AUTORECONF) -vif
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
