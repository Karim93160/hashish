import os
import sys
import subprocess
import recon
import socket
import httpx
import shutil
from urllib.parse import urlparse, urlunparse
from termcolor import colored
import re

try:
    import recon
except ImportError:
    print(colored("[ERREUR] Le module 'recon.py' est introuvable. Certaines fonctions OSINT (WHOIS, GeoIP) seront indisponibles.", 'red'))
    recon = None

def run_command(command):
    try:
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
    print(colored(f"[OSINT] Recherche Shodan pour '{query}'...", 'cyan'))
    if not shutil.which('shodan'):
        print(colored("[ERREUR] L'outil shodan CLI n'est pas installé ou n'est pas dans votre PATH.", 'red'))
        print(colored("         Vous pouvez l'installer avec: pip install shodan", 'yellow'))
        print(colored("         Assurez-vous également d'initialiser votre clé API: shodan init <YOUR_API_KEY>", 'yellow'))
        return
    command = f"shodan search '{query}'"
    run_command(command)

def lookup_email_breaches(email):
    print(colored(f"[OSINT] Recherche de fuites pour l'e-mail '{email}' (via Have I Been Pwned)...", 'cyan'))
    api_url = f"https://haveibeenpwned.com/api/v3/breachedaccount/{email}"
    headers = {'User-Agent': 'Hashish-OSINT-Tool'}
    try:
        response = httpx.get(api_url, headers=headers, timeout=10)
        if response.status_code == 200:
            breaches = response.json()
            if breaches:
                print(colored(f"[RÉSULTATS] L'e-mail '{email}' a été trouvé dans les fuites suivantes:", 'red', attrs=['bold']))
                for breach in breaches:
                    print(f"  - Nom de la fuite: {breach.get('Title', 'N/A')} ({breach.get('Name', 'N/A')})")
                    print(f"    Date de la fuite: {breach.get('BreachDate', 'N/A')}")
                    print(f"    Description: {breach.get('Description', 'N/A')[:100]}...")
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
    except httpx.RequestError as e:
        print(colored(f"[ERREUR] Impossible de contacter Have I Been Pwned. Problème de connexion ou de réseau: {e}", 'red'))
    except Exception as e:
        print(colored(f"[ERREUR] Une erreur inattendue est survenue lors de la recherche de fuites: {e}", 'red'))

def perform_dns_recon(target):
    print(colored(f"[OSINT] Reconnaissance DNS avancée pour {target}...", 'cyan'))
    print(colored("  [INFO] Tentative de zone transfer (AXFR)...", 'yellow'))
    run_command(f"dig axfr {target}")
    record_types = ['A', 'AAAA', 'MX', 'NS', 'TXT', 'SOA', 'CNAME', 'SPF']
    for r_type in record_types:
        print(colored(f"  [INFO] Recherche de l'enregistrement DNS {r_type}...", 'yellow'))
        run_command(f"dig {r_type} {target} +short")
    try:
        socket.inet_aton(target)
        is_ip = True
    except socket.error:
        is_ip = False
    if is_ip:
        print(colored(f"  [INFO] Recherche DNS inversée (PTR) pour l'IP {target}...", 'yellow'))
        run_command(f"dig -x {target} +short")

def extract_emails_from_website(url):
    print(colored(f"[OSINT] Extraction d'e-mails depuis {url} (approche basique)...", 'cyan'))
    try:
        with httpx.Client(verify=False) as client:
            response = client.get(url, timeout=15, headers={'User-Agent': 'Hashish-Email-Extractor'})
        response.raise_for_status()
        content = response.text
        email_pattern = r'[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}'
        found_emails = re.findall(email_pattern, content)
        if found_emails:
            print(colored(f"[RÉSULTATS] E-mails trouvés sur {url}:", 'green'))
            for email in sorted(list(set(found_emails))):
                print(f"  - {email}")
        else:
            print(colored(f"[INFO] Aucun e-mail trouvé sur {url} avec l'approche basique.", 'yellow'))
    except httpx.RequestError as e:
        print(colored(f"[ERREUR] Impossible de récupérer le contenu de {url} pour l'extraction d'e-mails: {e}", 'red'))
    except Exception as e:
        print(colored(f"[ERREUR] Erreur inattendue lors de l'extraction d'e-mails: {e}", 'red'))

