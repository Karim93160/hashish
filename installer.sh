#!/bin/bash

# Définition des codes couleurs pour une sortie console plus lisible
GREEN='\033[0;32m'
NC='\033[0m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
INFO='\033[0;34m' # Bleu clair pour les informations

# --- Fonctions Utilitaires ---

# Fonction pour effacer l'écran, compatible avec 'clear' ou un équivalent ANSI
clear_screen() {
    if command -v clear &>/dev/null; then
        clear
    else
        printf '\033c'
    fi
}

# Efface l'écran au début de l'exécution
clear_screen

# --- Bannière de Bienvenue ---
echo -e "${CYAN}=======================================${NC}"
echo -e "${CYAN}  Installation de HASHISH Ethical Toolkit${NC}"
echo -e "${CYAN}=======================================${NC}\n"

# --- Mise à Jour et Mise à Niveau de Termux ---
echo -e "${BLUE}Mise à jour et mise à niveau des paquets Termux...${NC}"
if pkg update -y && pkg upgrade -y; then
    echo -e "${GREEN}Termux paquets mis à jour et mis à niveau avec succès.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Impossible de mettre à jour/mettre à niveau les paquets Termux. Cela pourrait causer des problèmes de dépendances.${NC}"
    echo -e "${YELLOW}Veuillez vérifier votre connexion Internet et les dépôts, puis réessayez manuellement : 'pkg update && pkg upgrade'.${NC}\n"
fi

# --- Détection du Chemin du Dépôt ---
REPO_NAME="hashish"
DEFAULT_HOME_PATH="/data/data/com.termux/files/home"
DEFAULT_REPO_PATH="$DEFAULT_HOME_PATH/$REPO_NAME"

CURRENT_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_PATH=""

# Recherche du dossier 'hashish' en remontant l'arborescence
temp_dir="$CURRENT_SCRIPT_DIR"
while [[ "$temp_dir" != "/" && "$temp_dir" != "" ]]; do
    if [[ "$(basename "$temp_dir")" == "$REPO_NAME" ]]; then
        REPO_PATH="$temp_dir"
        break
    fi
    temp_dir=$(dirname "$temp_dir")
done

# Détermine le chemin final du dépôt
if [ -n "$REPO_PATH" ]; then
    echo -e "${INFO}Dépôt '${REPO_NAME}' détecté à partir de l'emplacement actuel : ${REPO_PATH}${NC}"
elif [ -d "$DEFAULT_REPO_PATH" ]; then
    REPO_PATH="$DEFAULT_REPO_PATH"
    echo -e "${INFO}Dépôt '${REPO_NAME}' détecté à l'emplacement par défaut : ${REPO_PATH}${NC}"
else
    echo -e "${RED}Erreur : Le répertoire '$REPO_NAME' est introuvable ni à l'emplacement actuel ni à l'emplacement par défaut (${DEFAULT_REPO_PATH}).${NC}"
    read -p "Voulez-vous entrer le chemin **complet** du dossier '$REPO_NAME' manuellement ? (o/n) : " confirm
    if [[ "$confirm" == "o" || "$confirm" == "O" ]]; then
        read -p "Veuillez entrer le chemin **complet** du dossier '$REPO_NAME' (ex: /sdcard/Hashish) : " CUSTOM_REPO_PATH
        if [ -n "$CUSTOM_REPO_PATH" ]; then
            REPO_PATH="$CUSTOM_REPO_PATH"
            echo -e "${INFO}Nouveau chemin du dépôt: ${REPO_PATH}${NC}"
        else
            echo -e "${RED}Installation annulée. Le chemin du dépôt est nécessaire.${NC}"
            exit 1
        fi
    else
        echo -e "${RED}Installation annulée. Impossible de trouver le dossier du dépôt.${NC}"
        exit 1
    fi
fi

# Définition des répertoires d'installation cibles
INSTALL_DIR="/data/data/com.termux/files/usr/bin"
MODULES_TARGET_DIR="$INSTALL_DIR/modules"
WORDLISTS_TARGET_DIR="$MODULES_TARGET_DIR/wordlists"

# --- Vérification et Installation des Prérequis Système ---
echo -e "${BLUE}Vérification et installation des prérequis système (build-essential, openssl, ncurses-utils)...${NC}"

# Fonction d'aide pour installer un paquet Termux
install_package() {
    local package_name=$1
    echo -e "${INFO}Installation du paquet Termux : ${package_name}...${NC}"
    if pkg install "$package_name" -y; then
        echo -e "${GREEN}Paquet '${package_name}' installé avec succès.${NC}"
        return 0
    else
        echo -e "${RED}Échec de l'installation du paquet '${package_name}'. Veuillez vérifier votre connexion ou les dépôts Termux.${NC}"
        return 1
    fi
}

# Liste des paquets essentiels pour Termux
REQUIRED_PKGS=("clang" "build-essential" "openssl" "git" "python" "ncurses-utils" "rsync" "curl" "nmap" "whois" "dnsutils")

