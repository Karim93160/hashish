#!/bin/bash

# Définition des codes couleurs
GREEN='\033[0;32m'
NC='\033[0m'      # Reset
YELLOW='\033[0;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
INFO='\033[0;34m' # Alias de BLUE pour les infos

# --- Variables de contrôle ---
AUTO_INSTALL=false # Nouvelle variable pour contrôler l'installation automatique

# --- Gestion des arguments de ligne de commande ---
for arg in "$@"; do
    case "$arg" in
        --auto)
            AUTO_INSTALL=true
            ;;
        # Ajoutez d'autres options si nécessaire
    esac
done

# --- Fonctions Utilitaires ---

# Fonction pour effacer l'écran
clear_screen() {
    command -v clear &>/dev/null && clear || printf '\033c'
}

# Fonction pour installer un paquet Termux
install_package() {
    local package_name=$1
    echo -e "${INFO}Tentative d'installation du paquet Termux : ${package_name}...${NC}"
    if pkg install "$package_name" -y; then
        echo -e "${GREEN}Paquet '${package_name}' installé avec succès.${NC}"
        return 0
    else
        echo -e "${RED}Échec de l'installation du paquet '${package_name}'. Vérifiez votre connexion ou les dépôts Termux.${NC}"
        return 1
    fi
}

# --- Début de l'Installation ---
clear_screen

# Bannière de Bienvenue
echo -e "${CYAN}=======================================${NC}"
echo -e "${CYAN}  Installation de HASHISH Ethical Toolkit${NC}"
echo -e "${CYAN}=======================================${NC}\n"

# --- Mise à Jour et Mise à Niveau de Termux ---
echo -e "${BLUE}Mise à jour et mise à niveau des paquets Termux...${NC}"
if pkg update -y && pkg upgrade -y; then
    echo -e "${GREEN}Termux paquets mis à jour et mis à niveau avec succès.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Échec de la mise à jour/mise à niveau. Cela pourrait causer des problèmes de dépendances.${NC}"
    echo -e "${YELLOW}Vérifiez votre connexion et les dépôts, puis réessayez : 'pkg update && pkg upgrade'.${NC}\n"
fi

# --- Détection du Chemin du Dépôt ---
REPO_NAME="hashish"
DEFAULT_HOME_PATH="/data/data/com.termux/files/home"
DEFAULT_REPO_PATH="$DEFAULT_HOME_PATH/$REPO_NAME"
CURRENT_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
REPO_PATH=""

echo -e "${BLUE}Détection du chemin du dépôt '${REPO_NAME}'...${NC}"

# Tente de trouver le dépôt en remontant l'arborescence ou à l'emplacement par défaut
temp_dir="$CURRENT_SCRIPT_DIR"
while [[ "$temp_dir" != "/" && "$temp_dir" != "" ]]; do
    if [[ "$(basename "$temp_dir")" == "$REPO_NAME" ]]; then
        REPO_PATH="$temp_dir"
        break
    fi
    temp_dir=$(dirname "$temp_dir")
done

if [ -z "$REPO_PATH" ] && [ -d "$DEFAULT_REPO_PATH" ]; then
    REPO_PATH="$DEFAULT_REPO_PATH"
    echo -e "${INFO}Dépôt '${REPO_NAME}' détecté à l'emplacement par défaut : ${REPO_PATH}${NC}"
fi

# Si le dépôt n'est toujours pas trouvé, demande à l'utilisateur ou utilise le chemin par défaut en mode auto
if [ -z "$REPO_PATH" ]; then
    if [ "$AUTO_INSTALL" = true ]; then
        echo -e "${YELLOW}Mode automatique détecté. Tentative d'utilisation du chemin par défaut si le dépôt n'est pas trouvé.${NC}"
        if [ -d "$DEFAULT_REPO_PATH" ]; then
            REPO_PATH="$DEFAULT_REPO_PATH"
            echo -e "${GREEN}Chemin du dépôt '${REPO_NAME}' défini automatiquement à l'emplacement par défaut : ${REPO_PATH}${NC}"
        else
            echo -e "${RED}Erreur : Le répertoire '$REPO_NAME' est introuvable ni à l'emplacement actuel ni à l'emplacement par défaut (${DEFAULT_REPO_PATH}).${NC}"
            echo -e "${RED}L'installation automatique ne peut pas continuer sans un chemin de dépôt valide.${NC}"
            exit 1
        fi
    else
        echo -e "${RED}Erreur : Le répertoire '$REPO_NAME' est introuvable ni à l'emplacement actuel ni à l'emplacement par défaut (${DEFAULT_REPO_PATH}).${NC}"
        read -p "Voulez-vous entrer le chemin **complet** du dossier '$REPO_NAME' manuellement ? (o/n) : " confirm
        if [[ "$confirm" =~ ^[oO]$ ]]; then
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
    fi
