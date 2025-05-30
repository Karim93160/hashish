import os
import sys
import subprocess
import recon
import socket
import requests
import shutil # Nécessaire pour shutil.which
from urllib.parse import urlparse, urlunparse # NOUVEL AJOUT : urlunparse
from termcolor import colored
import re # NOUVEL AJOUT : Nécessaire pour l'extraction d'e-mails

# Importe le module recon pour utiliser ses fonctions comme WHOIS et GeoIP
try:
    import recon
except ImportError:
    print(colored("[ERREUR] Le module 'recon.py' est introuvable. Certaines fonctions OSINT (WHOIS, GeoIP) seront indisponibles.", 'red'))
    recon = None # Marque recon comme non disponible si l'import échoue

def run_command(command):
    """Exécute une commande système et gère les erreurs."""
    try:
        # Ajout d'un timeout pour les commandes subprocess
        result = subprocess.run(command, shell=True, check=True, capture_output=True, text=True, timeout=30)
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(colored(f"[INFO COMMANDE] Erreur standard : {result.stderr.strip()}", 'yellow'))
    except subprocess.CalledProcessError as e:
        print(colored(f"[ERREUR] La commande a échoué (code {e.returncode}): {e.stderr.strip()}", 'red'))
    except FileNotFoundError:
        print(colored(f"[ERREUR] Commande introuvable ou non installée : '{command.split(' ')[0]}'.", 'red'))
    except subprocess.TimeoutExpired:
        print(colored(f"[ERREUR] La commande a expiré après 30 secondes.", 'red'))
    except Exception as e:
        print(colored(f"[ERREUR] Erreur inattendue lors de l'exécution de la commande : {e}", 'red'))

def search_shodan(query):
    """Effectue une recherche sur Shodan (nécessite une clé API et l'outil shodan CLI)."""
    print(colored(f"[OSINT] Recherche Shodan pour '{query}'...", 'cyan'))
    if not shutil.which('shodan'): # Utilise shutil.which pour vérifier si 'shodan' est dans le PATH
        print(colored("[ERREUR] L'outil shodan CLI n'est pas installé ou n'est pas dans votre PATH.", 'red'))
        print(colored("         Vous pouvez l'installer avec: pip install shodan", 'yellow'))
        print(colored("         Assurez-vous également d'initialiser votre clé API: shodan init <YOUR_API_KEY>", 'yellow'))
        return
    # Assurez-vous que la clé API Shodan est configurée (via 'shodan init <API_KEY>')
    command = f"shodan search '{query}'"
    run_command(command)

def lookup_email_breaches(email):
    """Recherche les fuites de données associées à une adresse e-mail (via Have I Been Pwned API)."""
    print(colored(f"[OSINT] Recherche de fuites pour l'e-mail '{email}' (via Have I Been Pwned)...", 'cyan'))
    api_url = f"https://haveibeenpwned.com/api/v3/breachedaccount/{email}"
    headers = {'User-Agent': 'Hashish-OSINT-Tool'} # Un User-Agent est requis par l'API
    try:
        response = requests.get(api_url, headers=headers, timeout=10)
        if response.status_code == 200:
            breaches = response.json()
            if breaches:
                print(colored(f"[RÉSULTATS] L'e-mail '{email}' a été trouvé dans les fuites suivantes:", 'red', attrs=['bold']))
                for breach in breaches:
                    # Affiche des informations plus détaillées sur la fuite
                    print(f"  - Nom de la fuite: {breach.get('Title', 'N/A')} ({breach.get('Name', 'N/A')})")
                    print(f"    Date de la fuite: {breach.get('BreachDate', 'N/A')}")
                    print(f"    Description: {breach.get('Description', 'N/A')[:100]}...") # Tronque la description
                    print(f"    Données compromises: {', '.join(breach.get('DataClasses', ['N/A']))}")
                    print("-" * 30)
            else:
                print(colored(f"[INFO] L'e-mail '{email}' n'a pas été trouvé dans les fuites publiques connues.", 'green'))
        elif response.status_code == 404:
            print(colored(f"[INFO] L'e-mail '{email}' n'a pas été trouvé dans les fuites publiques connues.", 'green'))
        elif response.status_code == 400:
            print(colored(f"[ERREUR] Requête Have I Been Pwned invalide pour '{email}'. Vérifiez le format de l'e-mail.", 'red'))
        elif response.status_code == 429:
            print(colored("[WARNING] Trop de requêtes vers Have I Been Pwned (Rate Limit). Veuillez attendre un moment avant de réessayer.", 'yellow'))
        else:
            print(colored(f"[ERREUR] Erreur inattendue lors de la requête à Have I Been Pwned: {response.status_code} - {response.text}", 'red'))
    except requests.exceptions.RequestException as e:
        print(colored(f"[ERREUR] Impossible de contacter Have I Been Pwned. Problème de connexion ou de réseau: {e}", 'red'))
    except Exception as e:
        print(colored(f"[ERREUR] Une erreur inattendue est survenue lors de la recherche de fuites: {e}", 'red'))


