import os
import sys
import time
import requests
from termcolor import colored
from urllib.parse import urljoin, urlparse
from requests.packages.urllib3.exceptions import InsecureRequestWarning
requests.packages.urllib3.disable_warnings(InsecureRequestWarning)

def scan_headers(url):
    print(colored(f"[WEB SCAN] Récupération et analyse des en-têtes HTTP pour {url}...", 'cyan'))
    try:
        response = requests.get(url, allow_redirects=True, timeout=10,
                                 headers={'User-Agent': 'Hashish Web Scanner/1.0'}, verify=False)
        response.raise_for_status()
        print(colored(f"\n[EN-TÊTES HTTP pour {response.url}]", 'yellow', attrs=['bold']))
        for header, value in response.headers.items():
            print(f"  {colored(header + ':', 'cyan')} {value}")
        print(colored("\n[ANALYSE DES EN-TÊTES DE SÉCURITÉ]", 'yellow', attrs=['bold']))
        security_headers = {
            'Strict-Transport-Security': 'Force la connexion HTTPS. Très recommandé.',
            'Content-Security-Policy': 'Protège contre les attaques de type XSS et injection de données.',
            'X-Frame-Options': 'Protège contre le "clickjacking".',
            'X-Content-Type-Options': 'Empêche le "MIME type sniffing".',
            'X-XSS-Protection': 'Active le filtre anti-XSS intégré au navigateur.'
        }
        found_security_headers = 0
        for header, description in security_headers.items():
            if header not in response.headers:
                print(colored(f"  [WARNING] En-tête de sécurité manquant: {header} - {description}", 'red'))
            else:
                print(colored(f"  [INFO] En-tête de sécurité présent: {header}", 'green'))
                found_security_headers += 1
        if found_security_headers == len(security_headers):
            print(colored("  [SUCCÈS] Tous les en-têtes de sécurité courants sont présents.", 'green', attrs=['bold']))
        elif found_security_headers > 0:
            print(colored(f"  [ATTENTION] {found_security_headers}/{len(security_headers)} en-têtes de sécurité présents.", 'yellow'))
        else:
            print(colored("  [CRITIQUE] Aucun en-tête de sécurité courant trouvé.", 'red', attrs=['bold']))
        return response.headers, response.text
    except requests.exceptions.RequestException as e:
        print(colored(f"[ERREUR] Impossible de récupérer les en-têtes/contenu pour {url}: {e}", 'red'))
        return None, None
    except Exception as e:
        print(colored(f"[ERREUR] Une erreur inattendue est survenue lors de l'analyse des en-têtes: {e}", 'red'))
        return None, None

def advanced_tech_detection(headers, content=""):
    print(colored("\n[DÉTECTION DE TECHNOLOGIES]", 'cyan', attrs=['bold']))
    technologies_found = set()
    server = headers.get('Server', '').lower()
    powered_by = headers.get('X-Powered-By', '').lower()
    x_generator = headers.get('X-Generator', '').lower()
    set_cookie = headers.get('Set-Cookie', '').lower()
    via = headers.get('Via', '').lower()
    if 'nginx' in server: technologies_found.add('Nginx')
    if 'apache' in server: technologies_found.add('Apache HTTP Server')
    if 'microsoft-iis' in server: technologies_found.add('Microsoft IIS')
    if 'cloudflare' in server or 'cloudflare' in via: technologies_found.add('Cloudflare CDN')
    if 'gws' in server: technologies_found.add('Google Web Server')
    if 'php' in powered_by: technologies_found.add('PHP')
    if 'asp.net' in powered_by: technologies_found.add('ASP.NET')
    if 'express' in powered_by: technologies_found.add('Node.js (Express)')
    if 'wordpress' in x_generator or 'wordpress' in set_cookie: technologies_found.add('WordPress (via en-têtes)')
    if 'drupal' in x_generator or 'drupal' in set_cookie: technologies_found.add('Drupal (via en-têtes)')
    if 'joomla!' in x_generator or 'joomla_' in set_cookie: technologies_found.add('Joomla! (via en-têtes)')
    if content:
        content_lower = content.lower()
        if 'wp-content' in content_lower or 'wp-includes' in content_lower: technologies_found.add('WordPress (via contenu)')
        if '<meta name="generator" content="wordpress' in content_lower: technologies_found.add('WordPress (via meta tag)')
        if '/sites/all/modules/' in content_lower or '/misc/drupal.js' in content_lower: technologies_found.add('Drupal (via contenu)')
        if '<meta name="generator" content="drupal' in content_lower: technologies_found.add('Drupal (via meta tag)')
        if '/media/system/js/' in content_lower or '/components/com_content/' in content_lower: technologies_found.add('Joomla! (via contenu)')
        if '<meta name="generator" content="joomla!' in content_lower: technologies_found.add('Joomla! (via meta tag)')
        if 'react-root' in content_lower or 'data-reactroot' in content_lower: technologies_found.add('React.js')
        if 'vue-app' in content_lower or '__vue__' in content_lower: technologies_found.add('Vue.js')
        if 'angular-app' in content_lower or 'ng-app' in content_lower: technologies_found.add('AngularJS/Angular')
        if 'jquery' in content_lower: technologies_found.add('jQuery')
        if '<script src="/socket.io/' in content_lower: technologies_found.add('Socket.IO')
    if technologies_found:
        print(colored("  [TECH] Technologies détectées:", 'green'))
        for tech in sorted(list(technologies_found)):
            print(f"    - {tech}")
    else:
        print(colored("  [INFO] Aucune technologie web spécifique détectée avec les méthodes courantes.", 'yellow'))

