#!/bin/bash

# Définition des codes couleurs pour une sortie console plus lisible
GREEN='\033[0;32m'
NC='\033[0m'      # Pas de couleur (Reset)
YELLOW='\033[0;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
INFO='\033[0;34m' # Bleu clair pour les informations (alias de BLUE)

# --- Fonctions Utilitaires ---

# Fonction pour effacer l'écran, compatible avec 'clear' ou un équivalent ANSI
clear_screen() {
    if command -v clear &>/dev/null; then
        clear
    else
        printf '\033c' # Code ANSI pour effacer l'écran
    fi
}

# Fonction d'aide pour installer un paquet Termux
install_package() {
    local package_name=$1
    echo -e "${INFO}Tentative d'installation du paquet Termux : ${package_name}...${NC}"
    if pkg install "$package_name" -y; then
        echo -e "${GREEN}Paquet '${package_name}' installé avec succès.${NC}"
        return 0 # Succès
    else
        echo -e "${RED}Échec de l'installation du paquet '${package_name}'. Veuillez vérifier votre connexion Internet ou les dépôts Termux.${NC}"
        return 1 # Échec
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

echo -e "${BLUE}Détection du chemin du dépôt '${REPO_NAME}'...${NC}"

# 1. Vérifie si le script est exécuté depuis le dossier racine du dépôt
#    ou si le dépôt est un répertoire parent du script.
temp_dir="$CURRENT_SCRIPT_DIR"
while [[ "$temp_dir" != "/" && "$temp_dir" != "" ]]; do
    if [[ "$(basename "$temp_dir")" == "$REPO_NAME" ]]; then
        REPO_PATH="$temp_dir"
        break
    fi
    temp_dir=$(dirname "$temp_dir")
done

# 2. Si non trouvé à l'emplacement actuel, vérifie l'emplacement par défaut de Termux.
if [ -z "$REPO_PATH" ]; then
    if [ -d "$DEFAULT_REPO_PATH" ]; then
        REPO_PATH="$DEFAULT_REPO_PATH"
        echo -e "${INFO}Dépôt '${REPO_NAME}' détecté à l'emplacement par défaut de Termux : ${REPO_PATH}${NC}"
    fi
fi

# 3. Si toujours pas trouvé, demande à l'utilisateur d'entrer le chemin manuellement.
if [ -z "$REPO_PATH" ]; then
    echo -e "${RED}Erreur : Le répertoire '$REPO_NAME' est introuvable ni à l'emplacement actuel ni à l'emplacement par défaut (${DEFAULT_REPO_PATH}).${NC}"
    read -p "Voulez-vous entrer le chemin **complet** du dossier '$REPO_NAME' manuellement ? (o/n) : " confirm
    if [[ "$confirm" == "o" || "$confirm" == "O" ]]; then
        read -p "Veuillez entrer le chemin **complet** du dossier '$REPO_NAME' (ex: /sdcard/Hashish) : " CUSTOM_REPO_PATH
        if [ -n "$CUSTOM_REPO_PATH" ] && [ -d "$CUSTOM_REPO_PATH" ]; then
            REPO_PATH="$CUSTOM_REPO_PATH"
            echo -e "${GREEN}Chemin du dépôt '${REPO_NAME}' défini manuellement : ${REPO_PATH}${NC}"
        else
            echo -e "${RED}Chemin invalide ou dossier introuvable. Installation annulée.${NC}"
            exit 1
        fi
    else
        echo -e "${RED}Installation annulée. Impossible de trouver le dossier du dépôt.${NC}"
        exit 1
    fi
else
    echo -e "${INFO}Dépôt '${REPO_NAME}' détecté à : ${REPO_PATH}${NC}\n"
fi

# Définition des répertoires d'installation cibles
INSTALL_DIR="/data/data/com.termux/files/usr/bin"
MODULES_TARGET_DIR="$INSTALL_DIR/modules"
WORDLISTS_TARGET_DIR="$MODULES_TARGET_DIR/wordlists"

# --- Vérification et Installation des Prérequis Système ---
echo -e "${BLUE}Vérification et installation des prérequis système (clang, build-essential, openssl, git, python, etc.)...${NC}"

# Liste des paquets essentiels pour Termux
REQUIRED_PKGS=("clang" "build-essential" "openssl" "git" "python" "ncurses-utils" "rsync" "curl" "nmap" "whois" "dnsutils")

# Parcours et installe les paquets manquants
for pkg_name in "${REQUIRED_PKGS[@]}"; do
    if ! dpkg -s "$pkg_name" &>/dev/null; then # Vérifie si le paquet est installé
        echo -e "${YELLOW}Paquet '${pkg_name}' non trouvé. Installation de '${pkg_name}'...${NC}"
        install_package "$pkg_name" || { # Appelle la fonction d'installation
            echo -e "${RED}Installation annulée. Le paquet '${pkg_name}' est nécessaire et n'a pas pu être installé.${NC}"
            exit 1
        }
    else
        echo -e "${GREEN}Paquet '${pkg_name}' est déjà installé.${NC}"
    fi
done

# Vérification spécifique du compilateur g++ après l'installation de build-essential
if ! command -v g++ &> /dev/null; then
  echo -e "${RED}Erreur : Le compilateur g++ n'est toujours pas disponible après l'installation de 'build-essential'.${NC}"
  echo -e "${YELLOW}Cela peut indiquer un problème avec votre installation Termux ou des dépôts. Impossible de compiler les modules C++.${NC}"
  echo -e "${YELLOW}Veuillez résoudre manuellement le problème d'installation de 'build-essential' et relancer le script.${NC}"
  exit 1
else
  echo -e "${GREEN}Compilateur g++ est maintenant disponible.${NC}"
fi

# Attribution des permissions d'exécution à la commande 'clear' si elle existe
echo -e "${BLUE}Attribution des permissions d'exécution à la commande 'clear'...${NC}"
if [ -f "/data/data/com.termux/files/usr/bin/clear" ]; then
    chmod +x /data/data/com.termux/files/usr/bin/clear
    echo -e "${GREEN}Permissions d'exécution accordées à '/data/data/com.termux/files/usr/bin/clear'.${NC}"
else
    echo -e "${YELLOW}Avertissement : La commande 'clear' n'a pas été trouvée à son emplacement habituel. Les permissions n'ont pas été modifiées.${NC}"
fi
echo ""

echo -e "${GREEN}Prérequis système vérifiés et installés si nécessaire.${NC}\n"

# --- Validation du Contenu du Dépôt ---
echo -e "${BLUE}Validation du contenu du dépôt '${REPO_PATH}'...${NC}"
if [ ! -d "$REPO_PATH" ]; then
  echo -e "${RED}Erreur : Le répertoire du dépôt '$REPO_NAME' n'existe pas à '${REPO_PATH}'. Vérifiez le chemin spécifié.${NC}"
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
echo -e "${BLUE}Création des dossiers cibles dans Termux (${INSTALL_DIR}, ${MODULES_TARGET_DIR}, ${WORDLISTS_TARGET_DIR})...${NC}"
mkdir -p "$INSTALL_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $INSTALL_DIR. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$MODULES_TARGET_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $MODULES_TARGET_DIR. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$WORDLISTS_TARGET_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $WORDLISTS_TARGET_DIR. Vérifiez les permissions.${NC}"; exit 1; }
echo -e "${GREEN}Dossiers cibles créés.${NC}\n"

