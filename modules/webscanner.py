# webscanner.py

import os
import sys
import time
import requests
from termcolor import colored
from urllib.parse import urljoin

def scan_headers(url):
    """Récupère et analyse les en-têtes HTTP."""
    try:
        response = requests.head(url, allow_redirects=True, timeout=5, headers={'User-Agent': 'Hashish Web Scanner'})
        print(colored(f"[EN-TÊTES HTTP pour {url}]", 'cyan'))
        for header, value in response.headers.items():
            print(f"  {colored(header + ':', 'yellow')} {value}")
        # Analyse basique des en-têtes pour des indices de sécurité
        server = response.headers.get('Server', 'Inconnu')
        if 'nginx' in server.lower():
            print(colored("  [INFO] Serveur web: Nginx", 'green'))
        elif 'apache' in server.lower():
            print(colored("  [INFO] Serveur web: Apache", 'green'))
        security_headers = ['Strict-Transport-Security', 'Content-Security-Policy', 'X-Frame-Options', 'X-Content-Type-Options', 'X-XSS-Protection']
        for header in security_headers:
            if header not in response.headers:
                print(colored(f"  [WARNING] En-tête de sécurité manquant: {header}", 'yellow'))
        return response.headers
    except requests.exceptions.RequestException as e:
        print(colored(f"[ERREUR] Impossible de récupérer les en-têtes pour {url}: {e}", 'red'))
        return None

def advanced_tech_detection(headers, url, content=""):
    """Détection de technologies web plus avancée."""
    print(colored("\n[DÉTECTION DE TECHNOLOGIES]", 'cyan'))
    server = headers.get('Server', '').lower()
    powered_by = headers.get('X-Powered-By', '').lower()
    x_generator = headers.get('X-Generator', '').lower()
    set_cookie = headers.get('Set-Cookie', '').lower()

    # Détection basée sur les en-têtes
    if 'php' in powered_by:
        print(colored("  [TECH] Probablement PHP.", 'green'))
    if 'wordpress' in powered_by or 'wordpress' in server or 'wordpress' in x_generator or 'wordpress_' in set_cookie:
        print(colored("  [TECH] Probablement WordPress.", 'green'))
    if 'drupal' in powered_by or 'drupal' in server or 'drupal' in x_generator or 'drupal_' in set_cookie:
        print(colored("  [TECH] Probablement Drupal.", 'green'))
    if 'joomla' in x_generator or 'joomla_' in set_cookie:
        print(colored("  [TECH] Probablement Joomla!.", 'green'))
    if 'nginx' in server:
        print(colored("  [TECH] Serveur: Nginx.", 'green'))
    if 'apache' in server:
        print(colored("  [TECH] Serveur: Apache.", 'green'))
    if 'asp.net' in powered_by or 'microsoft-iis' in server:
        print(colored("  [TECH] Probablement ASP.NET (IIS).", 'green'))

    # Détection basée sur le contenu HTML
    if content:
        if '<meta name="generator" content="WordPress' in content.lower():
            print(colored("  [TECH] Probablement WordPress (via meta).", 'green'))
        if '<meta name="generator" content="Drupal' in content.lower():
            print(colored("  [TECH] Probablement Drupal (via meta).", 'green'))
        if '<meta name="generator" content="Joomla!' in content.lower():
            print(colored("  [TECH] Probablement Joomla! (via meta).", 'green'))
        if 'wp-content' in content.lower():
            print(colored("  [TECH] Indices de WordPress (via contenu).", 'yellow'))
        if '/modules/mod_' in content.lower() or '/components/com_' in content.lower():
            print(colored("  [TECH] Indices de Joomla! (via contenu).", 'yellow'))

def check_robots_txt(url):
    """Vérifie et affiche le contenu de robots.txt."""
    robots_url = urljoin(url, '/robots.txt')
    try:
        response = requests.get(robots_url, timeout=5, headers={'User-Agent': 'Hashish Web Scanner'})
        if response.status_code == 200:
            print(colored(f"\n[CONTENU de {robots_url}]", 'cyan'))
            print(response.text)
        elif response.status_code == 404:
            print(colored(f"\n[INFO] Fichier {robots_url} non trouvé.", 'yellow'))
        else:
            print(colored(f"[INFO] Statut de {robots_url}: {response.status_code}", 'yellow'))
    except requests.exceptions.RequestException as e:
        print(colored(f"[ERREUR] Impossible de récupérer {robots_url}: {e}", 'red'))

