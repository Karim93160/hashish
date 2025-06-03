#!/usr/bin/env python3
import os
import sys
import time
import subprocess
import itertools

# --- Couleurs ANSI ---
RESET = "\033[0m"
CR_RED = "\033[1;31m"
CR_GREEN = "\033[1;32m"
CR_YELLOW = "\033[1;33m"
CR_BLUE = "\033[1;34m"
CR_MAGENTA = "\033[1;35m"
CR_CYAN = "\033[1;36m"
CR_WHITE = "\033[1;37m"
FAINT = "\033[2m"

# --- Chemins globaux ---
TERMUX_BIN_DIR = "/data/data/com.termux/files/usr/bin"
CURRENT_SCRIPT_PATH = os.path.abspath(sys.argv[0])
CURRENT_SCRIPT_DIR = os.path.dirname(CURRENT_SCRIPT_PATH)

# Mode Termux vs normal
if CURRENT_SCRIPT_DIR.startswith(TERMUX_BIN_DIR):
    MODULES_PATH = os.path.join(TERMUX_BIN_DIR, "modules")
    BANNER_PATH = os.path.join(TERMUX_BIN_DIR, "banner-hashish.txt")
else:
    MODULES_PATH = os.path.join(CURRENT_SCRIPT_DIR, "modules")
    BANNER_PATH = os.path.join(CURRENT_SCRIPT_DIR, "banner-hashish.txt")

HASHCRACKER_CPP_EXECUTABLE = os.path.join(MODULES_PATH, "hashcracker")


def clear_screen():
    # Attempt to use tput for better compatibility, fallback to ANSI escape codes
    if os.name == 'posix': # Linux/Unix/Termux
        # Try tput first (more robust, respects TERM variable)
        if os.system("tput reset 2>/dev/null") != 0:
            # Fallback to ANSI escape code if tput fails or is not found
            print("\033[H\033[J", end="") # Move cursor to top-left and clear screen
    else: # Windows
        os.system("cls")


def color_cycle():
    return itertools.cycle([CR_CYAN, CR_GREEN, CR_YELLOW, CR_BLUE, CR_MAGENTA])


def animate_banner(delay=0.008):
    clear_screen()
    try:
        with open(BANNER_PATH, 'r') as f:
            lines = f.readlines()
            colors = color_cycle()
            for line in lines:
                print(next(colors) + line.rstrip() + RESET)
                time.sleep(delay)
    except FileNotFoundError:
        print(CR_RED + "[ERROR] Banner file not found!" + RESET)
        print(CR_YELLOW + f" → Expected at: {BANNER_PATH}" + RESET)
        time.sleep(2)


def display_banner():
    animate_banner()
    time.sleep(0.4)


def check_executable(path):
    return os.path.isfile(path) and os.access(path, os.X_OK)


def ensure_modules_folder():
    if not os.path.isdir(MODULES_PATH):
        os.makedirs(MODULES_PATH)
        print(CR_YELLOW + f"[INFO] Created modules folder at {MODULES_PATH}" + RESET)
        time.sleep(1)


def run_hashcracker_cpp():
    clear_screen()
    print(CR_BLUE + "--- Launching: Hash Cracker (C++) ---" + RESET)
    time.sleep(1)
    if check_executable(HASHCRACKER_CPP_EXECUTABLE):
        try:
            subprocess.run([HASHCRACKER_CPP_EXECUTABLE])
            print(CR_GREEN + "\n[INFO] Hash Cracker terminé. Fin du script." + RESET)
        except Exception as e:
            print(CR_RED + f"[ERROR] Impossible de lancer le binaire C++ : {e}" + RESET)
    else:
        print(CR_RED + "[ERROR] C++ binary not found or not executable!" + RESET)
        print(CR_YELLOW + f" → Expected: {HASHCRACKER_CPP_EXECUTABLE}" + RESET)
        print(CR_MAGENTA + " ➜ Compile `hashcracker.cpp` and place the binary in 'modules/'." + RESET)
    sys.exit(0)


def run_module(module_name, script_filename):
    clear_screen()
    print(CR_BLUE + f"--- Launching: {module_name} ---" + RESET)
    script_path = os.path.join(MODULES_PATH, script_filename)
    if os.path.exists(script_path):
        try:
            subprocess.run([sys.executable, script_path], check=True)
        except subprocess.CalledProcessError as e:
            print(CR_RED + f"[ERROR] Script error: {e}" + RESET)
        except Exception as e:
            print(CR_RED + f"[ERROR] Unexpected error: {e}" + RESET)
    else:
        print(CR_RED + f"[ERROR] Script not found at: {script_path}" + RESET)
    input(CR_BLUE + "\nPress Enter to return to menu..." + RESET)


def main_menu():
    ensure_modules_folder()
    while True:
        display_banner()
        print(CR_BLUE + "\n--- [AVAILABLE OPTIONS] -------------------------------" + RESET)
        print(CR_CYAN + " [1] " + CR_WHITE + "» Hash Cracker (C++)")
        print(CR_CYAN + " [2] " + CR_WHITE + "» Web Scanner")
        print(CR_CYAN + " [3] " + CR_WHITE + "» Reconnaissance")
        print(CR_CYAN + " [4] " + CR_WHITE + "» OSINT")
        print(CR_CYAN + " [0] " + CR_WHITE + "» Exit")
        print(CR_BLUE + "-------------------------------------------------------" + RESET)
        choice = input(CR_YELLOW + "\n[CHOOSE YOUR WEAPON] > " + RESET).strip()

        if choice == '1':
            run_hashcracker_cpp()
        elif choice == '2':
            run_module("Web Scanner", "web_scanner.py")
        elif choice == '3':
            run_module("Reconnaissance", "recon.py")
        elif choice == '4':
            run_module("OSINT", "osint.py")
        elif choice == '0':
            print(CR_GREEN + "\n[EXIT] Stay ethical. See you soon!" + RESET)
            sys.exit(0)
        else:
            print(CR_RED + "[ERROR] Invalid selection. Choose from menu." + RESET)
            time.sleep(1.5)


if __name__ == "__main__":
    try:
        main_menu()
    except KeyboardInterrupt:
        print(CR_GREEN + "\n[EXIT] Interrupted. Stay safe!" + RESET)
        sys.exit(0)
    except Exception as e:
        print(CR_RED + f"[FATAL] Unexpected crash: {e}" + RESET)
        sys.exit(1)
