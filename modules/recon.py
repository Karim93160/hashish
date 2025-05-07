# recon.py
import os
import subprocess
import socket
from urllib.parse import urlparse
from termcolor import colored

def perform_recon_hardcore(target):
    print(colored(f"[RECON HARDCORE] Démarrage de la reconnaissance avancée sur {target}...", 'yellow'))

    domain = ""
    ip_address = None

    # Extraction du nom de domaine
    parsed_url = urlparse(target)
    if parsed_url.netloc:
        domain = parsed_url.netloc
        print(colored(f"[RECON] Nom de domaine extrait : {domain}", 'cyan'))
    else:
        domain = target
        print(colored(f"[RECON] Cible considérée comme un nom de domaine/IP : {domain}", 'cyan'))

    # Recherche DNS
    print(colored(f"[RECON] Recherche DNS pour {domain}...", 'cyan'))
    try:
        ip_address = socket.gethostbyname(domain)
        print(colored(f"[RECON] Adresse IP de {domain}: {ip_address}", 'cyan'))
    except socket.gaierror:
        print(colored(f"[ERREUR] Impossible de résoudre l'adresse pour {domain}.", 'red'))

    # Recherche DNS détaillée (nslookup)
    if domain:
        print(colored(f"[RECON] Recherche DNS détaillée (nslookup) pour {domain}...", 'cyan'))
        try:
            nslookup_info = subprocess.run(['nslookup', domain], capture_output=True, text=True, check=True)
            print(nslookup_info.stdout)
        except subprocess.CalledProcessError as e:
            print(colored(f"[ERREUR] La commande nslookup a échoué: {e}", 'red'))
        except FileNotFoundError:
            print(colored("[ERREUR] La commande 'nslookup' n'a pas été trouvée.", 'red'))

    # Recherche WHOIS
    if domain:
        print(colored(f"[RECON] Recherche WHOIS complète pour {domain}...", 'cyan'))
        try:
            whois_info = subprocess.run(['whois', domain], capture_output=True, text=True, check=True)
            print(whois_info.stdout)
        except subprocess.CalledProcessError as e:
            print(colored(f"[ERREUR] La commande whois a échoué: {e}", 'red'))
        except FileNotFoundError:
            print(colored("[ERREUR] La commande 'whois' n'a pas été trouvée.", 'red'))

    # Traceroute
    if ip_address and which("traceroute"):
        print(colored(f"[RECON] Traceroute avancé vers {ip_address}...", 'cyan'))
        try:
            traceroute_info = subprocess.run(['traceroute', '-I', '-n', '-w', '1', ip_address], capture_output=True, text=True, check=True)
            print(traceroute_info.stdout)
        except subprocess.CalledProcessError as e:
            print(colored(f"[ERREUR] La commande traceroute a échoué: {e}", 'red'))
        except FileNotFoundError:
            print(colored("[ERREUR] La commande 'traceroute' n'a pas été trouvée.", 'red'))
    elif not which("traceroute"):
        print(colored("[WARNING] La commande 'traceroute' n'est pas installée.", 'yellow'))
    else:
        print(colored("[INFO] Impossible de lancer traceroute car l'IP n'a pas été résolue.", 'yellow'))

    # Scan de ports (TCP connect scan)
    if ip_address and which("nmap"):
        print(colored(f"[RECON] Scan de ports TCP connect sur {ip_address} (top 100)...", 'cyan'))
        try:
            nmap_info = subprocess.run(['nmap', '-sT', '-T4', '--top-ports', '100', ip_address], capture_output=True, text=True, check=True)
            print(nmap_info.stdout)
        except subprocess.CalledProcessError as e:
            print(colored(f"[ERREUR] La commande nmap a échoué: {e}", 'red'))
        except FileNotFoundError:
            print(colored("[ERREUR] La commande 'nmap' n'a pas été trouvée.", 'red'))
    elif not which("nmap"):
        print(colored("[WARNING] La commande 'nmap' n'est pas installée.", 'yellow'))
    else:
        print(colored("[INFO] Impossible de lancer le scan de ports car l'IP n'a pas été résolue.", 'yellow'))

    # Recherche GeoIP (nécessite l'IP)
    if ip_address:
        print(colored(f"[RECON] Recherche GeoIP pour {ip_address}...", 'cyan'))
        try:
            # Remplacer ceci par ta logique de recherche GeoIP (API ou librairie)
            print(colored("[INFO] Fonctionnalité GeoIP (à implémenter).", 'yellow'))
        except Exception as e:
            print(colored(f"[ERREUR] Erreur lors de la recherche GeoIP: {e}", 'red'))
    else:
        print(colored("[INFO] Impossible d'effectuer la recherche GeoIP car l'IP n'a pas été résolue.", 'yellow'))

    print(colored(f"[RECON HARDCORE] Reconnaissance avancée sur {target} terminée.", 'green'))

def which(program):
    def is_executable(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_executable(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            exe_file = os.path.join(path, program)
            if is_executable(exe_file):
                return exe_file
    return None

if __name__ == '__main__':
    target_url = input(colored(" [ENTREZ L'IP OU L'URL CIBLE] > ", 'light_green'))
    perform_recon_hardcore(target_url)

