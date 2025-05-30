#!/usr/bin/env python3

import os
import sys
import time
import subprocess

# --- Codes de couleurs ANSI pour le terminal ---
# Ces codes sont les mêmes que ceux utilisés dans hashcracker.cpp pour la cohérence
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

# Couleurs personnalisées pour l'ambiance Cracker Mood
CR_RED = RED + BOLD
CR_GREEN = GREEN + BOLD
CR_YELLOW = YELLOW + BOLD
CR_BLUE = BLUE + BOLD + FAINT # Bleu sombre, moins lumineux
CR_CYAN = CYAN + BOLD
CR_MAGENTA = MAGENTA + BOLD + FAINT
CR_WHITE = WHITE + BOLD
CR_DARK_GRAY = "\033[90m" # Gris foncé pour les infos moins importantes

# --- Chemins du dépôt ---
# Détecter le chemin du dépôt en se basant sur l'emplacement de hashish.py
# Si hashish.py est dans /usr/bin, le répertoire 'modules' sera dans /usr/bin/modules
# Sinon, s'il est lancé depuis le dossier original, les modules sont à côté.
# On donne la priorité au chemin standard de Termux si hashish.py y est copié.

# Chemin standard pour les exécutables Termux
TERMUX_BIN_DIR = "/data/data/com.termux/files/usr/bin"
# Chemin du script lui-même
CURRENT_SCRIPT_PATH = os.path.abspath(__file__)
CURRENT_SCRIPT_DIR = os.path.dirname(CURRENT_SCRIPT_PATH)

# Définir le chemin du dossier modules
# Si hashish.py est dans TERMUX_BIN_DIR, alors les modules sont dans TERMUX_BIN_DIR/modules
if CURRENT_SCRIPT_DIR == TERMUX_BIN_DIR:
    MODULES_PATH = os.path.join(TERMUX_BIN_DIR, "modules")
    BANNER_PATH = os.path.join(TERMUX_BIN_DIR, "banner-hashish.txt")
else:
    # Sinon, on suppose que hashish.py est exécuté depuis le dossier de clonage
    MODULES_PATH = os.path.join(CURRENT_SCRIPT_DIR, "modules")
    BANNER_PATH = os.path.join(CURRENT_SCRIPT_DIR, "banner-hashish.txt")

# Chemin de l'exécutable hashcracker C++
HASHCRACKER_CPP_EXECUTABLE = os.path.join(MODULES_PATH, "hashcracker")


def clear_screen():
    """Efface le terminal."""
    # Correction pour l'erreur "Permission denied" :
    # Utiliser 'env -i sh -c "clear"' pour s'assurer que 'clear' est exécuté
    # dans un environnement propre, ou vérifier si 'tput reset' fonctionne mieux.
    # Pour Termux, 'clear' devrait fonctionner si les permissions sont bonnes.
    # Si l'erreur persiste, tu pourrais avoir besoin de 'chmod +x /data/data/com.termux/files/usr/bin/clear'
    # ou d'une solution alternative pour effacer l'écran.
    # Pour l'instant, je vais laisser 'os.system('clear')' car c'est la méthode standard.
    # Si tu vois encore l'erreur, il faudra vérifier les permissions de 'clear' dans Termux.
    try:
        os.system('clear')
    except Exception as e:
        print(CR_RED + f"[ERROR] Failed to clear screen: {e}. Check 'clear' command permissions." + RESET)
        time.sleep(1)


def display_banner():
    """Affiche la bannière ASCII art."""
    clear_screen()
    try:
        with open(BANNER_PATH, 'r') as f:
            banner = f.read()
            # On applique une couleur bleue à la bannière pour l'ambiance
            print(CR_CYAN + banner + RESET)
    except FileNotFoundError:
        print(CR_RED + "[ERROR] Banner file not found! " + BANNER_PATH + RESET)
        print(CR_RED + "         Please ensure 'banner-hashish.txt' is in the same directory as 'hashish.py'" + RESET)
        print(CR_RED + "         or in " + TERMUX_BIN_DIR + RESET)
        print(CR_YELLOW + "         Continuing without banner..." + RESET)
        time.sleep(2) # Laisse le temps de lire le message d'erreur

def check_executable(path):
    """Vérifie si un fichier est exécutable."""
    return os.path.isfile(path) and os.access(path, os.X_OK)

