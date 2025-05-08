#!/bin/bash

# Couleur pour les messages
GREEN='\033[0;32m'
NC='\033[0m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'

# Nom du dépôt (pour les messages)
REPO_NAME="hashish"
DEFAULT_REPO_PATH="/data/data/com.termux/files/home/$REPO_NAME"
REPO_PATH="$DEFAULT_REPO_PATH"
INSTALL_DIR="/data/data/com.termux/files/usr/bin/"

# Vérifier si le répertoire du dépôt existe
if [ ! -d "$REPO_PATH" ]; then
  echo -e "${YELLOW}Le répertoire '$REPO_NAME' n'a pas été trouvé à l'emplacement par défaut : '$DEFAULT_REPO_PATH'.${NC}"
  read -p "Veuillez entrer le chemin du dépôt '$REPO_NAME' cloné (laissez vide pour utiliser '$DEFAULT_REPO_PATH') : " CUSTOM_REPO_PATH
  if [ -n "$CUSTOM_REPO_PATH" ]; then
    REPO_PATH="$CUSTOM_REPO_PATH"
  fi
fi

# Vérifier si le chemin du dépôt est valide et contient le script hashish.py
if [ ! -f "$REPO_PATH/hashish.py" ]; then
  echo -e "${RED}Erreur : Le chemin courant est invalide ou ne contient pas le script 'hashish.py'.${NC}"
  exit 1
fi

# Créer le dossier 'modules' s'il n'existe pas (dans le dépôt)
if [ ! -d "$REPO_PATH/modules" ]; then
  echo -e "${BLUE}Création du dossier 'modules' dans '$REPO_PATH'...${NC}"
  mkdir "$REPO_PATH/modules"
fi

# Déplacer les fichiers de modules vers le dossier 'modules' (dans le dépôt)
MODULES=("hashcracker.py" "webscanner.py" "recon.py" "osint.py")
for module in "${MODULES[@]}"; do
  if [ -f "$REPO_PATH/$module" ]; then
    echo -e "${BLUE}Déplacement de '$module' vers '$REPO_PATH/modules'...${NC}"
    mv "$REPO_PATH/$module" "$REPO_PATH/modules/"
  elif [ -f "$REPO_PATH/modules/$module" ]; then
    echo -e "${GREEN}Le module '$module' est déjà dans le dossier 'modules'.${NC}"
  else
    echo -e "${YELLOW}Le fichier '$module' est introuvable dans le dépôt courant.${NC}"
  fi
done

# Rendre le script 'hashish.py' exécutable dans le répertoire du dépôt
if [ -f "$REPO_PATH/hashish.py" ]; then
  echo -e "${BLUE}Rend le script './hashish.py' exécutable dans le dépôt...${NC}"
  chmod +x "$REPO_PATH/hashish.py"
else
  echo -e "${RED}Le script 'hashish.py' est introuvable dans le répertoire courant.${NC}"
  exit 1
fi

# Installation des dépendances Python
echo -e "${BLUE}Installation des dépendances Python (via pip)...${NC}"
cd "$REPO_PATH" && pip install -r requirements.txt 2>/dev/null
if [ $? -eq 0 ]; then
  echo -e "${GREEN}Dépendances Python installées avec succès.${NC}"
else
  echo -e "${YELLOW}Certaines erreurs peuvent être survenues lors de l'installation des dépendances Python. Vérifiez les messages ci-dessus.${NC}"
fi

# Information sur l'installation des outils système essentiels pour Termux
echo -e "${YELLOW}\n[INFO] Veuillez installer IMPÉRATIVEMENT les outils système suivants sur Termux (via pkg install) pour une fonctionnalité RECON complète :${NC}"
echo -e "${YELLOW}  - nmap${NC}"
echo -e "${YELLOW}  - whois${NC}"
echo -e "${YELLOW}  - traceroute${NC}"
echo -e "${YELLOW}  - dnsutils (pour nslookup et dig)${NC}"
echo -e "${YELLOW}  - curl${NC}"
echo -e "${YELLOW}  - shodan (et configurez votre clé API avec 'shodan init <API_KEY>')${NC}"
echo -e "${YELLOW}  Sans ces outils, certaines fonctionnalités de RECON ne fonctionneront pas.${NC}"

