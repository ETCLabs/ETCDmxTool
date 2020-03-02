# ETCDmxTool
[![Build status](https://ci.appveyor.com/api/projects/status/406b2wcp87ala2o9?svg=true)](https://ci.appveyor.com/project/docsteer/etcdmxtool)

ETCDmxTool is a free, open-source tool with the ability to capture DMX/RDM data; to control DMX levels; and to act as an RDM controller. It is provided by ETC Inc without any warranty, in the hope that it is a useful tool.

# Supported Platforms and DMX Devices
To interface to a DMX/RDM system, ETCDmxTool supports two USB devices - the ETC Gadget II and the ETC-USB whip. The features available differ slightly depending on the selected device:

| Feature              | USB Whip | Gadget II                           |
|----------------------|----------|-------------------------------------|
| DMX/RDM Data Capture | Yes      | Yes (Needs Firmware 1.2.0 or above) |
| DMX Output           | Yes      | Yes                                 |
| RDM Controller       | No       | Yes                                 |

## Windows
ETCDmxTool supports Windows (Win 7/Win 10) only.

| Hardware      | Windows Support |
|---------------|-----------------|
| USB Whip      | Yes             |
| Gadget I      | Yes?            |
| Gadget II     | Yes             |

## Linux
ETCDmxTool also supports Linux, but only certain hardware currently.

| Hardware      | Linux Support |
|---------------|---------------|
| USB Whip      | Yes           |
| Gadget I      | Not yet       |
| Gadget II     | Not yet       |

### Permissions
Read/write access is required to the relevant USB bus device
For systems that support udev, this can be applyed by copying "linux/99-etcusb.rules" to /etc/udev

# Connecting to a system
In order to sniff DMX/RDM, you will need to wire your Gadget2 in to your system in a slighly unusual way using male-male XLR connectors or similar. Below is an example of how this might be set up.

![How to sniff graphic](./doc/HowToSniff.png)

# Installation
An installer for the application is coming soon!

# Building
If you are interested in building the application you will need the following:

* Qt Library and Creator - https://www.qt.io/ide/
* To build the windows installer - NSIS - http://nsis.sourceforge.net/
	* If NSIS is included in your $PATH the installer will be built automatically

The dissectors are arranged as plugins; additional dissectors can simply be added to the src/dissectors directory

# About this ETCLabs Project

ETCDmxTool is designed to interact with ETC products, but it is not official ETC software. For challenges using, integrating, compiling, or modifying items in this project, we encourage posting on the Issues page. ETCDmxTool is a community-supported initiative, and the community is the best place to ask for help!
