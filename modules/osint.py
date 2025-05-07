# osint.py

import os
import sys
import subprocess
import socket
import requests
from termcolor import colored

def search_shodan(query):
    """Effectue une recherche sur Shodan (nécessite une clé API et l'outil shodan CLI)."""
    print(colored(f"[OSINT] Recherche Shodan pour '{query}'...", 'cyan'))
    if not shutil.which('shodan'):
        print(colored("[ERREUR] L'outil shodan CLI n'est pas installé.", 'red'))
        print(colored("         Vous pouvez l'installer avec: pip install shodan", 'yellow'))
        return
    # Assurez-vous que la clé API Shodan est configurée (via 'shodan init <API_KEY>')
    command = f"shodan search '{query}'"
    run_command(command)

def lookup_email_breaches(email):
    """Recherche les fuites de données associées à une adresse e-mail (via une API publique)."""
    print(colored(f"[OSINT] Recherche de fuites pour l'e-mail '{email}'...", 'cyan'))
    api_url = f"https://haveibeenpwned.com/api/v3/breachedaccount/{email}"
    headers = {'User-Agent': 'Hashish-OSINT-Tool'}
    try:
        response = requests.get(api_url, headers=headers, timeout=10)
        if response.status_code == 200:
            breaches = response.json()
            if breaches:
                print(colored(f"[RÉSULTATS] L'e-mail '{email}' a été trouvé dans les fuites suivantes:", 'red'))
                for breach in breaches:
                    print(f"  - {breach['Name']} ({breach['BreachDate']})")
            else:
                print(colored(f"[INFO] L'e-mail '{email}' n'a pas été trouvé dans les fuites publiques.", 'green'))
        elif response.status_code == 404:
            print(colored(f"[INFO] L'e-mail '{email}' n'a pas été trouvé dans les fuites publiques.", 'green'))
        elif response.status_code == 429:
            print(colored("[WARNING] Trop de requêtes vers Have I Been Pwned. Ralentissez.", 'yellow'))
        else:
            print(colored(f"[ERREUR] Erreur lors de la requête à Have I Been Pwned: {response.status_code}", 'red'))
    except requests.exceptions.RequestException as e:
        print(colored(f"[ERREUR] Impossible de contacter Have I Been Pwned: {e}", 'red'))

def perform_dns_recon(target):
    """Effectue des requêtes DNS avancées (zone transfer si possible, enregistrements spécifiques)."""
    print(colored(f"[OSINT] Reconnaissance DNS avancée pour {target}...", 'cyan'))
    # Tentative de zone transfer (très rare que cela fonctionne)
    command_axfr = f"dig axfr {target}"
    print(colored("  [INFO] Tentative de zone transfer (AXFR)...", 'yellow'))
    run_command(command_axfr)
    # Recherche d'enregistrements DNS spécifiques (A, MX, NS, TXT, SOA)
    record_types = ['A', 'MX', 'NS', 'TXT', 'SOA']
    for r_type in record_types:
        command_dig = f"dig {r_type} {target}"
        print(colored(f"  [INFO] Recherche de l'enregistrement DNS {r_type}...", 'yellow'))
        run_command(command_dig)

def extract_emails_from_website(url):
    """Tente d'extraire les adresses e-mail d'une page web (approche basique avec grep)."""
    print(colored(f"[OSINT] Extraction d'e-mails depuis {url} (approche basique)...", 'cyan'))
    command = f"curl -s '{url}' | grep -oE '[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}'"
    run_command(command)

def perform_osint(target):
    """Orchestre les différentes étapes d'OSINT."""
    print(colored(f"[OSINT HARDCORE] Démarrage de la collecte d'informations sur {target}...", 'magenta', attrs=['bold']))
    perform_dns_recon(target)
    perform_whois_lookup(target) # On réutilise le WHOIS du module recon
    try:
        ip_address = socket.gethostbyname(target)
        # perform_geoip_lookup(ip_address) # On réutilise GeoIP si 'requests' est OK
    except socket.gaierror:
        print(colored("[INFO] Impossible de résoudre l'IP pour GeoIP.", 'yellow'))

    # Si la cible ressemble à un e-mail
    if "@" in target:
        lookup_email_breaches(target)
    # Si la cible ressemble à un nom de domaine ou une IP (pour Shodan)
    else:
        search_shodan(target)
        extract_emails_from_website(f"http://{target}")
        extract_emails_from_website(f"https://{target}")

    print(colored(f"[OSINT HARDCORE] Collecte d'informations sur {target} terminée.", 'magenta', attrs=['bold']))

def run_command(command):
    """Exécute une commande système et gère les erreurs."""
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(colored(f"[ERREUR] La commande a échoué: {e}", 'red'))
    except FileNotFoundError:
        print(colored(f"[ERREUR] Commande introuvable ou non installée.", 'red'))

if __name__ == "__main__":
    print(colored("[MODULE TEST] Lancement direct du module osint.py (HARDCORE)", 'yellow'))
    target = input(colored(" [ENTREZ L'E-MAIL, L'URL OU L'IP CIBLE POUR L'OSINT] > ", 'light_green'))
    perform_osint(target)
    print(colored("[MODULE TEST] Collecte d'informations OSINT terminée.", 'cyan'))

