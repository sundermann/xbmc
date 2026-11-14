include ../../Makefile.include
DEPS = ../../Makefile.include Makefile ../../download-files.include

# We use uuid from e2fsprogs for legacy. util-linux works fine on android now
# lib name, version
LIBNAME=libuuid
VERSION=1.47.4
SOURCE=e2fsprogs-$(VERSION)
ARCHIVE=$(SOURCE).tar.xz
SHA512=c316ce38d73786fb45b3728cd31991fe81b14a1e61dae645f3471c3f35983c4394e4606b47194c3e607897b7ae5a5ef1d376adb204383afa8c4219db8617d970
include ../../download-files.include

LIBDYLIB=$(PLATFORM)/lib/$(LIBNAME).a

all: .installed-$(PLATFORM)


.configured-$(PLATFORM): $(DEPS) | $(TARBALLS_LOCATION)/$(ARCHIVE).$(HASH_TYPE)
	rm -rf $(PLATFORM)/*; mkdir -p $(PLATFORM)
	cd $(PLATFORM); $(ARCHIVE_TOOL) $(ARCHIVE_TOOL_FLAGS) $(TARBALLS_LOCATION)/$(ARCHIVE)
	# e2fsprog libuuid often fails because it doesnt create include/uuid location before
	# installing. lets just create it and move on.
	mkdir -p $(PREFIX)/include/uuid
	cd $(PLATFORM); ./configure --prefix=$(PREFIX) --disable-fsck --enable-libuuid
	touch $@

$(LIBDYLIB): .configured-$(PLATFORM)
  # -j1 is required due to concurrency failures with dirpath.h generation
	cd $(PLATFORM)/lib/uuid ; $(MAKE) -j1

.installed-$(PLATFORM): $(LIBDYLIB)
  # -j1 is required due to concurrency failures with dirpath.h generation
	cd $(PLATFORM)/lib/uuid ; $(MAKE) -j1 install
	touch $@

clean:
	$(MAKE) -C $(PLATFORM) clean
	rm -f .installed-$(PLATFORM) .configured-$(PLATFORM)

distclean::
	rm -rf $(PLATFORM) .installed-$(PLATFORM) .configured-$(PLATFORM)