else
    echo -e "${INFO}Dépôt '${REPO_NAME}' détecté à : ${REPO_PATH}${NC}\n"
fi

# Définition des répertoires d'installation cibles
INSTALL_DIR="/data/data/com.termux/files/usr/bin"
MODULES_TARGET_DIR="$INSTALL_DIR/modules"
WORDLISTS_TARGET_DIR="$MODULES_TARGET_DIR/wordlists"

# --- Vérification et Installation des Prérequis Système ---
echo -e "${BLUE}Vérification et installation des prérequis système (clang, openssl, git, python, etc.)...${NC}"

# Liste des paquets essentiels pour Termux
# Suppression de 'libomp' de cette liste
REQUIRED_PKGS=("clang" "openssl" "openssl-tool" "git" "python" "ncurses-utils" "rsync" "curl" "nmap" "whois" "dnsutils")

for pkg_name in "${REQUIRED_PKGS[@]}"; do
    if ! dpkg -s "$pkg_name" &>/dev/null; then
        echo -e "${YELLOW}Paquet '${pkg_name}' non trouvé. Installation...${NC}"
        install_package "$pkg_name" || {
            echo -e "${RED}Installation annulée. Le paquet '${pkg_name}' est nécessaire et n'a pas pu être installé.${NC}"
            exit 1
        }
    else
        echo -e "${GREEN}Paquet '${pkg_name}' est déjà installé.${NC}"
    fi
done

# Vérification spécifique du compilateur (clang++ est l'équivalent de g++ sur Termux)
if ! command -v clang++ &> /dev/null; then
  echo -e "${RED}Erreur : Le compilateur clang++ n'est pas disponible. Impossible de compiler les modules C++.${NC}"
  echo -e "${YELLOW}Veuillez vérifier manuellement l'installation de 'clang' et relancer.${NC}"
  exit 1
else
  echo -e "${GREEN}Compilateur clang++ est maintenant disponible.${NC}"
fi

# Attribution des permissions d'exécution à la commande 'clear' si elle existe (pour la robustesse du lanceur)
if [ -f "/data/data/com.termux/files/usr/bin/clear" ]; then
    chmod +x /data/data/com.termux/files/usr/bin/clear &>/dev/null
    echo -e "${GREEN}Permissions d'exécution accordées à 'clear'.${NC}"
else
    echo -e "${YELLOW}Avertissement : Commande 'clear' non trouvée, permissions non modifiées.${NC}"
fi
echo -e "${GREEN}Prérequis système vérifiés et installés si nécessaire.${NC}\n"

# --- Validation du Contenu du Dépôt ---
echo -e "${BLUE}Validation du contenu du dépôt '${REPO_PATH}'...${NC}"
if [ ! -d "$REPO_PATH" ] || [ ! -f "$REPO_PATH/hashish.py" ] || \
   [ ! -d "$REPO_PATH/modules" ] || [ ! -f "$REPO_PATH/banner-hashish.txt" ]; then
    echo -e "${RED}Erreur : Contenu du dépôt '$REPO_NAME' incomplet ou invalide à '${REPO_PATH}'.${NC}"
    echo -e "${RED}Assurez-vous que 'hashish.py', 'modules/' et 'banner-hashish.txt' existent à la racine du dépôt.${NC}"
    exit 1
fi
[ ! -f "$REPO_PATH/requirements.txt" ] && echo -e "${YELLOW}Avertissement : 'requirements.txt' introuvable. Aucune dépendance Python via ce fichier.${NC}"
[ ! -d "$REPO_PATH/wordlists" ] && echo -e "${YELLOW}Avertissement : Dossier 'wordlists' introuvable. Les wordlists par défaut ne seront pas installées.${NC}"
echo -e "${GREEN}Dépôt '$REPO_PATH' validé.${NC}\n"

