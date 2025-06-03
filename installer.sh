#!/bin/bash

R='\033[1;31m'; G='\033[1;32m'; Y='\033[1;33m'; B='\033[1;34m'
M='\033[1;35m'; C='\033[1;36m'; W='\033[1;37m'; BL='\033[1;30m'; NC='\033[0m'
BOLD='\033[1m'; BLINK='\033[5m'

spinner() {
    local pid=$1 delay=0.1 spinstr='|/-\'
    while ps -p $pid > /dev/null; do
        local temp=${spinstr#?}
        printf " [%c] " "$spinstr"
        spinstr=$temp${spinstr%"$temp"}
        sleep $delay
        printf "\b\b\b\b\b"
    }
    printf "    \b\b\b\b"
}

pretty_print() {
    echo -e "${2}${1}$3${NC}"
}

clear; tput reset 2>/dev/null

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

pretty_print "$C" "$BOLD" "\n  Initializing HASHISH system..."
echo -e "${BL}=========================================================${NC}\n"

REPO_NAME="hashish"
HOME_PATH="/data/data/com.termux/files/home"
DEFAULT_REPO="$HOME_PATH/$REPO_NAME"
INSTALL_DIR="/data/data/com.termux/files/usr/bin"
MODULES_DIR="$INSTALL_DIR/modules"
WORDLISTS_DIR="$MODULES_DIR/wordlists"

pretty_print "$Y" "" "  [1/5] Locating HASHISH repository..."
sleep 1
if [[ -f "./hashish.py" ]]; then
    REPO_PATH=$(pwd)
elif [[ -f "$DEFAULT_REPO/hashish.py" ]]; then
    REPO_PATH="$DEFAULT_REPO"
else
    pretty_print "$R" "" "  HASHISH not found."
    read -p "  Enter full repository path: " REPO_PATH
    [[ ! -f "$REPO_PATH/hashish.py" ]] && pretty_print "$R" "$BLINK" "  ERROR: hashish.py not found!" && exit 1
fi
pretty_print "$G" "" "  ✓ Repository found: $REPO_PATH\n"

pretty_print "$Y" "" "  [2/5] Preparing directories and copying..."
mkdir -p "$MODULES_DIR" "$WORDLISTS_DIR"
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/"
[[ -f "$REPO_PATH/banner-hashish.txt" ]] && cp "$REPO_PATH/banner-hashish.txt" "$INSTALL_DIR/"
find "$REPO_PATH/modules/" -name "*.py" -exec cp {} "$MODULES_DIR/" \; 2>/dev/null
[[ -d "$REPO_PATH/wordlists" ]] && cp -r "$REPO_PATH/wordlists/"* "$WORDLISTS_DIR/" 2>/dev/null
pretty_print "$G" "" "  ✓ Files copied\n"

pretty_print "$Y" "" "  [3/5] Compiling modules..."
(
    clang++ "$REPO_PATH/modules/hashcracker.cpp" -o "$MODULES_DIR/hashcracker" -O3 -Wall -std=c++17 -lssl -lcrypto -lpthread -lc++ -lc++_shared
    clang++ "$REPO_PATH/modules/rainbow_generator.cpp" -o "$MODULES_DIR/rainbow_generator" -O3 -Wall -std=c++17 -lssl -lcrypto -lpthread -lc++ -lc++_shared
) > /dev/null 2>&1 &
spinner $!
pretty_print "$G" "" "  ✓ Compilation finished\n"

pretty_print "$Y" "" "  [4/5] Setting permissions..."
chmod -R +x "$MODULES_DIR"
chmod +x "$INSTALL_DIR/hashish.py"
pretty_print "$G" "" "  ✓ Permissions set\n"

pretty_print "$Y" "" "  [5/5] Creating launcher..."
cat > "$INSTALL_DIR/hashish" <<EOF
#!/bin/bash
exec python3 "$INSTALL_DIR/hashish.py" "\$@"
EOF
chmod +x "$INSTALL_DIR/hashish"
pretty_print "$G" "" "  ✓ Launcher created\n"

echo -e "${BL}=========================================================${NC}"
pretty_print "$C" "$BOLD" "\n  Installation complete!"
echo -e "${M}  HASHISH is now ready to use.${NC}\n"
sleep 2
exec hashish
