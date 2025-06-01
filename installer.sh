#!/bin/bash

# --- 1. Définitions et Initialisations ---

# Couleurs pour les messages dans le terminal, rend l'affichage plus lisible et agréable.
GREEN='\033[0;32m' # Vert pour les succès
NC='\033[0m'    # Pas de couleur (réinitialise la couleur après un message)
YELLOW='\033[0;33m' # Jaune pour les avertissements
RED='\033[0;31m'  # Rouge pour les erreurs critiques
BLUE='\033[0;34m'  # Bleu pour les informations générales ou les étapes
CYAN='\033[0;36m'  # Cyan pour les titres ou les actions spécifiques
INFO='\033[0;34m'  # Similaire à BLUE, utilisé pour les informations détaillées

# Nom du dépôt principal de l'outil
REPO_NAME="hashish"

# Chemins d'installation standard pour Termux.
# C'est là que les exécutables et scripts Termux sont stockés.
INSTALL_DIR="/data/data/com.termux/files/usr/bin"
# Le dossier 'modules' sera créé dans INSTALL_DIR pour contenir les scripts Python
# et les exécutables compilés C++ des modules de Hashish.
MODULES_TARGET_DIR="$INSTALL_DIR/modules"
# Nouveau dossier pour les wordlists par défaut, pour une meilleure organisation.
WORDLISTS_TARGET_DIR="$MODULES_TARGET_DIR/wordlists"

# --- 2. Fonctions Utilitaires ---

# Fonction pour effacer l'écran de manière portable.
# Elle tente d'utiliser 'clear' (plus efficace), sinon elle utilise une séquence d'échappement ANSI.
clear_screen() {
    if command -v clear &>/dev/null; then
        clear
    else
        printf '\033c'
    fi
}

# Fonction pour installer un paquet Termux de manière générique.
# Prend le nom du paquet en argument et affiche des messages colorés.
install_package() {
    local package_name=$1
    echo -e "${INFO}  -> Installation du paquet Termux : ${package_name}...${NC}"
    if pkg install "$package_name" -y; then
        echo -e "${GREEN}  -> Paquet '${package_name}' installé avec succès.${NC}"
        return 0 # Succès
    else
        echo -e "${RED}  -> Échec de l'installation du paquet '${package_name}'. Veuillez vérifier votre connexion Internet ou les dépôts Termux.${NC}"
        return 1 # Échec
    fi
}

# --- 3. Démarrage du Script et Nettoyage Initial ---

clear_screen # Efface l'écran pour un démarrage propre.

echo -e "${CYAN}=======================================${NC}"
echo -e "${CYAN}  Installation de HASHISH Ethical Toolkit${NC}"
echo -e "${CYAN}=======================================${NC}\n"

# --- 4. Mise à jour des paquets Termux ---
echo -e "${BLUE}--- Mise à jour et mise à niveau des paquets Termux ---${NC}"
echo -e "${INFO}  Cela peut prendre un certain temps et est important pour éviter les problèmes de dépendances.${NC}"
if pkg update -y && pkg upgrade -y; then
    echo -e "${GREEN}Termux paquets mis à jour et mis à niveau avec succès !${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Impossible de mettre à jour/mettre à niveau les paquets Termux.${NC}"
    echo -e "${YELLOW}  Cela pourrait causer des problèmes de dépendances. Veuillez vérifier votre connexion Internet et les dépôts, puis réessayez manuellement : 'pkg update && pkg upgrade'.${NC}\n"
fi

# --- 5. Détection du Chemin du Dépôt (où sont les fichiers sources de Hashish) ---
echo -e "${BLUE}--- Détection du chemin du dépôt '${REPO_NAME}' ---${NC}"
# Obtenir le répertoire où se trouve le script 'installer.sh'
CURRENT_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_PATH=""

# Stratégie de détection du chemin du dépôt :
# 1. Vérifier si le script est directement dans le dossier du dépôt (ex: hashish/installer.sh).
# 2. Remonter l'arborescence à partir du script pour trouver un dossier nommé 'hashish'.
# 3. Vérifier un chemin Termux par défaut (/data/data/com.termux/files/home/hashish).
# 4. Demander à l'utilisateur si tout échoue.