# --- Création des Dossiers Cibles ---
echo -e "${BLUE}Création des dossiers cibles dans Termux (${INSTALL_DIR}, ${MODULES_TARGET_DIR}, ${WORDLISTS_TARGET_DIR})...${NC}"
mkdir -p "$INSTALL_DIR" "$MODULES_TARGET_DIR" "$WORDLISTS_TARGET_DIR" || {
    echo -e "${RED}Erreur : Impossible de créer les dossiers cibles. Vérifiez les permissions.${NC}"; exit 1; }
echo -e "${GREEN}Dossiers cibles créés.${NC}\n"

# --- Copie des Fichiers Principaux ---
echo -e "${BLUE}Copie de 'hashish.py' et 'banner-hashish.txt' vers ${INSTALL_DIR}...${NC}"
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/hashish.py" || { echo -e "${RED}Erreur: Impossible de copier hashish.py.${NC}"; exit 1; }
cp "$REPO_PATH/banner-hashish.txt" "$INSTALL_DIR/banner-hashish.txt" || { echo -e "${RED}Erreur: Impossible de copier banner-hashish.txt.${NC}"; exit 1; }
chmod +x "$INSTALL_DIR/hashish.py"
chmod +r "$INSTALL_DIR/banner-hashish.txt"
echo -e "${GREEN}Fichiers principaux copiés avec succès.${NC}\n"

# --- Copie des Modules Python ---
echo -e "${BLUE}Copie des modules Python depuis '$REPO_PATH/modules/' vers '$MODULES_TARGET_DIR/'...${NC}"
if command -v rsync &> /dev/null; then
    echo -e "${INFO}Utilisation de rsync pour copier les modules Python (hors wordlists et autres fichiers non pertinents)...${NC}"
    rsync -av --include='*.py' --include='*/' --exclude='wordlists/' --exclude='*.cpp' --exclude='*.o' --exclude='*' "$REPO_PATH/modules/" "$MODULES_TARGET_DIR/" || \
        echo -e "${YELLOW}Avertissement : Erreur lors de la copie des modules Python avec rsync.${NC}"
else
    echo -e "${YELLOW}Avertissement : 'rsync' non installé. Copie des fichiers Python individuellement (méthode de secours).${NC}"
    # Plus robuste pour les fichiers uniques, évite les erreurs de dossier
    find "$REPO_PATH/modules/" -maxdepth 1 -name "*.py" -exec cp {} "$MODULES_TARGET_DIR/" \; 2>/dev/null
    echo -e "${YELLOW}Note : Pour une meilleure gestion des fichiers, installez 'rsync' (pkg install rsync).${NC}"
fi
find "$MODULES_TARGET_DIR" -maxdepth 1 -name "*.py" -exec chmod +x {} \; 2>/dev/null
echo -e "${GREEN}Modules Python copiés avec succès vers ${MODULES_TARGET_DIR}.${NC}\n"

# --- Copie des Wordlists ---
echo -e "${BLUE}Copie des wordlists par défaut depuis '$REPO_PATH/wordlists/' vers '$WORDLISTS_TARGET_DIR/'...${NC}"
if [ -d "$REPO_PATH/wordlists" ]; then
    if command -v rsync &> /dev/null; then
        rsync -av "$REPO_PATH/wordlists/" "$WORDLISTS_TARGET_DIR/" || \
            echo -e "${YELLOW}Avertissement : Aucun fichier de wordlist par défaut trouvé ou erreur lors de la copie avec rsync.${NC}"
    else
        cp -r "$REPO_PATH/wordlists/"* "$WORDLISTS_TARGET_DIR/" 2>/dev/null || \
            echo -e "${YELLOW}Avertissement : Aucun fichier de wordlist par défaut trouvé ou erreur lors de la copie.${NC}"
    fi
    echo -e "${GREEN}Wordlists par défaut copiées avec succès vers ${WORDLISTS_TARGET_DIR}.${NC}\n"
else
    echo -e "${YELLOW}Avertissement : Le dossier des wordlists par défaut '$REPO_PATH/wordlists' est introuvable. Les wordlists par défaut ne seront pas installées.${NC}\n"
fi

# --- Pré-traitement : Correction des fichiers C++ pour Termux ---
echo -e "${BLUE}Pré-traitement : Correction des fonctions 'reduce_hash' et PATH_MAX dans les fichiers C++...${NC}"

