#!/bin/bash

# --- 1. Définitions et Initialisations ---
# Définitions des couleurs pour une sortie console claire
GREEN='\033[0;32m'
NC='\033[0m'       # No Color
YELLOW='\033[0;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
INFO='\033[0;34m'  # Bleu clair pour les messages d'information
BOLD='\033[1m'     # Texte en gras

# Définition des chemins d'installation et de dépôt
REPO_NAME="hashish"
INSTALL_DIR="/data/data/com.termux/files/usr/bin"
MODULES_TARGET_DIR="$INSTALL_DIR/modules"
WORDLISTS_TARGET_DIR="$MODULES_TARGET_DIR/wordlists"

# --- 2. Fonctions Utilitaires ---

# Fonction pour effacer l'écran du terminal
clear_screen() {
    if command -v clear &>/dev/null; then
        clear
    else
        printf '\033c' # Commande alternative pour effacer l'écran
    fi
}

# Fonction pour installer un paquet Termux
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
clear_screen
echo -e "${BLUE}${BOLD}███╗  ███╗ █████╗ ███████╗██╗  ██╗██╗███████╗██╗  ██╗${NC}"
echo -e "${BLUE}${BOLD}████╗████║██╔══██╗██╔════╝██║  ██║██║██╔════╝██║  ██║${NC}"
echo -e "${BLUE}${BOLD}██╔████╔██║███████║███████╗███████║██║█████╗  ███████║${NC}"
echo -e "${BLUE}${BOLD}██║╚██╔╝██║██╔══██║╚════██║██╔══██║██║██╔══╝  ██╔══██║${NC}"
echo -e "${BLUE}${BOLD}██║ ╚═╝ ██║██║  ██║███████║██║  ██║██║███████╗██║  ██║${NC}"
echo -e "${BLUE}${BOLD}╚═╝     ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝╚══════╝╚═╝  ╚═╝${NC}\n"
echo -e "${CYAN}-----------------------------------------------------------${NC}"
echo -e "${CYAN}  Bienvenue dans le script d'installation de Hashish !    ${NC}"
echo -e "${CYAN}-----------------------------------------------------------${NC}\n"

echo -e "${INFO}Début du processus d'installation. Cela peut prendre quelques minutes.${NC}"
echo -e "${INFO}Nettoyage des installations précédentes pour éviter les conflits...${NC}"

# Suppression des anciens fichiers si existants pour une installation propre
rm -f "$INSTALL_DIR/hashish"
rm -rf "$MODULES_TARGET_DIR"

echo -e "${GREEN}Nettoyage terminé.${NC}\n"

# --- 4. Mise à jour des paquets Termux ---
echo -e "${BLUE}--- Mise à jour des paquets Termux ---${NC}"
echo -e "${INFO}Mise à jour et mise à niveau des paquets système. Cela peut prendre un certain temps...${NC}"
if pkg update -y && pkg upgrade -y; then
    echo -e "${GREEN}Mise à jour des paquets terminée avec succès.${NC}\n"
else
    echo -e "${RED}Erreur : Échec de la mise à jour des paquets Termux. Veuillez vérifier votre connexion.${NC}"
    exit 1
fi

# --- 5. Détection du Chemin du Dépôt (où sont les fichiers sources de Hashish) ---
echo -e "${BLUE}--- Détection du chemin du dépôt Hashish ---${NC}"
# Tente de trouver le chemin du dépôt en remontant l'arborescence
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_PATH=$(git rev-parse --show-toplevel 2>/dev/null)

if [ -z "$REPO_PATH" ]; then
    echo -e "${YELLOW}Avertissement : Le script n'est pas exécuté depuis un dépôt Git. Tentative de déduction du chemin...${NC}"
    # Si non exécuté depuis un dépôt Git, suppose que le script est dans le dossier principal ou un sous-dossier direct
    if [[ "$SCRIPT_DIR" == *"/$REPO_NAME"* ]]; then
        REPO_PATH="${SCRIPT_DIR%%/$REPO_NAME*}/$REPO_NAME"
    else
        REPO_PATH="$SCRIPT_DIR" # Dernier recours, suppose que le script est à la racine du dépôt
    fi
    echo -e "${INFO}Chemin du dépôt déduit : ${REPO_PATH}${NC}"
