# hashcracker.py

import os
import sys
import time
import hashlib
import random
from termcolor import colored

def matrix_animation(duration=5):
    width = os.get_terminal_size().columns
    start_time = time.time()
    while time.time() - start_time < duration:
        columns = []
        for _ in range(width):
            if random.random() < 0.05:
                columns.append(colored(random.choice("01"), "green"))
            else:
                columns.append(" ")
        print("".join(columns), end='\r')
        time.sleep(0.03)
    print()

def identify_hash_type(hash_value):
    length = len(hash_value)
    if length == 32:
        return "MD5"
    elif length == 40:
        return "SHA1"
    elif length == 64:
        return "SHA256"
    elif length == 128:
        return "SHA512"
    else:
        return "Inconnu"

def crack_hash(hash_value):
    hash_type = identify_hash_type(hash_value)
    print(colored(f"[ANALYSE] Type de hash détecté : {hash_type}", 'cyan'))

    common_passwords = ["password", "123456", "admin", "secret", "root"]
    mutations = ["", "1", "!", "a", "A", "123"]

    print(colored("[BRUTEFORCE LOCAL] Tentative avec mots de passe courants...", 'yellow'))
    matrix_animation(3)
    for password in common_passwords:
        encoded = password.encode('utf-8')
        if hash_type == "MD5" and hashlib.md5(encoded).hexdigest() == hash_value:
            return colored(f"[CRACKED MD5] Mot de passe trouvé : {password}", 'green')
        elif hash_type == "SHA1" and hashlib.sha1(encoded).hexdigest() == hash_value:
            return colored(f"[CRACKED SHA1] Mot de passe trouvé : {password}", 'green')
        elif hash_type == "SHA256" and hashlib.sha256(encoded).hexdigest() == hash_value:
            return colored(f"[CRACKED SHA256] Mot de passe trouvé : {password}", 'green')
        elif hash_type == "SHA512" and hashlib.sha512(encoded).hexdigest() == hash_value:
            return colored(f"[CRACKED SHA512] Mot de passe trouvé : {password}", 'green')

    print(colored("[MUTATION] Tentative de mutations basiques...", 'yellow'))
    matrix_animation(5)
    for password in common_passwords:
        for mutation in mutations:
            mutated_password = password + mutation
            encoded = mutated_password.encode('utf-8')
            if hash_type == "MD5" and hashlib.md5(encoded).hexdigest() == hash_value:
                return colored(f"[CRACKED MD5] Mot de passe (muté) trouvé : {mutated_password}", 'green')
            elif hash_type == "SHA1" and hashlib.sha1(encoded).hexdigest() == hash_value:
                return colored(f"[CRACKED SHA1] Mot de passe (muté) trouvé : {mutated_password}", 'green')
            elif hash_type == "SHA256" and hashlib.sha256(encoded).hexdigest() == hash_value:
                return colored(f"[CRACKED SHA256] Mot de passe (muté) trouvé : {mutated_password}", 'green')
            elif hash_type == "SHA512" and hashlib.sha512(encoded).hexdigest() == hash_value:
                return colored(f"[CRACKED SHA512] Mot de passe (muté) trouvé : {mutated_password}", 'green')

    return colored("[ÉCHEC] Aucune correspondance locale trouvée.", 'red')

if __name__ == "__main__":
    print(colored("[MODULE TEST] Lancement direct du module hashcracker.py", 'yellow'))
    hash_to_crack = input(colored(" [ENTREZ LE HASH À DÉCRYPTER POUR TEST] > ", 'light_green'))
    result = crack_hash(hash_to_crack)
    print(colored(f"\n [RÉSULTAT DU TEST] {result}", 'cyan'))

