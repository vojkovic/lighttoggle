lighttoggle
===========

A utility to toggle the lighting of Model O/O- mice on Linux/BSD.
Most of the code is from [enkore/gloriousctl](https://github.com/enkore/gloriousctl)

Build requirements
------------------

C compiler and libhidapi-hidraw. The libusb-based HIDAPI backend
should technically work, but it requires exclusive control over the
USB device, which is impractical for a mouse.