CPP_SOURCE_FILES=("$REPO_PATH/modules/hashcracker.cpp" "$REPO_PATH/modules/rainbow_generator.cpp")
# Pattern pour la correction de reduce_hash
# Utilise un délimiteur différent pour sed (e.g., #) pour éviter les problèmes avec les slashes et les accolades
REDUCTION_SED_PATTERN='std::string reduced_string = "";\\n\
    std::vector<unsigned int> seed_data;\\n\
    for (char c : hash) { seed_data.push_back(static_cast<unsigned int>(c)); }\\n\
    seed_data.push_back(static_cast<unsigned int>(r_index));\\n\\n\
    std::seed_seq seed_sequence(seed_data.begin(), seed_data.end());'

for file in "${CPP_SOURCE_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo -e "${INFO}Traitement de $file...${NC}"
        # Correction de la fonction 'reduce_hash'
        if grep -q "std::seed_seq seed_sequence(hash.begin(), hash.end());" "$file"; then
             sed -i "s#std::seed_seq seed_sequence(hash.begin(), hash.end());#${REDUCTION_SED_PATTERN}#g" "$file"
             echo -e "${GREEN}Correction de 'reduce_hash' appliquée à $file.${NC}"
        else
            echo -e "${INFO}Correction de 'reduce_hash' non nécessaire (motif non trouvé ou déjà appliqué) dans $file.${NC}"
        fi

        # Ajout de PATH_MAX si non présent (utile pour des fonctions comme realpath)
        if ! grep -q "#define PATH_MAX" "$file"; then
            # Insère au début du fichier
            sed -i '1s/^/#ifndef PATH_MAX\n#define PATH_MAX 4096\n#endif\n\n/' "$file"
            echo -e "${GREEN}Ajout de #define PATH_MAX à $file.${NC}"
        else
            echo -e "${INFO}PATH_MAX déjà défini dans $file.${NC}"
        fi
    else
        echo -e "${YELLOW}Avertissement : Fichier C++ source '$file' non trouvé. Aucune correction appliquée.${NC}"
    fi
done
echo -e "${GREEN}Correction des fichiers C++ terminée.${NC}\n"

# S'assurer que le dossier des modules source a les permissions d'écriture pour la compilation
echo -e "${BLUE}Vérification et attribution des permissions d'écriture pour le dossier des modules C++ source (${REPO_PATH}/modules)...${NC}"
if [ -d "$REPO_PATH/modules" ]; then
    chmod u+w "$REPO_PATH/modules" || { echo -e "${RED}Erreur : Impossible de donner les permissions d'écriture à $REPO_PATH/modules. Vérifiez les privilèges.${NC}"; exit 1; }
    echo -e "${GREEN}Permissions d'écriture accordées à $REPO_PATH/modules.${NC}\n"
else
    echo -e "${RED}Erreur : Le dossier '$REPO_PATH/modules' n'existe pas. Impossible de définir les permissions pour la compilation.${NC}"
    exit 1
fi

# --- Compilation des Modules C++ ---