def run_hashcracker_cpp():
    """Lance le module Hash Cracker (C++)."""
    clear_screen()
    print(CR_BLUE + "--- [LAUNCHING MODULE] Hash Cracker (C++) ---" + RESET)
    print(CR_DARK_GRAY + f"    Attempting to launch C++ module: {HASHCRACKER_CPP_EXECUTABLE}" + RESET)
    print(CR_DARK_GRAY + "    Note: Hashish will need to be relaunched after the C++ module finishes." + RESET)
    time.sleep(1) # Laisse un instant pour lire le message

    if check_executable(HASHCRACKER_CPP_EXECUTABLE):
        try:
            # Remplacer le processus Python par l'exécutable C++.
            # argv[0] doit être le chemin de l'exécutable lui-même.
            # Les arguments suivants sont les arguments passés au C++ (aucun ici).
            os.execv(HASHCRACKER_CPP_EXECUTABLE, [HASHCRACKER_CPP_EXECUTABLE])
            # Le code après os.execv() n'est jamais exécuté si execv réussit.

        except OSError as e:
            # Cette erreur se produit si execv ne peut pas lancer l'exécutable
            print(CR_RED + f"[ERROR] Failed to execute C++ module with os.execv: {e}" + RESET)
            print(CR_YELLOW + "          Please ensure the executable has correct permissions and path." + RESET)
            print(CR_YELLOW + "          Path attempted: " + HASHCRACKER_CPP_EXECUTABLE + RESET)
            time.sleep(3) # Pause pour lire l'erreur
        except Exception as e:
            print(CR_RED + f"[ERROR] An unexpected error occurred while preparing C++ module launch: {e}" + RESET)
            time.sleep(3) # Pause pour lire l'erreur
    else:
        print(CR_RED + f"[ERROR] C++ executable '{HASHCRACKER_CPP_EXECUTABLE}' is not found or not executable." + RESET)
        print(CR_YELLOW + "          Please compile hashcracker.cpp (located in your hashish/modules folder) " + RESET)
        print(CR_YELLOW + "          and ensure the compiled binary is at the specified path and has execute permissions." + RESET)
        print(CR_YELLOW + "          Run 'chmod +x " + HASHCRACKER_CPP_EXECUTABLE + "' to give execute permissions." + RESET)
        print(CR_YELLOW + "          Or simply re-run the 'installer.sh' script to fix this automatically." + RESET)
        time.sleep(3) # Pause pour lire l'erreur

    # Si os.execv échoue (par exemple, fichier introuvable), nous revenons ici
    # et affichons un message avant de quitter.
    print(CR_RED + "\n[INFO] Due to an error in launching the C++ module, or if it completed," + RESET)
    print(CR_RED + "       hashish.py needs to be restarted. Please relaunch hashish." + RESET)
    time.sleep(2)
    sys.exit(0) # Quitte proprement après l'échec de lancement ou si execv est censé prendre le relais


def run_web_scanner():
    """Lance le module Web Scanner."""
    clear_screen()
    print(CR_BLUE + "--- [LAUNCHING MODULE] Web Scanner ---" + RESET)
    web_scanner_script = os.path.join(MODULES_PATH, "web_scanner.py")
    if os.path.exists(web_scanner_script):
        try:
            # Exécute le script Python du module, en passant stdout/stderr pour conserver les couleurs si le module en utilise
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
    """Lance le module Reconnaissance."""
    clear_screen()
    print(CR_BLUE + "--- [LAUNCHING MODULE] Reconnaissance ---" + RESET)
    # Mis à jour : Le script de reconnaissance est maintenant nommé recon.py
    recon_script = os.path.join(MODULES_PATH, "recon.py")
    if os.path.exists(recon_script):
        try:
            # Exécute le script Python du module, en passant stdout/stderr pour conserver les couleurs si le module en utilise
            subprocess.run([sys.executable, recon_script], check=True,
                           stdout=sys.stdout, stderr=sys.stderr)
        except subprocess.CalledProcessError as e:
            print(CR_RED + f"[ERROR] Reconnaissance module exited with an error: {e}" + RESET)
        except Exception as e:
            print(CR_RED + f"[ERROR] An unexpected error occurred: {e}" + RESET)
    else:
        print(CR_RED + f"[ERROR] Reconnaissance module not found: {recon_script}" + RESET)
        print(CR_YELLOW + "         Please ensure 'recon.py' is in your 'modules' directory." + RESET) # Mis à jour ici aussi
    print(CR_BLUE + "\n--- [MODULE END] Press Enter to return to main menu ---" + RESET)
    input()

def run_osint():
    """Lance le module OSINT."""
    clear_screen()
    print(CR_BLUE + "--- [LAUNCHING MODULE] OSINT ---" + RESET)
    osint_script = os.path.join(MODULES_PATH, "osint.py")

    if os.path.exists(osint_script):
        try:
            # Exécute le script Python du module, en passant stdout/stderr pour conserver les couleurs si le module en utilise
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
    """Affiche le menu principal et gère les choix de l'utilisateur."""
    while True:
        display_banner()
        print(CR_BLUE + "\n--- [AVAILABLE OPTIONS] -------------------------------" + RESET)
        print(CR_CYAN + " [1] " + CR_WHITE + "» Hash Cracker (C++)" + RESET)
        print(CR_CYAN + " [2] " + CR_WHITE + "» Web Scanner" + RESET)
        print(CR_CYAN + " [3] " + CR_WHITE + "» Reconnaissance" + RESET)
        print(CR_CYAN + " [4] " + CR_WHITE + "» OSINT" + RESET)
        print(CR_CYAN + " [0] " + CR_WHITE + "» Exit" + RESET)
        print(CR_BLUE + "-------------------------------------------------------" + RESET)

        choice = input(CR_YELLOW + "\n [CHOOSE YOUR WEAPON] > " + RESET).strip()

        if choice == '1':
            run_hashcracker_cpp()
            break # Quitte la boucle du menu pour s'assurer que le script s'arrête ou redémarre
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
            time.sleep(1.5) # Laisse le temps à l'utilisateur de voir l'erreur

if __name__ == "__main__":
    try:
        main_menu()
    except KeyboardInterrupt:
        print(CR_GREEN + "\n[INFO] Exiting HASHISH. Stay safe and ethical! 👋" + RESET)
        sys.exit(0)
    except Exception as e:
        print(CR_RED + f"[CRITICAL ERROR] An unhandled error occurred: {e}" + RESET)
        sys.exit(1)

