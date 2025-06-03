#!/bin/bash

GREEN='\033[0;32m'
NC='\033[0m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
INFO='\033[0;34m'

clear_screen() {
    if command -v clear &>/dev/null; then
        clear
    else
        printf '\033c'
    }
}

clear_screen

echo -e "${CYAN}=======================================${NC}"
echo -e "${CYAN}  Installation de HASHISH Ethical Toolkit${NC}"
echo -e "${CYAN}=======================================${NC}\n"

echo -e "${BLUE}Mise à jour et mise à niveau des paquets Termux...${NC}"
if pkg update -y && pkg upgrade -y; then
    echo -e "${GREEN}Termux paquets mis à jour et mis à niveau avec succès.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Impossible de mettre à jour/mettre à niveau les paquets Termux. Cela pourrait causer des problèmes de dépendances.${NC}"
    echo -e "${YELLOW}Veuillez vérifier votre connexion Internet et les dépôts, puis réessayez manuellement : 'pkg update && pkg upgrade'.${NC}\n"
fi

REPO_NAME="hashish"
DEFAULT_HOME_PATH="/data/data/com.termux/files/home"
DEFAULT_REPO_PATH="$DEFAULT_HOME_PATH/$REPO_NAME"

CURRENT_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_PATH=""

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

INSTALL_DIR="/data/data/com.termux/files/usr/bin"
MODULES_TARGET_DIR="$INSTALL_DIR/modules"
WORDLISTS_TARGET_DIR="$MODULES_TARGET_DIR/wordlists"

echo -e "${BLUE}Vérification et installation des prérequis système (build-essential, openssl, ncurses-utils)...${NC}"

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

# --- DÉBUT DE LA SECTION CORRIGÉE ---

# Liste des paquets essentiels pour Termux
# build-essential inclut g++ et make
# `libssl-dev` n'existe pas directement, `openssl` suffit pour la compilation dans Termux.
REQUIRED_PKGS=("clang" "build-essential" "openssl" "git" "python" "ncurses-utils" "rsync" "curl" "nmap" "whois" "dnsutils")

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

# --- FIN DE LA SECTION CORRIGÉE ---

echo -e "${BLUE}Attribution des permissions d'exécution à la commande 'clear'...${NC}"
if [ -f "/data/data/com.termux/files/usr/bin/clear" ]; then
    chmod +x /data/data/com.termux/files/usr/bin/clear
    echo -e "${GREEN}Permissions d'exécution accordées à '/data/data/com.termux/files/usr/bin/clear'.${NC}"
else
    echo -e "${YELLOW}Avertissement : La commande 'clear' n'a pas été trouvée à '/data/data/com.termux/files/usr/bin/clear'. Les permissions n'ont pas été modifiées.${NC}"
fi
echo ""

echo -e "${GREEN}Prérequis système vérifiés et installés si nécessaire.${NC}\n"

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
  echo -e "${RED}Erreur : Le dossier 'modules' est introuvable dans '$REPO_PATH'. Assurez-se que les modules sont bien dans un sous-dossier 'modules'.${NC}"
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

echo -e "${BLUE}Création des dossiers cibles dans Termux...${NC}"
mkdir -p "$INSTALL_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $INSTALL_DIR. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$MODULES_TARGET_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $MODULES_TARGET_DIR. Vérifiez les permissions.${NC}"; exit 1; }
mkdir -p "$WORDLISTS_TARGET_DIR" || { echo -e "${RED}Erreur: Impossible de créer le dossier $WORDLISTS_TARGET_DIR. Vérifiez les permissions.${NC}"; exit 1; }
echo -e "${GREEN}Dossiers cibles créés.${NC}\n"

echo -e "${BLUE}Copie de 'hashish.py' et 'banner-hashish.txt' vers ${INSTALL_DIR}...${NC}"
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/hashish.py" || { echo -e "${RED}Erreur: Impossible de copier hashish.py. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
cp "$REPO_PATH/banner-hashish.txt" "$INSTALL_DIR/banner-hashish.txt" || { echo -e "${RED}Erreur: Impossible de copier banner-hashish.txt. Vérifiez les permissions ou l'existence du fichier source.${NC}"; exit 1; }
chmod +x "$INSTALL_DIR/hashish.py"
chmod +r "$INSTALL_DIR/banner-hashish.txt"
echo -e "${GREEN}Fichiers principaux copiés avec succès.${NC}\n"

echo -e "${BLUE}Copie des modules Python depuis '$REPO_PATH/modules/' vers '$MODULES_TARGET_DIR/'...${NC}"
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

