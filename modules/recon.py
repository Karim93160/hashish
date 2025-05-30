import os
import subprocess
import socket
import requests # Ajouté pour la fonction GeoIP
from urllib.parse import urlparse
from termcolor import colored
import sys # NOUVEL AJOUT : Nécessaire pour sys.platform

def which(program):
    """
    Vérifie si un programme est exécutable et se trouve dans le PATH.
    Similaire à la commande 'which' sous Linux/Unix.
    """
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

def perform_whois_lookup(target_domain):
    """Effectue une recherche WHOIS pour un nom de domaine."""
    print(colored(f"[RECON] Recherche WHOIS complète pour {target_domain}...", 'cyan'))
    try:
        # Ajout de timeout pour éviter que la commande ne bloque indéfiniment
        whois_info = subprocess.run(['whois', target_domain], capture_output=True, text=True, check=True, timeout=30)
        if whois_info.stdout:
            print(whois_info.stdout)
        else:
            print(colored(f"[INFO] Aucune information WHOIS trouvée pour {target_domain}.", 'yellow'))
    except subprocess.CalledProcessError as e:
        print(colored(f"[ERREUR] La commande 'whois' a échoué (code {e.returncode}): {e.stderr.strip()}", 'red'))
    except FileNotFoundError:
        print(colored("[ERREUR] La commande 'whois' n'a pas été trouvée. Veuillez l'installer (ex: apt install whois).", 'red'))
    except subprocess.TimeoutExpired:
        print(colored(f"[ERREUR] La commande 'whois' a expiré pour {target_domain}.", 'red'))
    except Exception as e:
        print(colored(f"[ERREUR] Une erreur inattendue est survenue lors de la recherche WHOIS: {e}", 'red'))

def perform_geoip_lookup(ip_address):
    """Recherche les informations GeoIP pour une adresse IP via une API publique."""
    print(colored(f"[RECON] Recherche GeoIP pour {ip_address}...", 'cyan'))
    api_url = f"http://ip-api.com/json/{ip_address}?fields=status,message,country,city,regionName,zip,lat,lon,isp,org,as,query"
    try:
        response = requests.get(api_url, timeout=5)
        if response.status_code == 200:
            data = response.json()
            if data.get('status') == 'success':
                print(colored(f"  Pays: {data.get('country')}", 'green'))
                print(colored(f"  Région: {data.get('regionName')}", 'green'))
                print(colored(f"  Ville: {data.get('city')}", 'green'))
                print(colored(f"  Code Postal: {data.get('zip')}", 'green'))
                print(colored(f"  Coordonnées: {data.get('lat')}, {data.get('lon')}", 'green'))
                print(colored(f"  ISP: {data.get('isp')}", 'green'))
                print(colored(f"  Organisation: {data.get('org')}", 'green'))
                print(colored(f"  ASN: {data.get('as')}", 'green'))
                print(colored(f"  IP Requêtée: {data.get('query')}", 'green'))
            else:
                print(colored(f"  [INFO] GeoIP API message: {data.get('message', 'Données non disponibles')}", 'yellow'))
        else:
            print(colored(f"[ERREUR] Erreur lors de la requête GeoIP: {response.status_code}", 'red'))
    except requests.exceptions.RequestException as e:
        print(colored(f"[ERREUR] Impossible de contacter l'API GeoIP: {e}", 'red'))
    except Exception as e:
        print(colored(f"[ERREUR] Erreur lors de la recherche GeoIP: {e}", 'red'))