compile_cpp_module() {
    local source_file=$1
    local temp_executable=$2
    local final_executable=$3
    local module_name=$(basename "$source_file" .cpp)

    # Flags de compilation spécifiques à Termux avec clang++
    local base_compilation_flags="-O3 -std=c++17 -Wall -pedantic"
    local includes_libs="-I/data/data/com.termux/files/usr/include -L/data/data/com.termux/files/usr/lib"
    local common_link_libs="-lssl -lcrypto -lpthread -lm"

    local compilation_flags="$base_compilation_flags $includes_libs $common_link_libs"

    # Ajout de -fopenmp si le module l'utilise (par exemple hashcracker.cpp)
    # Note: La prise en charge d'OpenMP est gérée par clang et n'a pas besoin d'une libomp séparée sur Termux.
    if [[ "$module_name" == "hashcracker" ]]; then
        compilation_flags+=" -lstdc++fs -fopenmp"
    fi

    echo -e "${BLUE}Vérification et compilation du module C++ '${module_name}.cpp'...${NC}"
    if [ -f "$source_file" ]; then
        echo -e "${INFO}Fichier source C++ '${module_name}.cpp' trouvé : $source_file.${NC}"
        echo -e "${CYAN}Lancement de la compilation de $source_file vers $temp_executable...${NC}"
        local compilation_cmd="clang++ \"$source_file\" -o \"$temp_executable\" ${compilation_flags}"
        echo -e "${CYAN}Commande de compilation exécutée : ${compilation_cmd}${NC}"

        if eval "$compilation_cmd"; then
            echo -e "${GREEN}Module C++ ${module_name} compilé avec succès vers : $temp_executable${NC}"

            if [ ! -d "$MODULES_TARGET_DIR" ]; then
                echo -e "${RED}Erreur : Le dossier cible des modules '$MODULES_TARGET_DIR' n'existe pas. Impossible de déplacer l'exécutable C++.${NC}"
                echo -e "${YELLOW}Le module ${module_name} C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
                return 1 # Échec de compilation
            fi

            echo -e "${INFO}Déplacement de l'exécutable compilé vers son emplacement final : $final_executable${NC}"
            if mv "$temp_executable" "$final_executable"; then
                echo -e "${GREEN}Exécutable C++ déplacé avec succès.${NC}"
                if [ -f "$final_executable" ]; then
                    chmod +x "$final_executable"
                    echo -e "${GREEN}Permissions d'exécution accordées à $final_executable.${NC}"
                    return 0 # Succès
                else
                    echo -e "${RED}Erreur : L'exécutable C++ n'a pas été trouvé après le déplacement. Problème de chemin ou de fichier manquant.${NC}"
                    echo -e "${YELLOW}Le module ${module_name} C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
                    return 1 # Échec de compilation
                fi
            else
                echo -e "${RED}Erreur : Impossible de déplacer l'exécutable C++ vers $final_executable. Vérifiez les permissions ou l'espace disque.${NC}"
                echo -e "${YELLOW}Le module ${module_name} C++ ne sera PAS disponible ou ne fonctionnera pas correctement.${NC}"
                return 1 # Échec de compilation
            fi
        else
            echo -e "${RED}------------------------------------------------------------------${NC}"
            echo -e "${RED}ERREUR CRITIQUE : Échec de la compilation de ${module_name}.cpp.${NC}"
            echo -e "${YELLOW}Veuillez examiner attentivement les messages d'erreur de clang++ ci-dessus pour le diagnostic.${NC}"
            echo -e "${YELLOW}Les causes possibles incluent des bibliothèques OpenSSL manquantes, des en-têtes non trouvés, ou des erreurs dans le code source C++ et sa compatibilité avec les versions d'OpenSSL de Termux.${NC}"
            echo -e "${RED}------------------------------------------------------------------${NC}"
            return 1 # Échec de compilation
        fi
    else
        echo -e "${YELLOW}Fichier source ${module_name}.cpp non trouvé dans $source_file. La compilation C++ est ignorée.${NC}"
        echo -e "${YELLOW}Le module ${module_name} C++ ne sera PAS disponible.${NC}"
        return 1 # Fichier source non trouvé
    fi
    echo "" # Nouvelle ligne pour la clarté
}

# Appel de la fonction de compilation pour chaque module
compile_cpp_module "$REPO_PATH/modules/hashcracker.cpp" "$REPO_PATH/modules/hashcracker_temp" "$MODULES_TARGET_DIR/hashcracker"
compile_cpp_module "$REPO_PATH/modules/rainbow_generator.cpp" "$REPO_PATH/modules/rainbow_generator_temp" "$MODULES_TARGET_DIR/rainbow_generator"

# --- Nettoyage des exécutables temporaires C++ ---
echo -e "${BLUE}Nettoyage des exécutables C++ temporaires...${NC}"
rm -f "$REPO_PATH/modules/hashcracker_temp" "$REPO_PATH/modules/rainbow_generator_temp"
echo -e "${GREEN}Nettoyage terminé.${NC}\n"

# --- Vérification et Création du Fichier rainbow.txt ---
RAINBOW_TXT_PATH="$MODULES_TARGET_DIR/rainbow.txt"
echo -e "${BLUE}Vérification et création du fichier rainbow.txt...${NC}"
mkdir -p "$(dirname "$RAINBOW_TXT_PATH")" || { echo -e "${RED}Erreur: Impossible de créer le répertoire pour rainbow.txt.${NC}"; exit 1; }
if [ ! -f "$RAINBOW_TXT_PATH" ]; then
    touch "$RAINBOW_TXT_PATH" || { echo -e "${RED}Erreur: Impossible de créer le fichier rainbow.txt.${NC}"; exit 1; }
    echo -e "${GREEN}Fichier rainbow.txt créé à $RAINBOW_TXT_PATH.${NC}\n"
else
    echo -e "${GREEN}Fichier rainbow.txt déjà existant à $RAINBOW_TXT_PATH.${NC}\n"
fi

