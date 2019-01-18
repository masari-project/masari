# Using QEMU

There's also the choice of cross-compiling via QEMU.

The inspiration for using QEMU came from [here](https://sentryytech.blogspot.de/2013/02/faster-compiling-on-emulated-raspberry.html)

## Setup
Download the latest Raspbian Image from the official Raspberry Pi site (the lite image will do just fine), at the time I'm writing this guide it's ``2018-11-13-raspbian-stretch-lite.img``
* You need to have the following packages installed on your computer before you proceed further: ``qemu-user-static``

``$ sudo apt-get install qemu-user-static``

* Create a temp folder in ``/mnt``

``$ sudo mkdir /mnt/temp``

* Mount the image you downloaded with ``kpartx`` (because it contains 2 partitions). If you don't have ``kpartx`` install it with:

      $ sudo apt-get install kpartx
  And then:
  
      $ sudo kpartx -a -v /(path_to_raspbian_image)/2018-11-13-raspbian-stretch-lite.img
  
  If everything went correctly, you'll see something like this:


      add map loopxp1 (252:8): 0 114688 linear /dev/loop0 8192
      add map loopxp2 (252:9): 0 5763072 linear /dev/loop0 122880

  In this particular example we'll assume x=0. 

* Mount ``loop0p2`` to the temp directory you created before:

      $ sudo mount /dev/mapper/loop0p2 /mnt/temp

* Copy ``qemu-arm-static`` to ``/mnt/temp/usr/bin``

      $ sudo cp /usr/bin/qemu-arm-static /mnt/temp/usr/bin

* Mount a few more things 
   
  

      $ sudo mount -o bind /dev /mnt/temp/dev
      $ sudo mount -o bind /proc /mnt/temp/proc
      $ sudo mount -o bind /sys /mnt/temp/sys
      $ sudo mount -o bind /tmp /mnt/temp/tmp
      $ sudo mount -o bind /home/your_username/workspace /mnt/temp/home


**Important:** Mounting ``/tmp`` and an extra folder where you clone the repository (in this example ``/home/your_username/workspace``) **is imperative** otherwise you **will run out of space**  in the middle of the building procedure and resizing the image is beyond the scope of this guide.
This may seem counterintuitive but this mounts ``/home/your_username/workspace`` to the `/home` directory of the Raspbian image.

* Register QEMU as the ARM interpreter to the kernel. Sudo isn't enough for this.
    
      $ sudo su
      # echo ':arm:M::\x7fELF\x01\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x28\x00:\xff\xff\xff\xff\xff\xff\xff\x00\xff\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xff\xff:/usr/bin/qemu-arm-static:' > /proc/sys/fs/binfmt_misc/register
      # exit
    
## Build

Now you're ready to chroot into the arm environment.

    $ sudo chroot /mnt/temp
    $ uname -m
    $ armv7l

You're root, so sudo is no longer needed.

    $ apt update && apt install -y git build-essential cmake pkg-config libboost-all-dev libssl-dev libzmq3-dev libunbound-dev libsodium-dev libreadline6-dev libpgm-dev
    $ git clone --recursive https://github.com/masari-project/masari.git
    $ cd masari

and then:

For armv7:
``$ make release-static-linux-armv7 -j y`` (y is the number of cores your processor has, this is optional)

For armv6:
``$ make release-static-linux-armv6 -j y`` (y is the number of cores your processor has, this is optional)

If there are no errors, the binaries should be located at : ``/home/masari/build/release/bin`` within the chroot image and at the same time at ``/home/your_username/workspace/masari/build/release/bin``(outside the chroot) if the what was described in the examples was used.

## Cleanup
After you finish it's recommended to unmount everything again:

    $ sudo umount /mnt/temp/dev
    $ sudo umount /mnt/temp/proc
    $ sudo umount /mnt/temp/sys
    $ sudo umount /mnt/temp/tmp 
    $ sudo umount /mnt/temp/workspace
    $ sudo umount /mnt/temp
    $ sudo kpartx -d -v /(path_to_raspbian_image)/2018-11-13-raspbian-stretch-lite.img

Done.
Of course this can be used not only to (cross)compile the binaries mentioned here but  *almost* anything that can be compiled on armv6/armv7.

**Note:** The method described here produces binaries compatible with SBCs, **NOT** with Android.

### Pros and Cons
**Pros**

 - Builds complete much quicker than on the actual hardware
 - No need to build dependencies from source
 - Versatile, can be used for other applications too
 - Built binaries can be actually executed in the QEMU environment, good for some very basic testing (see if the binaries actually run).
 
 **Cons**
 
 - Not as straightforward as a do-it-all bash script or dockerfile
 - Slower than a bash script
 - Your image can run out of space in the middle of the build if you're not careful