def perform_recon_hardcore(target):
    """Lance une reconnaissance avancée sur la cible."""
    print(colored(f"[RECON HARDCORE] Démarrage de la reconnaissance avancée sur {target}...", 'yellow', attrs=['bold']))

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
        return # Si on ne peut pas résoudre l'IP, beaucoup de fonctions ne fonctionneront pas

    # Recherche DNS détaillée (nslookup)
    if domain:
        print(colored(f"[RECON] Recherche DNS détaillée (nslookup) pour {domain}...", 'cyan'))
        try:
            # Utilise 'nslookup -type=any' pour obtenir plus d'informations
            nslookup_info = subprocess.run(['nslookup', '-type=any', domain], capture_output=True, text=True, check=True, timeout=20)
            print(nslookup_info.stdout)
        except subprocess.CalledProcessError as e:
            print(colored(f"[ERREUR] La commande nslookup a échoué (code {e.returncode}): {e.stderr.strip()}", 'red'))
        except FileNotFoundError:
            print(colored("[ERREUR] La commande 'nslookup' n'a pas été trouvée. Veuillez l'installer.", 'red'))
        except subprocess.TimeoutExpired:
            print(colored(f"[ERREUR] La commande 'nslookup' a expiré pour {domain}.", 'red'))
        except Exception as e:
            print(colored(f"[ERREUR] Une erreur inattendue est survenue lors de nslookup: {e}", 'red'))

    # Recherche WHOIS
    if domain:
        perform_whois_lookup(domain)

    # Traceroute
    # Vérifie si 'traceroute' (Linux) ou 'tracert' (Windows) est disponible
    traceroute_cmd = None
    if sys.platform.startswith('linux') or sys.platform == 'darwin': # Linux ou macOS
        if which("traceroute"):
            traceroute_cmd = ['traceroute', '-I', '-n', '-w', '1']
        elif which("tracepath"): # Alternative Linux
            traceroute_cmd = ['tracepath']
    elif sys.platform == 'win32': # Windows
        if which("tracert"):
            traceroute_cmd = ['tracert']
    
    if ip_address and traceroute_cmd:
        print(colored(f"[RECON] Traceroute avancé vers {ip_address}...", 'cyan'))
        try:
            # Ajoute l'IP à la commande
            full_command = traceroute_cmd + [ip_address]
            traceroute_info = subprocess.run(full_command, capture_output=True, text=True, check=True, timeout=60)
            print(traceroute_info.stdout)
        except subprocess.CalledProcessError as e:
            print(colored(f"[ERREUR] La commande traceroute/tracert a échoué (code {e.returncode}): {e.stderr.strip()}", 'red'))
        except FileNotFoundError:
            print(colored(f"[ERREUR] La commande '{traceroute_cmd[0]}' n'a pas été trouvée. Installez-la (ex: apt install iputils-traceroute ou termux-setup-storage && pkg install traceroute).", 'red'))
        except subprocess.TimeoutExpired:
            print(colored(f"[ERREUR] La commande traceroute/tracert a expiré pour {ip_address}.", 'red'))
        except Exception as e:
            print(colored(f"[ERREUR] Une erreur inattendue est survenue lors du traceroute: {e}", 'red'))
    elif not traceroute_cmd:
        print(colored("[WARNING] Aucune commande de traceroute (traceroute/tracepath/tracert) n'est installée ou trouvée dans le PATH.", 'yellow'))
    else:
        print(colored("[INFO] Impossible de lancer traceroute car l'IP n'a pas été résolue.", 'yellow'))

    # Scan de ports (TCP connect scan)
    if ip_address and which("nmap"):
        print(colored(f"[RECON] Scan de ports TCP connect sur {ip_address} (top 100)...", 'cyan'))
        try:
            # Ajout de -sV pour la détection de version et un timeout plus long
            nmap_info = subprocess.run(['nmap', '-sT', '-sV', '-T4', '--top-ports', '100', ip_address], capture_output=True, text=True, check=True, timeout=120)
            print(nmap_info.stdout)
        except subprocess.CalledProcessError as e:
            print(colored(f"[ERREUR] La commande nmap a échoué (code {e.returncode}): {e.stderr.strip()}", 'red'))
        except FileNotFoundError:
            print(colored("[ERREUR] La commande 'nmap' n'a pas été trouvée. Veuillez l'installer (ex: apt install nmap).", 'red'))
        except subprocess.TimeoutExpired:
            print(colored(f"[ERREUR] La commande nmap a expiré pour {ip_address}.", 'red'))
        except Exception as e:
            print(colored(f"[ERREUR] Une erreur inattendue est survenue lors du scan Nmap: {e}", 'red'))
    elif not which("nmap"):
        print(colored("[WARNING] La commande 'nmap' n'est pas installée. Le scan de ports est ignoré.", 'yellow'))
    else:
        print(colored("[INFO] Impossible de lancer le scan de ports car l'IP n'a pas été résolue.", 'yellow'))

    # Recherche GeoIP
    if ip_address:
        perform_geoip_lookup(ip_address)
    else:
        print(colored("[INFO] Impossible d'effectuer la recherche GeoIP car l'IP n'a pas été résolue.", 'yellow'))

    print(colored(f"[RECON HARDCORE] Reconnaissance avancée sur {target} terminée.", 'green', attrs=['bold']))

def perform_recon_hardcore_main():
    """Fonction principale pour l'exécution du module Reconnaissance."""
    print(colored("\n[MODULE] Lancement du module Reconnaissance (HARDCORE)", 'yellow', attrs=['bold']))
    target_url = input(colored(" [ENTREZ L'IP OU L'URL CIBLE POUR LA RECONNAISSANCE] > ", 'light_green')).strip()
    if not target_url:
        print(colored("[INFO] Aucune cible spécifiée. Retour au menu principal.", 'yellow'))
        return
    perform_recon_hardcore(target_url)
    print(colored("[MODULE] Reconnaissance terminée.", 'cyan'))

if __name__ == '__main__':
    perform_recon_hardcore_main()
