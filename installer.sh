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

# Vérifie si g++ est installé, sinon installe-le (clang sur Termux fournit g++)
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
# Le script suppose qu'il est exécuté depuis la racine de ton projet 'hashish'
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
HASHISH_HOME="$SCRIPT_DIR"
MODULES_SRC_DIR="$HASHISH_HOME/modules" # Où tes .cpp et .h sont situés
BIN_MODULES_DEST_DIR="/data/data/com.termux/files/usr/bin/modules" # Où les exécutables et les copies des sources seront

WORDLISTS_SRC="$HASHISH_HOME/wordlists"
WORDLISTS_DEST="$BIN_MODULES_DEST_DIR/wordlists"

# --- Création des répertoires nécessaires ---
echo -e "\n${CR_BLUE}Création des répertoires si nécessaire...${RESET}"
mkdir -p "$BIN_MODULES_DEST_DIR"
mkdir -p "$WORDLISTS_DEST"
echo -e "${CR_GREEN}Répertoires créés : $BIN_MODULES_DEST_DIR et $WORDLISTS_DEST${RESET}"

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

HASHCRACKER_CPP_PATH="$MODULES_SRC_DIR/hashcracker.cpp"
HASH_RECON_CPP_PATH="$MODULES_SRC_DIR/hash_recon.cpp"
HASH_RECON_H_PATH="$MODULES_SRC_DIR/hash_recon.h"

if [ -f "$HASHCRACKER_CPP_PATH" ] && [ -f "$HASH_RECON_CPP_PATH" ] && [ -f "$HASH_RECON_H_PATH" ]; then
    echo -e "${CR_GREEN}Fichiers sources C++ 'hashcracker.cpp', 'hash_recon.cpp' et 'hash_recon.h' trouvés dans '$MODULES_SRC_DIR'.${RESET}"

    # Copie des fichiers sources et de l'en-tête dans le répertoire de destination pour la compilation
    echo -e "${CR_BLUE}Copie des fichiers sources et de l'en-tête vers '$BIN_MODULES_DEST_DIR' pour la compilation...${RESET}"
    cp "$HASH_RECON_H_PATH" "$BIN_MODULES_DEST_DIR/"
    cp "$HASH_RECON_CPP_PATH" "$BIN_MODULES_DEST_DIR/"
    cp "$HASHCRACKER_CPP_PATH" "$BIN_MODULES_DEST_DIR/"
    echo -e "${CR_GREEN}Fichiers copiés.${RESET}"

    echo -e "${CR_BLUE}Lancement de la compilation de $HASHCRACKER_CPP_PATH et $HASH_RECON_CPP_PATH en 'hashcracker'...${RESET}"
    # Commande de compilation corrigée pour inclure les dépendances et les en-têtes
    COMPILATION_COMMAND="g++ \"$BIN_MODULES_DEST_DIR/hashcracker.cpp\" \"$BIN_MODULES_DEST_DIR/hash_recon.cpp\" -o \"$BIN_MODULES_DEST_DIR/hashcracker\" -lcrypto -lssl -std=c++17 -fopenmp -pthread -I\"$BIN_MODULES_DEST_DIR\" -I\"/data/data/com.termux/files/usr/include\" -L\"/data/data/com.termux/files/usr/lib\""
    echo -e "${CR_DARK_GRAY}Commande de compilation : $COMPILATION_COMMAND${RESET}"

    eval $COMPILATION_COMMAND
    COMPILATION_STATUS=$?

    if [ $COMPILATION_STATUS -eq 0 ]; then
        echo -e "${CR_GREEN}Compilation de 'hashcracker' réussie !${RESET}"
        chmod +x "$BIN_MODULES_DEST_DIR/hashcracker" # Rendre l'exécutable
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
    echo -e "${CR_RED}Erreur : Fichiers sources C++ ou en-tête manquants dans '$MODULES_SRC_DIR'. Veuillez vous assurer que 'hashcracker.cpp', 'hash_recon.cpp' et 'hash_recon.h' sont présents.${RESET}"
    exit 1
fi

echo -e "\n${CR_CYAN}----------------------------------------------------${RESET}"
echo -e "${CR_GREEN}Installation et compilation terminées !${RESET}"
echo -e "${CR_CYAN}----------------------------------------------------${RESET}"
echo -e "${CR_YELLOW}Vous pouvez maintenant utiliser 'hashcracker' en l'appelant directement depuis n'importe où dans Termux, par exemple : '${CR_GREEN}hashcracker${CR_YELLOW}'${RESET}"
