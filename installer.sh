#!/bin/bash

# Colors and effects
R='\033[1;31m'  # Red
G='\033[1;32m'  # Green
Y='\033[1;33m'  # Yellow
B='\033[1;34m'  # Blue
M='\033[1;35m'  # Magenta
C='\033[1;36m'  # Cyan
W='\033[1;37m'  # White
BL='\033[1;30m' # Black (dark gray)
NC='\033[0m'    # Reset

# Special effects
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

# Function to print styled messages
pretty_print() {
    local color=$1
    local effect=$2
    local msg=$3
    echo -e "${effect}${color}$msg${NC}"
}

# Clear screen (using tput for better compatibility)
clear_terminal() {
    tput clear 2>/dev/null || clear
}

clear_terminal

# ASCII Banner with colors
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

pretty_print $C $BOLD "\n  Initializing HASHISH system..."
echo -e "${BL}=========================================================${NC}\n"

# Repository detection
REPO_NAME="hashish"
HOME_PATH="/data/data/com.termux/files/home"
DEFAULT_REPO="$HOME_PATH/$REPO_NAME"
INSTALL_DIR="/data/data/com.termux/files/usr/bin"
MODULES_DIR="$INSTALL_DIR/modules"
WORDLISTS_DIR="$MODULES_DIR/wordlists"

pretty_print $Y "" "  [1/6] Locating HASHISH repository..."
sleep 1

if [[ -f "./hashish.py" ]]; then
    REPO_PATH=$(pwd)
elif [[ -f "$DEFAULT_REPO/hashish.py" ]]; then
    REPO_PATH="$DEFAULT_REPO"
else
    pretty_print $R "" "  HASHISH not found in standard locations."
    read -p "  Enter full repository path: " REPO_PATH
    if [[ ! -f "$REPO_PATH/hashish.py" ]]; then
        pretty_print $R $BLINK "  ERROR: HASHISH repository not found!"
        exit 1
    fi
fi

pretty_print $G "" "  ✓ Repository found: $REPO_PATH\n"

# Install dependencies
pretty_print $Y "" "  [2/6] Installing dependencies..."
(
    pkg update -y && \
    pkg upgrade -y && \
    pkg install -y clang openssl git python termux-tools
) > /dev/null 2>&1 &
spinner $!
echo -e "${G}  ✓ Dependencies installed${NC}\n"

# Prepare installation
pretty_print $Y "" "  [3/6] Preparing installation..."
mkdir -p "$MODULES_DIR" "$WORDLISTS_DIR"
cp "$REPO_PATH/hashish.py" "$INSTALL_DIR/"

# Improved banner handling
if [[ -f "$REPO_PATH/banner-hashish.txt" ]]; then
    cp "$REPO_PATH/banner-hashish.txt" "$INSTALL_DIR/"
    chmod 644 "$INSTALL_DIR/banner-hashish.txt"
    pretty_print $G "" "  ✓ Banner copied"
else
    pretty_print $Y "" "  ! Warning: banner-hashish.txt not found in repository"
    pretty_print $Y "" "  Program will run without custom banner"
fi

find "$REPO_PATH/modules/" -name "*.py" -exec cp {} "$MODULES_DIR/" \; 2>/dev/null
[[ -d "$REPO_PATH/wordlists" ]] && cp -r "$REPO_PATH/wordlists/"* "$WORDLISTS_DIR/" 2>/dev/null
echo -e "${G}  ✓ Files copied${NC}\n"

# Compile modules
pretty_print $Y "" "  [4/6] Compiling C++ modules..."
(
    clang++ "$REPO_PATH/modules/hashcracker.cpp" -o "$MODULES_DIR/hashcracker" \
    -O3 -Wall -std=c++17 -I/data/data/com.termux/files/usr/include \
    -L/data/data/com.termux/files/usr/lib -lssl -lcrypto -lpthread -lc++ -lc++_shared

    clang++ "$REPO_PATH/modules/rainbow_generator.cpp" -o "$MODULES_DIR/rainbow_generator" \
    -O3 -Wall -std=c++17 -I/data/data/com.termux/files/usr/include \
    -L/data/data/com.termux/files/usr/lib -lssl -lcrypto -lpthread -lc++ -lc++_shared
) > /dev/null 2>&1 &
spinner $!
echo -e "${G}  ✓ Modules compiled${NC}\n"

# Set permissions
pretty_print $Y "" "  [5/6] Configuring permissions..."
chmod -R +x "$MODULES_DIR"
chmod +x "$INSTALL_DIR/hashish.py"
echo -e "${G}  ✓ Permissions configured${NC}\n"

# Create launcher
pretty_print $Y "" "  [6/6] Creating launcher..."
cat > "$INSTALL_DIR/hashish" <<EOF
#!/bin/bash
# Check if banner exists
if [[ ! -f "$INSTALL_DIR/banner-hashish.txt" ]]; then
    echo -e "\033[1;33m[WARNING] Banner file not found. Running without custom banner.\033[0m" >&2
fi
python3 "$INSTALL_DIR/hashish.py" "\$@"
EOF
chmod +x "$INSTALL_DIR/hashish"
echo -e "${G}  ✓ Launcher created${NC}\n"

# Installation complete
echo -e "${BL}=========================================================${NC}"
pretty_print $C $BOLD "\n  Installation complete!"
echo -e "${M}  HASHISH is now ready to use.${NC}\n"

# Launch HASHISH
pretty_print $W $BLINK "  Launching HASHISH in 3 seconds..."
sleep 3
clear_terminal
hashish
