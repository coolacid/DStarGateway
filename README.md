- [1. Introduction](#1-introduction)
- [2. Current State](#2-current-state)
  - [2.1. Code sanity](#21-code-sanity)
  - [2.2. Code Credit](#22-code-credit)
  - [2.3. Features](#23-features)
- [3. Building and installing](#3-building-and-installing)
  - [3.1. Prerequisites and dependencies](#31-prerequisites-and-dependencies)
  - [3.2. Building](#32-building)
  - [3.3. Installing](#33-installing)
  - [3.4. Configuring](#34-configuring)

# 1. Introduction
This is a port of G4KLX Jonathan Naylor's [ircddbGateway](https://github.com/g4klx/ircDDBGateway). It is wxWidgets free and has minimal dependencies to boost (header libs only), libconfig++ and libcurl

# 2. Current State
## 2.1. Code sanity
The current code is working, yet ugly IMHO as it is a mix of C and C++ of various ages.
The code has also been amended to no longer rely on compiler defines for paths like log or data. These can be set in configuration file.
## 2.2. Code Credit
- Jonathan Naylor G4KLX (The original author of [ircddbGateway](https://github.com/g4klx/ircDDBGateway))
- Thomas A. Early N7TAE (Code taken from his [smart-group](https://github.com/n7tae/smart-group-server) software)
- Geoffrey Merck F4FXL / KC3FRA [That's me !](https://github.com/F4FXL/)
## 2.3. Features
All the features found in ircddbGateway are supposed to be working. I have mixed feelings about putting these back in or not.

Features that where left out :

- CCS (is still being used ?) 
- Starnet (you might consider running [Smart Group Server XL](https://github.com/F4FXL/smart-group-server-xl) from a dedicated computer instead)
- Announcement (same can be achieved using transmitd)

# 3. Building and installing
It is assumed you know how to clone a git repository.
## 3.1. Prerequisites and dependencies
Before first time building you need to install dependencies and prerequisites
```
apt install build-essential libconfig++-dev libcurl4-openssl-dev libboost-dev
```
## 3.2. Building
```
make
```
## 3.3. Installing
The program is meant to run as a systemd service. All bits an pieces are provided.
```
sudo make install
```
## 3.4. Configuring
After installing you have to edit the configuration file. If you went with default paths, the config file is located in `/usr/local/etc/dstargateway.cfg`
The syntax is libconfig syntax, keep in my mind that **configuration keys are keys sensitive**.
When done with configuration, the daemon will be started automatically on boot. To manual start and stop it use the usual systemd commands
```
sudo systemctl start dstargateway.service
sudo systemctl stop dstargateway.service
```