def perform_osint(target):
    print(colored(f"\n[OSINT HARDCORE] Démarrage de la collecte d'informations sur {target}...", 'magenta', attrs=['bold']))
    original_target = target
    parsed_url = urlparse(target)
    domain_or_ip = ""
    if parsed_url.netloc:
        domain_or_ip = parsed_url.netloc
    else:
        domain_or_ip = target
    resolved_ip = None
    if "." in domain_or_ip and not "@" in domain_or_ip and not domain_or_ip.replace('.', '').isdigit():
        try:
            resolved_ip = socket.gethostbyname(domain_or_ip)
            print(colored(f"[INFO] Cible '{domain_or_ip}' résolue en IP : {resolved_ip}", 'cyan'))
        except socket.gaierror:
            print(colored(f"[INFO] Impossible de résoudre l'adresse IP pour '{domain_or_ip}'.", 'yellow'))
    elif domain_or_ip.replace('.', '').isdigit():
        resolved_ip = domain_or_ip
        print(colored(f"[INFO] La cible est une IP : {resolved_ip}", 'cyan'))
    if domain_or_ip and not "@" in domain_or_ip:
        perform_dns_recon(domain_or_ip)
    else:
        print(colored("[INFO] Reconnaissance DNS ignorée (la cible n'est pas un domaine ou une IP).", 'yellow'))
    if recon:
        if not "@" in domain_or_ip and "." in domain_or_ip and not domain_or_ip.replace('.', '').isdigit():
            recon.perform_whois_lookup(domain_or_ip)
        else:
            print(colored("[INFO] WHOIS non applicable : la cible n'est pas un nom de domaine valide ou est déjà une IP/email.", 'yellow'))
        if resolved_ip:
            recon.perform_geoip_lookup(resolved_ip)
        else:
            print(colored("[INFO] GeoIP non applicable : Impossible de résoudre l'adresse IP ou la cible n'est pas une IP pure.", 'yellow'))
    else:
        print(colored("[INFO] Le module 'recon' n'est pas disponible, les fonctions WHOIS et GeoIP sont ignorées.", 'yellow'))
    if "@" in original_target:
        lookup_email_breaches(original_target)
    else:
        if domain_or_ip and not domain_or_ip.replace('.', '').isdigit():
            search_shodan(domain_or_ip)
        elif resolved_ip:
            search_shodan(resolved_ip)
        else:
            print(colored("[INFO] Impossible d'effectuer la recherche Shodan car la cible n'est ni un domaine ni une IP valide pour Shodan.", 'yellow'))
        if parsed_url.netloc:
            extract_emails_from_website(f"{parsed_url.scheme if parsed_url.scheme else 'http'}://{parsed_url.netloc}{parsed_url.path}")
            if parsed_url.scheme != 'https':
                extract_emails_from_website(f"https://{parsed_url.netloc}{parsed_url.path}")
        elif domain_or_ip and not domain_or_ip.replace('.', '').isdigit():
            extract_emails_from_website(f"http://{domain_or_ip}")
            extract_emails_from_website(f"https://{domain_or_ip}")
        else:
            print(colored("[INFO] L'extraction d'e-mails à partir de sites web est ignorée car la cible n'est pas une URL ou un domaine valide.", 'yellow'))
    print(colored(f"\n[OSINT HARDCORE] Collecte d'informations sur {original_target} terminée.", 'magenta', attrs=['bold']))

def perform_osint_main():
    print(colored("[MODULE] Lancement du module OSINT (HARDCORE)", 'yellow', attrs=['bold']))
    target = input(colored(" [ENTREZ L'E-MAIL, LE NOM DE DOMAINE OU L'URL CIBLE POUR L'OSINT] > ", 'light_green')).strip()
    if not target:
        print(colored("[INFO] Aucune cible spécifiée. Retour au menu principal.", 'yellow'))
        return
    perform_osint(target)
    print(colored("[MODULE] Collecte d'informations OSINT terminée.", 'cyan'))

if __name__ == "__main__":
    perform_osint_main()