fi

if [ ! -d "$REPO_PATH" ]; then
    echo -e "${RED}Erreur : Chemin du dépôt Hashish introuvable. Veuillez exécuter le script depuis le dossier 'hashish'.${NC}"
    exit 1
fi
echo -e "${GREEN}Chemin du dépôt détecté : ${REPO_PATH}${NC}\n"

# --- 6. Vérification des Fichiers Essentiels du Dépôt ---
echo -e "${BLUE}--- Vérification des fichiers essentiels du dépôt ---${NC}"
REQUIRED_FILES=(
    "$REPO_PATH/hashish.py"
    "$REPO_PATH/banner-hashish.txt"
    "$REPO_PATH/modules/hashcracker.py"
    "$REPO_PATH/modules/hash_recon.py"
    "$REPO_PATH/modules/rainbow_generator.py" # Assurez-vous que c'est bien le .py
    "$REPO_PATH/modules/hashcracker.cpp"
    "$REPO_PATH/modules/hash_recon.cpp"
    "$REPO_PATH/modules/hash_recon.h" # Le fichier d'en-tête
    "$REPO_PATH/modules/dictionaries/rockyou.txt"
)

ALL_FILES_EXIST=true
for file in "${REQUIRED_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo -e "${RED}Erreur : Fichier essentiel introuvable : '$file'. Veuillez vous assurer que le dépôt est complet et à jour.${NC}"
        ALL_FILES_EXIST=false
    fi
done

if ! $ALL_FILES_EXIST; then
    echo -e "${RED}Installation annulée en raison de fichiers manquants.${NC}"
    exit 1
fi
echo -e "${GREEN}Tous les fichiers essentiels sont présents.${NC}\n"

# --- 7. Installation des Prérequis Système Termux ---
echo -e "${BLUE}--- Installation des prérequis système Termux ---${NC}"
install_package "python" || exit 1
install_package "python-pip" || exit 1
install_package "clang" || exit 1 # Compilateur C/C++
install_package "openssl-tool" || exit 1 # Pour les fonctions de hachage C++
install_package "libcrypt" || exit 1 # Pour crypt() si utilisé (moins courant)
install_package "git" || exit 1 # Pour cloner le dépôt si l'utilisateur ne l'a pas déjà
echo -e "${GREEN}Tous les prérequis système sont installés.${NC}\n"