# Parcours et installe les paquets manquants
for pkg_name in "${REQUIRED_PKGS[@]}"; do
    if ! dpkg -s "$pkg_name" &>/dev/null; then
        echo -e "${YELLOW}Paquet '${pkg_name}' non trouvé. Installation de '${pkg_name}'...${NC}"
        install_package "$pkg_name" || { 
            echo -e "${RED}Installation annulée. Le paquet '${pkg_name}' est nécessaire.${NC}"
            exit 1
        }
    else
        echo -e "${GREEN}Paquet '${pkg_name}' est déjà installé.${NC}"
    fi
done

# Vérification spécifique du compilateur g++ après l'installation de build-essential
if ! command -v g++ &> /dev/null; then
  echo -e "${RED}Erreur : g++ n'est toujours pas disponible après l'installation de 'build-essential'. Impossible de compiler les modules C++.${NC}"
  echo -e "${YELLOW}Veuillez résoudre manuellement le problème d'installation de 'build-essential' dans Termux et relancer le script.${NC}"
  exit 1
else
  echo -e "${GREEN}Compilateur g++ est maintenant disponible.${NC}"
fi

# Attribution des permissions d'exécution à la commande 'clear'
echo -e "${BLUE}Attribution des permissions d'exécution à la commande 'clear'...${NC}"
if [ -f "/data/data/com.termux/files/usr/bin/clear" ]; then
    chmod +x /data/data/com.termux/files/usr/bin/clear
    echo -e "${GREEN}Permissions d'exécution accordées à '/data/data/com.termux/files/usr/bin/clear'.${NC}"
else
    echo -e "${YELLOW}Avertissement : La commande 'clear' n'a pas été trouvée à '/data/data/com.termux/files/usr/bin/clear'. Les permissions n'ont pas été modifiées.${NC}"
fi
echo ""

echo -e "${GREEN}Prérequis système vérifiés et installés si nécessaire.${NC}\n"

# --- Validation du Contenu du Dépôt ---
echo -e "${BLUE}Validation du contenu du dépôt '${REPO_PATH}'...${NC}"
if [ ! -d "$REPO_PATH" ]; then
  echo -e "${RED}Erreur : Le répertoire du dépôt '$REPO_NAME' n'existe pas à '${REPO_PATH}'.${NC}"
  exit 1
fi
if [ ! -f "$REPO_PATH/hashish.py" ]; then
  echo -e "${RED}Erreur : 'hashish.py' introuvable dans '$REPO_PATH'. Assurez-vous que le fichier est présent et nommé correctement.${NC}"
  exit 1
fi
if [ ! -d "$REPO_PATH/modules" ]; then
  echo -e "${RED}Erreur : Le dossier 'modules' est introuvable dans '$REPO_PATH'. Assurez-vous que les modules sont bien dans un sous-dossier 'modules'.${NC}"
  exit 1
fi
if [ ! -f "$REPO_PATH/banner-hashish.txt" ]; then
  echo -e "${RED}Erreur : 'banner-hashish.txt' introuvable dans '$REPO_PATH'.${NC}"
  exit 1
fi
if [ ! -f "$REPO_PATH/requirements.txt" ]; then
  echo -e "${YELLOW}Avertissement : 'requirements.txt' introuvable dans '$REPO_PATH'. Aucune dépendance Python ne sera installée via ce fichier.${NC}"
fi
if [ ! -d "$REPO_PATH/wordlists" ]; then
  echo -e "${YELLOW}Avertissement : Le dossier 'wordlists' est introuvable au niveau racine du dépôt '$REPO_PATH'. Les wordlists par défaut ne seront pas installées.${NC}"
fi

echo -e "${GREEN}Dépôt '$REPO_PATH' validé.${NC}\n"

# --- Création des Dossiers Cibles ---
echo -e "${BLUE}Création des dossiers cibles dans Termux...${NC}"
mkdir -p "$INSTALL_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $INSTALL_DIR. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$MODULES_TARGET_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $MODULES_TARGET_DIR. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$WORDLISTS_TARGET_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $WORDLISTS_TARGET_DIR. Vérifiez les permissions.${NC}"; exit 1; }
echo -e "${GREEN}Dossiers cibles créés.${NC}\n"

