#!/usr/bin/env python3

import os
import sys
import time
import subprocess

# Couleurs & effets
RESET = "\033[0m"
RED = "\033[1;31m"
GREEN = "\033[1;32m"
YELLOW = "\033[1;33m"
CYAN = "\033[1;36m"
WHITE = "\033[1;37m"
BOLD = "\033[1m"
BLINK = "\033[5m"

CRITICAL = RED + BOLD
INFO = CYAN + BOLD
SUCCESS = GREEN + BOLD
WARNING = YELLOW + BOLD

TERMUX_BIN_DIR = "/data/data/com.termux/files/usr/bin"
CURRENT_SCRIPT_PATH = os.path.abspath(__file__)
CURRENT_SCRIPT_DIR = os.path.dirname(CURRENT_SCRIPT_PATH)

if CURRENT_SCRIPT_DIR == TERMUX_BIN_DIR:
    MODULES_PATH = os.path.join(TERMUX_BIN_DIR, "modules")
    BANNER_PATH = os.path.join(TERMUX_BIN_DIR, "banner-hashish.txt")
else:
    MODULES_PATH = os.path.join(CURRENT_SCRIPT_DIR, "modules")
    BANNER_PATH = os.path.join(CURRENT_SCRIPT_DIR, "banner-hashish.txt")

# Chargement du banner
def display_banner():
    if os.path.exists(BANNER_PATH):
        with open(BANNER_PATH, 'r') as f:
            print(CYAN + BOLD + f.read() + RESET)
    else:
        print(INFO + "[INFO] Banner non trouvé, lancement normal." + RESET)

# Menu principal
def main_menu():
    print(f"{WHITE}[CHOOSE YOUR WEAPON]{RESET}")
    print("  1. Brute force hash")
    print("  2. Générateur rainbow table")
    print("  3. Quitter")
    print()

    # Évite crash en mode non-interactif
    if not sys.stdin.isatty():
        print(CRITICAL + "[CRITICAL ERROR] Le script a été lancé sans terminal interactif. Ignoré." + RESET)
        return

    try:
        choice = input(f"{CYAN}[CHOIX] > {RESET}")
        if choice == '1':
            run_hashcracker()
        elif choice == '2':
            run_rainbow()
        elif choice == '3':
            print(GREEN + "[+] Fermeture de HASHISH." + RESET)
            sys.exit(0)
        else:
            print(RED + "[ERROR] Choix invalide. Réessaie." + RESET)
            main_menu()
    except EOFError:
        print(CRITICAL + "[CRITICAL ERROR] Entrée non lue (EOF). Lancement probablement non-interactif." + RESET)

# Module 1
def run_hashcracker():
    bin_path = os.path.join(MODULES_PATH, "hashcracker")
    if os.path.isfile(bin_path):
        subprocess.run([bin_path])
    else:
        print(RED + "[ERREUR] hashcracker non trouvé." + RESET)

# Module 2
def run_rainbow():
    bin_path = os.path.join(MODULES_PATH, "rainbow_generator")
    if os.path.isfile(bin_path):
        subprocess.run([bin_path])
    else:
        print(RED + "[ERREUR] rainbow_generator non trouvé." + RESET)

# Lancement
if __name__ == "__main__":
    display_banner()
    main_menu()