# Exportation du contenu de hashish vers /usr/bin/
echo -e "${BLUE}\nEXPORTATION du contenu de '$REPO_PATH' vers '$INSTALL_DIR' (via liens symboliques)...${NC}"
for item in "$REPO_PATH"/*; do
  base_name=$(basename "$item")
  link_path="$INSTALL_DIR/$base_name"
  if [ -e "$link_path" ]; then
    echo -e "${YELLOW}Le fichier ou répertoire '$link_path' existe déjà. Suppression...${NC}"
    rm -rf "$link_path"
  fi
  echo -e "${BLUE}Création du lien symbolique '$link_path' -> '$item'...${NC}"
  ln -s "$item" "$link_path"
  if [[ "$base_name" == "hashish.py" ]]; then
    echo -e "${BLUE}Rend './hashish.py' exécutable dans '$INSTALL_DIR'...${NC}"
    chmod +x "$link_path"
  fi
done

# Exportation du contenu du dossier modules vers /usr/bin/
echo -e "${BLUE}\nEXPORTATION du contenu de '$REPO_PATH/modules' vers '$INSTALL_DIR' (via liens symboliques)...${NC}"
for item in "$REPO_PATH/modules"/*; do
  base_name=$(basename "$item")
  link_path="$INSTALL_DIR/$base_name"
  if [ -e "$link_path" ]; then
    echo -e "${YELLOW}Le fichier ou répertoire '$link_path' existe déjà. Suppression...${NC}"
    rm -rf "$link_path"
  fi
  echo -e "${BLUE}Création du lien symbolique '$link_path' -> '$item'...${NC}"
  ln -s "$item" "$link_path"
  if [[ "$base_name" == "hashcracker.py" || "$base_name" == "webscanner.py" || "$base_name" == "recon.py" || "$base_name" == "osint.py" ]]; then
    echo -e "${BLUE}Rend './$base_name' exécutable dans '$INSTALL_DIR'...${NC}"
    chmod +x "$link_path"
  fi
done

# Copie de banner.txt vers le répertoire bin
if [ -f "$REPO_PATH/banner.txt" ]; then
  echo -e "${BLUE}Copie de 'banner.txt' vers '$INSTALL_DIR'...${NC}"
  cp "$REPO_PATH/banner.txt" "$INSTALL_DIR/banner.txt"
else
  echo -e "${YELLOW}Le fichier 'banner.txt' est introuvable dans le dépôt.${NC}"
fi

# Wrappers executables 
echo -e "${BLUE}Création des wrappers d'exécution...${NC}" 

cat > "$INSTALL_DIR/hashish" << 'EOF' 
#!/data/data/com.termux/files/usr/bin/bash 
SCRIPT_PATH="/data/data/com.termux/files/usr/bin/hashish.py" 
[ ! -f "$SCRIPT_PATH" ] && echo "Erreur: hashish.py introuvable" && exit 1 
exec python3 "$SCRIPT_PATH" "$@" 
EOF 
chmod +x "$INSTALL_DIR/hashish" 

for module in "${MODULES[@]}"; do 
    cmd_name="${module%.py}" 
    cat > "$INSTALL_DIR/$cmd_name" << EOF 
#!/data/data/com.termux/files/usr/bin/bash 
exec python3 "/data/data/com.termux/files/usr/bin/$module" "\$@" 
EOF 
    chmod +x "$INSTALL_DIR/$cmd_name" 
done
echo -e "${GREEN}\nINSTALLATION TERMINÉE !${NC}"
echo -e "${GREEN}Vous pouvez maintenant utiliser les outils directement depuis le terminal.${NC}"
echo -e "${GREEN}Par exemple, tapez 'hashish' pour lancer le toolkit principal, ou 'hashcracker.py' pour utiliser le module de cracking de hash.${NC}"

exit 0
