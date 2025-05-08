# ☠︎ Hashish Ethical Toolkit ☠︎

[![Version](https://img.shields.io/badge/Version-1.0-blue.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](https://opensource.org/licenses/MIT)
[![Maintenance](https://img.shields.io/badge/Maintained-Yes-green.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/commits/main)
[![Platform](https://img.shields.io/badge/Platform-Termux%20%7C%20Linux-lightgrey.svg?style=flat-square)](https://termux.com/)
[![Python](https://img.shields.io/badge/Python-3.x-blueviolet.svg?style=flat-square)](https://www.python.org/)
[![Contributions Welcome](https://img.shields.io/badge/Contributions-Welcome-brightgreen.svg?style=flat-square)](https://github.com/YOUR_GITHUB_USERNAME/hashish/CONTRIBUTING.md)

<p align="center">
  <img src="https://github.com/Karim93160/Dark-Web/blob/e86debecc2b826d0743a21cea883812e31482fc2/Screenshot_20250508_014648_Termux.jpg" alt="Hashish Banner" width="600">
</p>

> ⚠️ **Ceci est un outil éthique destiné aux tests de sécurité et à l'éducation. Toute utilisation malveillante est strictement interdite.** ⚠️

## 🎯 Présentation

**Hashish Ethical Toolkit** est une collection d'outils open-source conçus pour les professionnels de la sécurité, les passionnés de cybersécurité et les chercheurs en sécurité éthique. Développé pour être utilisé principalement sur Termux et les environnements Linux, Hashish offre une interface simple et intuitive pour effectuer diverses tâches, allant du craquage de hachages à la reconnaissance réseau et à l'OSINT.

Notre objectif est de fournir une boîte à outils polyvalente et facile à utiliser, permettant aux utilisateurs de comprendre et d'améliorer leur posture de sécurité.

## ✨ Fonctionnalités Principales

 **Hash Cracker :** 
 Déchiffrez différents types de hachages en utilisant des techniques courantes.

 
 **Web Scanner :** 
 Effectuez des analyses de base de sécurité sur des sites web et des adresses IP.

 
 **Reconnaissance :** 
 Collectez des informations sur des cibles en utilisant diverses techniques de reconnaissance réseau.

 
 **OSINT (Open-Source Intelligence) :** Rassemblez des informations accessibles publiquement sur des cibles spécifiques.

 
 **(⚙️ À venir) SQL Injection :** Modules pour identifier et exploiter les vulnérabilités d'injection SQL.

 
 **(⚙️ À venir) Hydra Bruteforce :** Outil de bruteforce pour divers services.

## 🛠️ Installation

Suivez ces étapes pour installer Hashish sur votre système Termux ou Linux :

1.  **Cloner le dépôt :**
    ```bash
    git clone https://github.com/karim93160/hashish.git
    ```

2.  **Se rendre dans le répertoire cloné :**
    ```bash
    cd hashish
    ```

3.  **Rendre le script d'installation exécutable :**
    ```bash
    chmod +x installer.sh
    ```

4.  **Exécuter le script d'installation :**
    ```bash
    ./installer.sh
    ```

    Ce script va :
    * Créer le dossier `modules`.
    * Déplacer les modules nécessaires.
    * Installer les dépendances Python requises (listées dans `requirements.txt`).
    * Tenter de rendre `hashish` et les wrappers des modules accessibles globalement.

5.  **(Facultatif) Installer les outils système nécessaires :**
    ```bash
    pkg install nmap whois traceroute dnsutils curl shodan -y
    ```
    N'oubliez pas de configurer votre clé API Shodan avec `shodan init <VOTRE_CLE_API>`.

## 🚀 Utilisation

Pour lancer l'outil principal :

```bash
hashish
```
*Ou, si l'installation globale a échoué :*

python3 hashish.py

*Vous pouvez également utiliser directement les commandes pour les modules (si l'installation des wrappers a réussi) :* 

hashcracker <hash_à_décrypter>

webscanner <url_ou_ip>

recon <ip_ou_url_cible>

osint <email_ou_url_ou_ip>

________

*🤝 Contributions*

**Les contributions sont les bienvenues ! Si vous souhaitez améliorer Hashish, corriger des bugs ou ajouter de nouvelles fonctionnalités, veuillez consulter notre Guide de Contribution.**

[![Sponsor me on GitHub](https://img.shields.io/badge/Sponsor-GitHub-brightgreen.svg)](https://github.com/sponsors/karim93160)
[![Buy me a coffee](https://img.shields.io/badge/Donate-Buy%20Me%20A%20Coffee-FFDD00.svg)](https://www.buymeacoffee.com/karim93160)
[![Support me on Ko-fi](https://img.shields.io/badge/Donate-Ko--fi-F16061.svg)](https://ko-fi.com/karim93160)
[![Support me on Patreon](https://img.shields.io/badge/Patreon-Support%20me-FF424D.svg)](https://www.patreon.com/karim93160)
[![Donate on Liberapay](https://img.shields.io/badge/Donate-Liberapay-F6C915.svg)](https://liberapay.com/karim93160/donate)


_________

*📜 Licence*

**Ce projet est sous licence MIT.**

_________

**⚠️ Avertissement**

*Hashish Ethical Toolkit est destiné à un usage éthique et légal uniquement. Les développeurs ne sont pas responsables de toute utilisation abusive ou illégale de cet outil.*
<p align="center">
Fait avec ❤️ par Karim
</p>