# --- Attribution des Permissions aux Modules (Vérification finale et renforcement) ---
echo -e "${BLUE}Vérification et attribution des permissions finales aux modules...${NC}"
# Assurons-nous que hashish.py et les exécutables C++ ont les bonnes permissions
chmod +x "$INSTALL_DIR/hashish.py" &>/dev/null || echo -e "${YELLOW}Avertissement : $INSTALL_DIR/hashish.py non trouvé ou impossible de donner les permissions.${NC}"
chmod +x "$MODULES_TARGET_DIR/hashcracker" &>/dev/null || echo -e "${YELLOW}Avertissement : $MODULES_TARGET_DIR/hashcracker non trouvé ou impossible de donner les permissions.${NC}"
chmod +x "$MODULES_TARGET_DIR/rainbow_generator" &>/dev/null || echo -e "${YELLOW}Avertissement : $MODULES_TARGET_DIR/rainbow_generator non trouvé ou impossible de donner les permissions.${NC}"
find "$MODULES_TARGET_DIR" -maxdepth 1 -name "*.py" -exec chmod +x {} \; 2>/dev/null
echo -e "${GREEN}Permissions d'exécution vérifiées et accordées pour les modules.${NC}\n"

# --- Création d'un Script Exécutable Global ---
echo -e "${BLUE}Création d'un script exécutable global 'hashish' dans ${INSTALL_DIR}...${NC}"
cat > "$INSTALL_DIR/hashish" << EOF
#!/data/data/com.termux/files/usr/bin/bash
clear_screen_func() {
    command -v clear &>/dev/null && clear || printf '\033c'
}
clear_screen_func
exec python3 "$INSTALL_DIR/hashish.py" "\$@"
EOF
chmod +x "$INSTALL_DIR/hashish"
echo -e "${GREEN}Raccourci 'hashish' créé dans $INSTALL_DIR. Vous pouvez maintenant lancer l'outil avec 'hashish'.${NC}\n"

# --- Installation des Dépendances Python ---
echo -e "${BLUE}Installation des dépendances Python listées dans $REPO_PATH/requirements.txt...${NC}"
if [ -f "$REPO_PATH/requirements.txt" ]; then
    if ! command -v pip &> /dev/null; then
        echo -e "${YELLOW}pip non trouvé. Tentative d'installation de 'python-pip' via pkg...${NC}"
        install_package "python-pip" || echo -e "${RED}Impossible d'installer pip. Installez-le manuellement (pkg install python-pip) et relancez.${NC}"
    fi

    if command -v pip &> /dev/null; then
        echo -e "${INFO}Installation des dépendances Python via pip...${NC}"
        if pip install -r "$REPO_PATH/requirements.txt"; then
            echo -e "${GREEN}Dépendances Python installées avec succès.${NC}\n"
        else
            echo -e "${RED}Erreur : Impossible d'installer les dépendances Python via pip.${NC}"
            echo -e "${YELLOW}Vérifiez '$REPO_PATH/requirements.txt', votre connexion Internet, ou essayez 'pip install --upgrade pip'.${NC}\n"
        fi
    else
        echo -e "${RED}Erreur : pip n'est toujours pas disponible. Impossible d'installer les dépendances Python.${NC}"
        echo -e "${YELLOW}Installez-le manuellement (pkg install python-pip) et les dépendances si nécessaire.${NC}\n"
    fi
else
    echo -e "${YELLOW}Fichier 'requirements.txt' introuvable. Aucune dépendance Python spécifique à installer.${NC}\n"
fi

# --- Message de Fin d'Installation ---
echo -e "${CYAN}=====================================================${NC}"
echo -e "${GREEN}  Installation de HASHISH terminée avec succès ! 🚀 ${NC}"
echo -e "${CYAN}=====================================================${NC}\n"
echo -e "${GREEN}Vous pouvez maintenant lancer l'outil avec la commande : ${CYAN}hashish${NC}\n"
echo -e "${BLUE}Pour tester, lancement de HASHISH (Appuyez sur Ctrl+C pour quitter)...${NC}"

if command -v hashish &> /dev/null; then
  hashish
else
  echo -e "${YELLOW}Impossible de lancer 'hashish' automatiquement. Le raccourci pourrait être manquant ou une erreur s'est produite.${NC}"
  echo -e "${YELLOW}Commande manuelle alternative : ${CYAN}python3 $INSTALL_DIR/hashish.py${NC}"
fi

echo -e "${CYAN}Merci d'avoir installé HASHISH. Bon travail !${NC}\n"
exit 0