# --- Copie des Fichiers Principaux ---
echo -e "${BLUE}Copie de 'hashish.py' et 'banner-hashish.txt' vers ${INSTALL_DIR}...${NC}"
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/hashish.py" || { echo -e "${RED}Erreur: Impossible de copier hashish.py. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
cp "$REPO_PATH/banner-hashish.txt" "$INSTALL_DIR/banner-hashish.txt" || { echo -e "${RED}Erreur: Impossible de copier banner-hashish.txt. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
chmod +x "$INSTALL_DIR/hashish.py" # Permissions d'exécution pour le script principal Python
chmod +r "$INSTALL_DIR/banner-hashish.txt" # Permissions de lecture pour la bannière
echo -e "${GREEN}Fichiers principaux copiés avec succès.${NC}\n"

# --- Copie des Modules Python ---
echo -e "${BLUE}Copie des modules Python depuis '$REPO_PATH/modules/' vers '$MODULES_TARGET_DIR/'...${NC}"
if command -v rsync &> /dev/null; then
    echo -e "${INFO}Utilisation de rsync pour copier les modules Python (hors wordlists et autres fichiers non pertinents)...${NC}"
    # rsync pour exclure les wordlists et ne copier que les .py et les sous-dossiers vides
    # Le --include='*/' permet de copier les sous-répertoires (importants pour l'organisation des modules)
    # Le --exclude='*' à la fin signifie que tout le reste (non explicitement inclus) est exclu.
    rsync -av --include='*.py' --include='*/' --exclude='wordlists/' --exclude='*' "$REPO_PATH/modules/" "$MODULES_TARGET_DIR/" || { echo -e "${YELLOW}Avertissement : Erreur lors de la copie des modules Python avec rsync. Vérifiez le dossier '$REPO_PATH/modules/'.${NC}"; }
else
    echo -e "${YELLOW}Avertissement : 'rsync' n'est pas installé. Copie des fichiers Python individuellement et des sous-dossiers (méthode de secours)...${NC}"
    # Fallback si rsync n'est pas disponible : copie les fichiers .py et les sous-dossiers (sauf 'wordlists')
    find "$REPO_PATH/modules/" -maxdepth 1 -name "*.py" -exec cp {} "$MODULES_TARGET_DIR/" \; 2>/dev/null || true

    for dir in "$REPO_PATH/modules"/*/; do
        dir_name=$(basename "$dir")
        if [ "$dir_name" != "wordlists" ]; then
            cp -r "$dir" "$MODULES_TARGET_DIR/" 2>/dev/null || true
        fi
    done
    echo -e "${YELLOW}Note : Pour une meilleure gestion des fichiers, il est recommandé d'installer 'rsync' (pkg install rsync).${NC}"
fi
# Attribution des permissions d'exécution à tous les scripts Python copiés dans le dossier des modules
find "$MODULES_TARGET_DIR" -maxdepth 1 -name "*.py" -exec chmod +x {} \; 2>/dev/null || true
echo -e "${GREEN}Modules Python copiés avec succès vers ${MODULES_TARGET_DIR}.${NC}\n"

# --- Copie des Wordlists ---
echo -e "${BLUE}Copie des wordlists par défaut depuis '$REPO_PATH/wordlists/' vers '$WORDLISTS_TARGET_DIR/'...${NC}"
if [ -d "$REPO_PATH/wordlists" ]; then
    # Utilisation de rsync si disponible pour une copie plus robuste et incrémentale
    if command -v rsync &> /dev/null; then
        rsync -av "$REPO_PATH/wordlists/" "$WORDLISTS_TARGET_DIR/" || { echo -e "${YELLOW}Avertissement : Aucun fichier de wordlist par défaut trouvé à copier ou erreur lors de la copie avec rsync.${NC}"; }
    else
        # Fallback pour la copie des wordlists
        cp -r "$REPO_PATH/wordlists/"* "$WORDLISTS_TARGET_DIR/" 2>/dev/null || { echo -e "${YELLOW}Avertissement : Aucun fichier de wordlist par défaut trouvé à copier ou erreur lors de la copie.${NC}"; }
    fi
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
        # Utilisation de sed pour corriger la fonction. On utilise un autre délimiteur '#' pour éviter les problèmes avec les '/'
        # On s'assure que le motif à remplacer est bien présent avant d'appliquer la correction.
        # \x0A est la représentation hexadécimale du caractère de nouvelle ligne (newline)
        if grep -q "std::seed_seq seed_sequence(hash.begin(), hash.end());" "$file"; then
            sed -i "s|std::string reduced_string = \"\";\x0A.*std::seed_seq seed_sequence(hash.begin(), hash.end());|std::string reduced_string = \"\";\x0A\
    std::vector<unsigned int> seed_data;\x0A\
    for (char c : hash) { seed_data.push_back(static_cast<unsigned int>(c)); }\x0A\
    seed_data.push_back(static_cast<unsigned int>(r_index));\x0A\x0A\
    std::seed_seq seed_sequence(seed_data.begin(), seed_data.end());|g" "$file"
            echo -e "${GREEN}Correction appliquée à $file.${NC}"
        else
            echo -e "${INFO}La correction de $file ne semble pas nécessaire (déjà appliquée ou motif non trouvé).${NC}"
        fi
    else
        echo -e "${YELLOW}Avertissement : Fichier C++ '$file' non trouvé. Aucune correction appliquée.${NC}"
    fi
done
echo -e "${GREEN}Correction des fichiers C++ terminée.${NC}\n"

# S'assurer que le dossier des modules source a les permissions d'écriture pour la compilation
echo -e "${BLUE}Vérification et attribution des permissions d'écriture pour le dossier des modules C++ source (${REPO_PATH}/modules)...${NC}"
if [ -d "$REPO_PATH/modules" ]; then
    # u+w donne les permissions d'écriture à l'utilisateur propriétaire du fichier/dossier
    chmod u+w "$REPO_PATH/modules" || { echo -e "${RED}Erreur : Impossible de donner les permissions d'écriture à $REPO_PATH/modules. Vérifiez si vous êtes propriétaire ou exécutez avec des privilèges suffisants.${NC}"; exit 1; }
    echo -e "${GREEN}Permissions d'écriture accordées à $REPO_PATH/modules.${NC}\n"
else
    echo -e "${RED}Erreur : Le dossier '$REPO_PATH/modules' n'existe pas. Impossible de définir les permissions pour la compilation.${NC}"
    exit 1
fi

# --- Compilation du Module C++ 'hashcracker.cpp' ---
HASHCRACKER_CPP_SOURCE="$REPO_PATH/modules/hashcracker.cpp"
HASHCRACKER_TEMP_EXECUTABLE="$REPO_PATH/modules/hashcracker_temp" # Exécutable temporaire avant déplacement
HASHCRACKER_FINAL_EXECUTABLE="$MODULES_TARGET_DIR/hashcracker" # Emplacement final de l'exécutable

echo -e "${BLUE}Vérification et compilation du module C++ 'hashcracker.cpp'...${NC}"

if [ -f "$HASHCRACKER_CPP_SOURCE" ]; then
  echo -e "${INFO}Fichier source C++ 'hashcracker.cpp' trouvé : $HASHCRACKER_CPP_SOURCE.${NC}"

  echo -e "${CYAN}Lancement de la compilation de $HASHCRACKER_CPP_SOURCE vers $HASHCRACKER_TEMP_EXECUTABLE...${NC}"

  # Commande de compilation complète avec chemins d'inclusion et de bibliothèque pour Termux
  COMPILATION_CMD="g++ \"$HASHCRACKER_CPP_SOURCE\" -o \"$HASHCRACKER_TEMP_EXECUTABLE\" -O3 -fopenmp -lssl -lcrypto -std=c++17 -Wall -pedantic -I/data/data/com.termux/files/usr/include -L/data/data/com.termux/files/usr/lib"
  echo -e "${CYAN}Commande de compilation exécutée : ${COMPILATION_CMD}${NC}"

  # Exécution de la commande de compilation
  if eval "$COMPILATION_CMD"; then # Utilisation de eval pour exécuter la chaîne de commande correctement
    echo -e "${GREEN}Module C++ hashcracker compilé avec succès vers : $HASHCRACKER_TEMP_EXECUTABLE${NC}"

    # Vérifie si le dossier cible des modules existe avant de déplacer
    if [ ! -d "$MODULES_TARGET_DIR" ]; then
        echo -e "${RED}Erreur: Le dossier cible des modules '$MODULES_TARGET_DIR' n'existe pas. Impossible de déplacer l'exécutable C++.${NC}"
        echo -e "${YELLOW}Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
        exit 1
    fi

    echo -e "${INFO}Déplacement de l'exécutable compilé vers son emplacement final : $HASHCRACKER_FINAL_EXECUTABLE${NC}"
    if mv "$HASHCRACKER_TEMP_EXECUTABLE" "$HASHCRACKER_FINAL_EXECUTABLE"; then
        echo -e "${GREEN}Exécutable C++ déplacé avec succès.${NC}"
        # Rend l'exécutable exécutable
        if [ -f "$HASHCRACKER_FINAL_EXECUTABLE" ]; then
            chmod +x "$HASHCRACKER_FINAL_EXECUTABLE"
            echo -e "${GREEN}Permissions d'exécution accordées à $HASHCRACKER_FINAL_EXECUTABLE.${NC}"
        else
            echo -e "${RED}Erreur: L'exécutable C++ n'a pas été trouvé après le déplacement. Problème de chemin ou de fichier manquant.${NC}"
            echo -e "${YELLOW}Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
            exit 1
        fi
    else
        echo -e "${RED}Erreur: Impossible de déplacer l'exécutable C++ vers $HASHCRACKER_FINAL_EXECUTABLE. Vérifiez les permissions du dossier cible ou l'espace disque.${NC}"
        echo -e "${YELLOW}Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
        exit 1
    fi

  else
    echo -e "${RED}------------------------------------------------------------------${NC}"
    echo -e "${RED}ERREUR CRITIQUE : Échec de la compilation de hashcracker.cpp.${NC}"
    echo -e "${YELLOW}Veuillez examiner attentivement les messages d'erreur de g++ ci-dessus pour le diagnostic.${NC}"
    echo -e "${YELLOW}Les causes possibles incluent des bibliothèques OpenSSL manquantes, des en-têtes non trouvés, ou des erreurs dans le code source C++ et sa compatibilité avec les versions d'OpenSSL de Termux.${NC}"
    echo -e "${RED}------------------------------------------------------------------${NC}"
    exit 1 # Arrête le script en cas d'échec de compilation critique
  fi
else
  echo -e "${YELLOW}Fichier source hashcracker.cpp non trouvé dans $HASHCRACKER_CPP_SOURCE. La compilation C++ est ignorée.${NC}"
  echo -e "${YELLOW}Le module Hash Cracker C++ ne sera PAS disponible.${NC}"
fi
echo ""

# --- Nettoyage de l'ancien exécutable rainbow_generator ---
RAINBOW_GENERATOR_OLD_EXECUTABLE="$MODULES_TARGET_DIR/rainbow_generator"
if [ -f "$RAINBOW_GENERATOR_OLD_EXECUTABLE" ]; then
    echo -e "${BLUE}Nettoyage de l'ancien exécutable rainbow_generator...${NC}"
    rm "$RAINBOW_GENERATOR_OLD_EXECUTABLE" || { echo -e "${YELLOW}Avertissement : Impossible de supprimer l'ancien exécutable rainbow_generator. Veuillez le supprimer manuellement si nécessaire.${NC}"; }
    echo -e "${GREEN}Ancien rainbow_generator supprimé.${NC}\n"
fi

# --- Vérification et Création du Fichier rainbow.txt ---
RAINBOW_TXT_PATH="$MODULES_TARGET_DIR/rainbow.txt"
echo -e "${BLUE}Vérification et création du fichier rainbow.txt...${NC}"
# Assurons-nous que le répertoire parent du fichier cible existe avant de le créer
mkdir -p "$(dirname "$RAINBOW_TXT_PATH")" || { echo -e "${RED}Erreur: Impossible de créer le répertoire pour rainbow.txt. Vérifiez les permissions.${NC}"; exit 1; }

if [ ! -f "$RAINBOW_TXT_PATH" ]; then
    touch "$RAINBOW_TXT_PATH" || { echo -e "${RED}Erreur: Impossible de créer le fichier rainbow.txt à $RAINBOW_TXT_PATH. Vérifiez les permissions ou l'espace disque.${NC}"; exit 1; }
    echo -e "${GREEN}Fichier rainbow.txt créé à $RAINBOW_TXT_PATH.${NC}\n"
else
    echo -e "${GREEN}Fichier rainbow.txt déjà existant à $RAINBOW_TXT_PATH.${NC}\n"
fi

# --- Attribution des Permissions aux Modules (Vérification finale et renforcement) ---
echo -e "${BLUE}Vérification et attribution des permissions finales aux modules...${NC}"

# Assurons-nous que hashish.py dans INSTALL_DIR a les bonnes permissions d'exécution
if [ -f "$INSTALL_DIR/hashish.py" ]; then
    chmod +x "$INSTALL_DIR/hashish.py"
    echo -e "${GREEN}Permissions d'exécution accordées à $INSTALL_DIR/hashish.py.${NC}"
else
    echo -e "${YELLOW}Avertissement : $INSTALL_DIR/hashish.py non trouvé pour les permissions finales. Le script principal pourrait ne pas s'exécuter.${NC}"
fi

# Assurons-nous que l'exécutable C++ a les bonnes permissions d'exécution
if [ -f "$MODULES_TARGET_DIR/hashcracker" ]; then
    chmod +x "$MODULES_TARGET_DIR/hashcracker"
    echo -e "${GREEN}Permissions d'exécution accordées à $MODULES_TARGET_DIR/hashcracker.${NC}"
else
    echo -e "${YELLOW}Avertissement : $MODULES_TARGET_DIR/hashcracker non trouvé pour les permissions finales. Le module C++ pourrait ne pas fonctionner.${NC}"
fi

# Pour les autres modules Python qui pourraient être dans le dossier des modules
find "$MODULES_TARGET_DIR" -maxdepth 1 -name "*.py" -exec chmod +x {} \; 2>/dev/null
echo -e "${GREEN}Permissions d'exécution vérifiées et accordées pour les modules Python dans $MODULES_TARGET_DIR.${NC}\n"

# --- Création d'un Script Exécutable Global ---
echo -e "${BLUE}Création d'un script exécutable global 'hashish' dans ${INSTALL_DIR}...${NC}"
cat > "$INSTALL_DIR/hashish" << EOF
#!/data/data/com.termux/files/usr/bin/bash
# Script de lancement pour HASHISH

# Fonction pour effacer l'écran
clear_screen_func() {
    if command -v clear &>/dev/null; then
        clear
    else
        printf '\033c'
    fi
}

clear_screen_func
# Exécute le script Python principal avec tous les arguments passés
exec python3 "$INSTALL_DIR/hashish.py" "\$@"
EOF
chmod +x "$INSTALL_DIR/hashish"
echo -e "${GREEN}Raccourci 'hashish' créé dans $INSTALL_DIR. Vous pouvez maintenant lancer l'outil simplement en tapant 'hashish'.${NC}\n"

# --- Installation des Dépendances Python ---
echo -e "${BLUE}Installation des dépendances Python listées dans $REPO_PATH/requirements.txt...${NC}"
if [ -f "$REPO_PATH/requirements.txt" ]; then
    # Vérifie si pip est installé, sinon tente de l'installer
    if ! command -v pip &> /dev/null; then
        echo -e "${YELLOW}pip n'est pas trouvé. Tentative d'installation de 'python-pip' via pkg...${NC}"
        install_package "python-pip" || {
            echo -e "${RED}Impossible d'installer pip via pkg. Veuillez l'installer manuellement (pkg install python-pip) et relancer le script.${NC}";
            # On ne sort pas ici, pour laisser l'utilisateur tenter une installation manuelle
        }
    fi

    # Si pip est disponible après les vérifications/tentatives d'installation
    if command -v pip &> /dev/null; then
        echo -e "${INFO}Installation des dépendances Python via pip...${NC}"
        if pip install -r "$REPO_PATH/requirements.txt"; then
            echo -e "${GREEN}Dépendances Python installées avec succès.${NC}\n"
        else
            echo -e "${RED}Erreur: Impossible d'installer les dépendances Python via pip.${NC}"
            echo -e "${YELLOW}Veuillez vérifier '$REPO_PATH/requirements.txt', votre connexion Internet, ou essayez 'pip install --upgrade pip'.${NC}"
            echo -e "${YELLOW}Vous pouvez essayer de les installer manuellement plus tard avec 'pip install -r $REPO_PATH/requirements.txt'.${NC}\n"
        fi
    else
        echo -e "${RED}Erreur: pip n'est toujours pas disponible. Impossible d'installer les dépendances Python.${NC}"
        echo -e "${YELLOW}Veuillez l'installer manuellement (pkg install python-pip) et les dépendances si nécessaire.${NC}\n"
    fi
else
    echo -e "${YELLOW}Fichier 'requirements.txt' introuvable dans le dépôt. Aucune dépendance Python spécifique à installer.${NC}\n"
fi

# --- Message de Fin d'Installation ---
echo -e "${CYAN}=====================================================${NC}"
echo -e "${GREEN}  Installation de HASHISH terminée avec succès ! 🚀 ${NC}"
echo -e "${CYAN}=====================================================${NC}\n"
echo -e "${GREEN}Vous pouvez maintenant lancer l'outil avec la commande : ${CYAN}hashish${NC}\n"
echo -e "${BLUE}Pour tester, lancement de HASHISH (Appuyez sur Ctrl+C pour quitter)...${NC}"

# Lance l'outil après l'installation
if command -v hashish &> /dev/null; then
  hashish
else
  echo -e "${YELLOW}Impossible de lancer 'hashish' automatiquement. Le raccourci pourrait ne pas être dans votre PATH ou une erreur précédente a bloqué sa création.${NC}"
  echo -e "${YELLOW}Commande manuelle alternative : ${CYAN}python3 $INSTALL_DIR/hashish.py${NC}"
fi

echo -e "${CYAN}Merci d'avoir installé HASHISH. Bon travail !${NC}\n"
exit 0 # Termine le script avec succès