def check_robots_txt(url):
    robots_url = urljoin(url, '/robots.txt')
    print(colored(f"\n[WEB SCAN] Vérification de {robots_url}...", 'cyan'))
    try:
        response = requests.get(robots_url, timeout=5, headers={'User-Agent': 'Hashish Web Scanner'})
        if response.status_code == 200:
            print(colored(f"\n[CONTENU de {robots_url}] (Code: {response.status_code})", 'yellow', attrs=['bold']))
            print(response.text)
        elif response.status_code == 404:
            print(colored(f"[INFO] Fichier {robots_url} non trouvé (Code: {response.status_code}).", 'green'))
        else:
            print(colored(f"[WARNING] Statut inattendu pour {robots_url}: {response.status_code}", 'yellow'))
    except requests.exceptions.RequestException as e:
        print(colored(f"[ERREUR] Impossible de récupérer {robots_url}: {e}", 'red'))

def check_sitemap_xml(url):
    sitemap_url = urljoin(url, '/sitemap.xml')
    print(colored(f"\n[WEB SCAN] Vérification de {sitemap_url}...", 'cyan'))
    try:
        response = requests.get(sitemap_url, timeout=5, headers={'User-Agent': 'Hashish Web Scanner'})
        if response.status_code == 200:
            print(colored(f"\n[CONTENU de {sitemap_url}] (Code: {response.status_code})", 'yellow', attrs=['bold']))
            print(response.text[:500] + ("..." if len(response.text) > 500 else ""))
            print(colored(f"  [INFO] Sitemap trouvé. Peut contenir des URL intéressantes.", 'green'))
        elif response.status_code == 404:
            print(colored(f"[INFO] Fichier {sitemap_url} non trouvé (Code: {response.status_code}).", 'green'))
        else:
            print(colored(f"[WARNING] Statut inattendu pour {sitemap_url}: {response.status_code}", 'yellow'))
    except requests.exceptions.RequestException as e:
        print(colored(f"[ERREUR] Impossible de récupérer {sitemap_url}: {e}", 'red'))

def check_sensitive_files(url):
    sensitive_paths = [
        '.env', '.git/config', 'config.php', 'backup.sql', 'wp-config.php',
        '.htaccess', 'web.config', 'phpinfo.php', 'test.php',
        'admin/', 'admin.php', 'login.php', 'wp-login.php',
        'ftp.txt', 'passwords.txt', 'credentials.txt', 'robots.txt.bak',
        '~root', '~admin', 'README.md', 'LICENSE'
    ]
    print(colored("\n[WEB SCAN] Vérification de fichiers et répertoires sensibles...", 'cyan'))
    found_any_sensitive = False
    for path in sensitive_paths:
        check_url = urljoin(url, path)
        try:
            response = requests.head(check_url, timeout=3, headers={'User-Agent': 'Hashish Web Scanner'}, verify=False)
            if response.status_code == 200:
                print(colored(f"  [POTENTIELLEMENT SENSIBLE] {check_url} (Code: {response.status_code}) - ACCESSIBLE", 'red', attrs=['bold']))
                found_any_sensitive = True
            elif response.status_code == 401 or response.status_code == 403:
                print(colored(f"  [INFO] {check_url} (Code: {response.status_code}) - Accès Refusé/Authentification Requise", 'yellow'))
            elif response.status_code != 404:
                print(colored(f"  [INFO] Statut de {check_url}: {response.status_code}", 'yellow'))
        except requests.exceptions.RequestException as e:
            print(colored(f"  [ERREUR] Impossible de vérifier {check_url}: {e}", 'red'))
        except Exception as e:
            print(colored(f"  [ERREUR] Erreur inattendue lors de la vérification de fichiers sensibles: {e}", 'red'))
    if not found_any_sensitive:
        print(colored("  [INFO] Aucune ressource sensible courante n'a été trouvée directement accessible.", 'green'))

