#!/bin/bash

# Couleurs pour les messages
GREEN='\033[0;32m'
NC='\033[0m' # No Color
YELLOW='\033[0;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
INFO='\033[0;34m' # Utilisé pour info, similaire à BLUE
BOLD='\033[1m' # Ajout de BOLD pour une meilleure visibilité des titres

# Fonction pour effacer l'écran de manière portable
clear_screen() {
    # Tente d'utiliser 'clear' si disponible et avec les bonnes permissions
    if command -v clear &>/dev/null; then
        clear
    else
        # Fallback universel pour effacer l'écran
        printf '\033c'
    fi
}

clear_screen # Efface l'écran au début de l'exécution

echo -e "${CYAN}=======================================${NC}"
echo -e "${CYAN}${BOLD}  Installation de HASHISH Ethical Toolkit${NC}" # Ajout de BOLD
echo -e "${CYAN}=======================================${NC}\n"

# --- Mise à jour et mise à niveau de Termux (bonne pratique) ---
echo -e "${BLUE}--- Mise à jour et mise à niveau des paquets Termux ---${NC}" # Ajout de titre de section
echo -e "${INFO}Mise à jour et mise à niveau des paquets système. Cela peut prendre un certain temps...${NC}"
if pkg update -y && pkg upgrade -y; then
    echo -e "${GREEN}Termux paquets mis à jour et mis à niveau avec succès.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Impossible de mettre à jour/mettre à niveau les paquets Termux. Cela pourrait causer des problèmes de dépendances.${NC}"
    echo -e "${YELLOW}Veuillez vérifier votre connexion Internet et les dépôts, puis réessayez manuellement : 'pkg update && pkg upgrade'.${NC}\n"
fi


# Nom du dépôt et chemins par défaut
REPO_NAME="hashish"
# Chemin par défaut où l'on s'attend à trouver le dépôt après un git clone dans Termux
DEFAULT_HOME_PATH="/data/data/com.termux/files/home"
DEFAULT_REPO_PATH="$DEFAULT_HOME_PATH/$REPO_NAME"

# Déterminer le chemin réel du dépôt.
echo -e "${BLUE}--- Détection du chemin du dépôt Hashish ---${NC}" # Ajout de titre de section
CURRENT_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_PATH=""

# Chercher le répertoire du dépôt 'hashish' en remontant l'arborescence
temp_dir="$CURRENT_SCRIPT_DIR"
while [[ "$temp_dir" != "/" && "$temp_dir" != "" ]]; do
    if [[ "$(basename "$temp_dir")" == "$REPO_NAME" ]]; then
        REPO_PATH="$temp_dir"
        break
    fi
    temp_dir=$(dirname "$temp_dir")
done

if [ -n "$REPO_PATH" ]; then
    echo -e "${INFO}Dépôt '${REPO_NAME}' détecté à partir de l'emplacement actuel : ${REPO_PATH}${NC}"
elif [ -d "$DEFAULT_REPO_PATH" ]; then
    REPO_PATH="$DEFAULT_REPO_PATH"
    echo -e "${INFO}Dépôt '${REPO_NAME}' détecté à l'emplacement par défaut : ${REPO_PATH}${NC}"
else
    echo -e "${RED}Erreur : Le répertoire '$REPO_NAME' est introuvable ni à l'emplacement actuel ni à l'emplacement par défaut (${DEFAULT_REPO_PATH}).${NC}"
    read -p "Voulez-vous entrer le chemin ${BOLD}complet${NC}${RED} du dossier '$REPO_NAME' manuellement ? (o/n) : " confirm
    if [[ "$confirm" == "o" || "$confirm" == "O" ]]; then
        read -p "Veuillez entrer le chemin ${BOLD}complet${NC}${INFO} du dossier '$REPO_NAME' (ex: /sdcard/Hashish) : ${NC}" CUSTOM_REPO_PATH
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

# Dossiers d'installation cibles dans le système Termux
INSTALL_DIR="/data/data/com.termux/files/usr/bin"
MODULES_TARGET_DIR="$INSTALL_DIR/modules"
WORDLISTS_TARGET_DIR="$MODULES_TARGET_DIR/wordlists"

# --- Vérification et Installation des Prérequis Système ---
echo -e "${BLUE}--- Vérification et installation des prérequis système ---${NC}" # Ajout de titre de section

# Fonction pour installer un paquet
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

