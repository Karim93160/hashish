#!/usr/bin/env python3

import os
import sys
import time
import subprocess
import itertools

# --- Couleurs ANSI ---
RESET = "\033[0m"
BLACK = "\033[30m"
RED = "\033[31m"
GREEN = "\033[32m"
BLUE = "\033[34m"
YELLOW = "\033[33m"
MAGENTA = "\033[35m"
CYAN = "\033[36m"
WHITE = "\033[37m"
BOLD = "\033[1m"
FAINT = "\033[2m"
ITALIC = "\033[3m"
UNDERLINE = "\033[4m"
BLINK = "\033[5m"
REVERSE = "\033[7m"
HIDDEN = "\033[8m"
STRIKETHROUGH = "\033[9m"

CR_RED = RED + BOLD
CR_GREEN = GREEN + BOLD
CR_YELLOW = YELLOW + BOLD
CR_BLUE = BLUE + BOLD + FAINT
CR_CYAN = CYAN + BOLD
CR_MAGENTA = MAGENTA + BOLD + FAINT
CR_WHITE = WHITE + BOLD
CR_DARK_GRAY = "\033[90m"

# --- Chemins globaux ---
TERMUX_BIN_DIR = "/data/data/com.termux/files/usr/bin"

try:
    CURRENT_SCRIPT_PATH = os.path.abspath(__file__)
except NameError:
    CURRENT_SCRIPT_PATH = os.path.abspath(sys.argv[0])

CURRENT_SCRIPT_DIR = os.path.dirname(CURRENT_SCRIPT_PATH)

# Gestion du mode Termux vs normal
if CURRENT_SCRIPT_DIR.startswith(TERMUX_BIN_DIR):
    MODULES_PATH = os.path.join(TERMUX_BIN_DIR, "modules")
    BANNER_PATH = os.path.join(TERMUX_BIN_DIR, "banner-hashish.txt")
else:
    MODULES_PATH = os.path.join(CURRENT_SCRIPT_DIR, "modules")
    BANNER_PATH = os.path.join(CURRENT_SCRIPT_DIR, "banner-hashish.txt")

HASHCRACKER_CPP_EXECUTABLE = os.path.join(MODULES_PATH, "hashcracker")


def clear_screen():
    try:
        if os.name == "nt":
            os.system("cls")
        else:
            os.system("clear")
    except:
        print("\n" * 100)


def color_generator():
    colors = [CR_CYAN, CR_GREEN, CR_YELLOW, CR_BLUE, CR_MAGENTA]
    return itertools.cycle(colors)


def animate_banner(delay=0.01):
    clear_screen()
    try:
        with open(BANNER_PATH, 'r') as f:
            lines = f.readlines()
            colors = color_generator()
            for line in lines:
                print(next(colors) + line.rstrip() + RESET)
                time.sleep(delay)
    except FileNotFoundError:
        print(CR_RED + "[ERROR] Banner file not found!" + RESET)
        print(CR_YELLOW + f"         → Expected at: {BANNER_PATH}" + RESET)
        time.sleep(2)
    except Exception as e:
        print(CR_RED + f"[ERROR] Banner animation failed: {e}" + RESET)
        time.sleep(2)


def display_banner():
    animate_banner()
    time.sleep(0.5)


def check_executable(path):
    return os.path.isfile(path) and os.access(path, os.X_OK)


def run_hashcracker_cpp():
    clear_screen()
    print(CR_BLUE + "--- [LAUNCHING MODULE] Hash Cracker (C++) ---" + RESET)
    time.sleep(1)
    if check_executable(HASHCRACKER_CPP_EXECUTABLE):
        try:
            os.execv(HASHCRACKER_CPP_EXECUTABLE, [HASHCRACKER_CPP_EXECUTABLE])
        except Exception as e:
            print(CR_RED + f"[ERROR] Could not execute C++ binary: {e}" + RESET)
    else:
        print(CR_RED + "[ERROR] C++ binary not found or not executable." + RESET)
        print(CR_YELLOW + "  → Compile 'hashcracker.cpp' and place the binary in 'modules/'." + RESET)
    print(CR_RED + "\n[INFO] Please relaunch hashish.py." + RESET)
    time.sleep(2)
    sys.exit(0)


def run_module(name, script_name):
    clear_screen()
    print(CR_BLUE + f"--- [LAUNCHING MODULE] {name} ---" + RESET)
    script_path = os.path.join(MODULES_PATH, script_name)
    if os.path.exists(script_path):
        try:
            subprocess.run([sys.executable, script_path], check=True)
        except subprocess.CalledProcessError as e:
            print(CR_RED + f"[ERROR] Module crashed: {e}" + RESET)
        except Exception as e:
            print(CR_RED + f"[ERROR] Unexpected error: {e}" + RESET)
    else:
        print(CR_RED + f"[ERROR] Module not found: {script_path}" + RESET)
    print(CR_BLUE + "\n--- [MODULE END] Press Enter to return to main menu ---" + RESET)
    input()


def run_web_scanner():
    run_module("Web Scanner", "web_scanner.py")


def run_reconnaissance():
    run_module("Reconnaissance", "recon.py")


def run_osint():
    run_module("OSINT", "osint.py")


def main_menu():
    while True:
        display_banner()
        print(CR_BLUE + "\n--- [AVAILABLE OPTIONS] -------------------------------" + RESET)
        print(CR_CYAN + " [1] " + CR_WHITE + "» Hash Cracker (C++)" + RESET)
        print(CR_CYAN + " [2] " + CR_WHITE + "» Web Scanner" + RESET)
        print(CR_CYAN + " [3] " + CR_WHITE + "» Recon" + RESET)
        print(CR_CYAN + " [4] " + CR_WHITE + "» OSINT" + RESET)
        print(CR_CYAN + " [0] " + CR_WHITE + "» Exit" + RESET)
        print(CR_BLUE + "-------------------------------------------------------" + RESET)
        choice = input(CR_YELLOW + "\n [CHOOSE YOUR WEAPON] > " + RESET).strip()
        if choice == '1':
            run_hashcracker_cpp()
        elif choice == '2':
            run_web_scanner()
        elif choice == '3':
            run_reconnaissance()
        elif choice == '4':
            run_osint()
        elif choice == '0':
            print(CR_GREEN + "\n[INFO] Exiting HASHISH. Stay safe and ethical! 👋" + RESET)
            sys.exit(0)
        else:
            print(CR_RED + "[ERROR] Invalid choice. Please enter a number from the menu." + RESET)
            time.sleep(1.5)


if __name__ == "__main__":
    try:
        main_menu()
    except KeyboardInterrupt:
        print(CR_GREEN + "\n[INFO] Exiting HASHISH. Stay safe and ethical! 👋" + RESET)
        sys.exit(0)
    except Exception as e:
        print(CR_RED + f"[CRITICAL ERROR] Unhandled exception: {e}" + RESET)
        sys.exit(1)
