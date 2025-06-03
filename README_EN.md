![GitHub stars](https://img.shields.io/github/stars/Karim93160/hashish?style=social)
![GitHub forks](https://img.shields.io/github/forks/Karim93160/hashish?style=social)

[🇫🇷 Français](https://github.com/karim93160/hashish/blob/main/README.md) | [🇬🇧 English](https://github.com/karim93160/hashish/blob/main/README_EN.md) | [🇪🇸 Español](https://github.com/karim93160/hashish/blob/main/README_ES.md)

![Project Status](https://img.shields.io/badge/Project%20Status-STABLE%20%F0%9F%91%8D-green)
![GitHub Gif](https://github.com/Karim93160/Dark-Web/blob/7612fb26e60cc7816e80ae5bb792eeac1942dee8/20250530_022342.gif)

---

<div align="center">

[![Python](https://img.shields.io/badge/-%F0%9F%90%8DPython-3776AB?style=for-the-badge&logo=python&logoColor=FFD43B&labelColor=3776AB)](https://www.python.org/)
[![Bash](https://img.shields.io/badge/-%E2%9A%A1%EF%B8%8FShell-4EAA25?style=for-the-badge&logo=gnu-bash&logoColor=white&labelColor=4EAA25)](https://www.gnu.org/software/bash/)
[![C++](https://img.shields.io/badge/-%E2%9C%94%EF%B8%8FC++-FF69B4?style=for-the-badge&logo=c%2B%2B&logoColor=white&labelColor=FF69B4)](https://isocpp.org/)

</div>

![GitHub Gif](https://github.com/Karim93160/Dark-Web/blob/1ac92f41e1878d3e101b6177d0ca3249896d21fd/Screen_Recording_20250603_234629_Termux-ezgif.com-video-to-gif-converter.gif)
[![Version](https://img.shields.io/badge/Version-5.5.1-blue.svg)](https://github.com/Karim93160/wazabi)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://opensource.org/licenses/MIT)
[![Maintenance](https://img.shields.io/badge/Maintained-Yes-green.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/commits/main)
[![Platform](https://img.shields.io/badge/Platform-Termux%20%7C%20Linux-lightgrey.svg?style=flat-square)](https://termux.com/)
[![Contributions Welcome](https://img.shields.io/badge/Contributions-Welcome-brightgreen.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/CONTRIBUTING.md)

> ⚠️ **This is an ethical tool intended for security testing and education. Any malicious use is strictly prohibited.** ⚠️

## 🎯 Introduction

**Hashish Ethical Toolkit** is a collection of open-source tools designed for security professionals, cybersecurity enthusiasts, and ethical security researchers. Developed primarily for use on Termux and Linux environments, Hashish offers a simple and intuitive interface to perform various tasks, from hash cracking to network reconnaissance and OSINT.

Our goal is to provide a versatile and easy-to-use toolkit that allows users to understand and improve their security posture.

---
## 🛠️ Quick Installation 📥
*Copy* and *Paste* into Termux
the following command:
```
{ yes "N" | pkg update || true; } && \
{ yes "N" | pkg upgrade || true; } && \
pkg install -y git python nmap whois dnsutils curl rsync build-essential openssl \
clang ncurses-utils termux-tools && \
pip install shodan && \
git clone https://github.com/Karim93160/hashish.git ~/hashish && \
cd ~/hashish && \
chmod +x installer.sh && \
./installer.sh && \
termux-reload-settings

```

---
<div align="center">
  <h2>💬 Hashish Community - Join the Discussion! 💬</h2>
  <p>
    Have questions, ideas, or just want to chat about the Hashish project?
    Join our community on GitHub Discussions! It's the perfect place for open conversations and general support.
  </p>
  <p>
    <a href="https://github.com/karim93160/hashish/discussions">
      <img src="https://img.shields.io/badge/Join%20Community-Discussions-blue?style=for-the-badge&logo=github" alt="Join Community">
    </a>
  </p>
</div>


**We are excited to announce a**
### *Major Update: Integration of Optimized C++ Cracking Module and Rainbow Table Attack!* 🎉

We are thrilled to announce a significant improvement to HASHISH: the integration of a **completely rewritten hash cracking module in C++** (`hashcracker.cpp`) and the introduction of a **rainbow table attack method** with its dedicated generator (`rainbow_generator.cpp`)! This update brings increased performance and new capabilities for your analyses.

#### **What's New with `hashcracker.cpp`?**

1. **Superior Speed 🚀:** C++ allows much faster execution of hash calculations and comparisons compared to previous versions. Expect notable acceleration, especially for brute force and dictionary attacks on large lists.

2. **Precise Time Estimation ⏱️:** Before launching a brute force attack, HASHISH now performs a **quick benchmark** to estimate your system's hash speed (in H/s). Based on this and the complexity (password length, character set), it provides an estimate of the **total number of attempts** needed and, most importantly, the **estimated time** to complete cracking (displayed in days, hours, minutes, seconds).
    * This allows you to make informed decisions about the feasibility of an attack before starting it, thus avoiding waiting indefinitely.

3. **Improved Multithreading Management 🔥:** The C++ module fully leverages your processor's **multiple cores** (thanks to OpenMP when available) to distribute hashing tasks, thus accelerating attacks.

---

### **New Method: Rainbow Table Attack 🌈**

In addition to optimizations, HASHISH introduces the powerful **rainbow table** attack technique. This method offers a time-memory trade-off, allowing hash cracking much faster than brute force, provided you have a pre-calculated table.

#### **Rainbow Method Description**

A rainbow table stores reduced hash chains. Each chain starts with a potential password, which is hashed. The result is then passed through a "reduction function" to convert it back into a new potential password, which is in turn hashed, and so on, for a certain length. Only the initial password and final hash of each chain are stored.

During the attack, the target hash is repeatedly reduced and hashed, checking if any of the resulting hashes match a final hash in the table. If a match is found, the chain is reconstructed from the stored initial password to recover the original password.

* **Advantages:**
    * ⚡ **High cracking speed** once the table is generated and loaded.
    * 💡 Fewer real-time calculations compared to brute force for the same search space (if the table covers it).
* **Disadvantages:**
    * ⏳ **Significant precalculation time** to generate the table.
    * 💾 **Considerable storage space** for large tables.
    * 🎯 **Specificity:** A table is generally specific to a hash algorithm (MD5, SHA256, etc.), character set, and password length range.

#### **`rainbow_generator.cpp`: Forge Your `rainbow.txt` ⚙️**

Before you can launch a rainbow table attack, you need to generate a table. This is the role of `rainbow_generator.cpp`. This standalone tool allows you to create your table file (named `rainbow.txt` by default, used later by `hashcracker.cpp`).

**User Manual for `rainbow_generator.cpp` (Presets for `rainbow.txt`):**

1. **Launch `rainbow_generator.cpp`** (it will be compiled as `rainbow_generator` or similar).
2. **Parameters to define during generation:** The tool will guide you to enter the following information. The **consistency** of these parameters between generation and attack is **ABSOLUTELY CRUCIAL**.

    | Parameter          | Example Value                    | Description                                                                                                | Concerned Module       | Emphasis          |
    | :----------------- | :-------------------------------- | :--------------------------------------------------------------------------------------------------------- | :-------------------- | :---------------- |
    | 📤 **Output File** | `rainbow.txt`                     | Name of the file where the table will be saved (format: `initial_password:final_chain_hash`).              | `rainbow_generator.cpp` | 📝                |
    | 🔡 **Charset** | `abcdefghijklmnopqrstuvwxyz0123456789` | Character set used to generate passwords in chains. **MUST match target charset.** | `rainbow_generator.cpp` | 🔑 **Crucial!** |
    | 📏 **Min Pass Len**| `6`                               | Minimum length of passwords to generate in chains.                                                        | `rainbow_generator.cpp` | 🔢                |
    | 📐 **Max Pass Len**| `8`                               | Maximum length of passwords to generate in chains.                                                        | `rainbow_generator.cpp` | 🔢                |
    | 🔗 **Num. Chains** | `1000000` (1 Million)            | Number of chains to generate. Impacts table coverage and size. More is better (but bigger/longer).        | `rainbow_generator.cpp` | 📈                |
    | ⛓️ **Chain Length** | `10000` (ten thousand)           | Length of each chain (number of hashes/reductions). Trade-off between search time and size.               | `rainbow_generator.cpp` | ⚖️                |
    | 🛡️ **Hash Type** | `MD5` / `SHA256` etc.             | Hash algorithm used (e.g., MD5, SHA1, SHA256). **MUST match target hash type.** | `rainbow_generator.cpp` | 🔑 **Crucial!** |

3. **Important Considerations:**
    * **Absolute Consistency 🔑:** The `Charset`, `Min/Max Pass Len` range, and `Hash Type` used for generation **must imperatively be the same** as those you will specify during the attack with `hashcracker.cpp` and must correspond to the expected characteristics of the target password. Any inconsistency will make the table useless for that specific hash.
    * **Time & Space ⏳💾:** Rainbow table generation, especially for large character sets, long chains, large number of chains, or long passwords, can take **enormous amounts of time** (hours, days, even weeks) and consume a **significant amount of disk space** (from a few MB to several GB or TB). Plan accordingly!
    * The generator will produce a file (e.g., `rainbow.txt`) containing `initial_password:final_chain_hash` pairs.

#### **Using `rainbow.txt` in `hashcracker.cpp` (Attack Phase) ⚔️**

Once your `rainbow.txt` table (or a file with a custom name) is generated:

1. Launch `hashcracker.cpp`.
2. Enter the hash to crack.
3. Choose option **3. Rainbow Table Attack**.
4. The program will ask for:
    * The path to your rainbow table file (by default, it will look for `rainbow.txt` in the executable's directory).
    * **Table parameters:**
        * The **Reduction Charset** (the one used during table generation).
        * The **chain length** (Chain Length) of the table.
        * The **assumed minimum and maximum password length** (Min/Max Password Length) during generation.
5. `hashcracker.cpp` will then load the table into memory (this can take time for large tables) and begin the search process. It will apply reduction and hash functions to the target hash, looking for matches with final hashes in the table. If a match is found, it will reconstruct the chain to recover the password.

---

### **How to Use the New Modules?**

The `hashcracker.cpp` module is now the main engine for **dictionary** and **brute force** attacks when you choose these options in HASHISH. For the **rainbow table** attack, you will first need to generate a table with `rainbow_generator.cpp` (or obtain a compatible table), then use the dedicated option in `hashcracker.cpp`.

We are convinced that these updates will make HASHISH even more powerful and useful for your ethical security and learning needs. Don't hesitate to try them and share your feedback!

---
## ✨ Main Features

**Hash Cracker:**
Decrypt different types of hashes using common techniques.

**Web Scanner:**
Perform basic security analyses on websites and IP addresses.

**Reconnaissance:**
Collect information about targets using various network reconnaissance techniques.

**OSINT (Open-Source Intelligence):**
Gather publicly accessible information about specific targets.

---

## 🚀 Usage

To launch the main tool:


*Or, if global installation failed:*


________

*🤝 Contributions*

**Contributions are welcome! If you want to improve Hashish, fix bugs, or add new features, please consult our Contribution Guide.**

[![Sponsor me on GitHub](https://img.shields.io/badge/Sponsor-GitHub-brightgreen.svg)](https://github.com/sponsors/karim93160)
[![Buy me a coffee](https://img.shields.io/badge/Donate-Buy%20Me%20A%20Coffee-FFDD00.svg)](https://www.buymeacoffee.com/karim93160)
[![Support me on Ko-fi](https://img.shields.io/badge/Donate-Ko--fi-F16061.svg)](https://ko-fi.com/karim93160)
[![Support me on Patreon](https://img.shields.io/badge/Patreon-Support%20me-FF424D.svg)](https://www.patreon.com/karim93160)
[![Donate on Liberapay](https://img.shields.io/badge/Donate-Liberapay-F6C915.svg)](https://liberapay.com/karim93160/donate)

_________

## License 📜

hashish is distributed under the [MIT License](https://github.com/Karim93160/hashish/blob/677eecb0be4fc81a0becc2c2154c65ffe73ddbb1/LICENSE)

_________

## Contact 📧

For any questions or suggestions, don't hesitate to open an [issue on GitHub](https://github.com/Karim93160/hashish/issues) or contact us by email:

[![Contact by Email](https://img.shields.io/badge/Contact-by%20Email-blue.svg)](mailto:karim9316077185@gmail.com)

_________
<div align="center">
  <h2>🌿 Hashish - Code of Conduct 🌿</h2>
  <p>
    We are committed to creating a welcoming and respectful environment for all contributors.
    Please take a moment to read our <a href="CODE_OF_CONDUCT.md">Code of Conduct</a>.
    By participating in this project, you agree to abide by its terms.
  </p>
  <p>
    <a href="CODE_OF_CONDUCT.md">
      <img src="https://img.shields.io/badge/Code%20of%20Conduct-Please%20Read-blueviolet?style=for-the-badge&logo=github" alt="Code of Conduct">
    </a>
  </p>
</div>

<div align="center">
  <h2>🐞 Report a Bug in Hashish 🐞</h2>
  <p>
    Encountering an issue with Hashish? Help us improve the project by reporting bugs!
    Click the button below to directly open a new, pre-filled bug report.
  </p>
  <p>
    <a href="https://github.com/karim93160/hashish/issues/new?assignees=&labels=bug&projects=&template=bug_report.md&title=">
      <img src="https://img.shields.io/badge/Report%20a%20Bug-Open%20an%20Issue-red?style=for-the-badge&logo=bugsnag" alt="Report a Bug">
    </a>
  </p>
</div>

**⚠️ Warning**

*Hashish Ethical Toolkit is intended for ethical and legal use only. The developers are not responsible for any abusive or illegal use of this tool.*
<p align="center">
Made with ❤️ by Karim
</p>
