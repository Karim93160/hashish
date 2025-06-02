#!/bin/bash

# Définitions de couleurs pour la sortie du script
CR_RED="\033[31;1m"
CR_GREEN="\033[32;1m"
CR_YELLOW="\033[33;1m"
CR_BLUE="\033[34;1m"
CR_CYAN="\033[36;1m"
CR_MAGENTA="\033[35;1m"
CR_DARK_GRAY="\033[90m"
RESET="\033[0m"

echo -e "\n${CR_CYAN}----------------------------------------------------${RESET}"
echo -e "${CR_CYAN}      Lancement de l'installation et compilation      ${RESET}"
echo -e "${CR_CYAN}----------------------------------------------------${RESET}"

# --- Vérification et installation des dépendances ---
echo -e "\n${CR_BLUE}Vérification et installation des dépendances...${RESET}"
pkg update -y
pkg upgrade -y
pkg install -y git make clang openssl libcrypt-dev libssl-dev coreutils

# Vérifie si g++ est installé, sinon installe-le
if ! command -v g++ &> /dev/null
then
    echo -e "${CR_YELLOW}g++ n'est pas trouvé. Installation de clang (qui inclut g++ sur Termux)...${RESET}"
    pkg install -y clang
    if ! command -v g++ &> /dev/null
    then
        echo -e "${CR_RED}Erreur : Impossible d'installer g++. La compilation pourrait échouer.${RESET}"
        exit 1
    fi
fi
echo -e "${CR_GREEN}Dépendances vérifiées/installées avec succès.${RESET}"

# --- Définition des chemins ---
# Assurez-vous que le script est exécuté depuis le répertoire principal de 'hashish'
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
HASHISH_HOME="$SCRIPT_DIR"
MODULES_DIR="$HASHISH_HOME/modules"
BIN_MODULES_DIR="/data/data/com.termux/files/usr/bin/modules"
WORDLISTS_SRC="$HASHISH_HOME/wordlists"
WORDLISTS_DEST="/data/data/com.termux/files/usr/bin/modules/wordlists"

# --- Création des répertoires nécessaires ---
echo -e "\n${CR_BLUE}Création des répertoires si nécessaire...${RESET}"
mkdir -p "$BIN_MODULES_DIR"
mkdir -p "$WORDLISTS_DEST"
echo -e "${CR_GREEN}Répertoires créés : $BIN_MODULES_DIR et $WORDLISTS_DEST${RESET}"

# --- Copie des wordlists par défaut ---
echo -e "\n${CR_BLUE}Copie des wordlists par défaut depuis '$WORDLISTS_SRC' vers '$WORDLISTS_DEST'...${RESET}"
if [ -d "$WORDLISTS_SRC" ]; then
    cp -r "$WORDLISTS_SRC"/* "$WORDLISTS_DEST/"
    echo -e "${CR_GREEN}Wordlists par défaut copiées avec succès vers $WORDLISTS_DEST.${RESET}"
else
    echo -e "${CR_YELLOW}Avertissement : Le répertoire des wordlists par défaut '$WORDLISTS_SRC' n'existe pas. Aucune wordlist copiée.${RESET}"
fi

# --- Vérification et compilation des modules C++ ---
echo -e "\n${CR_BLUE}Vérification et compilation des modules C++ 'hashcracker.cpp' et 'hash_recon.cpp' et leurs en-têtes...${RESET}"

HASHCRACKER_CPP="$MODULES_DIR/hashcracker.cpp"
HASH_RECON_CPP="$MODULES_DIR/hash_recon.cpp"
HASH_RECON_H="$MODULES_DIR/hash_recon.h"

if [ -f "$HASHCRACKER_CPP" ] && [ -f "$HASH_RECON_CPP" ] && [ -f "$HASH_RECON_H" ]; then
    echo -e "${CR_GREEN}Fichiers sources C++ 'hashcracker.cpp', 'hash_recon.cpp' et 'hash_recon.h' trouvés.${RESET}"

    # Copie des fichiers sources et de l'en-tête dans le répertoire de compilation
    echo -e "${CR_BLUE}Copie de hash_recon.h, hash_recon.cpp et hashcracker.cpp vers $BIN_MODULES_DIR pour la compilation...${RESET}"
    cp "$HASH_RECON_H" "$BIN_MODULES_DIR/"
    cp "$HASH_RECON_CPP" "$BIN_MODULES_DIR/"
    cp "$HASHCRACKER_CPP" "$BIN_MODULES_DIR/"
    echo -e "${CR_GREEN}Fichiers copiés.${RESET}"

    echo -e "${CR_BLUE}Lancement de la compilation de $HASHCRACKER_CPP et $HASH_RECON_CPP en 'hashcracker'...${RESET}"
    COMPILATION_COMMAND="g++ \"$BIN_MODULES_DIR/hashcracker.cpp\" \"$BIN_MODULES_DIR/hash_recon.cpp\" -o \"$BIN_MODULES_DIR/hashcracker\" -lcrypto -lssl -std=c++17 -fopenmp -pthread -I\"$BIN_MODULES_DIR\" -I\"/data/data/com.termux/files/usr/include\" -L\"/data/data/com.termux/files/usr/lib\""
    echo -e "${CR_DARK_GRAY}Commande de compilation : $COMPILATION_COMMAND${RESET}"

    eval $COMPILATION_COMMAND
    COMPILATION_STATUS=$?

    if [ $COMPILATION_STATUS -eq 0 ]; then
        echo -e "${CR_GREEN}Compilation de 'hashcracker' réussie !${RESET}"
        chmod +x "$BIN_MODULES_DIR/hashcracker" # Rendre l'exécutable
    else
        echo -e "\n${CR_RED}--------------------------------------------------${RESET}"
        echo -e "${CR_RED}ERREUR CRITIQUE : Échec de la compilation de hashcracker.cpp et hash_recon.cpp.${RESET}"
        echo -e "${CR_YELLOW}Veuillez examiner attentivement les messages d'erreur de g++ ci-dessus pour le diagnostic.${RESET}"
        echo -e "${CR_YELLOW}Les causes possibles incluent des bibliothèques OpenSSL manquantes, des en-têtes non trouvés, ou des erreurs dans le code source C++.${RESET}"
        echo -e "${CR_RED}Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${RESET}"
        echo -e "${CR_RED}--------------------------------------------------${RESET}"
        exit 1
    fi
else
    echo -e "${CR_RED}Erreur : Fichiers sources C++ ou en-tête manquants dans '$MODULES_DIR'. Veuillez vous assurer que 'hashcracker.cpp', 'hash_recon.cpp' et 'hash_recon.h' sont présents.${RESET}"
    exit 1
fi

echo -e "\n${CR_CYAN}----------------------------------------------------${RESET}"
echo -e "${CR_GREEN}Installation et compilation terminées !${RESET}"
echo -e "${CR_CYAN}----------------------------------------------------${RESET}"
echo -e "${CR_YELLOW}Vous pouvez maintenant utiliser 'hashcracker' depuis n'importe où dans Termux.${RESET}"