def perform_dns_recon(target):
    """Effectue des requêtes DNS avancées (zone transfer si possible, enregistrements spécifiques)."""
    print(colored(f"[OSINT] Reconnaissance DNS avancée pour {target}...", 'cyan'))
    # Tentative de zone transfer (très rare que cela fonctionne sur des serveurs configurés correctement)
    print(colored("  [INFO] Tentative de zone transfer (AXFR)...", 'yellow'))
    run_command(f"dig axfr {target}")
    
    # Recherche d'enregistrements DNS spécifiques (A, AAAA, MX, NS, TXT, SOA, CNAME, SPF)
    record_types = ['A', 'AAAA', 'MX', 'NS', 'TXT', 'SOA', 'CNAME', 'SPF']
    for r_type in record_types:
        print(colored(f"  [INFO] Recherche de l'enregistrement DNS {r_type}...", 'yellow'))
        run_command(f"dig {r_type} {target} +short") # +short pour un output plus concis
    
    # Recherche inversée (PTR) si la cible est une IP
    try:
        # Vérifie si target est une IP valide
        socket.inet_aton(target) # Tente de convertir en format binaire IPv4
        is_ip = True
    except socket.error:
        is_ip = False
    
    if is_ip:
        print(colored(f"  [INFO] Recherche DNS inversée (PTR) pour l'IP {target}...", 'yellow'))
        run_command(f"dig -x {target} +short")


def extract_emails_from_website(url):
    """Tente d'extraire les adresses e-mail d'une page web."""
    print(colored(f"[OSINT] Extraction d'e-mails depuis {url} (approche basique)...", 'cyan'))
    
    try:
        response = requests.get(url, timeout=15, headers={'User-Agent': 'Hashish-Email-Extractor'}, verify=False)
        response.raise_for_status() # Lève une exception pour les codes d'erreur HTTP
        content = response.text
        
        # Utilise re pour trouver les e-mails
        email_pattern = r'[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}'
        found_emails = re.findall(email_pattern, content)
        
        if found_emails:
            print(colored(f"[RÉSULTATS] E-mails trouvés sur {url}:", 'green'))
            for email in sorted(list(set(found_emails))): # Supprime les doublons et trie
                print(f"  - {email}")
        else:
            print(colored(f"[INFO] Aucun e-mail trouvé sur {url} avec l'approche basique.", 'yellow'))
            
    except requests.exceptions.RequestException as e:
        print(colored(f"[ERREUR] Impossible de récupérer le contenu de {url} pour l'extraction d'e-mails: {e}", 'red'))
    except Exception as e:
        print(colored(f"[ERREUR] Erreur inattendue lors de l'extraction d'e-mails: {e}", 'red'))


