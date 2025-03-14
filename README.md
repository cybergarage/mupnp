# mUPnP for C

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/cybergarage/mupnp)
[![Build Status](https://github.com/cybergarage/mupnp/actions/workflows/make.yml/badge.svg)](https://github.com/cybergarage/mupnp/actions/workflows/make.yml)
[![doxygen](https://github.com/cybergarage/mupnp/actions/workflows/doxygen.yml/badge.svg)](http://cybergarage.github.io/mupnp/)

mUPnP for C is a development package for UPnP™ developers. It provides a set of APIs to create UPnP™ devices and control points quickly and easily. This README file provides an overview of the package, its contents, and instructions on how to set up and use it.

## Overview

The UPnP™ (Universal Plug and Play) architecture is designed to enable the discovery and control of networked devices and services, such as media servers and players, within a home network. UPnP™ is based on several standard protocols, including GENA, SSDP, SOAP, HTTPU, and HTTP. To create UPnP™ devices, you need to understand and implement these protocols.

mUPnP for C simplifies this process by handling these protocols automatically. It allows developers to focus on creating their devices and control points without worrying about the underlying protocol details.

![UPnP applications](doc/img/upnpapp.png)

**mUPnP for C** is supported from IPA, INFORMATION-TECHNOLOGY PROMOTION AGENCY, JAPAN, as a project of MitohProgram2004.

\* UPnP ™ is a certification mark of the UPnP™ Implementers Corporation.

## Package Contents

The mUPnP package includes the following components:

- **Source files**: The core implementation of mUPnP.
- **Header files**: The API definitions for mUPnP.
- **Sample files**: Example applications demonstrating how to use mUPnP.
- **Project files**: Build configurations for various platforms.

The files are organized into the following directories:

| File Type     | Directory             |
|---------------|-----------------------|
| Source files  | mupnp/src             |
| Header Files  | mupnp/include         |
| Sample files  | mupnp/sample          |
| Project files | mupnp                 |
|               | mupnp/*/win32/vc60    |
|               | mupnp/*/tengine/gnu   |
|               | mupnp/*/itron         |
|               | mupnp/*/macosx        |

## System Requirements

mUPnP requires the following package to parse XML and SOAP requests:

| Package | URL                           |
|---------|-------------------------------|
| Expat   | https://libexpat.github.io/   |

### WindowsXP

On the Windows platform, you need to install the latest Platform SDK and build on WindowsXP if possible. Please download and install the SDK from the following URL:

| Package      | URL                                                        |
|--------------|------------------------------------------------------------|
| Platform SDK | http://www.microsoft.com/msdownload/platformsdk/sdkupdate/ |

### T-Engine

On the T-Engine platform, you need to use the following development kit based on GNU GCC and a TCP/IP protocol stack that supports the multicast protocol. mUPnP uses the multicast protocol to search and announce UPnP devices, so you must use a protocol stack that supports multicast.

| Package                  | URL                                                          |
|--------------------------|--------------------------------------------------------------|
| T-Engine Development Kit | http://www.personal-media.co.jp/te/welcome.html              |
| KASAGO for T-Engine      | http://www.elwsc.co.jp/japanese/products/kasago_tengine.html |

mUPnP also supports the following TCP/IP protocol stack for T-Engine, but it does not support the multicast protocol, and the functions are not implemented yet.

| Package         | URL                                             |
|-----------------|-------------------------------------------------|
| PMC T-Shell Kit | http://www.personal-media.co.jp/te/welcome.html |

## Building the Library and Samples

mUPnP supports the following compiler options to change the XML parser or disable UPnP functions. You do not need to set these options if you use Expat as the XML parser and all functions of mUPnP.

| Option                     | Description                                               |
|----------------------------|-----------------------------------------------------------|
| MUPNP_XMLPARSER_LIBXML2    | Use libxml2 as the XML parser instead of Expat.           |
| MUPNP_NOUSE_CONTROLPOINT   | Disable UPnP™ control point functions.                    |
| MUPNP_NOUSE_SUBSCRIPTION   | Disable UPnP™ subscription functions.                     |
| MUPNP_NOUSE_ACTIONCTRL     | Disable UPnP™ action control functions.                   |
| MUPNP_NOUSE_QUERYCTRL      | Disable UPnP™ query control functions.                    |

mUPnP uses Expat as the default parser, but the following XML parser is supported with the compiler option. Please refer to the XML parser at the following site:

| Package | URL                 |
|---------|---------------------|
| libxml2 | https://github.com/GNOME/libxml2 |

### Unix

For Unix platforms, you can build the library and samples using the following steps. Use the --enable-libxml2 option of the configure script instead of the compiler option to use libxml2.

```
cd mupnp
./bootstrap
./configure
make
```

### Windows

For Windows platforms, mUPnP includes platform projects for Visual Studio 2005. Please check the platform directories, mupnp/*/win32/vs2005, to use the projects. On WindowsCE, mUPnP does not have platform projects, but a contributor has verified that the source codes compile normally.

### T-Engine

For T-Engine platforms, you need to set the following compiler options. mUPnP supports both process-based and T-Kernel-based programs. Use the PROCESS_BASE option to compile the process-based program. Please refer to the development manual of your T-Engine development kit.

| Option                     | Description                                               |
|----------------------------|-----------------------------------------------------------|
| TENGINE                    | Enable the platform option.                               |
| MUPNP_TENGINE_NET_KASAGO   | Enable KASAGO for T-Engine option.                        |

mUPnP is compiled using the functions for PMC T-Shell Kit as the TCP/IP protocol stack, but it is not recommended because the protocol stack does not support the multicast protocol, and the functions are not implemented yet.

To run applications using mUPnP, the driver of the TCP/IP protocol stack must be loaded, and the network address must be determined. Please refer to the manual of the protocol stack for instructions on setting the network interface.

You need to set the EXPATROOT environment variable to the top directory of the installed Expat on your shell as follows. The source codes of Expat must be included in the "lib" directory.

```
export EXPATROOT=/usr/local/expat-1.95.8
```

I have built the library with the T-Engine/SH7727 development kit with KASAGO for T-Engine. Please check the platform directories, mupnp/*/tengine/gnu, for the sample projects. To compile the samples, run the configure script in the directory first. Please refer to the development manual of your T-Engine development kit if you want to use it on other T-Engine platforms.

### MacOSX

For MacOSX, I have released a wrapper class for Objective-C on Cocoa. Currently, the framework supports only basic functions of the control point. Please use the standard C library if you need to use all functions of mUPnP for C.

## License

mUPnP for C is licensed under a BSD-style license. See the COPYING file for more details.

## References

### mUPnP for C

To develop UPnP devices or control point applications using **mUPnP for C**, please check the following developer's documentation.

* [Programming Guide for C](http://cybergarage.github.io/mupnp/)
* [Doxygen](http://cybergarage.github.io/mupnp/)

### mUPnP for Objective-C

**mUPnP for C** releases the wrapper library for Objective-C to use the UPnP library on iOS and MacOSX platforms too. Please check the following developer's documentation to know the wrapper library in more detail.

* [Programming Guide for Objective-C](http://cybergarage.github.io/mupnp/objc/html/)
* [Doxygen](http://cybergarage.github.io/mupnp/objc/html/)

## Examples

- UPnP control point examples
  - UPnP controller for UPnP devices
    - [UPnP multicast dump utility](https://github.com/cybergarage/mupnp/tree/master/examples/upnpdump)
  - UPnP controller for UPnP stardard devices
    - [UPnP Internet gateway utility ](https://github.com/cybergarage/mupnp/tree/master/examples/upnpigddump)
    - [UPnP/AV media server utility ](https://github.com/cybergarage/mupnp/tree/master/examples/upnpavdump)
 
- UPnP device examples
  - UPnP non-standard devices
    - [UPnP clock device](https://github.com/cybergarage/mupnp/tree/master/examples/clock)
    
## Adoption in Consumer Products

**mUPnP for C** is used in a variety of consumer products. While not exhaustive, the following is a list of notable implementations.

### Nokia 770 Internet Tablet

Nokia engineers contributed to the improvement of the initial **mUPnP for C** release. Nokia developed the UPnP/AV Control Point application, **Media Streamer**, for the Nokia 770 Internet Tablet.　The application is based on **mUPnP for C**, incorporating custom patches by Nokia.  

![Nokia 770 Internet Tablet](doc/img/mupnpc-example-nokia770-01.jpg)

### Panasonic VIERA Remote for iOS Devices
The [**VIERA Remote**](http://panasonic.jp/support/global/cs/tv/vremote/index.html) is a TV remote control app for Panasonic VIERA TVs (plasma and LCD models).  It allows users to control channels, switch inputs, and adjust the volume directly from an **iPhone, iPod touch, or iPad** (iOS 4.2 or later). **mUPnP for C** functions as the UPnP framework for this app.  

![VIERA Remote](doc/img/mupnpc-example-vieraremote-02.gif)

### Toshiba REGZA Televisions

[**REGZA**](https://www.regza.com/) is Toshiba’s former AV equipment brand. **mUPnP for C** is embedded in REGZA televisions, enabling seamless media streaming and connectivity.  

![REGZA Televisions](doc/img/mupnpc-example-regzatv-01.png)