# Tenter de trouver le dossier 'hashish' en remontant l'arborescence
# C'est utile si 'installer.sh' est dans un sous-dossier comme 'hashish/scripts/installer.sh'.
temp_dir="$CURRENT_SCRIPT_DIR"
while [[ "$temp_dir" != "/" && -n "$temp_dir" ]]; do # S'assurer de ne pas aller au-delà de la racine ou d'avoir un répertoire vide
    if [[ "$(basename "$temp_dir")" == "$REPO_NAME" ]]; then
        REPO_PATH="$temp_dir"
        break
    fi
    temp_dir=$(dirname "$temp_dir") # Remonter d'un niveau
done

# Si le chemin n'a pas été trouvé en remontant
if [ -n "$REPO_PATH" ]; then
    echo -e "${INFO}  -> Dépôt '${REPO_NAME}' détecté à partir de l'emplacement actuel : ${REPO_PATH}${NC}"
else
    # Vérifier l'emplacement par défaut de git clone dans Termux
    DEFAULT_HOME_PATH="/data/data/com.termux/files/home"
    DEFAULT_REPO_PATH="$DEFAULT_HOME_PATH/$REPO_NAME"
    if [ -d "$DEFAULT_REPO_PATH" ]; then
        REPO_PATH="$DEFAULT_REPO_PATH"
        echo -e "${INFO}  -> Dépôt '${REPO_NAME}' détecté à l'emplacement par défaut Termux : ${REPO_PATH}${NC}"
    else
        echo -e "${YELLOW}Avertissement : Le répertoire '$REPO_NAME' n'a été trouvé ni près du script ni à l'emplacement par défaut (${DEFAULT_REPO_PATH}).${NC}"
        read -p "Voulez-vous entrer le chemin **complet** du dossier '$REPO_NAME' manuellement ? (o/n) : " confirm
        if [[ "$confirm" == "o" || "$confirm" == "O" ]]; then
            read -p "Veuillez entrer le chemin **complet** du dossier '$REPO_NAME' (ex: /sdcard/Hashish) : " CUSTOM_REPO_PATH
            if [ -n "$CUSTOM_REPO_PATH" ]; then
                REPO_PATH="$CUSTOM_REPO_PATH"
                echo -e "${INFO}  -> Nouveau chemin du dépôt: ${REPO_PATH}${NC}"
            else
                echo -e "${RED}Erreur : Installation annulée. Le chemin du dépôt est nécessaire.${NC}"
                exit 1 # Quitte le script avec un code d'erreur
            fi
        else
            echo -e "${RED}Erreur : Installation annulée. Impossible de trouver le dossier du dépôt.${NC}"
            exit 1 # Quitte le script
        fi
    fi
fi
echo -e "${GREEN}Chemin du dépôt '${REPO_NAME}' validé à : ${REPO_PATH}${NC}\n"

# --- 6. Vérification des Fichiers Essentiels du Dépôt ---
echo -e "${BLUE}--- Validation du contenu du dépôt '${REPO_PATH}' ---${NC}"
# Ces vérifications s'assurent que les fichiers et dossiers nécessaires sont bien présents dans le dépôt.
# Si un élément essentiel manque, le script s'arrête car l'installation ne pourrait pas se faire correctement.
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
if [ ! -f "$REPO_PATH/modules/hash_recon.h" ]; then
    echo -e "${RED}Erreur : 'hash_recon.h' introuvable dans '$REPO_PATH/modules/'. Ce fichier est nécessaire pour la compilation des modules C++.${NC}"
    exit 1
fi
if [ ! -f "$REPO_PATH/modules/hash_recon.cpp" ]; then
    echo -e "${RED}Erreur : 'hash_recon.cpp' introuvable dans '$REPO_PATH/modules/'. Ce fichier est nécessaire pour la compilation des modules C++.${NC}"
    exit 1
fi
# Avertissements pour les fichiers/dossiers non critiques mais utiles.
if [ ! -f "$REPO_PATH/requirements.txt" ]; then
    echo -e "${YELLOW}Avertissement : 'requirements.txt' introuvable dans '$REPO_PATH'. Aucune dépendance Python ne sera installée via ce fichier.${NC}"
fi
if [ ! -d "$REPO_PATH/wordlists" ]; then
    echo -e "${YELLOW}Avertissement : Le dossier 'wordlists' est introuvable au niveau racine du dépôt '$REPO_PATH'. Les wordlists par défaut ne seront pas installées.${NC}"
fi
echo -e "${GREEN}Contenu du dépôt '$REPO_PATH' validé.${NC}\n"

