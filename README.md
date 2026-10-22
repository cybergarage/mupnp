# mUPnP for C

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/cybergarage/mupnp)
[![Build Status](https://github.com/cybergarage/mupnp/actions/workflows/make.yml/badge.svg)](https://github.com/cybergarage/mupnp/actions/workflows/make.yml)
[![doxygen](https://github.com/cybergarage/mupnp/actions/workflows/doxygen.yml/badge.svg)](http://cybergarage.github.io/mupnp/)
[![codecov](https://codecov.io/gh/cybergarage/mupnp/graph/badge.svg?token=GTSS6CC5YB)](https://codecov.io/gh/cybergarage/mupnp)

mUPnP for C is a development package for UPnP™ [^1] (Universal Plug and Play) developers. It provides a set of APIs to create UPnP™ devices and control points quickly and easily. This README file provides an overview of the package, its contents, and instructions on how to set up and use it.


The UPnP™ architecture is designed to enable the discovery and control of networked devices and services, such as media servers and players, within a home network. UPnP™ is based on several standard protocols, including GENA, SSDP, SOAP, HTTPU, and HTTP. To create UPnP™ devices, you need to understand and implement these protocols.

![UPnP applications](doc/img/upnpapp.png)

mUPnP for C simplifies this process by handling these protocols automatically. It allows developers to focus on creating their devices and control points without worrying about the underlying protocol details.

mUPnP for C runs on Unix-like systems, macOS/iOS, Windows, and, since 3.1.0, on **ESP32 microcontrollers as an ESP-IDF component**.

## What's New in 3.1.0

- **ESP32 / ESP-IDF support**: use mUPnP as an ESP-IDF component on ESP32 (Wi-Fi station, IPv4), with a ready-to-build [Wi-Fi control-point example](examples/espidf/control_point) and an [ESP-IDF guide](doc/espidf.md).
- Reliability fixes found on ESP32 hardware: safe handling of XML allocation failures, GENA subscriptions kept across LOCATION changes, and HTTP server rebinding over TIME_WAIT.
- ESP-IDF v5.5.5 cross-build and host regression tests in CI.
- NULL-safe XML child-node lookups, correct `bool` handling from C++, and Xcode 27 project fixes.

See the [ChangeLog](ChangeLog) for details, including one API change: `mupnp_xml_attributelist_set()` now returns `bool`.

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
### MacOSX

For macOS platforms, you can easily install using Homebrew with the following `brew` commands:

```
brew tap cybergarage/homebrew
brew install mupnp
```

For MacOSX, I have released a wrapper class for Objective-C on Cocoa. Currently, the framework supports only basic functions of the control point. Please use the standard C library if you need to use all functions of mUPnP for C.

The bundled Xcode projects build with Xcode 27 and target macOS 12.0 / iOS 15.0 or later.

### ESP32 (ESP-IDF)

The repository can be used directly as an ESP-IDF component with Espressif's
managed Expat dependency. The port targets **ESP32 with one active Wi-Fi station
interface and IPv4**; the build baseline is ESP-IDF v5.5.5 (`>=5.5,<6.0`).

To try the Wi-Fi control-point example:

```
cd mupnp/examples/espidf/control_point
idf.py set-target esp32
idf.py menuconfig   # mUPnP control point example -> Wi-Fi SSID/password
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

To use mUPnP in your own application, place this repository at
`<project>/components/mupnp` (for example, as a Git submodule pinned to the
`3.1.0` tag), add `REQUIRES mupnp` to your component, and include
`<mupnp/upnp.h>`.

Limited control-point hardware testing on an ESP32 DevKitC-VE is reported.
Device hosting, other ESP32-family targets, IPv6, Ethernet, and long-duration
operation have not been validated yet. See the [ESP-IDF guide](doc/espidf.md)
for required `sdkconfig` settings, lifetime and memory notes, and a hardware
validation checklist.

### Windows

For Windows platforms, mUPnP includes platform projects for Visual Studio 2005. Please check the platform directories, mupnp/*/win32/vs2005, to use the projects. On WindowsCE, mUPnP does not have platform projects, but a contributor has verified that the source codes compile normally.

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
  - Embedded control point
    - [ESP32 Wi-Fi control point (ESP-IDF)](https://github.com/cybergarage/mupnp/tree/master/examples/espidf/control_point)
 
- UPnP device examples
  - UPnP standard devices
    - [UPnP BinaryLight:1 device](https://github.com/cybergarage/mupnp/tree/master/examples/binarylight)
  - UPnP non-standard devices
    - [UPnP clock device](https://github.com/cybergarage/mupnp/tree/master/examples/clock)
    
## Adoption and Usage

**mUPnP for C (formerly CyberLink for C)** has been used in the products and
projects below. The [ChangeLog](ChangeLog) records the name change in 2015;
these entries describe one continuous project's adoption history.

| Manufacturer / project | Product / project scope | Evidence |
| --- | --- | --- |
| NEC Aterm | WG2600HP, WG2600HP2, WG2600HP3 | [Official manuals](doc/adoption.md#nec-aterm) |
| TCL | 50CF630K, 55CF630K (UK manual) | [Official manual](doc/adoption.md#tcl) |
| iFFALCON | U75A / U85A series | [Official manual](doc/adoption.md#iffalcon) |
| Zyxel | EX5601-T1, EX3600-T0, DX3300-T0 and related models; specified firmware | [Official release notes](doc/adoption.md#zyxel) |
| Audio Group Denmark | Aavik Stream app | [Official licenses](doc/adoption.md#audio-group-denmark) |
| Panasonic | Blu-ray Remote 2011 app | [Official app page and EULA](doc/adoption.md#panasonic) |
| llink (open source) | Media server, macOS build integration | [Adopting project's README](doc/adoption.md#llink) |
| Nokia | Media Streamer app for Nokia 770 | [Project report and historical references](doc/adoption.md#nokia-770--media-streamer) |
| Panasonic | VIERA Remote for iOS | [Project report](doc/adoption.md#panasonic-viera-remote) |
| Toshiba / REGZA | Televisions reported by the project | [Project report and user-provided photo](doc/adoption.md#toshiba--regza-televisions) |

[Adoption evidence](doc/adoption.md) records source-reported component names and
versions, verification dates, product and firmware limits, and supporting images.
It distinguishes manufacturer disclosures, project reports and copyright-only
notices. The list does not imply manufacturer endorsement or use throughout a
manufacturer's range.

[^1]: UPnP™ is a certification mark of the UPnP™ Implementers Corporation.