echo -e "${BLUE}Copie des wordlists par défaut depuis '$REPO_PATH/wordlists/' vers '$WORDLISTS_TARGET_DIR/'...${NC}"
if [ -d "$REPO_PATH/wordlists" ]; then
    cp -r "$REPO_PATH/wordlists/"* "$WORDLISTS_TARGET_DIR/" 2>/dev/null || { echo -e "${YELLOW}Avertissement : Aucun fichier de wordlist par défaut trouvé à copier ou erreur lors de la copie.${NC}"; }
    echo -e "${GREEN}Wordlists par défaut copiées avec succès vers ${WORDLISTS_TARGET_DIR}.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Le dossier des wordlists par défaut '$REPO_PATH/wordlists' est introuvable. Les wordlists par défaut ne seront pas installées.${NC}\n"
fi

echo -e "${BLUE}Pré-traitement : Correction de la fonction 'reduce_hash' dans les fichiers C++...${NC}"

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

HASHCRACKER_CPP_SOURCE="$REPO_PATH/modules/hashcracker.cpp"
HASHCRACKER_TEMP_EXECUTABLE="$REPO_PATH/modules/hashcracker_temp"
HASHCRACKER_FINAL_EXECUTABLE="$MODULES_TARGET_DIR/hashcracker"

echo -e "${BLUE}Vérification et compilation du module C++ 'hashcracker.cpp'...${NC}"

if [ -f "$HASHCRACKER_CPP_SOURCE" ]; then
  echo -e "${INFO}Fichier source C++ 'hashcracker.cpp' trouvé : $HASHCRACKER_CPP_SOURCE.${NC}"

  echo -e "${CYAN}Lancement de la compilation de $HASHCRACKER_CPP_SOURCE vers $HASHCRACKER_TEMP_EXECUTABLE avec les options pour Termux...${NC}"
  COMPILE_COMMAND="g++ \"$HASHCRACKER_CPP_SOURCE\" -o \"$HASHCRACKER_TEMP_EXECUTABLE\" -O3 -fopenmp -lssl -lcrypto -std=c++17 -Wall -pedantic"
  echo -e "${CYAN}Commande de compilation : ${COMPILE_COMMAND}${NC}"

  if $COMPILE_COMMAND; then
    echo -e "${GREEN}Module C++ hashcracker compilé avec succès vers : $HASHCRACKER_TEMP_EXECUTABLE${NC}"

    if [ ! -d "$MODULES_TARGET_DIR" ]; then
        echo -e "${RED}Erreur: Le dossier cible des modules '$MODULES_TARGET_DIR' n'existe pas. Impossible de déplacer l'exécutable C++.${NC}"
        echo -e "${YELLOW}Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
        exit 1
    fi

    echo -e "${INFO}Déplacement de l'exécutable compilé vers : $HASHCRACKER_FINAL_EXECUTABLE${NC}"
    if mv "$HASHCRACKER_TEMP_EXECUTABLE" "$HASHCRACKER_FINAL_EXECUTABLE"; then
        echo -e "${GREEN}Exécutable C++ déplacé avec succès.${NC}"
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
    echo -e "${YELLOW}Le module Hash Cracker C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
    echo -e "${RED}------------------------------------------------------------------${NC}"
    exit 1
  fi
else
  echo -e "${YELLOW}Fichier source hashcracker.cpp non trouvé dans $HASHCRACKER_CPP_SOURCE. La compilation C++ est ignorée.${NC}"
  echo -e "${YELLOW}Le module Hash Cracker C++ ne sera PAS disponible.${NC}"
fi
echo ""

RAINBOW_GENERATOR_OLD_EXECUTABLE="$MODULES_TARGET_DIR/rainbow_generator"
if [ -f "$RAINBOW_GENERATOR_OLD_EXECUTABLE" ]; then
    echo -e "${BLUE}Nettoyage de l'ancien exécutable rainbow_generator...${NC}"
    rm "$RAINBOW_GENERATOR_OLD_EXECUTABLE" || { echo -e "${YELLOW}Avertissement : Impossible de supprimer l'ancien rainbow_generator.cpp exécutable. Veuillez le supprimer manuellement si nécessaire.${NC}"; }
    echo -e "${GREEN}Ancien rainbow_generator supprimé.${NC}\n"
fi