# --- 7. Installation des Prérequis Système Termux ---
echo -e "${BLUE}--- Vérification et installation des prérequis système (build-essential, openssl, ncurses-utils, rsync) ---${NC}"

# Vérifier et installer 'build-essential' (contient g++ et d'autres outils de compilation)
if ! command -v g++ &> /dev/null; then
    echo -e "${YELLOW}Compilateur g++ non trouvé. Installation de 'build-essential'...${NC}"
    install_package "build-essential" || { echo -e "${RED}Erreur critique : 'build-essential' est nécessaire pour compiler les modules C++. Installation annulée.${NC}"; exit 1; }
fi

# Vérifier et installer 'openssl' (pour les bibliothèques de développement SSL/Crypto utilisées par C++)
if ! pkg list-installed | grep -q "^openssl/"; then
    echo -e "${YELLOW}OpenSSL non trouvé. Nécessaire pour la compilation C++. Installation de 'openssl'...${NC}"
    install_package "openssl" || { echo -e "${RED}Erreur critique : 'openssl' est nécessaire pour compiler les modules C++. Installation annulée.${NC}"; exit 1; }
else
    echo -e "${GREEN}OpenSSL est déjà installé.${NC}"
fi

# Vérifier et installer 'ncurses-utils' (pour la commande 'clear')
if ! command -v clear &> /dev/null; then
    echo -e "${YELLOW}Commande 'clear' non trouvée. Installation de 'ncurses-utils'...${NC}"
    install_package "ncurses-utils" || { echo -e "${YELLOW}Avertissement : Impossible d'installer 'ncurses-utils'. La commande 'clear' pourrait ne pas fonctionner correctement, mais un fallback sera utilisé.${NC}"; }
else
    echo -e "${GREEN}Commande 'clear' est déjà disponible.${NC}"
fi

# Installer rsync si non présent (utile pour une copie efficace des fichiers)
if ! command -v rsync &> /dev/null; then
    echo -e "${YELLOW}rsync non trouvé. Installation de 'rsync'...${NC}"
    install_package "rsync" || { echo -e "${YELLOW}Avertissement : Impossible d'installer 'rsync'. La copie des fichiers sera moins optimisée.${NC}"; }
else
    echo -e "${GREEN}rsync est déjà installé.${NC}"
fi

# Vérification finale de g++ après les tentatives d'installation
if ! command -v g++ &> /dev/null; then
  echo -e "${RED}Erreur critique : g++ n'est toujours pas disponible après l'installation des prérequis. Impossible de compiler les modules C++.${NC}"
  echo -e "${YELLOW}Veuillez résoudre manuellement le problème d'installation de 'build-essential' dans Termux et relancer le script.${NC}"
  exit 1
fi
echo -e "${GREEN}Prérequis système vérifiés et installés si nécessaire.${NC}\n"