def scan_known_vulnerabilities(url):
    vulnerable_paths_and_signatures = {
        '/wp-admin/install.php': 'Congratulations, you have successfully installed WordPress!',
        '/wp-config.php.bak': 'DB_NAME',
        '/wp-content/debug.log': 'WordPress debug log',
        '/administrator/install.php': 'Joomla! Installation',
        '/user/register': 'Create new account',
        '/vendor/phpunit/phpunit/src/Util/PHP/eval-stdin.php': 'PHPUnit',
        '/web.config': '<configuration>',
        '/.git/HEAD': 'ref: refs/heads/',
        '/server-status': 'Apache Server Status',
        '/phpmyadmin/': 'phpMyAdmin',
        '/admin/pma/': 'phpMyAdmin',
        '/_admin/': '',
        '/backup/': '',
        '/test/': '',
    }
    print(colored("\n[WEB SCAN] Recherche de chemins connus pour des vulnérabilités/infos sensibles...", 'cyan', attrs=['bold']))
    found_any_vuln = False
    for path, signature in vulnerable_paths_and_signatures.items():
        check_url = urljoin(url, path)
        try:
            response = requests.get(check_url, timeout=7, headers={'User-Agent': 'Hashish Vulnerability Scanner'}, allow_redirects=True, verify=False)
            if response.status_code == 200:
                print(colored(f"  [POTENTIELLEMENT VULNÉRABLE] {check_url} (Code: {response.status_code})", 'red'))
                if signature and signature in response.text:
                    print(colored(f"    [ALERTE] Signature '{signature}' trouvée dans le contenu. Forte probabilité de vulnérabilité/exposition!", 'red', attrs=['bold']))
                    found_any_vuln = True
                elif not signature:
                     print(colored(f"    [INFO] Chemin connu accessible. Examiner manuellement le contenu.", 'yellow'))
                     found_any_vuln = True
            elif response.status_code == 401 or response.status_code == 403:
                print(colored(f"  [INFO] {check_url} (Code: {response.status_code}) - Accès Refusé/Authentification Requise.", 'green'))
            elif response.status_code != 404:
                print(colored(f"  [INFO] Statut de {check_url}: {response.status_code}", 'yellow'))
        except requests.exceptions.RequestException as e:
            if not isinstance(e, requests.exceptions.ConnectionError):
                print(colored(f"  [ERREUR] Impossible de vérifier {check_url}: {e}", 'red'))
        except Exception as e:
            print(colored(f"  [ERREUR] Erreur inattendue lors du scan de vulnérabilités: {e}", 'red'))
    if not found_any_vuln:
        print(colored("  [INFO] Aucun chemin connu indiquant une vulnérabilité ou une information sensible n'a été trouvé directement accessible.", 'green'))

def scan_web_hardcore(target_url):
    parsed_target = urlparse(target_url)
    if not parsed_target.scheme:
        print(colored(f"[INFO] Le schéma (http:// ou https://) est manquant. Tentative avec HTTPS, puis HTTP.", 'yellow'))
        target_url_https = f"https://{target_url}"
        target_url_http = f"http://{target_url}"
        print(colored(f"[SCAN WEB HARDCORE] Tentative sur {target_url_https}...", 'magenta', attrs=['bold']))
        headers, content = scan_headers(target_url_https)
        if not headers:
            print(colored(f"[INFO] HTTPS n'a pas répondu ou a échoué. Tentative sur {target_url_http}...", 'yellow'))
            headers, content = scan_headers(target_url_http)
            target_url = target_url_http
        else:
            target_url = target_url_https
    else:
        print(colored(f"[SCAN WEB HARDCORE] Ciblage de {target_url}...", 'magenta', attrs=['bold']))
        headers, content = scan_headers(target_url)
    if headers:
        advanced_tech_detection(headers, content)
        check_robots_txt(target_url)
        check_sitemap_xml(target_url)
        check_sensitive_files(target_url)
        scan_known_vulnerabilities(target_url)
    else:
        print(colored("[INFO] Impossible de procéder au scan web complet car la connexion ou la récupération des en-têtes a échoué.", 'yellow'))
    print(colored(f"\n[SCAN WEB HARDCORE] Scan web hardcore sur {target_url} terminé.", 'magenta', attrs=['bold']))

def scan_web_hardcore_main():
    print(colored("[MODULE] Lancement du module Web Scanner (HARDCORE)", 'yellow', attrs=['bold']))
    target_url = input(colored(" [ENTREZ L'URL CIBLE POUR LE SCAN WEB HARDCORE (ex: http://example.com)] > ", 'light_green')).strip()
    if not target_url:
        print(colored("[INFO] Aucune URL spécifiée. Retour au menu principal.", 'yellow'))
        return
    scan_web_hardcore(target_url)
    print(colored("[MODULE] Scan web hardcore terminé.", 'cyan'))

if __name__ == "__main__":
    scan_web_hardcore_main()
