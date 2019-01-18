# Cross Compile Stuff

A couple of bash scripts for cross compiling the Masari binaries from linux x86_64 to armv7 and arm64 (armv8a or aarch64). This is intended to work for SBCs like the Raspberry Pi 3 etc. It is not supposed to work with Android armv7 and armv8a.

The scripts have been tested on Ubuntu 18.04 with gcc 6 and 8 for arm. If you're planning to use the binaries on a RPi 3 running Raspbian and you build on Ubuntu 18.04, you'll run into a libc incompatibility (Raspbian's is older).

**UPDATE**

You can now compile the binaries using a version of Ubuntu > 16.04 (for example 18.04) and a newer version of gcc (I tested gcc 8.2 for arm). To overcome the libc incompatibility do the following:

1.  Edit the Raspberry's `/etc/apt/sources.list` and add the following line `deb http://raspbian.raspberrypi.org/raspbian/ testing main contrib non-free rpi`
2.  Do a `sudo apt update` and update **ONLY** the libc libraries to the latest version with `apt-get install --only-upgrade <packagename>`
3.  You might want to check if the procedure worked by typing `ldd --version`. If its output contains `ldd (Debian GLIBC 2.28-2) 2.28` the procedure worked.
4.  Revert the changes to `/etc/apt/sources.list` by removing the line `deb http://raspbian.raspberrypi.org/raspbian/ testing main contrib non-free rpi`

The script has been updated to let the user choose which version of gcc they want to use. Of course this can be easily overriden by editing the script if you want to use your own version of gcc.

There's a version for [armv7](cross-armv7) and a version for [armv8a/aarch64](cross-aarch64)

Alternatively, there's also the [QEMU Way](QEMU.md)
