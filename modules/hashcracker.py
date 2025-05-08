import os
import sys
import time
import hashlib
import random
import glob
from termcolor import colored

# Configuration des chemins
MODULE_DIR = os.path.dirname(os.path.abspath(__file__))
COMMON_FILES = [f"common.part.{i:02d}" for i in range(1, 11)]  # common.part.01 à .10

def matrix_animation(duration=5):
    """Animation Matrix style"""
    width = os.get_terminal_size().columns
    start_time = time.time()
    while time.time() - start_time < duration:
        print("".join(
            colored(random.choice("01"), "green") if random.random() < 0.05 else " "
            for _ in range(width)
        ), end='\r')
        time.sleep(0.03)
    print()

def identify_hash_type(hash_value):
    """Identifie le type de hash"""
    hash_lengths = {
        32: "MD5",
        40: "SHA1",
        56: "SHA224",
        64: "SHA256",
        96: "SHA384",
        128: "SHA512"
    }
    return hash_lengths.get(len(hash_value), "Inconnu")

def load_common_files():
    """Charge tous les fichiers common.part"""
    dictionaries = []
    for common_file in COMMON_FILES:
        file_path = os.path.join(MODULE_DIR, common_file)
        try:
            with open(file_path, 'r', errors='ignore') as f:
                passwords = [line.strip() for line in f if line.strip()]
                dictionaries.extend(passwords)
                print(colored(f"[+] Chargé {len(passwords)} mots depuis {common_file}", 'blue'))
        except FileNotFoundError:
            print(colored(f"[!] Fichier {common_file} introuvable dans {MODULE_DIR}", 'yellow'))
        except Exception as e:
            print(colored(f"[!] Erreur lecture {common_file}: {str(e)}", 'red'))
    return dictionaries

def crack_hash(hash_value, wordlist=None):
    """Fonction principale de cracking"""
    hash_type = identify_hash_type(hash_value)
    print(colored(f"\n[ANALYSE] Type de hash: {hash_type}", 'cyan'))

    # Étape 1: Chargement des dictionnaires
    dictionaries = load_common_files()
    
    if wordlist:
        try:
            with open(wordlist, 'r', errors='ignore') as f:
                dictionaries.extend([line.strip() for line in f if line.strip()])
            print(colored(f"[+] {len(dictionaries)} mots chargés au total", 'blue'))
        except Exception as e:
            print(colored(f"[!] Erreur wordlist: {str(e)}", 'red'))

    # Étape 2: Vérification des dictionnaires
    if not dictionaries:
        print(colored("[!] Aucun mot de passe chargé", 'yellow'))
        return None

    print(colored(f"[ATTACK] Test de {len(dictionaries):,} mots...", 'yellow'))
    matrix_animation(3)

    # Étape 3: Recherche
    hash_func = {
        "MD5": hashlib.md5,
        "SHA1": hashlib.sha1,
        "SHA256": hashlib.sha256,
        "SHA512": hashlib.sha512
    }.get(hash_type)

    if not hash_func:
        return colored("[!] Algorithme non supporté", 'red')

    for password in dictionaries:
        if hash_func(password.encode()).hexdigest() == hash_value:
            return colored(f"[CRACKED] Mot de passe trouvé: {password}", 'green')

    # Étape 4: Mutations simples
    mutations = ["", "1", "!", "123", "2023"]
    print(colored("[MUTATION] Essai de mutations...", 'yellow'))
    
    for base in ["password", "admin", "root", "123456"] + dictionaries[:100]:  # Top 100 + bases
        for mut in mutations:
            trial = f"{base}{mut}"
            if hash_func(trial.encode()).hexdigest() == hash_value:
                return colored(f"[CRACKED] Mot de passe (muté) trouvé: {trial}", 'green')

    return colored("[FAIL] Aucune correspondance trouvée", 'red')

if __name__ == "__main__":
    print(colored("\n[TEST] Module HashCracker", 'yellow', attrs=['bold']))
    try:
        hash_input = input(colored(" [HASH] > ", 'light_green'))
        result = crack_hash(hash_input)
        print(colored(f"\n[RESULT] {result}\n", 'cyan'))
    except KeyboardInterrupt:
        print(colored("\n[ABORT] Opération annulée", 'red'))
