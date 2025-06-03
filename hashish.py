#!/usr/bin/env python3

import os
import sys
import time
import subprocess

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

TERMUX_BIN_DIR = "/data/data/com.termux/files/usr/bin"
CURRENT_SCRIPT_PATH = os.path.abspath(__file__)
CURRENT_SCRIPT_DIR = os.path.dirname(CURRENT_SCRIPT_PATH)

if CURRENT_SCRIPT_DIR == TERMUX_BIN_DIR:
    MODULES_PATH = os.path.join(TERMUX_BIN_DIR, "modules")
    BANNER_PATH = os.path.join(TERMUX_BIN_DIR, "banner-hashish.txt")
else:
    MODULES_PATH = os.path.join(CURRENT_SCRIPT_DIR, "modules")
    BANNER_PATH = os.path.join(CURRENT_SCRIPT_DIR, "banner-hashish.txt")

HASHCRACKER_CPP_EXECUTABLE = os.path.join(MODULES_PATH, "hashcracker")

def clear_screen():
    try:
        os.system('clear')
    except Exception as e:
        print(CR_RED + f"[ERROR] Failed to clear screen: {e}. Check 'clear' command permissions." + RESET)
        time.sleep(1)

def display_banner():
    clear_screen()
    try:
        with open(BANNER_PATH, 'r') as f:
            banner = f.read()
            print(CR_CYAN + banner + RESET)
    except FileNotFoundError:
        print(CR_RED + "[ERROR] Banner file not found! " + BANNER_PATH + RESET)
        print(CR_RED + "         Please ensure 'banner-hashish.txt' is in the same directory as 'hashish.py'" + RESET)
        print(CR_RED + "         or in " + TERMUX_BIN_DIR + RESET)
        print(CR_YELLOW + "         Continuing without banner..." + RESET)
        time.sleep(2)

def check_executable(path):
    return os.path.isfile(path) and os.access(path, os.X_OK)

def run_hashcracker_cpp():
    clear_screen()
    print(CR_BLUE + "--- [LAUNCHING MODULE] Hash Cracker (C++) ---" + RESET)
    print(CR_DARK_GRAY + f"    Attempting to launch C++ module: {HASHCRACKER_CPP_EXECUTABLE}" + RESET)
    print(CR_DARK_GRAY + "    Note: Hashish will need to be relaunched after the C++ module finishes." + RESET)
    time.sleep(1)
    if check_executable(HASHCRACKER_CPP_EXECUTABLE):
        try:
            os.execv(HASHCRACKER_CPP_EXECUTABLE, [HASHCRACKER_CPP_EXECUTABLE])
        except OSError as e:
            print(CR_RED + f"[ERROR] Failed to execute C++ module with os.execv: {e}" + RESET)
            print(CR_YELLOW + "          Please ensure the executable has correct permissions and path." + RESET)
            print(CR_YELLOW + "          Path attempted: " + HASHCRACKER_CPP_EXECUTABLE + RESET)
            time.sleep(3)
        except Exception as e:
            print(CR_RED + f"[ERROR] An unexpected error occurred while preparing C++ module launch: {e}" + RESET)
            time.sleep(3)
    else:
        print(CR_RED + f"[ERROR] C++ executable '{HASHCRACKER_CPP_EXECUTABLE}' is not found or not executable." + RESET)
        print(CR_YELLOW + "          Please compile hashcracker.cpp (located in your hashish/modules folder) " + RESET)
        print(CR_YELLOW + "          and ensure the compiled binary is at the specified path and has execute permissions." + RESET)
        print(CR_YELLOW + "          Run 'chmod +x " + HASHCRACKER_CPP_EXECUTABLE + "' to give execute permissions." + RESET)
        print(CR_YELLOW + "          Or simply re-run the 'installer.sh' script to fix this automatically." + RESET)
        time.sleep(3)
    print(CR_RED + "\n[INFO] Due to an error in launching the C++ module, or if it completed," + RESET)
    print(CR_RED + "       hashish.py needs to be restarted. Please relaunch hashish." + RESET)
    time.sleep(2)
    sys.exit(0)

def run_web_scanner():
    clear_screen()
    print(CR_BLUE + "--- [LAUNCHING MODULE] Web Scanner ---" + RESET)
    web_scanner_script = os.path.join(MODULES_PATH, "web_scanner.py")
    if os.path.exists(web_scanner_script):
        try:
            subprocess.run([sys.executable, web_scanner_script], check=True,
                           stdout=sys.stdout, stderr=sys.stderr)
        except subprocess.CalledProcessError as e:
            print(CR_RED + f"[ERROR] Web Scanner module exited with an error: {e}" + RESET)
        except Exception as e:
            print(CR_RED + f"[ERROR] An unexpected error occurred: {e}" + RESET)
    else:
        print(CR_RED + f"[ERROR] Web Scanner module not found: {web_scanner_script}" + RESET)
        print(CR_YELLOW + "         Please ensure 'web_scanner.py' is in your 'modules' directory." + RESET)
    print(CR_BLUE + "\n--- [MODULE END] Press Enter to return to main menu ---" + RESET)
    input()

def run_reconnaissance():
    clear_screen()
    print(CR_BLUE + "--- [LAUNCHING MODULE] Reconnaissance ---" + RESET)
    recon_script = os.path.join(MODULES_PATH, "recon.py")
    if os.path.exists(recon_script):
        try:
            subprocess.run([sys.executable, recon_script], check=True,
                           stdout=sys.stdout, stderr=sys.stderr)
        except subprocess.CalledProcessError as e:
            print(CR_RED + f"[ERROR] Reconnaissance module exited with an error: {e}" + RESET)
        except Exception as e:
            print(CR_RED + f"[ERROR] An unexpected error occurred: {e}" + RESET)
    else:
        print(CR_RED + f"[ERROR] Reconnaissance module not found: {recon_script}" + RESET)
        print(CR_YELLOW + "         Please ensure 'recon.py' is in your 'modules' directory." + RESET)
    print(CR_BLUE + "\n--- [MODULE END] Press Enter to return to main menu ---" + RESET)
    input()

def run_osint():
    clear_screen()
    print(CR_BLUE + "--- [LAUNCHING MODULE] OSINT ---" + RESET)
    osint_script = os.path.join(MODULES_PATH, "osint.py")
    if os.path.exists(osint_script):
        try:
            subprocess.run([sys.executable, osint_script], check=True,
                           stdout=sys.stdout, stderr=sys.stderr)
        except subprocess.CalledProcessError as e:
            print(CR_RED + f"[ERROR] OSINT module exited with an error: {e}" + RESET)
        except Exception as e:
            print(CR_RED + f"[ERROR] An unexpected error occurred: {e}" + RESET)
    else:
        print(CR_RED + f"[ERROR] OSINT module not found: {osint_script}" + RESET)
        print(CR_YELLOW + "         Please ensure 'osint.py' is in your 'modules' directory." + RESET)
    print(CR_BLUE + "\n--- [MODULE END] Press Enter to return to main menu ---" + RESET)
    input()

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
            break
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
        print(CR_RED + f"[CRITICAL ERROR] An unhandled error occurred: {e}" + RESET)
        sys.exit(1)