# --- 8. Création des Dossiers Cibles dans Termux ---
echo -e "${BLUE}--- Création des dossiers cibles ---${NC}"
mkdir -p "$MODULES_TARGET_DIR" || { echo -e "${RED}Erreur : Impossible de créer le dossier des modules. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$WORDLISTS_TARGET_DIR" || { echo -e "${RED}Erreur : Impossible de créer le dossier des wordlists. Vérifiez les permissions.${NC}"; exit 1; }
echo -e "${GREEN}Dossiers cibles créés avec succès.${NC}\n"

# --- 9. Copie des Fichiers Principaux (hashish.py, banner-hashish.txt) ---
echo -e "${BLUE}--- Copie des fichiers principaux ---${NC}"
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/" || { echo -e "${RED}Erreur : Impossible de copier hashish.py.${NC}"; exit 1; }
cp "$REPO_PATH/banner-hashish.txt" "$INSTALL_DIR/" || { echo -e "${RED}Erreur : Impossible de copier banner-hashish.txt.${NC}"; exit 1; }
echo -e "${GREEN}Fichiers principaux copiés avec succès.${NC}\n"

# --- 10. Copie des Modules Python ---
echo -e "${BLUE}--- Copie des modules Python vers '$MODULES_TARGET_DIR/' ---${NC}"
cp "$REPO_PATH/modules/hashcracker.py" "$MODULES_TARGET_DIR/" || { echo -e "${RED}Erreur : Impossible de copier hashcracker.py.${NC}"; exit 1; }
cp "$REPO_PATH/modules/hash_recon.py" "$MODULES_TARGET_DIR/" || { echo -e "${RED}Erreur : Impossible de copier hash_recon.py.${NC}"; exit 1; }
cp "$REPO_PATH/modules/rainbow_generator.py" "$MODULES_TARGET_DIR/" || { echo -e "${RED}Erreur : Impossible de copier rainbow_generator.py.${NC}"; exit 1; }
echo -e "${GREEN}Modules Python copiés avec succès.${NC}\n"

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
echo -e "${BLUE}--- Copie des wordlists par défaut ---${NC}"
# Vérifiez si rockyou.txt est déjà décompressé, sinon décompressez-le.
if [ -f "$REPO_PATH/modules/dictionaries/rockyou.txt" ]; then
    cp "$REPO_PATH/modules/dictionaries/rockyou.txt" "$WORDLISTS_TARGET_DIR/" || { echo -e "${RED}Erreur : Impossible de copier rockyou.txt.${NC}"; }
    echo -e "${GREEN}Wordlist 'rockyou.txt' copiée.${NC}\n"
elif [ -f "$REPO_PATH/modules/dictionaries/rockyou.txt.gz" ]; then
    echo -e "${INFO}Décompression de 'rockyou.txt.gz'. Cela peut prendre un moment...${NC}"
    gunzip -c "$REPO_PATH/modules/dictionaries/rockyou.txt.gz" > "$WORDLISTS_TARGET_DIR/rockyou.txt" || { echo -e "${RED}Erreur : Impossible de décompresser et copier rockyou.txt.gz.${NC}"; }
    echo -e "${GREEN}Wordlist 'rockyou.txt' décompressée et copiée.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : rockyou.txt (ou .gz) non trouvé. La wordlist par défaut ne sera pas copiée.${NC}"
    echo -e "${YELLOW}Vous devrez ajouter vos propres wordlists manuellement dans '$WORDLISTS_TARGET_DIR/'.${NC}\n"
fi

# --- 13. Pré-traitement : Correction des Fichiers C++ (si nécessaire) ---
# Cette étape est souvent spécifique si des problèmes de compatibilité ou de syntaxe sont connus.
# Pour l'instant, on se contente d'une validation simple.
echo -e "${BLUE}--- Pré-traitement des fichiers C++ ---${NC}"
if [ -f "$REPO_PATH/modules/hashcracker.cpp" ]; then
    echo -e "${INFO}Vérification de 'hashcracker.cpp'...${NC}"
    # Exemple: Vérifier la présence de fonctions essentielles ou la syntaxe de base
    if grep -q "main(" "$REPO_PATH/modules/hashcracker.cpp"; then
        echo -e "${GREEN}Vérification de 'hashcracker.cpp' réussie.${NC}"
    else
        echo -e "${YELLOW}Avertissement : 'main()' non trouvé dans hashcracker.cpp. Cela pourrait être un problème de compilation.${NC}"
    fi
else
    echo -e "${RED}Erreur : 'hashcracker.cpp' introuvable pour le pré-traitement.${NC}"
    exit 1
fi
echo -e "${GREEN}Pré-traitement C++ terminé.${NC}\n"

# --- 14. Compilation des Modules C++ (hashcracker et hash_recon) ---
echo -e "${BLUE}--- Compilation et déplacement des modules C++ ('hashcracker' et 'hash_recon') ---${NC}"
HASHCRACKER_CPP_SOURCE="$REPO_PATH/modules/hashcracker.cpp"
HASH_RECON_CPP_SOURCE="$REPO_PATH/modules/hash_recon.cpp"
HASHCRACKER_TEMP_EXECUTABLE="$REPO_PATH/modules/hashcracker_temp"
HASHCRACKER_FINAL_EXECUTABLE="$MODULES_TARGET_DIR/hashcracker"

if [ -f "$HASHCRACKER_CPP_SOURCE" ] && [ -f "$HASH_RECON_CPP_SOURCE" ]; then
    echo -e "${INFO}  -> Fichiers sources C++ trouvés : '${HASHCRACKER_CPP_SOURCE}' et '${HASH_RECON_CPP_SOURCE}'.${NC}"

    # Définition des chemins OpenSSL pour la compilation
    OPENSSL_INCLUDE_PATH="/data/data/com.termux/files/usr/include"
    OPENSSL_LIB_PATH="/data/data/com.termux/files/usr/lib"

    echo -e "${CYAN}  -> Lancement de la compilation avec g++...${NC}"
    # Commande de compilation clé : inclut hashcracker.cpp ET hash_recon.cpp
    if g++ "$HASHCRACKER_CPP_SOURCE" "$HASH_RECON_CPP_SOURCE" -o "$HASHCRACKER_TEMP_EXECUTABLE" \
       -std=c++17 -fopenmp -pthread \
       -I"$OPENSSL_INCLUDE_PATH" \
       -I"$MODULES_TARGET_DIR" \
       -L"$OPENSSL_LIB_PATH" \
       -lssl -lcrypto; then # -lssl et -lcrypto sont pour OpenSSL

        echo -e "${GREEN}  -> Modules C++ hashcracker et hash_recon compilés avec succès vers : ${HASHCRACKER_TEMP_EXECUTABLE}${NC}"

        if [ ! -d "$MODULES_TARGET_DIR" ]; then
            echo -e "${RED}Erreur : Le dossier cible des modules '${MODULES_TARGET_DIR}' n'existe pas. Impossible de déplacer l'exécutable C++.${NC}"
            echo -e "${YELLOW}  Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
            exit 1
        fi

        echo -e "${INFO}  -> Déplacement de l'exécutable compilé vers : ${HASHCRACKER_FINAL_EXECUTABLE}${NC}"
        if mv "$HASHCRACKER_TEMP_EXECUTABLE" "$HASHCRACKER_FINAL_EXECUTABLE"; then
            echo -e "${GREEN}  -> Exécutable C++ déplacé avec succès.${NC}"
            chmod +x "$HASHCRACKER_FINAL_EXECUTABLE" # Rendre l'exécutable
            echo -e "${GREEN}  -> Permissions d'exécution accordées à ${HASHCRACKER_FINAL_EXECUTABLE}.${NC}"
        else
            echo -e "${RED}Erreur : Impossible de déplacer l'exécutable C++ vers ${HASHCRACKER_FINAL_EXECUTABLE}. Vérifiez les permissions du dossier cible ou l'espace disque.${NC}"
            echo -e "${YELLOW}  Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
            exit 1
        fi
    else
        echo -e "${RED}------------------------------------------------------------------${NC}"
        echo -e "${RED}${BOLD}ERREUR CRITIQUE : Échec de la compilation des modules C++.${NC}"
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
# Si tu avais un ancien binaire rainbow_generator, le retirer.
# Actuellement, la génération est intégrée en Python, mais bon à garder au cas où.
echo -e "${BLUE}--- Nettoyage des anciens exécutables (si présents) ---${NC}"
if [ -f "$MODULES_TARGET_DIR/rainbow_generator_old" ]; then
    rm "$MODULES_TARGET_DIR/rainbow_generator_old"
    echo -e "${GREEN}Ancien 'rainbow_generator_old' supprimé.${NC}"
fi
echo -e "${GREEN}Nettoyage des anciens exécutables terminé.${NC}\n"

# --- 16. Vérification et Création du Fichier 'rainbow.txt' ---
# Assurez-vous que le fichier de base pour les tables arc-en-ciel existe.
echo -e "${BLUE}--- Vérification du fichier de table arc-en-ciel par défaut ---${NC}"
RAINBOW_TABLE_DEFAULT="$MODULES_TARGET_DIR/rainbow.txt"
if [ ! -f "$RAINBOW_TABLE_DEFAULT" ]; then
    echo -e "${INFO}Le fichier de table arc-en-ciel par défaut '${RAINBOW_TABLE_DEFAULT}' n'existe pas. Création d'un fichier vide.${NC}"
    touch "$RAINBOW_TABLE_DEFAULT" || { echo -e "${RED}Erreur : Impossible de créer le fichier 'rainbow.txt'. Vérifiez les permissions.${NC}"; exit 1; }
    echo -e "${GREEN}Fichier '${RAINBOW_TABLE_DEFAULT}' créé avec succès.${NC}\n"
else
    echo -e "${GREEN}Fichier '${RAINBOW_TABLE_DEFAULT}' trouvé.${NC}\n"
fi

# --- 17. Attribution des Permissions aux Fichiers et Dossiers ---
echo -e "${BLUE}--- Attribution des permissions ---${NC}"
chmod +x "$INSTALL_DIR/hashish.py" # Le script principal
chmod +x "$MODULES_TARGET_DIR/hashcracker.py"
chmod +x "$MODULES_TARGET_DIR/hash_recon.py"
chmod +x "$MODULES_TARGET_DIR/rainbow_generator.py" # Le script Python de génération
# Note: hashcracker (le binaire C++) a déjà reçu les permissions +x à l'étape 14.
echo -e "${GREEN}Permissions attribuées aux scripts Python et aux dossiers.${NC}\n"

# --- 18. Création du Raccourci Global 'hashish' ---
echo -e "${BLUE}--- Création du raccourci global 'hashish' ---${NC}"
# Crée un script wrapper pour exécuter hashish.py avec python
WRAPPER_SCRIPT_PATH="$INSTALL_DIR/hashish"
echo "#!/data/data/com.termux/files/usr/bin/bash" > "$WRAPPER_SCRIPT_PATH"
echo "python '$INSTALL_DIR/hashish.py' \"\$@\"" >> "$WRAPPER_SCRIPT_PATH"
chmod +x "$WRAPPER_SCRIPT_PATH"
echo -e "${GREEN}Raccourci 'hashish' créé avec succès. Vous pouvez maintenant lancer Hashish en tapant '${BOLD}hashish${NC}${GREEN}' dans votre terminal.${NC}\n"

# --- 19. Installation des Dépendances Python ---
echo -e "${BLUE}--- Installation des dépendances Python via pip ---${NC}"
# Liste des dépendances Python (ajoutez-en d'autres si nécessaire)
PYTHON_DEPS=("pycryptodome" "colorama") # colorama pour la coloration cross-plateforme
for dep in "${PYTHON_DEPS[@]}"; do
    echo -e "${INFO}  -> Installation de la dépendance Python : ${dep}...${NC}"
    if pip install "$dep"; then
        echo -e "${GREEN}  -> Dépendance '${dep}' installée avec succès.${NC}"
    else
        echo -e "${RED}  -> Échec de l'installation de la dépendance '${dep}'. Veuillez vérifier votre connexion Internet.${NC}"
        echo -e "${YELLOW}  Cela pourrait affecter certaines fonctionnalités de Hashish.${NC}"
    fi
done
echo -e "${GREEN}Installation des dépendances Python terminée.${NC}\n"

# --- 20. Finalisation et Lancement Automatique ---
echo -e "${CYAN}-----------------------------------------------------------${NC}"
echo -e "${CYAN}${BOLD}Installation de Hashish terminée avec succès !${NC}"
echo -e "${CYAN}-----------------------------------------------------------${NC}\n"

echo -e "${YELLOW}Pour lancer Hashish, tapez simplement : ${BOLD}hashish${NC}\n"

read -p "Voulez-vous lancer Hashish maintenant ? (y/n) > " launch_now
if [[ "$launch_now" =~ ^[Yy]$ ]]; then
    echo -e "${INFO}Lancement de Hashish...${NC}"
    hashish
else
    echo -e "${INFO}Pour lancer Hashish plus tard, n'oubliez pas la commande : ${BOLD}hashish${NC}"
fi

exit 0