# Installation de build-essential (pour g++)
if ! command -v g++ &> /dev/null; then
    echo -e "${YELLOW}Compilateur g++ non trouvé. Installation de 'build-essential'...${NC}"
    install_package "build-essential" || { echo -e "${RED}${BOLD}Installation annulée.${NC}${RED} 'build-essential' est nécessaire pour compiler les modules C++.${NC}"; exit 1; }
fi

# Installation d'openssl (pour les bibliothèques de développement SSL/Crypto)
# Note: 'pkg list-installed' vérifie si le paquet est déjà là.
if ! pkg list-installed | grep -q "^openssl/"; then
    echo -e "${YELLOW}OpenSSL non trouvé. Nécessaire pour la compilation C++. Installation de 'openssl'...${NC}"
    install_package "openssl" || { echo -e "${RED}${BOLD}Installation annulée.${NC}${RED} 'openssl' est nécessaire pour compiler les modules C++.${NC}"; exit 1; }
else
    echo -e "${GREEN}OpenSSL est déjà installé.${NC}"
fi

# Installation de ncurses-utils pour la commande 'clear'
if ! command -v clear &> /dev/null; then
    echo -e "${YELLOW}Commande 'clear' non trouvée ou non exécutable. Installation de 'ncurses-utils'...${NC}"
    install_package "ncurses-utils" || { echo -e "${YELLOW}Avertissement : Impossible d'installer 'ncurses-utils'. La commande 'clear' pourrait ne pas fonctionner correctement, mais l'outil utilisera un fallback.${NC}"; }
else
    echo -e "${GREEN}Commande 'clear' est déjà disponible.${NC}"
fi

# Ajout de la permission d'exécution à 'clear'
echo -e "${BLUE}Attribution des permissions d'exécution à la commande 'clear'...${NC}"
if [ -f "/data/data/com.termux/files/usr/bin/clear" ]; then
    chmod +x /data/data/com.termux/files/usr/bin/clear
    echo -e "${GREEN}Permissions d'exécution accordées à '/data/data/com.termux/files/usr/bin/clear'.${NC}"
else
    echo -e "${YELLOW}Avertissement : La commande 'clear' n'a pas été trouvée à '/data/data/com.termux/files/usr/bin/clear'. Les permissions n'ont pas été modifiées.${NC}"
fi
echo "" # Nouvelle ligne pour une meilleure mise en forme


# Vérification finale de g++ après tentatives d'installation
if ! command -v g++ &> /dev/null; then
  echo -e "${RED}${BOLD}Erreur : g++ n'est toujours pas disponible après l'installation des prérequis.${NC}${RED} Impossible de compiler les modules C++.${NC}"
  echo -e "${YELLOW}Veuillez résoudre manuellement le problème d'installation de 'build-essential' dans Termux et relancer le script.${NC}"
  exit 1
fi
echo -e "${GREEN}Prérequis système vérifiés et installés si nécessaire.${NC}\n"


# --- Validation du répertoire du dépôt ---
echo -e "${BLUE}--- Validation du contenu du dépôt '${REPO_PATH}' ---${NC}" # Ajout de titre de section
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
# Nouvelle vérification pour le dossier wordlists au niveau racine du dépôt
if [ ! -d "$REPO_PATH/wordlists" ]; then
  echo -e "${YELLOW}Avertissement : Le dossier 'wordlists' est introuvable au niveau racine du dépôt '$REPO_PATH'. Les wordlists par défaut ne seront pas installées.${NC}"
fi
# Vérification pour les nouveaux fichiers C++
if [ ! -f "$REPO_PATH/modules/hash_recon.h" ]; then
  echo -e "${RED}Erreur : 'hash_recon.h' introuvable dans '$REPO_PATH/modules/'. Ce fichier est nécessaire pour la compilation des modules C++.${NC}"
  exit 1
fi
if [ ! -f "$REPO_PATH/modules/hash_recon.cpp" ]; then
  echo -e "${RED}Erreur : 'hash_recon.cpp' introuvable dans '$REPO_PATH/modules/'. Ce fichier est nécessaire pour la compilation des modules C++.${NC}"
  exit 1
fi


echo -e "${GREEN}Dépôt '$REPO_PATH' validé.${NC}\n"

