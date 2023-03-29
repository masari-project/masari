package=hidapi
$(package)_version=0.11.0
$(package)_download_path=https://github.com/libusb/hidapi/archive
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_sha256_hash=391d8e52f2d6a5cf76e2b0c079cfefe25497ba1d4659131297081fc0cd744632
$(package)_linux_dependencies=libusb eudev
$(package)_patches=missing_win_include.patch

define $(package)_set_vars
$(package)_config_opts=--enable-static --disable-shared
$(package)_config_opts+=--prefix=$(host_prefix)
$(package)_config_opts_linux+=libudev_LIBS="-L$(host_prefix)/lib -ludev"
$(package)_config_opts_linux+=libudev_CFLAGS=-I$(host_prefix)/include
$(package)_config_opts_linux+=libusb_LIBS="-L$(host_prefix)/lib -lusb-1.0"
$(package)_config_opts_linux+=libusb_CFLAGS=-I$(host_prefix)/include/libusb-1.0
$(package)_config_opts_linux+=--with-pic
endef

define $(package)_preprocess_cmds
  patch -p1 < $($(package)_patch_dir)/missing_win_include.patch && ./bootstrap
endef

define $(package)_config_cmds
  $($(package)_autoconf) AR_FLAGS=$($(package)_arflags)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

define $(package)_postprocess_cmds
  rm lib/*.la
endef

