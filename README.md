# mUPnP for C

[![Build Status](https://github.com/cybergarage/mupnp/actions/workflows/make.yml/badge.svg)](https://github.com/cybergarage/mupnp/actions/workflows/make.yml)

## Overview

mUPnP for C is a development package for UPnP™ developers. mUPnP
controls these protocols automatically, and supports to create your
devices and control points quickly.

UPnP™ \* architecture is an open network to enable discovery and control
of networked devices and services, such as media servers and players at
home.

UPnP™ \* protocols are based on many standard, such as GENA, SSDP, SOAP,
HTTPU and HTTP. Therefore you have to understand and implement these
protocols to create your devices of UPnP™.

![upnp-app](doc/img/upnpapp.png)

mUPnP for C is supported from IPA, INFORMATION-TECHNOLOGY PROMOTION
AGENCY, JAPAN, as a project of MitohProgram2004.

\* UPnP ™ is a certification mark of the UPnP™ Implementers Corporation.

## Building and Installation

### Homebrew (macOS, Linux, Raspbian)

For any platforms which support [Homebrew](https://brew.sh/), you can easily install using Homebrew with the following `brew` commands:

```
brew tap cybergarage/homebrew
brew install mupnp
```

### Installation from source

mUPnP for C is distributed as an Automake project, and so you can install the library from the source codes with the following commands:

```
./configure
make install
```

## References

### mUPnP for C

To develop UPnP devices or control point applications using mUPnP for C, please check the following developer's documentation.

* [Programming Guide](doc/mupnpcproguide.pdf)
* [Doxygen](http://cybergarage.github.io/mupnp/)

### mUPnP for Objective-C

mUPnP for C releases the wrapper library for Objective-C to use the UPnP library on iOS and MacOSX platforms too. Please check the following developer's documentation to know the wrapper library in more detail.

* [Programming Guide](doc/mupnpobjcproguide.pdf)
* [Doxygen](http://cybergarage.github.io/mupnp/objc/html/)

## Examples

- UPnP control point examples
  - UPnP controller for UPnP devices
    - [UPnP multicast dump utility](examples/upnpdump)
  - UPnP controller for UPnP stardard devices
    - [UPnP Internet gateway utility ](examples/upnpigddump)
    - [UPnP/AV media server utility ](examples/upnpavdump)
 
- UPnP device examples
  - UPnP non-standard devices
    - [UPnP clock device](examples/clock)
    
## Adoptions for Consumer Products

### Nokia 770 Internet Tablet

Nokia released the UPnP/AV Control point, Media Streamer, for Nokia 770.
The control point is created based on CyberLinkForC with their patches.

![Nokia770](doc/img/mupnpc-example-nokia770-01.jpg)

### Panasonic VIERA remote for iPhone / iPod touch / iPad

[VIERA remote](http://panasonic.jp/support/global/cs/tv/vremote/index.html) is a TV remote controller app for Panasonic VIERA TVs, both plasma TV and
LCD TV, to let TV channel selection, input selection and volume control
from iPhone / iPod touch / iPad (of iOS 4.2 or later). mUPnP for C
is used as the UPnP framework.

![VIERA Remote](doc/img/mupnpc-example-vieraremote-02.gif)