# --- Copie des Fichiers Principaux ---
echo -e "${BLUE}Copie de 'hashish.py' et 'banner-hashish.txt' vers ${INSTALL_DIR}...${NC}"
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/hashish.py" || { echo -e "${RED}Erreur: Impossible de copier hashish.py. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
cp "$REPO_PATH/banner-hashish.txt" "$INSTALL_DIR/banner-hashish.txt" || { echo -e "${RED}Erreur: Impossible de copier banner-hashish.txt. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
chmod +x "$INSTALL_DIR/hashish.py" # Permissions d'exécution pour le script principal
chmod +r "$INSTALL_DIR/banner-hashish.txt"
echo -e "${GREEN}Fichiers principaux copiés avec succès.${NC}\n"

# --- Copie des Modules Python ---
echo -e "${BLUE}Copie des modules Python depuis '$REPO_PATH/modules/' vers '$MODULES_TARGET_DIR/'...${NC}"
if command -v rsync &> /dev/null; then
    echo -e "${INFO}Utilisation de rsync pour copier les modules Python...${NC}"
    # rsync pour exclure les wordlists et ne copier que les .py et les sous-dossiers vides
    rsync -av --exclude 'wordlists/' --include='*.py' --include='*/' --exclude='*' "$REPO_PATH/modules/" "$MODULES_TARGET_DIR/" || { echo -e "${YELLOW}Avertissement : Erreur lors de la copie des modules Python avec rsync. Vérifiez le dossier '$REPO_PATH/modules/'.${NC}"; }
else
    echo -e "${YELLOW}Avertissement : 'rsync' non trouvé. Copie des fichiers Python individuellement (fallback)...${NC}"
    # Fallback si rsync n'est pas disponible
    find "$REPO_PATH/modules/" -maxdepth 1 -name "*.py" -exec cp {} "$MODULES_TARGET_DIR/" \; 2>/dev/null || true

    for dir in "$REPO_PATH/modules"/*/; do
        dir_name=$(basename "$dir")
        if [ "$dir_name" != "wordlists" ]; then
            cp -r "$dir" "$MODULES_TARGET_DIR/" 2>/dev/null || true
        fi
    done
    echo -e "${YELLOW}Note : Pour une meilleure gestion des fichiers, il est recommandé d'installer 'rsync' (pkg install rsync).${NC}"
fi
# Permissions d'exécution pour tous les scripts Python copiés dans le dossier des modules
find "$MODULES_TARGET_DIR" -maxdepth 1 -name "*.py" -exec chmod +x {} \; 2>/dev/null || true
echo -e "${GREEN}Modules Python copiés avec succès vers ${MODULES_TARGET_DIR}.${NC}\n"

# --- Copie des Wordlists ---
echo -e "${BLUE}Copie des wordlists par défaut depuis '$REPO_PATH/wordlists/' vers '$WORDLISTS_TARGET_DIR/'...${NC}"
if [ -d "$REPO_PATH/wordlists" ]; then
    cp -r "$REPO_PATH/wordlists/"* "$WORDLISTS_TARGET_DIR/" 2>/dev/null || { echo -e "${YELLOW}Avertissement : Aucun fichier de wordlist par défaut trouvé à copier ou erreur lors de la copie.${NC}"; }
    echo -e "${GREEN}Wordlists par défaut copiées avec succès vers ${WORDLISTS_TARGET_DIR}.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Le dossier des wordlists par défaut '$REPO_PATH/wordlists' est introuvable. Les wordlists par défaut ne seront pas installées.${NC}\n"
fi

# --- Pré-traitement : Correction de la fonction 'reduce_hash' dans les fichiers C++ ---
echo -e "${BLUE}Pré-traitement : Correction de la fonction 'reduce_hash' dans les fichiers C++...${NC}"

CPP_FILES=("$REPO_PATH/modules/hashcracker.cpp") # Liste des fichiers C++ à corriger
for file in "${CPP_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo -e "${INFO}Correction de $file...${NC}"
        # Vérifie si la ligne à modifier existe avant de la remplacer
        if grep -q "std::seed_seq seed_sequence(hash.begin(), hash.end());" "$file"; then
            # Utilise sed pour remplacer la ligne, en ajoutant les lignes nécessaires
            sed -i '/std::string reduced_string = "";/{
                N;N;N;N;N;N;N;N;N;
                s/std::seed_seq seed_sequence(hash.begin(), hash.end());/\
std::vector<unsigned int> seed_data;\
for (char c : hash) { seed_data.push_back(static_cast<unsigned int>(c)); }\
seed_data.push_back(static_cast<unsigned int>(r_index));\
\
std::seed_seq seed_sequence(seed_data.begin(), seed_data.end());/
            }' "$file"
            echo -e "${GREEN}Correction appliquée à $file.${NC}"
        else
            echo -e "${INFO}La correction de $file ne semble pas nécessaire (déjà appliquée ou motif non trouvé).${NC}"
        fi
    fi
done
echo -e "${GREEN}Correction des fichiers C++ terminée.${NC}\n"

# AJOUT : S'assurer que le dossier des modules a les permissions d'écriture pour la compilation
echo -e "${BLUE}Vérification et attribution des permissions d'écriture pour le dossier des modules C++ source...${NC}"
if [ -d "$REPO_PATH/modules" ]; then
    # chmod -R pour récursif, +w pour l'écriture pour le propriétaire
    # C'est pour s'assurer que le dossier où le temporaire est créé est bien inscriptible.
    chmod +w "$REPO_PATH/modules" || { echo -e "${RED}Erreur : Impossible de donner les permissions d'écriture à $REPO_PATH/modules. Vérifiez si vous êtes propriétaire ou exécutez avec des privilèges suffisants.${NC}"; exit 1; }
    echo -e "${GREEN}Permissions d'écriture accordées à $REPO_PATH/modules.${NC
