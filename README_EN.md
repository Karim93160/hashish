[🇫🇷 Français](https://github.com/karim93160/hashish/blob/main/README.md) | [🇬🇧 English](https://github.com/karim93160/hashish/blob/main/README_EN.md) | [🇪🇸 Español](https://github.com/karim93160/hashish/blob/main/README_ES.md)

---

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
[![Maintenance](https://img.shields.io/badge/Maintained-Yes-green.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/commits/main)
[![Platform](https://img.shields.io/badge/Platform-Termux%20%7C%20Linux-lightgrey.svg?style=flat-square)](https://termux.com/)
[![Contributions Welcome](https://img.shields.io/badge/Contributions-Welcome-brightgreen.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/CONTRIBUTING.md)

> ⚠️ **This is an ethical tool for security testing and education. Any malicious use is strictly prohibited.** ⚠️

## 🎯 Overview

**Hashish Ethical Toolkit** is a collection of open-source tools designed for security professionals, cybersecurity enthusiasts, and ethical security researchers.

Our goal is to provide a versatile and easy-to-use toolkit, allowing users to understand and improve their security posture.

---

## 🛠️ Quick Installation 📥
*Copy* and then *Paste* into Termux
the following command:
```bash
{ yes "N" | pkg update -y || true; } && \
{ yes "N" | pkg upgrade -y || true; } && \
pkg install -y git python nmap whois dnsutils curl rsync build-essential openssl ncurses-utils || \
{ echo "Package installation failed - attempting to continue..."; } && \
pip install shodan && \
git clone [https://github.com/Karim93160/hashish.git](https://github.com/Karim93160/hashish.git) ~/hashish && \
cd ~/hashish && \
chmod +x installer.sh && \
./installer.sh && \
termux-reload-settings

```

We are excited to announce a
Major Update: Integration of an Optimized C++ Cracking Module and Rainbow Table Attack! 🎉
We are excited to announce a significant improvement to HASHISH: the integration of a hash cracking module completely rewritten in C++ (hashcracker.cpp) and the introduction of a rainbow table attack method with its dedicated generator (rainbow_generator.cpp)! This update brings increased performance and new capabilities for your analyses.
What changes with hashcracker.cpp?
 * Superior Speed 🚀: C++ allows for much faster execution of hash calculations and comparisons compared to previous versions. Expect a noticeable acceleration, especially for brute-force and dictionary attacks on large lists.
 * Precise Time Estimation ⏱️: Before launching a brute-force attack, HASHISH now performs a quick benchmark to estimate your system's hashing speed (in H/s). Based on this and the complexity (password length, character set), it provides you with an estimate of the total number of attempts required and, most importantly, the estimated time to complete the cracking (displayed in days, hours, minutes, seconds).
   * This allows you to make informed decisions about the feasibility of an attack before launching it, thus avoiding waiting indefinitely.
 * Improved Multithreading Management 🔥: The C++ module fully leverages the multiple cores of your processor (thanks to OpenMP when available) to distribute hashing tasks, thus accelerating attacks.
New Method: Rainbow Table Attack 🌈
In addition to the optimizations, HASHISH introduces the powerful rainbow table attack technique. This method offers a time-memory trade-off, allowing for much faster hash cracking than brute force, provided you have a pre-calculated table.
Description of the Rainbow Method
A rainbow table stores chains of reduced hashes. Each chain starts with a potential password, which is hashed. The result is then passed through a "reduction function" to convert it back into a new potential password, which is in turn hashed, and so on, over a certain length. Only the starting password and the final hash of each chain are stored.
 * Advantages:
   * ⚡ High cracking speed once the table is generated and loaded.
   * 💡 Fewer real-time calculations compared to brute force for the same search space (if the table covers it).
 * Disadvantages:
   * ⏳ Significant pre-calculation time to generate the table.
   * 💾 Considerable storage space for large tables.
   * 🎯 Specificity: A table is generally specific to a hashing algorithm (MD5, SHA256, etc.), a character set, and a password length range.
rainbow_generator.cpp: Forge your rainbow.txt ⚙️
Before you can launch a rainbow table attack, you need to generate a table. This is the role of rainbow_generator.cpp. This standalone tool allows you to create your table file (named rainbow.txt by default, used later by hashcracker.cpp).
User Manual for rainbow_generator.cpp (Presets for rainbow.txt):
 * Launch rainbow_generator.cpp (it will be compiled under the name rainbow_generator or similar).
 * Parameters to set during generation: The tool will guide you to enter the following information. The consistency of these parameters between generation and attack is ABSOLUTELY CRUCIAL.
   | Parameter | Example Value | Description | Concerned Module | Emphasis |
   |---|---|---|---|---|
   | 📤 Output File | rainbow.txt | Name of the file where the table will be saved (format: initial_password:final_hash_of_chain). | rainbow_generator.cpp | 📝 |
   | 🔡 Charset | abcdefghijklmnopqrstuvwxyz0123456789 | Set of characters used to generate passwords in the chains. MUST match the target charset. | rainbow_generator.cpp | 🔑 Crucial! |
   | 📏 Min Pass Len | 6 | Minimum length of passwords to generate within the chains. | rainbow_generator.cpp | 🔢 |
   | 📐 Max Pass Len | 8 | Maximum length of passwords to generate within the chains. | rainbow_generator.cpp | 🔢 |
   | 🔗 Nb. Chains | 1000000 (1 Million) | Number of chains to generate. Impacts the coverage of the table and its size. The more, the better (but the larger/longer). | rainbow_generator.cpp | 📈 |
   | ⛓️ Chain Length | 10000 (ten thousand) | Length of each chain (number of hashes/reductions). Trade-off between search time and size. | rainbow_generator.cpp | ⚖️ |
   | 🛡️ Hash Type | MD5 / SHA256 etc. | Hashing algorithm used (ex: MD5, SHA1, SHA256). MUST match the target hash type. | rainbow_generator.cpp | 🔑 Crucial! |
 * Important Considerations:
   * Absolute Consistency 🔑: The Charset, the Min/Max Pass Len range, and the Hash Type used for generation must absolutely be the same as those you will specify during the attack with hashcracker.cpp and must match the expected characteristics of the target password. Any inconsistency will make the table useless for that specific hash.
   * Time & Space ⏳💾: Generating rainbow tables, especially for large character sets, long chains, a large number of chains, or long passwords, can take a lot of time (hours, days, or even weeks) and consume a significant amount of disk space (from a few MB to several GB or TB). Plan accordingly!
   * The generator will produce a file (for example, rainbow.txt) containing starting_password:end_of_chain_hash pairs.
Using rainbow.txt in hashcracker.cpp (Attack Phase) ⚔️
Once your rainbow.txt table (or a file with a custom name) is generated:
 * Launch hashcracker.cpp.
 * Enter the hash to crack.
 * Choose option 3. Rainbow Table Attack.
 * The program will ask you:
   * The path to your rainbow table file (by default, it will look for rainbow.txt in the executable's directory).
   * The table parameters:
     * The Charset for the reduction (the one used during table generation).
     * The chain length of the table.
     * The minimum and maximum password length assumed (Min/Max Password Length) during generation.
 * hashcracker.cpp will then load the table into memory (this may take time for large tables) and begin the search process. It will apply the reduction and hash functions to the target hash, looking for matches with the final hashes in the table. If a match is found, it will reconstruct the chain to find the password.
How to use the new modules?
The hashcracker.cpp module is now the main engine for dictionary and brute-force attacks when you choose these options in HASHISH. For the rainbow table attack, you will first need to generate a table with rainbow_generator.cpp (or obtain a compatible table), then use the dedicated option in hashcracker.cpp.
We are confident that these updates will make HASHISH even more powerful and useful for your ethical security needs and learning. Feel free to try them out and give us your feedback!
✨ Main Features
Hash Cracker:
Decrypt different types of hashes using common techniques.
Web Scanner:
Perform basic security scans on websites and IP addresses.
Reconnaissance:
Collect information about targets using various network reconnaissance techniques.
OSINT (Open-Source Intelligence): Gather publicly accessible information about specific targets.
🚀 Usage
To launch the main tool:
hashish

Or, if global installation failed:
python3 hashish.py

🤝 Contributions
Contributions are welcome! If you want to improve Hashish, fix bugs, or add new features, please see our Contribution Guide.





License 📜
hashish is distributed under the MIT License
Contact 📧
For any questions or suggestions, feel free to open an issue on GitHub or contact us by email:
⚠️ Warning
Hashish Ethical Toolkit is intended for ethical and legal use only. The developers are not responsible for any misuse or illegal use of this tool.
<p align="center">
Made with ❤️ by Karim
</p>

