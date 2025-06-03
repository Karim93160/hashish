#!/bin/bash

# Couleurs et effets
R='\033[1;31m'  # Rouge
G='\033[1;32m'  # Vert
Y='\033[1;33m'  # Jaune
B='\033[1;34m'  # Bleu
M='\033[1;35m'  # Magenta
C='\033[1;36m'  # Cyan
W='\033[1;37m'  # Blanc
BL='\033[1;30m' # Noir (gris foncé)
NC='\033[0m'    # Reset

# Effets spéciaux
BOLD='\033[1m'
BLINK='\033[5m'
REVERSE='\033[7m'

# Animation
spinner() {
    local pid=$1
    local delay=0.1
    local spinstr='|/-\'
    while [ "$(ps a | awk '{print $1}' | grep $pid)" ]; do
        local temp=${spinstr#?}
        printf " [%c] " "$spinstr"
        local spinstr=$temp${spinstr%"$temp"}
        sleep $delay
        printf "\b\b\b\b\b"
    done
    printf "    \b\b\b\b"
}

# Fonction pour afficher un message avec style
pretty_print() {
    local color=$1
    local effect=$2
    local msg=$3
    echo -e "${effect}${color}$msg${NC}"
}

# Effacer l'écran
clear

# Bannière ASCII animée avec plus de couleurs
echo -e "${C}${BOLD}"
cat << "EOF"
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣘⡘⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠇⡇⢱⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠳⡢⣄⠀⠀⠀⠀⠀⠀⢸⠄⡇⠸⠀⠀⠀⠀⠀⠀⢀⡤⡾⠁⠀⠀⠀
⠀⠀⠀⠀⢹⣢⡉⠢⡀⠀⠀⠀⢸⠄⡇⢘⠃⠀⠀⠀⡠⠊⡑⣹⠁⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠱⡜⢄⠘⢢⡀⠀⢸⠁⡇⢸⡄⠀⣠⠊⢀⠌⡴⠁⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠱⡀⠡⡀⠑⡄⠸⠄⡇⢸⠀⡔⠁⡐⠁⡲⠁⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠈⢢⡈⢂⠘⡜⡇⡇⣘⡜⠠⠊⣠⠊⠀⠀⠀⠀⠀⠀⠀⠀
⠑⠲⢖⡯⠘⣀⠒⠂⠤⢽⡦⡑⠜⢏⠀⡟⠔⡡⣖⡡⠤⠰⠒⣒⠒⣒⣶⠖⠂
⠀⠀⠀⠉⠓⠲⠅⣉⣀⡐⠠⠉⠊⣼⣼⡤⠚⠩⠀⠂⢀⣉⠰⠔⠒⠉⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡌⡻⢹⡜⡟⢯⡩⣛⢭⡉⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣠⣾⣳⠥⠒⠁⠀⡇⠀⠙⠲⢕⣞⣦⡀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠈⠉⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠉⠀⠀⠀⠀⠀⠀⠀
EOF
echo -e "${R}${BOLD}         __ HASHISH AUTOMATIC INSTALLER __ ${NC}"
echo -e "${NC}"

pretty_print $C $BOLD "\n  Initialisation du système HASHISH..."
echo -e "${BL}=========================================================${NC}\n"

# Détection du dépôt
REPO_NAME="hashish"
HOME_PATH="/data/data/com.termux/files/home"
DEFAULT_REPO="$HOME_PATH/$REPO_NAME"
INSTALL_DIR="/data/data/com.termux/files/usr/bin"
MODULES_DIR="$INSTALL_DIR/modules"
WORDLISTS_DIR="$MODULES_DIR/wordlists"

pretty_print $Y "" "  [1/6] Recherche du dépôt HASHISH..."
sleep 1

if [[ -f "./hashish.py" ]]; then
    REPO_PATH=$(pwd)
elif [[ -f "$DEFAULT_REPO/hashish.py" ]]; then
    REPO_PATH="$DEFAULT_REPO"
else
    pretty_print $R "" "  HASHISH n'a pas été trouvé aux emplacements standards."
    read -p "  Entrez le chemin complet du dépôt : " REPO_PATH
    if [[ ! -f "$REPO_PATH/hashish.py" ]]; then
        pretty_print $R $BLINK "  ERREUR: Dépôt HASHISH introuvable!"
        exit 1
    fi
fi

pretty_print $G "" "  ✓ Dépôt trouvé: $REPO_PATH\n"

# Installation des dépendances
pretty_print $Y "" "  [2/6] Installation des dépendances..."
(
    pkg update -y && \
    pkg upgrade -y && \
    pkg install -y clang openssl git python
) > /dev/null 2>&1 &
spinner $!
echo -e "${G}  ✓ Dépendances installées${NC}\n"

# Préparation de l'installation
pretty_print $Y "" "  [3/6] Préparation de l'installation..."
mkdir -p "$MODULES_DIR" "$WORDLISTS_DIR"
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/"
find "$REPO_PATH/modules/" -name "*.py" -exec cp {} "$MODULES_DIR/" \; 2>/dev/null
[[ -d "$REPO_PATH/wordlists" ]] && cp -r "$REPO_PATH/wordlists/"* "$WORDLISTS_DIR/" 2>/dev/null
echo -e "${G}  ✓ Fichiers copiés${NC}\n"

# Compilation
pretty_print $Y "" "  [4/6] Compilation des modules C++..."
(
    clang++ "$REPO_PATH/modules/hashcracker.cpp" -o "$MODULES_DIR/hashcracker" \
    -O3 -Wall -std=c++17 -I/data/data/com.termux/files/usr/include \
    -L/data/data/com.termux/files/usr/lib -lssl -lcrypto -lpthread -lc++ -lc++_shared

    clang++ "$REPO_PATH/modules/rainbow_generator.cpp" -o "$MODULES_DIR/rainbow_generator" \
    -O3 -Wall -std=c++17 -I/data/data/com.termux/files/usr/include \
    -L/data/data/com.termux/files/usr/lib -lssl -lcrypto -lpthread -lc++ -lc++_shared
) > /dev/null 2>&1 &
spinner $!
echo -e "${G}  ✓ Modules compilés${NC}\n"

# Permissions
pretty_print $Y "" "  [5/6] Configuration des permissions..."
chmod -R +x "$MODULES_DIR"
chmod +x "$INSTALL_DIR/hashish.py"
echo -e "${G}  ✓ Permissions configurées${NC}\n"

# Création du lanceur
pretty_print $Y "" "  [6/6] Création du lanceur..."
cat > "$INSTALL_DIR/hashish" <<EOF
#!/bin/bash
python3 "$INSTALL_DIR/hashish.py" "\$@"
EOF
chmod +x "$INSTALL_DIR/hashish"
echo -e "${G}  ✓ Lanceur créé${NC}\n"

# Installation terminée
echo -e "${BL}=========================================================${NC}"
pretty_print $C $BOLD "\n  Installation complète!"
echo -e "${M}  HASHISH est maintenant prêt à être utilisé.${NC}\n"

# Lancement de HASHISH
pretty_print $W $BLINK "  Lancement de HASHISH dans 3 secondes..."
sleep 3
clear
hashish
