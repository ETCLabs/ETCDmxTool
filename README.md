# ETCDmxTool
![Build Status](https://github.com/ETCLabs/ETCDmxTool/actions/workflows/build.yml/badge.svg)

ETCDmxTool is a free, open-source tool with the ability to capture DMX/RDM data; to control DMX levels; and to act as an RDM controller. It is provided by ETC Inc without any warranty, in the hope that it is a useful tool.

# Supported DMX Devices
To interface to a DMX/RDM system, ETCDmxTool supports three USB devices - the ETC Gadget I, the ETC Gadget II and the ETC USB Whip. The features available differ slightly depending on the selected device:

| Feature              | USB Whip           | Gadget I<sup>1</sup> | Gadget II<sup>1</sup>           | Gadget II (CPU2)<sup>1</sup>    |
|----------------------|--------------------|----------------------|---------------------------------|---------------------------------|
| DMX/RDM Data Capture | :heavy_check_mark: | :x:                  | :heavy_check_mark: <sup>2</sup> | :white_check_mark: <sup>3</sup> |
| DMX Output           | :heavy_check_mark: | :heavy_check_mark:   | :heavy_check_mark:              | :heavy_check_mark:              |
| RDM Controller       | :x:                | :heavy_check_mark:   | :heavy_check_mark:              | :white_check_mark: <sup>3</sup> |

<sup>1</sup> Windows Only  
<sup>2</sup> Firmware v1.2.0 or above
<sup>3</sup> Coming in future firmware versions

# Supported Platforms
## Windows
ETCDmxTool is supported on Windows (Win 7/Win 10).

An installer is provided on the [release page](https://github.com/ETCLabs/ETCDmxTool/releases/latest/)

## Linux
ETCDmxTool also supports Linux, but only certain hardware currently (see the table above).

For help with configuring on Linux, please see the [Linux readme](linux/README.md).

# Connecting to a system
In order to sniff DMX/RDM, you will need to wire your DMX device in to your system in a slighly unusual way using male-male XLR connectors or similar. Below is an example of how this might be set up.

![How to sniff graphic](./doc/HowToSniff.png)

# Scripting
The application supports the ability to run in a scripted mode which may help with automation of testing or troubleshooting. For more info, take a look at the [Scripting](./doc/SCRIPTING.md) readme file.

# Building
If you are interested in building the application you will need the following:

* Qt Library and Creator - https://www.qt.io/ide/
* To build the windows installer - NSIS - http://nsis.sourceforge.net/
	* If NSIS is included in your $PATH the installer will be built automatically

The dissectors are arranged as plugins; additional dissectors can simply be added to the src/dissectors directory

# About this ETCLabs Project

ETCDmxTool is designed to interact with ETC products, but it is not official ETC software. For challenges using, integrating, compiling, or modifying items in this project, we encourage posting on the Issues page. ETCDmxTool is a community-supported initiative, and the community is the best place to ask for help!