# --- 8. Création des Dossiers Cibles dans Termux ---
echo -e "${BLUE}--- Création des dossiers cibles dans Termux (${INSTALL_DIR}, ${MODULES_TARGET_DIR}, ${WORDLISTS_TARGET_DIR}) ---${NC}"
# 'mkdir -p' crée le dossier et tous ses parents si nécessaire, sans erreur s'il existe déjà.
mkdir -p "$INSTALL_DIR" || { echo -e "${RED}Erreur : Impossible de créer le dossier ${INSTALL_DIR}. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$MODULES_TARGET_DIR" || { echo -e "${RED}Erreur : Impossible de créer le dossier ${MODULES_TARGET_DIR}. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$WORDLISTS_TARGET_DIR" || { echo -e "${RED}Erreur : Impossible de créer le dossier ${WORDLISTS_TARGET_DIR}. Vérifiez les permissions.${NC}"; exit 1; }
echo -e "${GREEN}Dossiers cibles créés avec succès.${NC}\n"

# --- 9. Copie des Fichiers Principaux (hashish.py, banner-hashish.txt) ---
echo -e "${BLUE}--- Copie des fichiers principaux vers ${INSTALL_DIR} ---${NC}"
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/hashish.py" || { echo -e "${RED}Erreur : Impossible de copier hashish.py. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
cp "$REPO_PATH/banner-hashish.txt" "$INSTALL_DIR/banner-hashish.txt" || { echo -e "${RED}Erreur : Impossible de copier banner-hashish.txt. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
echo -e "${GREEN}Fichiers principaux copiés avec succès.${NC}\n"

# --- 10. Copie des Modules Python ---
echo -e "${BLUE}--- Copie des modules Python depuis '$REPO_PATH/modules/' vers '$MODULES_TARGET_DIR/' ---${NC}"
# Utilise rsync si disponible pour une copie efficace des fichiers .py et des sous-dossiers
# en excluant 'wordlists/' car il sera géré séparément.
if command -v rsync &> /dev/null; then
    echo -e "${INFO}  -> Utilisation de 'rsync' pour copier les modules Python...${NC}"
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
fi
echo -e "${GREEN}Modules Python copiés avec succès vers ${MODULES_TARGET_DIR}.${NC}\n"

# --- 11. Copie du fichier d'en-tête C++ (hash_recon.h) ---
echo -e "${BLUE}--- Copie de 'hash_recon.h' vers '$MODULES_TARGET_DIR/' ---${NC}"
if [ -f "$REPO_PATH/modules/hash_recon.h" ]; then
    cp "$REPO_PATH/modules/hash_recon.h" "$MODULES_TARGET_DIR/" || { echo -e "${RED}Erreur : Impossible de copier hash_recon.h. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
    chmod +r "$MODULES_TARGET_DIR/hash_recon.h" # Donner des permissions de lecture seulement aux fichiers d'en-tête
    echo -e "${GREEN}Fichier d'en-tête C++ 'hash_recon.h' copié avec succès et permissions définies.${NC}\n"
else
    echo -e "${RED}Erreur : Fichier 'hash_recon.h' introuvable dans '$REPO_PATH/modules/'. Impossible de compiler les modules C++.${NC}"
    exit 1
fi

# --- 12. Copie des Wordlists par Défaut ---
echo -e "${BLUE}--- Copie des wordlists par défaut vers '$WORDLISTS_TARGET_DIR/' ---${NC}"
if [ -d "$REPO_PATH/wordlists" ]; then
    # Copie le contenu du dossier wordlists.
    cp -r "$REPO_PATH/wordlists/"* "$WORDLISTS_TARGET_DIR/" 2>/dev/null || { echo -e "${YELLOW}Avertissement : Aucun fichier de wordlist par défaut trouvé à copier ou erreur lors de la copie.${NC}"; }
    chmod -R +r "$WORDLISTS_TARGET_DIR" # Définir les permissions de lecture pour tous les fichiers et dossiers
    echo -e "${GREEN}Wordlists par défaut copiées avec succès vers ${WORDLISTS_TARGET_DIR} et permissions définies.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Le dossier des wordlists par défaut '$REPO_PATH/wordlists' est introuvable. Les wordlists par défaut ne seront pas installées.${NC}\n"
fi

# --- 13. Pré-traitement : Correction des Fichiers C++ (si nécessaire) ---
echo -e "${BLUE}--- Pré-traitement : Correction de la fonction 'reduce_hash' dans les fichiers C++ ---${NC}"
# La correction est appliquée spécifiquement à hashcracker.cpp.
CPP_FILES_TO_CORRECT=("$REPO_PATH/modules/hashcracker.cpp")
for file in "${CPP_FILES_TO_CORRECT[@]}"; do
    if [ -f "$file" ]; then
        echo -e "${INFO}  -> Correction de $file...${NC}"
        # La commande 'sed -i' modifie le fichier sur place.
        # Elle remplace la ligne spécifique par le bloc de code corrigé.
        # La regex est conçue pour être précise afin d'éviter des remplacements indésirables.
        if grep -q "std::seed_seq seed_sequence(hash.begin(), hash.end());" "$file"; then
            sed -i '/std::string reduced_string = "";/{
                N;N;N;N;N;N;N;N;N; # Lire suffisamment de lignes pour englober la zone
                s/std::seed_seq seed_sequence(hash.begin(), hash.end());/\
std::vector<unsigned int> seed_data;\
for (char c : hash) { seed_data.push_back(static_cast<unsigned int>(c)); }\
seed_data.push_back(static_cast<unsigned int>(r_index));\
\
std::seed_seq seed_sequence(seed_data.begin(), seed_data.end());/
            }' "$file"
            echo -e "${GREEN}  -> Correction appliquée à $file.${NC}"
        else
            echo -e "${INFO}  -> La correction de $file ne semble pas nécessaire (déjà appliquée ou motif non trouvé).${NC}"
        fi
    else
        echo -e "${YELLOW}Avertissement : Fichier C++ '$file' non trouvé pour la correction.${NC}"
    fi
done
echo -e "${GREEN}Correction des fichiers C++ terminée.${NC}\n"

# --- 14. Compilation des Modules C++ (hashcracker et hash_recon) ---
echo -e "${BLUE}--- Compilation et déplacement des modules C++ ('hashcracker' et 'hash_recon') ---${NC}"
HASHCRACKER_CPP_SOURCE="$REPO_PATH/modules/hashcracker.cpp"
HASH_RECON_CPP_SOURCE="$REPO_PATH/modules/hash_recon.cpp"
HASHCRACKER_TEMP_EXECUTABLE="$REPO_PATH/modules/hashcracker_temp" # Exécutable temporaire avant déplacement
HASHCRACKER_FINAL_EXECUTABLE="$MODULES_TARGET_DIR/hashcracker" # Emplacement final de l'exécutable

if [ -f "$HASHCRACKER_CPP_SOURCE" ] && [ -f "$HASH_RECON_CPP_SOURCE" ]; then
    echo -e "${INFO}  -> Fichiers sources C++ trouvés : '$HASHCRACKER_CPP_SOURCE' et '$HASH_RECON_CPP_SOURCE'.${NC}"

    # Définir les chemins d'inclusion et de bibliothèque OpenSSL pour Termux.
    OPENSSL_INCLUDE_PATH="/data/data/com.termux/files/usr/include"
    OPENSSL_LIB_PATH="/data/data/com.termux/files/usr/lib"

    echo -e "${CYAN}  -> Lancement de la compilation avec g++...${NC}"
    # Commande de compilation g++ :
    # -std=c++17 : Utilise la norme C++17.
    # -fopenmp : Active la prise en charge d'OpenMP pour le parallélisme.
    # -pthread : Lie la bibliothèque Pthreads pour la gestion des threads.
    # -I... : Ajoute des chemins d'inclusion pour les fichiers d'en-tête (headers).
    # -L... : Ajoute des chemins pour les bibliothèques.
    # -lssl -lcrypto : Lie les bibliothèques OpenSSL (SSL et Crypto).
    if g++ "$HASHCRACKER_CPP_SOURCE" "$HASH_RECON_CPP_SOURCE" -o "$HASHCRACKER_TEMP_EXECUTABLE" \
       -std=c++17 -fopenmp -pthread \
       -I"$OPENSSL_INCLUDE_PATH" \
       -I"$MODULES_TARGET_DIR" \
       -L"$OPENSSL_LIB_PATH" \
       -lssl -lcrypto; then

        echo -e "${GREEN}  -> Modules C++ 'hashcracker' et 'hash_recon' compilés avec succès vers : $HASHCRACKER_TEMP_EXECUTABLE${NC}"

        if [ ! -d "$MODULES_TARGET_DIR" ]; then
            echo -e "${RED}Erreur : Le dossier cible des modules '$MODULES_TARGET_DIR' n'existe pas. Impossible de déplacer l'exécutable C++.${NC}"
            echo -e "${YELLOW}  Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
            exit 1
        fi

        echo -e "${INFO}  -> Déplacement de l'exécutable compilé vers : $HASHCRACKER_FINAL_EXECUTABLE${NC}"
        if mv "$HASHCRACKER_TEMP_EXECUTABLE" "$HASHCRACKER_FINAL_EXECUTABLE"; then
            echo -e "${GREEN}  -> Exécutable C++ déplacé avec succès.${NC}"
            chmod +x "$HASHCRACKER_FINAL_EXECUTABLE" # Rendre l'exécutable compilé... exécutable !
            echo -e "${GREEN}  -> Permissions d'exécution accordées à $HASHCRACKER_FINAL_EXECUTABLE.${NC}"
        else
            echo -e "${RED}Erreur : Impossible de déplacer l'exécutable C++ vers $HASHCRACKER_FINAL_EXECUTABLE. Vérifiez les permissions du dossier cible ou l'espace disque.${NC}"
            echo -e "${YELLOW}  Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
            exit 1
        fi
    else
        echo -e "${RED}------------------------------------------------------------------${NC}"
        echo -e "${RED}ERREUR CRITIQUE : Échec de la compilation de hashcracker.cpp et/ou hash_recon.cpp.${NC}"
        echo -e "${YELLOW}  Veuillez examiner attentivement les messages d'erreur de g++ ci-dessus pour le diagnostic.${NC}"
        echo -e "${YELLOW}  Les causes possibles incluent des bibliothèques OpenSSL manquantes, des en-têtes non trouvés, ou des erreurs dans le code source C++.${NC}"
        echo -e "${YELLOW}  Les modules C++ ne seront PAS disponibles ou ne fonctionneront pas correctement.${NC}"
        echo -e "${RED}------------------------------------------------------------------${NC}"
        exit 1
    fi
else
    echo -e "${YELLOW}Avertissement : Fichiers source C++ (hashcracker.cpp ou hash_recon.cpp) non trouvés dans le dossier des modules. La compilation C++ est ignorée.${NC}"
    echo -e "${YELLOW}  Les modules C++ ne seront PAS disponibles.${NC}"
fi
echo -e "${GREEN}Compilation des modules C++ terminée.${NC}\n"

# --- 15. Nettoyage de l'ancien 'rainbow_generator' (s'il existe) ---
echo -e "${BLUE}--- Nettoyage de l'ancien exécutable 'rainbow_generator' ---${NC}"
RAINBOW_GENERATOR_OLD_EXECUTABLE="$MODULES_TARGET_DIR/rainbow_generator"
if [ -f "$RAINBOW_GENERATOR_OLD_EXECUTABLE" ]; then
    rm "$RAINBOW_GENERATOR_OLD_EXECUTABLE" || { echo -e "${YELLOW}Avertissement : Impossible de supprimer l'ancien 'rainbow_generator'. Veuillez le supprimer manuellement si nécessaire.${NC}"; }
    echo -e "${GREEN}Ancien 'rainbow_generator' supprimé.${NC}\n"
else
    echo -e "${INFO}  -> Ancien 'rainbow_generator' non trouvé, aucun nettoyage nécessaire.${NC}\n"
fi

# --- 16. Vérification et Création du Fichier 'rainbow.txt' ---
echo -e "${BLUE}--- Vérification et création du fichier 'rainbow.txt' ---${NC}"
RAINBOW_TXT_PATH="$MODULES_TARGET_DIR/rainbow.txt"
if [ ! -f "$RAINBOW_TXT_PATH" ]; then
    touch "$RAINBOW_TXT_PATH" || { echo -e "${RED}Erreur : Impossible de créer le fichier rainbow.txt à $RAINBOW_TXT_PATH. Vérifiez les permissions.${NC}"; exit 1; }
    echo -e "${GREEN}Fichier 'rainbow.txt' créé à $RAINBOW_TXT_PATH.${NC}\n"
else
    echo -e "${GREEN}Fichier 'rainbow.txt' déjà existant à $RAINBOW_TXT_PATH.${NC}\n"
fi
chmod 664 "$RAINBOW_TXT_PATH" # rw-rw-r-- (lecture/écriture pour propriétaire/groupe, lecture pour les autres)
echo -e "${GREEN}Permissions de 'rainbow.txt' définies.${NC}\n"

# --- 17. Attribution des Permissions aux Fichiers et Dossiers ---
echo -e "${BLUE}--- Attribution des permissions finales aux fichiers et dossiers ---${NC}"

# Permissions d'exécution pour le script principal hashish.py
chmod +x "$INSTALL_DIR/hashish.py"
echo -e "${GREEN}  -> Permissions d'exécution accordées à ${INSTALL_DIR}/hashish.py.${NC}"

# Permissions de lecture pour le fichier de bannière
chmod +r "$INSTALL_DIR/banner-hashish.txt"
echo -e "${GREEN}  -> Permissions de lecture accordées à ${INSTALL_DIR}/banner-hashish.txt.${NC}"

# Permissions d'exécution pour tous les scripts Python dans le dossier des modules
find "$MODULES_TARGET_DIR" -type f -name "*.py" -exec chmod +x {} \;
echo -e "${GREEN}  -> Permissions d'exécution accordées aux modules Python dans ${MODULES_TARGET_DIR}.${NC}"

# S'assurer que les dossiers ont les bonnes permissions (lecture, écriture, exécution pour la navigation)
chmod 755 "$INSTALL_DIR"
chmod 755 "$MODULES_TARGET_DIR"
chmod 755 "$WORDLISTS_TARGET_DIR"
echo -e "${GREEN}  -> Permissions des dossiers définies.${NC}\n"

# --- 18. Création du Raccourci Global 'hashish' ---
echo -e "${BLUE}--- Création du raccourci 'hashish' pour un lancement facile ---${NC}"
# Crée un script shell simple qui efface l'écran puis lance le script Python principal.
# Le 'exec python3 "$INSTALL_DIR/hashish.py" "\$@"' est important :
# - 'exec' remplace le shell courant par le programme python, ce qui est plus efficace.
# - '"\$@"' passe tous les arguments reçus par le raccourci au script Python.
cat > "$INSTALL_DIR/hashish" << EOF
#!/data/data/com.termux/files/usr/bin/bash
# Script de lancement pour HASHISH Ethical Toolkit

# Fonction pour effacer l'écran de manière portable
clear_screen_func() {
    if command -v clear &>/dev/null; then
        clear
    else
        printf '\033c'
    fi
}

clear_screen_func # Efface l'écran avant de lancer hashish
exec python3 "$INSTALL_DIR/hashish.py" "\$@"
EOF
chmod +x "$INSTALL_DIR/hashish" # Rendre le raccourci exécutable.
echo -e "${GREEN}Raccourci 'hashish' créé dans ${INSTALL_DIR}. Vous pouvez maintenant lancer l'outil simplement en tapant 'hashish'.${NC}\n"

# --- 19. Installation des Dépendances Python ---
echo -e "${BLUE}--- Installation des dépendances Python ---${NC}"
if [ -f "$REPO_PATH/requirements.txt" ]; then
    if ! command -v pip &> /dev/null; then
        echo -e "${YELLOW}pip n'est pas trouvé. Tentative d'installation de 'python-pip'...${NC}"
        install_package "python-pip" || { echo -e "${RED}Erreur : Impossible d'installer pip. Veuillez l'installer manuellement (pkg install python-pip).${NC}"; }
    fi

    if command -v pip &> /dev/null; then
        echo -e "${INFO}  -> Installation des dépendances listées dans '$REPO_PATH/requirements.txt'...${NC}"
        if pip install -r "$REPO_PATH/requirements.txt"; then
            echo -e "${GREEN}Dépendances Python installées avec succès.${NC}\n"
        else
            echo -e "${RED}Erreur : Impossible d'installer les dépendances Python.${NC}"
            echo -e "${YELLOW}  Veuillez vérifier '$REPO_PATH/requirements.txt', votre connexion Internet, ou essayez 'pip install --upgrade pip'.${NC}"
            echo -e "${YELLOW}  Vous pouvez essayer de les installer manuellement plus tard avec 'pip install -r $REPO_PATH/requirements.txt'.${NC}\n"
        fi
    else
        echo -e "${RED}Erreur : pip n'est pas disponible. Impossible d'installer les dépendances Python.${NC}"
        echo -e "${YELLOW}  Veuillez l'installer manuellement et les dépendances si nécessaire.${NC}\n"
    fi
else
    echo -e "${YELLOW}Avertissement : Fichier 'requirements.txt' introuvable. Aucune dépendance Python à installer via ce script.${NC}\n"
fi

# --- 20. Finalisation et Lancement Automatique ---
echo -e "${CYAN}=====================================================${NC}"
echo -e "${GREEN}  Installation de HASHISH terminée avec succès ! 🚀 ${NC}"
echo -e "${CYAN}=====================================================${NC}\n"
echo -e "${GREEN}Vous pouvez maintenant lancer l'outil avec la commande : ${CYAN}hashish${NC}\n"
echo -e "${BLUE}Pour tester, lancement de HASHISH (Appuyez sur Ctrl+C pour quitter)...${NC}"

# Tente de lancer hashish via le raccourci créé.
if command -v hashish &> /dev/null; then
  hashish # Exécute le nouveau raccourci
else
  echo -e "${YELLOW}Impossible de lancer 'hashish' automatiquement. Le raccourci pourrait ne pas être dans votre PATH ou une erreur précédente a bloqué sa création.${NC}"
  echo -e "${YELLOW}  Commande manuelle pour lancer l'outil : ${CYAN}python3 $INSTALL_DIR/hashish.py${NC}"
fi

echo -e "${CYAN}Merci d'avoir installé HASHISH. Bon travail !${NC}\n"
exit 0 # Quitte le script avec un code de succès.