RAINBOW_TXT_PATH="$MODULES_TARGET_DIR/rainbow.txt"
echo -e "${BLUE}Vérification et création du fichier rainbow.txt...${NC}"
if [ ! -f "$RAINBOW_TXT_PATH" ]; then
    touch "$RAINBOW_TXT_PATH" || { echo -e "${RED}Erreur: Impossible de créer le fichier rainbow.txt à $RAINBOW_TXT_PATH. Vérifiez les permissions.${NC}"; }
    echo -e "${GREEN}Fichier rainbow.txt créé (ou déjà existant) à $RAINBOW_TXT_PATH.${NC}\n"
else
    echo -e "${GREEN}Fichier rainbow.txt déjà existant à $RAINBOW_TXT_PATH.${NC}\n"
fi

echo -e "${BLUE}Attribution des permissions aux modules...${NC}"
chmod +x "$REPO_PATH/hashish.py"
chmod +x "$REPO_PATH/modules/hash_recon.cpp" 2>/dev/null || true
chmod +x "$REPO_PATH/modules/hashcracker.cpp" 2>/dev/null || true
chmod +x "$REPO_PATH/modules/rainbow_generator.cpp" 2>/dev/null || true
chmod +x "$REPO_PATH/modules/web_scanner.py" 2>/dev/null || true
chmod +x "$REPO_PATH/modules/osint.py" 2>/dev/null || true
chmod +x "$REPO_PATH/modules/recon.py" 2>/dev/null || true
chmod +r "$REPO_PATH/modules/hash_recon.h" 2>/dev/null || true
echo -e "${GREEN}Permissions accordées aux fichiers spécifiés dans le REPO_PATH.${NC}\n"
echo -e "${GREEN}Note: Les permissions pour les fichiers copiés dans $INSTALL_DIR et $MODULES_TARGET_DIR sont définies séparément.${NC}\n"

echo -e "${BLUE}Création d'un script exécutable global...${NC}"
cat > "$INSTALL_DIR/hashish" << EOF
#!/data/data/com.termux/files/usr/bin/bash
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
echo -e "${GREEN}Raccourci 'hashish' créé dans $INSTALL_DIR. Vous pouvez maintenant lancer l'outil simplement en tapant 'hashish'.${NC}\n"

echo -e "${BLUE}Installation des dépendances Python listées dans $REPO_PATH/requirements.txt...${NC}"
if [ -f "$REPO_PATH/requirements.txt" ]; then
    if ! command -v pip &> /dev/null; then
        echo -e "${YELLOW}pip n'est pas trouvé. Tentative d'installation de 'python-pip'...${NC}"
        install_package "python-pip" || { echo -e "${RED}Impossible d'installer pip. Veuillez l'installer manuellement (pkg install python-pip).${NC}"; }
    fi

    if command -v pip &> /dev/null; then
        if pip install -r "$REPO_PATH/requirements.txt"; then
            echo -e "${GREEN}Dépendances Python installées avec succès.${NC}\n"
        else
            echo -e "${RED}Erreur: Impossible d'installer les dépendances Python.${NC}"
            echo -e "${YELLOW}Veuillez vérifier $REPO_PATH/requirements.txt, votre connexion Internet, ou essayez 'pip install --upgrade pip'.${NC}"
            echo -e "${YELLOW}Vous pouvez essayer de les installer manuellement plus tard avec 'pip install -r $REPO_PATH/requirements.txt'.${NC}\n"
        fi
    else
        echo -e "${RED}Erreur: pip n'est pas disponible. Impossible d'installer les dépendances Python.${NC}"
        echo -e "${YELLOW}Veuillez l'installer manuellement et les dépendances si nécessaire.${NC}\n"
    fi
else
    echo -e "${YELLOW}Fichier 'requirements.txt' introuvable. Aucune dépendance Python à installer.${NC}\n"
fi

echo -e "${CYAN}=====================================================${NC}"
echo -e "${GREEN}  Installation de HASHISH terminée avec succès ! 🚀 ${NC}"
echo -e "${CYAN}=====================================================${NC}\n"
echo -e "${GREEN}Vous pouvez maintenant lancer l'outil avec la commande : ${CYAN}hashish${NC}\n"
echo -e "${BLUE}Pour tester, lancement de HASHISH (Appuyez sur Ctrl+C pour quitter)...${NC}"

if command -v hashish &> /dev/null; then
  hashish
else
  echo -e "${YELLOW}Impossible de lancer 'hashish' automatiquement. Le raccourci pourrait ne pas être dans votre PATH ou une erreur précédente a bloqué sa création.${NC}"
  echo -e "${YELLOW}Commande manuelle : ${CYAN}python3 $INSTALL_DIR/hashish.py${NC}"
fi

echo -e "${CYAN}Merci d'avoir installé HASHISH. Bon travail !${NC}\n"
exit 0
