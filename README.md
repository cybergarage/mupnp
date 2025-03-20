# mUPnP for C

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/cybergarage/mupnp)
[![Build Status](https://github.com/cybergarage/mupnp/actions/workflows/make.yml/badge.svg)](https://github.com/cybergarage/mupnp/actions/workflows/make.yml)
[![doxygen](https://github.com/cybergarage/mupnp/actions/workflows/doxygen.yml/badge.svg)](http://cybergarage.github.io/mupnp/)

mUPnP for C is a development package for UPnP™ [^1] (Universal Plug and Play) developers. It provides a set of APIs to create UPnP™ devices and control points quickly and easily. This README file provides an overview of the package, its contents, and instructions on how to set up and use it.

## Overview

The UPnP™ architecture is designed to enable the discovery and control of networked devices and services, such as media servers and players, within a home network. UPnP™ is based on several standard protocols, including GENA, SSDP, SOAP, HTTPU, and HTTP. To create UPnP™ devices, you need to understand and implement these protocols.

![UPnP applications](doc/img/upnpapp.png)

mUPnP for C simplifies this process by handling these protocols automatically. It allows developers to focus on creating their devices and control points without worrying about the underlying protocol details.

**mUPnP for C** is supported from IPA, INFORMATION-TECHNOLOGY PROMOTION AGENCY, JAPAN, as a project of [Mitoh Program 2004](https://www.ipa.go.jp/archive/jinzai/mitou/mitoipedia/2004shimoki-seika.html#sakamura).

## Building the Library and Samples

To build the library and samples, you need to have the following tools installed on your system:

### Unix

mUPnP for C is distributed as an Automake project, and so you can install the libary from the source codes with the following commands:

```
cd mupnp
./configure
make
```
### Windows

For Windows platforms, mUPnP includes platform projects for Visual Studio 2005. Please check the platform directories, mupnp/*/win32/vs2005, to use the projects. On WindowsCE, mUPnP does not have platform projects, but a contributor has verified that the source codes compile normally.

### MacOSX

For macOS platforms, you can easily install using Homebrew with the following `brew` commands:

```
brew tap cybergarage/homebrew
brew install mupnp
```

For MacOSX, I have released a wrapper class for Objective-C on Cocoa. Currently, the framework supports only basic functions of the control point. Please use the standard C library if you need to use all functions of mUPnP for C.

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

[^1]: UPnP™ is a certification mark of the UPnP™ Implementers Corporation.