# --- Création des dossiers cibles ---
echo -e "${BLUE}--- Création des dossiers cibles dans Termux ---${NC}" # Ajout de titre de section
mkdir -p "$INSTALL_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $INSTALL_DIR. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$MODULES_TARGET_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $MODULES_TARGET_DIR. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$WORDLISTS_TARGET_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $WORDLISTS_TARGET_DIR. Vérifiez les permissions.${NC}"; exit 1; } # Création du dossier wordlists
echo -e "${GREEN}Dossiers cibles créés.${NC}\n"

# --- Copie des fichiers principaux ---
echo -e "${BLUE}--- Copie des fichiers principaux (hashish.py, banner-hashish.txt) ---${NC}" # Ajout de titre de section
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/hashish.py" || { echo -e "${RED}Erreur: Impossible de copier hashish.py. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
cp "$REPO_PATH/banner-hashish.txt" "$INSTALL_DIR/banner-hashish.txt" || { echo -e "${RED}Erreur: Impossible de copier banner-hashish.txt. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }

echo -e "${GREEN}Fichiers principaux copiés avec succès.${NC}\n"

# --- Copie des modules Python dans /usr/bin/modules/ ---
echo -e "${BLUE}--- Copie des modules Python ---${NC}" # Ajout de titre de section
echo -e "${INFO}Copie des modules Python depuis '$REPO_PATH/modules/' vers '$MODULES_TARGET_DIR/'...${NC}"

if command -v rsync &> /dev/null; then
    echo -e "${INFO}Utilisation de rsync pour copier les modules Python...${NC}"
    rsync -av --exclude 'wordlists/' --include='*.py' --include='*/' --exclude='*' "$REPO_PATH/modules/" "$MODULES_TARGET_DIR/" || { echo -e "${YELLOW}Avertissement : Erreur lors de la copie des modules Python avec rsync. Vérifiez le dossier '$REPO_PATH/modules/'.${NC}"; }
else
    echo -e "${YELLOW}Avertissement : 'rsync' non trouvé. Copie des fichiers Python individuellement (fallback)...${NC}"
    find "$REPO_PATH/modules/" -maxdepth 1 -name "*.py" -exec cp {} "$MODULES_TARGET_DIR/" \; 2>/dev/null || true

    for dir in "$REPO_PATH/modules"/*/; do
        dir_name=$(basename "$dir")
        if [ "$dir_name" != "wordlists" ]; then
            cp -r "$dir" "$MODULES_TARGET_DIR/" 2>/dev/null || true
        fi
    done
    echo -e "${YELLOW}Note : Pour une meilleure gestion des fichiers, il est recommandé d'installer 'rsync' (pkg install rsync).${NC}"
fi
echo -e "${GREEN}Modules Python copiés avec succès vers ${MODULES_TARGET_DIR}.${NC}\n"

# --- Copie des fichiers d'en-tête C++ (hash_recon.h) ---
echo -e "${BLUE}--- Copie du fichier d'en-tête C++ (hash_recon.h) ---${NC}" # Ajout de titre de section
if [ -f "$REPO_PATH/modules/hash_recon.h" ]; then
    cp "$REPO_PATH/modules/hash_recon.h" "$MODULES_TARGET_DIR/" || { echo -e "${RED}Erreur: Impossible de copier hash_recon.h. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
    chmod +r "$MODULES_TARGET_DIR/hash_recon.h"
    echo -e "${GREEN}Fichier d'en-tête C++ 'hash_recon.h' copié avec succès et permissions définies.${NC}\n"
else
    echo -e "${RED}Erreur: Fichier 'hash_recon.h' introuvable dans '$REPO_PATH/modules/'. Impossible de compiler les modules C++.${NC}"
    exit 1
fi


# --- Copie des wordlists par défaut ---
echo -e "${BLUE}--- Copie des wordlists par défaut ---${NC}" # Ajout de titre de section
echo -e "${INFO}Copie des wordlists par défaut depuis '$REPO_PATH/wordlists/' vers '$WORDLISTS_TARGET_DIR/'...${NC}"
if [ -d "$REPO_PATH/wordlists" ]; then
    cp -r "$REPO_PATH/wordlists/"* "$WORDLISTS_TARGET_DIR/" 2>/dev/null || { echo -e "${YELLOW}Avertissement : Aucun fichier de wordlist par défaut trouvé à copier ou erreur lors de la copie.${NC}"; }
    chmod -R +r "$WORDLISTS_TARGET_DIR"
    echo -e "${GREEN}Wordlists par défaut copiées avec succès vers ${WORDLISTS_TARGET_DIR} et permissions définies.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Le dossier des wordlists par défaut '$REPO_PATH/wordlists' est introuvable. Les wordlists par défaut ne seront pas installées.${NC}\n"
fi


# --- Pré-traitement : Correction des fichiers C++ avant compilation ---
echo -e "${BLUE}--- Pré-traitement des fichiers C++ ---${NC}" # Ajout de titre de section
echo -e "${INFO}Correction de la fonction 'reduce_hash' dans les fichiers C++...${NC}"

CPP_FILES=("$REPO_PATH/modules/hashcracker.cpp")
for file in "${CPP_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo -e "${INFO}Correction de $file...${NC}"
        if grep -q "std::seed_seq seed_sequence(hash.begin(), hash.end());" "$file"; then
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
    else
        echo -e "${YELLOW}Avertissement : Fichier C++ '$file' non trouvé pour la correction. ${NC}"
    fi
done
echo -e "${GREEN}Correction des fichiers C++ terminée.${NC}\n"


# --- Compilation et déplacement des modules C++ (hashcracker et hash_recon) ---
echo -e "${BLUE}--- Compilation des modules C++ (hashcracker et hash_recon) ---${NC}" # Ajout de titre de section
HASHCRACKER_CPP_SOURCE="$REPO_PATH/modules/hashcracker.cpp"
HASH_RECON_CPP_SOURCE="$REPO_PATH/modules/hash_recon.cpp"
HASHCRACKER_TEMP_EXECUTABLE="$REPO_PATH/modules/hashcracker_temp"
HASHCRACKER_FINAL_EXECUTABLE="$MODULES_TARGET_DIR/hashcracker"

echo -e "${INFO}Vérification et compilation des modules C++ '${HASHCRACKER_CPP_SOURCE}' et '${HASH_RECON_CPP_SOURCE}'...${NC}"

if [ -f "$HASHCRACKER_CPP_SOURCE" ] && [ -f "$HASH_RECON_CPP_SOURCE" ]; then
  echo -e "${INFO}Fichiers sources C++ trouvés : ${HASHCRACKER_CPP_SOURCE} et ${HASH_RECON_CPP_SOURCE}.${NC}"

  OPENSSL_INCLUDE_PATH="/data/data/com.termux/files/usr/include"
  OPENSSL_LIB_PATH="/data/data/com.termux/files/usr/lib"

  echo -e "${CYAN}Lancement de la compilation de ${HASHCRACKER_CPP_SOURCE} et ${HASH_RECON_CPP_SOURCE} vers ${HASHCRACKER_TEMP_EXECUTABLE}...${NC}"
  echo -e "${CYAN}Commande de compilation : g++ \"$HASHCRACKER_CPP_SOURCE\" \"$HASH_RECON_CPP_SOURCE\" -o \"$HASHCRACKER_TEMP_EXECUTABLE\" -std=c++17 -fopenmp -pthread -I\"$OPENSSL_INCLUDE_PATH\" -I\"$MODULES_TARGET_DIR\" -L\"$OPENSSL_LIB_PATH\" -lssl -lcrypto${NC}"

  if g++ "$HASHCRACKER_CPP_SOURCE" "$HASH_RECON_CPP_SOURCE" -o "$HASHCRACKER_TEMP_EXECUTABLE" \
     -std=c++17 -fopenmp -pthread \
     -I"$OPENSSL_INCLUDE_PATH" \
     -I"$MODULES_TARGET_DIR" \
     -L"$OPENSSL_LIB_PATH" \
     -lssl -lcrypto; then

    echo -e "${GREEN}Modules C++ hashcracker et hash_recon compilés avec succès vers : ${HASHCRACKER_TEMP_EXECUTABLE}${NC}"

    if [ ! -d "$MODULES_TARGET_DIR" ]; then
        echo -e "${RED}Erreur: Le dossier cible des modules '${MODULES_TARGET_DIR}' n'existe pas. Impossible de déplacer l'exécutable C++.${NC}"
        echo -e "${YELLOW}Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
        exit 1
    fi

    echo -e "${INFO}Déplacement de l'exécutable compilé vers : ${HASHCRACKER_FINAL_EXECUTABLE}${NC}"
    if mv "$HASHCRACKER_TEMP_EXECUTABLE" "$HASHCRACKER_FINAL_EXECUTABLE"; then
        echo -e "${GREEN}Exécutable C++ déplacé avec succès.${NC}"
        chmod +x "$HASHCRACKER_FINAL_EXECUTABLE"
        echo -e "${GREEN}Permissions d'exécution accordées à ${HASHCRACKER_FINAL_EXECUTABLE}.${NC}"
    else
        echo -e "${RED}Erreur: Impossible de déplacer l'exécutable C++ vers ${HASHCRACKER_FINAL_EXECUTABLE}. Vérifiez les permissions du dossier cible ou l'espace disque.${NC}"
        echo -e "${YELLOW}Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
        exit 1
    fi

  else
    echo -e "${RED}------------------------------------------------------------------${NC}"
    echo -e "${RED}${BOLD}ERREUR CRITIQUE : Échec de la compilation des modules C++.${NC}"
    echo -e "${YELLOW}Veuillez examiner attentivement les messages d'erreur de g++ ci-dessus pour le diagnostic.${NC}"
    echo -e "${YELLOW}Les causes possibles incluent des bibliothèques OpenSSL manquantes, des en-têtes non trouvés, ou des erreurs dans le code source C++.${NC}"
    echo -e "${YELLOW}Les modules C++ ne seront PAS disponibles ou ne fonctionneront pas correctement.${NC}"
    echo -e "${RED}------------------------------------------------------------------${NC}"
    exit 1
  fi
else
  echo -e "${YELLOW}Fichiers source C++ (hashcracker.cpp ou hash_recon.cpp) non trouvés dans le dossier des modules. La compilation C++ est ignorée.${NC}"
  echo -e "${YELLOW}Les modules C++ ne seront PAS disponibles.${NC}"
fi
echo "" # Nouvelle ligne pour une meilleure mise en forme

# --- Nettoyage de l'ancien rainbow_generator (s'il existe) ---
echo -e "${BLUE}--- Nettoyage des anciens exécutables (rainbow_generator) ---${NC}" # Ajout de titre de section
RAINBOW_GENERATOR_OLD_EXECUTABLE="$MODULES_TARGET_DIR/rainbow_generator"
if [ -f "$RAINBOW_GENERATOR_OLD_EXECUTABLE" ]; then
    echo -e "${INFO}Nettoyage de l'ancien exécutable rainbow_generator...${NC}"
    rm "$RAINBOW_GENERATOR_OLD_EXECUTABLE" || { echo -e "${YELLOW}Avertissement : Impossible de supprimer l'ancien rainbow_generator. Veuillez le supprimer manuellement si nécessaire.${NC}"; }
    echo -e "${GREEN}Ancien rainbow_generator supprimé.${NC}\n"
fi


# --- Création du fichier rainbow.txt vide si non existant ---
echo -e "${BLUE}--- Vérification et création du fichier rainbow.txt ---${NC}" # Ajout de titre de section
RAINBOW_TXT_PATH="$MODULES_TARGET_DIR/rainbow.txt"
if [ ! -f "$RAINBOW_TXT_PATH" ]; then
    echo -e "${INFO}Le fichier de table arc-en-ciel par défaut '${RAINBOW_TXT_PATH}' n'existe pas. Création d'un fichier vide.${NC}"
    touch "$RAINBOW_TXT_PATH" || { echo -e "${RED}Erreur: Impossible de créer le fichier rainbow.txt à ${RAINBOW_TXT_PATH}. Vérifiez les permissions.${NC}"; }
    echo -e "${GREEN}Fichier rainbow.txt créé (ou déjà existant) à ${RAINBOW_TXT_PATH}.${NC}\n"
else
    echo -e "${GREEN}Fichier rainbow.txt déjà existant à ${RAINBOW_TXT_PATH}.${NC}\n"
fi
chmod 664 "$RAINBOW_TXT_PATH"
echo -e "${GREEN}Permissions de rainbow.txt définies.${NC}\n"


# --- Permissions pour les scripts Python des modules et le script principal ---
echo -e "${BLUE}--- Attribution des permissions aux fichiers et dossiers ---${NC}" # Ajout de titre de section

chmod +x "$INSTALL_DIR/hashish.py"
echo -e "${GREEN}Permissions d'exécution accordées à ${INSTALL_DIR}/hashish.py.${NC}"

chmod +r "$INSTALL_DIR/banner-hashish.txt"
echo -e "${GREEN}Permissions de lecture accordées à ${INSTALL_DIR}/banner-hashish.txt.${NC}"

find "$MODULES_TARGET_DIR" -type f -name "*.py" -exec chmod +x {} \;
echo -e "${GREEN}Permissions d'exécution accordées aux modules Python dans ${MODULES_TARGET_DIR}.${NC}"

chmod 755 "$INSTALL_DIR"
chmod 755 "$MODULES_TARGET_DIR"
chmod 755 "$WORDLISTS_TARGET_DIR"
echo -e "${GREEN}Permissions des dossiers définies.${NC}\n"


# --- Création d'un script exécutable global ---
echo -e "${BLUE}--- Création du raccourci 'hashish' ---${NC}" # Ajout de titre de section
echo -e "${INFO}Création du raccourci 'hashish' pour un lancement facile...${NC}"
cat > "$INSTALL_DIR/hashish" << EOF
#!/data/data/com.termux/files/usr/bin/bash
# Script de lancement pour HASHISH Ethical Toolkit

clear_screen_func() {
    if command -v clear &>/dev/null; then
        clear
    else
        printf '\033c'
    fi
}

clear_screen_func
exec python3 "$INSTALL_DIR/hashish.py" "\$@"
EOF
chmod +x "$INSTALL_DIR/hashish"
echo -e "${GREEN}Raccourci 'hashish' créé dans ${INSTALL_DIR}. Vous pouvez maintenant lancer l'outil simplement en tapant '${BOLD}hashish${NC}${GREEN}'.${NC}\n"

# --- Installation des dépendances Python ---
echo -e "${BLUE}--- Installation des dépendances Python ---${NC}" # Ajout de titre de section
echo -e "${INFO}Installation des dépendances Python listées dans ${REPO_PATH}/requirements.txt...${NC}"
if [ -f "$REPO_PATH/requirements.txt" ]; then
    if ! command -v pip &> /dev/null; then
        echo -e "${YELLOW}pip n'est pas trouvé. Tentative d'installation de 'python-pip'...${NC}"
        install_package "python-pip" || { echo -e "${RED}${BOLD}Impossible d'installer pip.${NC}${RED} Veuillez l'installer manuellement (pkg install python-pip).${NC}"; }
    fi

    if command -v pip &> /dev/null; then
        if pip install -r "$REPO_PATH/requirements.txt"; then
            echo -e "${GREEN}Dépendances Python installées avec succès.${NC}\n"
        else
            echo -e "${RED}Erreur: Impossible d'installer les dépendances Python.${NC}"
            echo -e "${YELLOW}Vérifiez ${REPO_PATH}/requirements.txt, votre connexion Internet, ou essayez 'pip install --upgrade pip'.${NC}"
            echo -e "${YELLOW}Vous pouvez essayer de les installer manuellement plus tard avec 'pip install -r ${REPO_PATH}/requirements.txt'.${NC}\n"
        fi
    else
        echo -e "${RED}Erreur: pip n'est pas disponible. Impossible d'installer les dépendances Python.${NC}"
        echo -e "${YELLOW}Veuillez l'installer manuellement et les dépendances si nécessaire.${NC}\n"
    fi
else
    echo -e "${YELLOW}Fichier 'requirements.txt' introuvable. Aucune dépendance Python à installer.${NC}\n"
fi


# --- Finalisation et Lancement Automatique ---
echo -e "${CYAN}=====================================================${NC}"
echo -e "${GREEN}${BOLD}  Installation de HASHISH terminée avec succès ! 🚀 ${NC}"
echo -e "${CYAN}=====================================================${NC}\n"
echo -e "${GREEN}Vous pouvez maintenant lancer l'outil avec la commande : ${CYAN}${BOLD}hashish${NC}\n"
echo -e "${BLUE}Pour tester, lancement de HASHISH (Appuyez sur Ctrl+C pour quitter)...${NC}"

if command -v hashish &> /dev/null; then
  hashish
else
  echo -e "${YELLOW}Impossible de lancer 'hashish' automatiquement. Le raccourci pourrait ne pas être dans votre PATH ou une erreur précédente a bloqué sa création.${NC}"
  echo -e "${YELLOW}Commande manuelle : ${CYAN}python3 ${INSTALL_DIR}/hashish.py${NC}"
fi

echo -e "${CYAN}Merci d'avoir installé HASHISH. Bon travail !${NC}\n"
exit 0
