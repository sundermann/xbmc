-include ../../Makefile.include
include WEBOS-WAYLAND-EXTENSIONS-VERSION
DEPS = WEBOS-WAYLAND-EXTENSIONS-VERSION Makefile ../../download-files.include

ifeq ($(CROSS_COMPILING), yes)
  DEPS += ../../Makefile.include
else
  ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

  ifeq ($(PLATFORM),)
    PLATFORM = native
    TARBALLS_LOCATION = $(ROOT_DIR)
  endif
endif

RETRIEVE_TOOL := curl -Ls --create-dirs --retry 10 --retry-delay 3
ARCHIVE_TOOL := tar --strip-components=1 -xf

include ../../download-files.include
all: .installed-$(PLATFORM)

download: $(TARBALLS_LOCATION)/$(ARCHIVE)

$(TARBALLS_LOCATION)/$(ARCHIVE):
	cd $(TARBALLS_LOCATION); $(RETRIEVE_TOOL) -o $(TARBALLS_LOCATION)/$(ARCHIVE) $(BASE_URL)/archive/refs/tags/submissions/$(VERSION).tar.gz

$(PLATFORM): $(DEPS) | $(TARBALLS_LOCATION)/$(ARCHIVE).$(HASH_TYPE)
	rm -rf $(PLATFORM); mkdir -p $(PLATFORM)
	cd $(PLATFORM); $(ARCHIVE_TOOL) $(TARBALLS_LOCATION)/$(ARCHIVE)

.installed-$(PLATFORM): $(PLATFORM)
	mkdir -p $(PREFIX)/share/webos-wayland-extensions
	cp $(PLATFORM)/protocol/webos-shell.xml $(PREFIX)/share/webos-wayland-extensions
	touch $@

clean:
	rm -f .installed-$(PLATFORM)

distclean::
	rm -rf $(PLATFORM) .installed-$(PLATFORM)

