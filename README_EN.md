# Hashish Ethical Toolkit

![Project Status](https://img.shields.io/badge/Project%20Status-STABLE%20%F0%9F%91%8D-green)  
![GitHub Gif](https://github.com/Karim93160/Dark-Web/blob/7612fb26e60cc7816e80ae5bb792eeac1942dee8/20250530_022342.gif)

---

<div align="center">

[![Python](https://img.shields.io/badge/-%F0%9F%90%8DPython-3776AB?style=for-the-badge&logo=python&logoColor=FFD43B&labelColor=3776AB)](https://www.python.org/)  
[![Bash](https://img.shields.io/badge/-%E2%9A%A1%EF%B8%8FShell-4EAA25?style=for-the-badge&logo=gnu-bash&logoColor=white&labelColor=4EAA25)](https://www.gnu.org/software/bash/)  
[![C++](https://img.shields.io/badge/-%E2%9C%94%EF%B8%8FC++-FF69B4?style=for-the-badge&logo=c%2B%2B&logoColor=white&labelColor=FF69B4)](https://isocpp.org/)

</div>

![GitHub Gif](https://github.com/Karim93160/Dark-Web/blob/8ce3d8fac41a5cbeb94f8082d4febd5d688732d1/Screen_Recording_20250531_163158_Termux-ezgif.com-video-to-gif-converter-1.gif)  
[![Version](https://img.shields.io/badge/Version-5.1-red.svg)](https://github.com/Karim93160/wazabi)  
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://opensource.org/licenses/MIT)  
[![Maintained](https://img.shields.io/badge/Maintained-Yes-green.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/commits/main)  
[![Platform](https://img.shields.io/badge/Platform-Termux%20%7C%20Linux-lightgrey.svg?style=flat-square)](https://termux.com/)  
[![Contributions Welcome](https://img.shields.io/badge/Contributions-Welcome-brightgreen.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/CONTRIBUTING.md)

> ⚠️ **This is an ethical tool intended for security testing and educational purposes only. Malicious use is strictly prohibited.** ⚠️

---

## 🎯 Description

**Hashish Ethical Toolkit** is an open-source collection of tools designed for security professionals, cybersecurity enthusiasts, and ethical hackers. Primarily developed for Termux and Linux environments, Hashish provides a simple and intuitive interface for performing various tasks ranging from hash cracking to network reconnaissance and OSINT.

Our goal is to offer a versatile, easy-to-use toolkit that empowers users to understand and improve their security posture.

---

## 🛠️ Quick Installation 📥

*Copy* and *paste* the following command into Termux:

```bash
{ yes "N" | pkg update -y || true; } && \
{ yes "N" | pkg upgrade -y || true; } && \
pkg install -y git python nmap whois dnsutils curl rsync build-essential openssl ncurses-utils || \
{ echo "Package installation failed - attempting to continue..."; } && \
pip install shodan && \
git clone https://github.com/Karim93160/hashish.git ~/hashish && \
cd ~/hashish && \
chmod +x installer.sh && \
./installer.sh && \
termux-reload-settings
```
