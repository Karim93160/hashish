![GitHub stars](https://img.shields.io/github/stars/Karim93160/hashish?style=social)
![GitHub forks](https://img.shields.io/github/forks/Karim93160/hashish?style=social)

[🇫🇷 Français](https://github.com/karim93160/hashish/blob/main/README.md) | [🇬🇧 English](https://github.com/karim93160/hashish/blob/main/README_EN.md) | [🇪🇸 Español](https://github.com/karim93160/hashish/blob/main/README_ES.md)

![Statut du Projet](https://img.shields.io/badge/Statut%20du%20Projet-STABLE%20%F0%9F%91%8D-green)

---

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


> ⚠️ **Ceci est un outil éthique destiné aux tests de sécurité et à l'éducation. Toute utilisation malveillante est strictement interdite.** ⚠️

## 🎯 Présentation

**Hashish Ethical Toolkit** est une collection d'outils open-source conçus pour les professionnels de la sécurité, les passionnés de cybersécurité et les chercheurs en sécurité éthique. Développé pour être utilisé principalement sur Termux et les environnements Linux, Hashish offre une interface simple et intuitive pour effectuer diverses tâches, allant du craquage de hachages à la reconnaissance réseau et à l'OSINT.

Notre objectif est de fournir une boîte à outils polyvalente et facile à utiliser, permettant aux utilisateurs de comprendre et d'améliorer leur posture de sécurité.

---
## 🛠️ Installation Rapide 📥
*Copiez* puis *Collez* dans Termux
la commande suivante :
```{ yes "N" | pkg update -y || true; } && \
{ yes "N" | pkg upgrade -y || true; } && \
pkg install -y git python nmap whois dnsutils curl rsync build-essential openssl ncurses-utils || \
{ echo "Installation des paquets échouée - tentative de continuation..."; } && \
pip install shodan && \
git clone https://github.com/Karim93160/hashish.git ~/hashish && \
cd ~/hashish && \
chmod +x installer.sh && \
./installer.sh && \
termux-reload-settings

```
---
**Nous sommes ravis d'annoncer une**
### *mise à Jour Majeure : Intégration d'un Module de Cracking C++ Optimisé et Attaque par Table Arc-en-Ciel!* 🎉

Nous sommes ravis d'annoncer une amélioration significative de HASHISH : l'intégration d'un **module de cracking de hachage entièrement réécrit en C++** (`hashcracker.cpp`) et l'introduction d'une **méthode d'attaque par table arc-en-ciel** avec son générateur dédié (`rainbow_generator.cpp`) ! Cette mise à jour apporte des performances accrues et de nouvelles capacités pour vos analyses.

#### **Qu'est-ce qui change avec `hashcracker.cpp` ?**

1.  **Vitesse Supérieure 🚀 :** Le C++ permet une exécution beaucoup plus rapide des calculs de hachage et des comparaisons par rapport aux versions précédentes. Attendez-vous à une accélération notable, en particulier pour les attaques par force brute et par dictionnaire sur de grandes listes.
2.  **Estimation Précise du Temps ⏱️ :** Avant de lancer une attaque par force brute, HASHISH effectue désormais un **benchmark rapide** pour estimer la vitesse de hachage (en H/s) de votre système. En fonction de cela et de la complexité (longueur du mot de passe, jeu de caractères), il vous fournit une estimation du **nombre total de tentatives** nécessaires et, surtout, le **temps estimé** pour compléter le cracking (affiché en jours, heures, minutes, secondes).
    * Cela vous permet de prendre des décisions éclairées sur la faisabilité d'une attaque avant de la lancer, évitant ainsi d'attendre indéfiniment.
3.  **Gestion Améliorée du Multithreading 🔥 :** Le module C++ tire pleinement parti des **multiples cœurs de votre processeur** (grâce à OpenMP lorsque disponible) pour distribuer les tâches de hachage, accélérant ainsi les attaques.

---

### **Nouvelle Méthode : Attaque par Table Arc-en-Ciel 🌈**

En plus des optimisations, HASHISH introduit la puissante technique d'attaque par **table arc-en-ciel**. Cette méthode offre un compromis temps-mémoire, permettant de cracker des hachages beaucoup plus rapidement que la force brute, à condition d'avoir une table précalculée.

#### **Description de la Méthode Arc-en-Ciel**

Une table arc-en-ciel stocke des chaînes de hachages réduits. Chaque chaîne commence par un mot de passe potentiel, qui est haché. Le résultat est ensuite passé par une "fonction de réduction" pour le reconvertir en un nouveau mot de passe potentiel, qui est à son tour haché, et ainsi de suite, sur une certaine longueur. Seuls le mot de passe de départ et le hachage final de chaque chaîne sont stockés.

Lors de l'attaque, le hachage cible est réduit et haché à plusieurs reprises, en cherchant si l'un des hachages résultants correspond à un hachage final dans la table. Si une correspondance est trouvée, la chaîne est reconstruite à partir du mot de passe de départ stocké pour retrouver le mot de passe original.

* **Avantages :**
    * ⚡ **Vitesse de cracking élevée** une fois la table générée et chargée.
    * 💡 Moins de calculs en temps réel comparé à la force brute pour un même espace de recherche (si la table le couvre).
* **Inconvénients :**
    * ⏳ **Temps de précalcul important** pour générer la table.
    * 💾 **Espace de stockage conséquent** pour les tables volumineuses.
    * 🎯 **Spécificité :** Une table est généralement spécifique à un algorithme de hachage (MD5, SHA256, etc.), un jeu de caractères et une plage de longueurs de mot de passe.

#### **`rainbow_generator.cpp` : Forgez votre `rainbow.txt` ⚙️**

Avant de pouvoir lancer une attaque par table arc-en-ciel, vous devez générer une table. C'est le rôle de `rainbow_generator.cpp`. Cet outil autonome vous permet de créer votre fichier de table (nommé `rainbow.txt` par défaut, utilisé ensuite par `hashcracker.cpp`).

**Manuel d'utilisation de `rainbow_generator.cpp` (Pré-réglages pour `rainbow.txt`) :**

1.  **Lancez `rainbow_generator.cpp`** (il sera compilé sous le nom `rainbow_generator` ou similaire).
2.  **Paramètres à définir lors de la génération :** L'outil vous guidera pour entrer les informations suivantes. La **cohérence** de ces paramètres entre la génération et l'attaque est **ABSOLUMENT CRUCIALE**.

    | Paramètre          | Exemple de Valeur                 | Description                                                                                                | Module Concerné         | Emphase           |
    | :----------------- | :-------------------------------- | :--------------------------------------------------------------------------------------------------------- | :---------------------- | :---------------- |
    | 📤 **Output File** | `rainbow.txt`                     | Nom du fichier où la table sera sauvegardée (format: `mot_de_passe_initial:hash_final_de_chaine`).         | `rainbow_generator.cpp` | 📝                |
    | 🔡 **Charset** | `abcdefghijklmnopqrstuvwxyz0123456789` | Ensemble de caractères utilisés pour générer les mots de passe dans les chaînes. **DOIT correspondre au charset cible.** | `rainbow_generator.cpp` | 🔑 **Crucial !** |
    | 📏 **Min Pass Len**| `6`                               | Longueur minimale des mots de passe à générer au sein des chaînes.                                        | `rainbow_generator.cpp` | 🔢                |
    | 📐 **Max Pass Len**| `8`                               | Longueur maximale des mots de passe à générer au sein des chaînes.                                        | `rainbow_generator.cpp` | 🔢                |
    | 🔗 **Nb. Chains** | `1000000` (1 Million)             | Nombre de chaînes à générer. Impacte la couverture de la table et sa taille. Plus il y en a, mieux c'est (mais plus c'est gros/long). | `rainbow_generator.cpp` | 📈                |
    | ⛓️ **Chain Length** | `10000` (dix mille)               | Longueur de chaque chaîne (nombre de hachages/réductions). Compromis entre temps de recherche et taille.      | `rainbow_generator.cpp` | ⚖️                |
    | 🛡️ **Hash Type** | `MD5` / `SHA256` etc.             | Algorithme de hachage utilisé (ex: MD5, SHA1, SHA256). **DOIT correspondre au type de hachage cible.** | `rainbow_generator.cpp` | 🔑 **Crucial !** |

3.  **Considérations Importantes :**
    * **Cohérence Absolue 🔑 :** Le `Charset`, la plage `Min/Max Pass Len`, et le `Hash Type` utilisés pour la génération **doivent impérativement être les mêmes** que ceux que vous spécifierez lors de l'attaque avec `hashcracker.cpp` et doivent correspondre aux caractéristiques attendues du mot de passe cible. Toute incohérence rendra la table inutile pour ce hachage spécifique.
    * **Temps & Espace ⏳💾 :** La génération de tables arc-en-ciel, surtout pour des jeux de caractères larges, de longues chaînes, un grand nombre de chaînes, ou des mots de passe longs, peut prendre **énormément de temps** (heures, jours, voire semaines) et consommer une **quantité significative d'espace disque** (de quelques Mo à plusieurs Go ou To). Planifiez en conséquence !
    * Le générateur produira un fichier (par exemple, `rainbow.txt`) contenant des paires `mot_de_passe_de_départ:hachage_de_fin_de_chaîne`.

#### **Utiliser `rainbow.txt` dans `hashcracker.cpp` (Phase d'Attaque) ⚔️**

Une fois votre table `rainbow.txt` (ou un fichier avec un nom personnalisé) générée :

1.  Lancez `hashcracker.cpp`.
2.  Entrez le hachage à cracker.
3.  Choisissez l'option **3. Rainbow Table Attack**.
4.  Le programme vous demandera :
    * Le chemin vers votre fichier de table arc-en-ciel (par défaut, il cherchera `rainbow.txt` dans le répertoire de l'exécutable).
    * **Les paramètres de la table :**
        * Le **Charset pour la réduction** (celui utilisé lors de la génération de la table).
        * La **longueur des chaînes** (Chain Length) de la table.
        * La **longueur minimale et maximale des mots de passe assumée** (Min/Max Password Length) lors de la génération.
5.  `hashcracker.cpp` chargera alors la table en mémoire (cela peut prendre du temps pour les grosses tables) et commencera le processus de recherche. Il appliquera les fonctions de réduction et de hachage au hachage cible, cherchant des correspondances avec les hachages finaux de la table. Si une correspondance est trouvée, il reconstruira la chaîne pour retrouver le mot de passe.

---

### **Comment utiliser les nouveaux modules ?**

Le module `hashcracker.cpp` est désormais le moteur principal pour les attaques par **dictionnaire** et par **force brute** lorsque vous choisissez ces options dans HASHISH. Pour l'attaque par **table arc-en-ciel**, vous devrez d'abord générer une table avec `rainbow_generator.cpp` (ou obtenir une table compatible), puis utiliser l'option dédiée dans `hashcracker.cpp`.

Nous sommes convaincus que ces mises à jour rendront HASHISH encore plus puissant et utile pour vos besoins en sécurité éthique et en apprentissage. N'hésitez pas à les essayer et à nous faire part de vos retours !

 
---
## ✨ Fonctionnalités Principales

 **Hash Cracker :** 
 Déchiffrez différents types de hachages en utilisant des techniques courantes.

 
 **Web Scanner :** 
 Effectuez des analyses de base de sécurité sur des sites web et des adresses IP.

 
 **Reconnaissance :** 
 Collectez des informations sur des cibles en utilisant diverses techniques de reconnaissance réseau.

 
 **OSINT (Open-Source Intelligence) :** Rassemblez des informations accessibles publiquement sur des cibles spécifiques.

---




## 🚀 Utilisation

Pour lancer l'outil principal :

```bash
hashish

```
*Ou, si l'installation globale a échoué :*
```
python3 hashish.py

```

________

*🤝 Contributions*

**Les contributions sont les bienvenues ! Si vous souhaitez améliorer Hashish, corriger des bugs ou ajouter de nouvelles fonctionnalités, veuillez consulter notre Guide de Contribution.**

[![Sponsor me on GitHub](https://img.shields.io/badge/Sponsor-GitHub-brightgreen.svg)](https://github.com/sponsors/karim93160)
[![Buy me a coffee](https://img.shields.io/badge/Donate-Buy%20Me%20A%20Coffee-FFDD00.svg)](https://www.buymeacoffee.com/karim93160)
[![Support me on Ko-fi](https://img.shields.io/badge/Donate-Ko--fi-F16061.svg)](https://ko-fi.com/karim93160)
[![Support me on Patreon](https://img.shields.io/badge/Patreon-Support%20me-FF424D.svg)](https://www.patreon.com/karim93160)
[![Donate on Liberapay](https://img.shields.io/badge/Donate-Liberapay-F6C915.svg)](https://liberapay.com/karim93160/donate)


_________

## Licence 📜

hashish est distribué sous la licence [MIT License](https://github.com/Karim93160/hashish/blob/677eecb0be4fc81a0becc2c2154c65ffe73ddbb1/LICENSE)

_________

## Contact 📧

Pour toute question ou suggestion, n'hésitez pas à ouvrir une [issue sur GitHub](https://github.com/Karim93160/hashish/issues) ou à nous contacter par email :

[![Contact par Email](https://img.shields.io/badge/Contact-par%20Email-blue.svg)](mailto:karim9316077185@gmail.com)

_________

**⚠️ Avertissement**

*Hashish Ethical Toolkit est destiné à un usage éthique et légal uniquement. Les développeurs ne sont pas responsables de toute utilisation abusive ou illégale de cet outil.*
<p align="center">
Fait avec ❤️ par Karim
</p>
