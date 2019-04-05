5G-EmPOWER: Mobile Networks Operating System
=========================================

### What is 5G-EmPOWER?
5G-EmPOWER is a mobile network operating system designed for heterogeneous wireless/mobile networks.

### Top-Level Features
* Supports both LTE and WiFi radio access networks
* Northbound abstractions for a global network view, network graph, and application intents.
* REST API and native (Python) API for accessing the Northbound abstractions
* Support for Click-based Lightweight Virtual Networks Functions
* Declarative VNF chaning on precise portion of the flowspace
* Flexible southbound interface supporting WiFi APs and LTE eNBs

Checkout out our [website](http://5g-empower.io/) and our [wiki](https://github.com/5g-empower/5g-empower.github.io/wiki)

This repository includes the 5G-EmPOWER SBi protocol definition. This component is usually included by the eNB Agent to correctly send and receive messages from EmPOWER Runtime application.

### Pre-requisites
In order to successfully build the protocols you need to install the standard linux build tools

`sudo apt-get install build-essential`

### Build from source
The standard procedure assume that you want to install the software and the necessary headers in the system default directories `/usr/include/emage` for headers, and `/usr/lib` for the library shared objects. 

You can always personalize such installation by modifying the `INCLDIR` and `INSTDIR` variable present in the Makefile present in the root directory of the repository.

After having setup the required paths for installation, run (in the root directory) the following command to build the software:

`make`

### Install
As previously said, the software will be installed in your system alongside other libraries. To change this behavior you can modify the variables present in the makefile (see build instruction).

After having built the software, to install it run the following command:

`sudo make install`

Again, as for any other installation, you will need superuser rights in order to perform such changes on the filesystem.

### Uninstalling
You can always undo installation operations sometimes. In order to do so a "cleaning" operation is also present as a makefile option. This uninstalling steps revert the modification done by the installation stage, completely cleaning your Linux from the software:

`sudo make uninstall`

Again, as for any other installation, you will need superuser rights in order to perform such changes on the filesystem.

### License
Code is released under the Apache License, Version 2.0.