def perform_osint(target):
    """Orchestre les différentes étapes d'OSINT."""
    print(colored(f"\n[OSINT HARDCORE] Démarrage de la collecte d'informations sur {target}...", 'magenta', attrs=['bold']))
    
    # Normalisation de la cible
    original_target = target
    parsed_url = urlparse(target)
    domain_or_ip = ""
    
    # Prioriser le netloc si c'est une URL complète, sinon la cible brute
    if parsed_url.netloc:
        domain_or_ip = parsed_url.netloc
    else:
        domain_or_ip = target

    # Tente d'obtenir une adresse IP résolue pour la cible primaire (si c'est un domaine)
    resolved_ip = None
    if "." in domain_or_ip and not "@" in domain_or_ip and not domain_or_ip.replace('.', '').isdigit():
        try:
            resolved_ip = socket.gethostbyname(domain_or_ip)
            print(colored(f"[INFO] Cible '{domain_or_ip}' résolue en IP : {resolved_ip}", 'cyan'))
        except socket.gaierror:
            print(colored(f"[INFO] Impossible de résoudre l'adresse IP pour '{domain_or_ip}'.", 'yellow'))
    elif domain_or_ip.replace('.', '').isdigit(): # Si la cible est déjà une IP
        resolved_ip = domain_or_ip
        print(colored(f"[INFO] La cible est une IP : {resolved_ip}", 'cyan'))

    # Étape 1: Reconnaissance DNS avancée sur le domaine/IP pur
    if domain_or_ip and not "@" in domain_or_ip: # Ne pas faire de DNS recon sur un email
        perform_dns_recon(domain_or_ip)
    else:
        print(colored("[INFO] Reconnaissance DNS ignorée (la cible n'est pas un domaine ou une IP).", 'yellow'))
    
    # Étape 2: Recherche WHOIS et GeoIP (via le module recon si disponible)
    if recon:
        # WHOIS pour le domaine pur
        if not "@" in domain_or_ip and "." in domain_or_ip and not domain_or_ip.replace('.', '').isdigit(): # Est un domaine et non une IP pure ou un email
            recon.perform_whois_lookup(domain_or_ip) 
        else:
            print(colored("[INFO] WHOIS non applicable : la cible n'est pas un nom de domaine valide ou est déjà une IP/email.", 'yellow'))
        
        # GeoIP pour l'IP résolue ou la cible si c'est déjà une IP
        if resolved_ip:
            recon.perform_geoip_lookup(resolved_ip)
        else:
            print(colored("[INFO] GeoIP non applicable : Impossible de résoudre l'adresse IP ou la cible n'est pas une IP pure.", 'yellow'))
    else:
        print(colored("[INFO] Le module 'recon' n'est pas disponible, les fonctions WHOIS et GeoIP sont ignorées.", 'yellow'))

    # Étape 3: Actions spécifiques selon le type de cible
    if "@" in original_target: # Si la cible ressemble à un e-mail
        lookup_email_breaches(original_target)
    else: # Si la cible ressemble à un nom de domaine ou une IP
        # Recherche Shodan sur le domaine/IP pur
        if domain_or_ip and not domain_or_ip.replace('.', '').isdigit(): # Shodan plus pertinent pour les domaines/IPs, pas les URLs complètes ou purement des emails
            search_shodan(domain_or_ip)
        elif resolved_ip: # Si la cible est une IP purement numérique
            search_shodan(resolved_ip)
        else:
            print(colored("[INFO] Impossible d'effectuer la recherche Shodan car la cible n'est ni un domaine ni une IP valide pour Shodan.", 'yellow'))
        
        # Tente d'extraire des e-mails depuis les versions HTTP et HTTPS
        # S'applique uniquement si la cible est une URL (avec schéma et netloc) ou un nom de domaine
        if parsed_url.netloc: # Si l'entrée était une URL, utilise l'URL complète
            extract_emails_from_website(f"{parsed_url.scheme if parsed_url.scheme else 'http'}://{parsed_url.netloc}{parsed_url.path}")
            # Tente aussi HTTPS si l'URL d'origine n'était pas HTTPS
            if parsed_url.scheme != 'https':
                extract_emails_from_website(f"https://{parsed_url.netloc}{parsed_url.path}")
        elif domain_or_ip and not domain_or_ip.replace('.', '').isdigit(): # Si c'est un domaine pur (pas une IP)
            extract_emails_from_website(f"http://{domain_or_ip}")
            extract_emails_from_website(f"https://{domain_or_ip}")
        else:
            print(colored("[INFO] L'extraction d'e-mails à partir de sites web est ignorée car la cible n'est pas une URL ou un domaine valide.", 'yellow'))

    print(colored(f"\n[OSINT HARDCORE] Collecte d'informations sur {original_target} terminée.", 'magenta', attrs=['bold']))

def perform_osint_main():
    """Fonction principale pour l'exécution du module OSINT."""
    print(colored("[MODULE] Lancement du module OSINT (HARDCORE)", 'yellow', attrs=['bold']))
    target = input(colored(" [ENTREZ L'E-MAIL, LE NOM DE DOMAINE OU L'URL CIBLE POUR L'OSINT] > ", 'light_green')).strip()
    if not target:
        print(colored("[INFO] Aucune cible spécifiée. Retour au menu principal.", 'yellow'))
        return
    perform_osint(target)
    print(colored("[MODULE] Collecte d'informations OSINT terminée.", 'cyan'))

if __name__ == "__main__":
    perform_osint_main()