def check_sensitive_files(url):
    """Vérifie la présence de fichiers sensibles courants (approche prudente)."""
    sensitive_paths = ['.env', '.git/config', 'config.php', 'backup.sql', 'wp-config.php', '.htaccess', 'web.config']
    print(colored("\n[VÉRIFICATION DE FICHIERS SENSIBLES]", 'cyan'))
    for path in sensitive_paths:
        check_url = urljoin(url, path)
        try:
            response = requests.head(check_url, timeout=3, headers={'User-Agent': 'Hashish Web Scanner'}) # HEAD request
            if response.status_code == 200:
                print(colored(f"  [POTENTIELLEMENT SENSIBLE] {check_url} (Code: {response.status_code})", 'red'))
            elif response.status_code != 404:
                print(colored(f"  [INFO] Statut de {check_url}: {response.status_code}", 'yellow'))
        except requests.exceptions.RequestException as e:
            print(colored(f"  [ERREUR] Impossible de vérifier {check_url}: {e}", 'red'))

def scan_known_vulnerabilities(url):
    """Recherche de chemins connus associés à des vulnérabilités."""
    vulnerable_paths = [
        urljoin(url, '/wp-admin/install.php'),  # WordPress install not locked
        urljoin(url, '/administrator/install.php'), # Joomla install not locked
        urljoin(url, '/user/register'),         # Open user registration
        urljoin(url, '/vendor/phpunit/phpunit/src/Util/PHP/eval-stdin.php'), # PHPUnit RCE (CVE-2017-9841) - VERY DANGEROUS, USE WITH CAUTION
        urljoin(url, '/web.config')             # Potential disclosure of configuration
        # Ajouter d'autres chemins connus pour des vulnérabilités courantes
    ]
    print(colored("\n[SCAN DE VULNÉRABILITÉS POTENTIELLES (CHEMINS CONNUS)]", 'cyan'))
    for path in vulnerable_paths:
        try:
            response = requests.get(path, timeout=5, headers={'User-Agent': 'Hashish Vulnerability Scanner'}, allow_redirects=True, verify=False)
            if response.status_code == 200:
                print(colored(f"  [VULNÉRABILITÉ POTENTIELLE] {path} (Code: {response.status_code})", 'red'))
                if "PHPUnit" in response.text and "eval-stdin.php" in path:
                    print(colored("    [CRITIQUE] Potentiel RCE via PHPUnit!", 'bold', 'on_red'))
                elif "Congratulations, you have successfully installed WordPress!" in response.text:
                    print(colored("    [ALERTE] Installation WordPress non verrouillée!", 'bold', 'on_yellow'))
                elif "Joomla! Installation" in response.text:
                    print(colored("    [ALERTE] Installation Joomla! non verrouillée!", 'bold', 'on_yellow'))
                elif "Create new account" in response.text:
                    print(colored("    [INFO] Inscription utilisateur ouverte détectée.", 'yellow'))
                elif "<configuration>" in response.text:
                    print(colored("    [ALERTE] web.config potentiellement exposé.", 'bold', 'on_yellow'))
            elif response.status_code != 404:
                print(colored(f"  [INFO] Statut de {path}: {response.status_code}", 'yellow'))
        except requests.exceptions.RequestException as e:
            print(colored(f"  [ERREUR] Impossible de vérifier {path}: {e}", 'red'))

def scan_web_hardcore(target_url):
    """Lance un scan web 'hardcore' sur l'URL cible."""
    print(colored(f"[SCAN WEB HARDCORE] Ciblage de {target_url}...", 'magenta', attrs=['bold']))
    headers = scan_headers(target_url)
    content = ""
    if headers:
        try:
            response = requests.get(target_url, headers={'User-Agent': 'Hashish Web Scanner'}, timeout=10, verify=False)
            response.raise_for_status()
            content = response.text
            advanced_tech_detection(headers, target_url, content)
        except requests.exceptions.RequestException as e:
            print(colored(f"[WARNING] Erreur lors de la récupération du contenu principal pour la détection avancée: {e}", 'yellow'))
            advanced_tech_detection(headers, target_url) # Tentative de détection par en-têtes uniquement
        check_robots_txt(target_url)
        check_sensitive_files(target_url)
        scan_known_vulnerabilities(target_url) # Ajouter le scan de vulnérabilités
    else:
        print(colored("[INFO] Impossible de procéder au scan car la récupération des en-têtes a échoué.", 'yellow'))

if __name__ == "__main__":
    print(colored("[MODULE TEST] Lancement direct du module webscanner.py (HARDCORE)", 'yellow'))
    target_url = input(colored(" [ENTREZ L'URL CIBLE POUR LE SCAN WEB HARDCORE] > ", 'light_green'))
    scan_web_hardcore(target_url)
    print(colored("[MODULE TEST] Scan web hardcore terminé.", 'cyan'))

