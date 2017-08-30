BoCA - fre:ac Component Architecture
====================================================================================================

BoCA is the component framework behind the fre:ac audio converter. It provides unified interfaces
for components like encoders, decoders, taggers and extensions as well as code to support
communication between the application and its components.


Contents
--------
The code for the main library can be found in the `runtime` directory with headers in the `include`
folder.

The `components` directory contains various components of different types that can be used with
audio software like fre:ac and also serve as examples on how to implement BoCA components.

The `scripts` directory contains XML scripts that are interpreted by BoCA in order to use various
external command line encoders and decoders.


Installation
------------

The following packages must be installed in order to compile BoCA:

- the _smooth_ Class Library
- libasound2 / libalsa2 development package (Linux only)
- libcdio and libcdio-paranoia development packages (except OpenBSD)
- libcdparanoia development package (OpenBSD only)
- libexpat development package
- liburiparser development package

When all prerequisites are met, run `make` followed by `sudo make install` to compile and install
BoCA.


----------------------------------------------------------------------------------------------------
The official fre:ac homepage: https://www.freac.org/

robert.kausch@freac.org,<br>
Robert Kausch
