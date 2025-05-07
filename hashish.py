#!/data/data/com.termux/files/usr/bin/python3

import os
import sys
import time
from termcolor import colored

# Ajouter le chemin courant au sys.path (car les modules sont copiés ici)
sys.path.insert(0, os.path.dirname(__file__))

def show_banner():
    print("\033[H\033[J", end="")  # Séquence d'échappement ANSI pour effacer l'écran
    banner_path = os.path.join(os.path.dirname(__file__), 'banner.txt')
    try:
        with open(banner_path, 'r') as f:
            print(colored(f.read(), 'light_cyan', attrs=['bold']))
    except FileNotFoundError:
        print(colored("[WARNING] banner.txt introuvable dans le répertoire d'installation.", 'yellow'))
    time.sleep(0.1) # Réduction du délai si nécessaire

def show_menu():
    print(colored("=" * os.get_terminal_size().columns, 'light_blue'))
    print(colored(" ☠︎ HASHISH ETHICAL TOOLKIT ☠︎ ".center(os.get_terminal_size().columns), 'red', attrs=['bold', 'blink']))
    print(colored("=" * os.get_terminal_size().columns, 'light_blue'))
    print(colored("\n [OPTIONS DISPONIBLES] ", 'green', attrs=['bold']))
    options = [
        ("[1]", "Hash Cracker", "hashcracker"),
        ("[2]", "Web Scanner", "webscanner"),
        ("[3]", "Reconnaissance", "recon"),
        ("[4]", "OSINT", "osint"),
        ("[5]", "SQL Injection (À venir)", "sqli"),
        ("[6]", "Hydra Bruteforce (À venir)", "hydra"),
        ("[0]", "Quitter", "exit")
    ]
    for num, label, _ in options:
        print(colored(f" {num} » {label}", 'cyan'))

    choice = input(colored("\n [CHOISISSEZ VOTRE ARME] > ", 'yellow', attrs=['bold']))
    choice_map = {str(i + 1): module for i, (_, _, module) in enumerate(options[:-1])}
    choice_map['0'] = 'exit'
    return choice_map.get(choice)

if __name__ == "__main__":
    try:
        while True:
            print("\033[H\033[J", end="")  # Effacer l'écran en début de boucle
            show_banner()
            module_name = show_menu()

            if module_name == "hashcracker":
                try:
                    import hashcracker
                    hash_to_crack = input(colored(" [ENTREZ LE HASH À DÉCRYPTER] > ", 'light_green'))
                    result = hashcracker.crack_hash(hash_to_crack)
                    print(colored(f"\n [RÉSULTAT] {result}", 'cyan'))
                    del sys.modules['hashcracker']
                except ImportError:
                    print(colored("[ERREUR] Le module hashcracker.py est introuvable.", 'red'))
                except Exception as e:
                    print(colored(f"[ERREUR] Une erreur est survenue dans le module : {e}", 'red'))

            elif module_name == "webscanner":
                try:
                    import webscanner
                    target_url = input(colored(" [ENTREZ L'URL OU L'IP CIBLE] > ", 'light_green'))
                    webscanner.scan_web_hardcore(target_url)
                    del sys.modules['webscanner']
                except ImportError:
                    print(colored("[ERREUR] Le module webscanner.py est introuvable.", 'red'))
                except Exception as e:
                    print(colored(f"[ERREUR] Une erreur est survenue dans le module : {e}", 'red'))

            elif module_name == "recon":
                try:
                    import recon
                    target = input(colored(" [ENTREZ L'IP OU L'URL CIBLE] > ", 'light_green'))
                    recon.perform_recon_hardcore(target)
                    del sys.modules['recon']
                except ImportError:
                    print(colored("[ERREUR] Le module recon.py est introuvable.", 'red'))
                except Exception as e:
                    print(colored(f"[ERREUR] Une erreur est survenue dans le module : {e}", 'red'))

            elif module_name == "osint":
                try:
                    import osint
                    target = input(colored(" [ENTREZ L'E-MAIL, L'URL OU L'IP CIBLE] > ", 'light_green'))
                    osint.perform_osint(target)
                    del sys.modules['osint']
                except ImportError:
                    print(colored("[ERREUR] Le module osint.py est introuvable.", 'red'))
                except Exception as e:
                    print(colored(f"[ERREUR] Une erreur est survenue dans le module : {e}", 'red'))

            elif module_name == "exit":
                print(colored("\n [SYSTÈME OFF] Extinction...", 'red'))
                sys.exit(0)

            elif module_name:
                print(colored(f"[INFO] Module '{module_name}' en cours de développement.", 'yellow'))
            elif not module_name:
                print(colored("[ERREUR] Choix invalide. Réessayez.", 'red'))

            input(colored("\n [RETOUR AU MENU] Appuyez sur Entrée ", 'yellow'))
            # L'effacement d'écran se fait au début de la boucle

    except KeyboardInterrupt:
        print(colored("\n [ABORT] Opération annulée.", 'yellow'))
        sys.exit(1)
    except Exception as e:
        print(colored(f"\n [CRITICAL ERROR] {e}", 'red'))

