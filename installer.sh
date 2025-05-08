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
if [ ! -f "./hashish.py" ]; then
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
  if [ -f "./$module" ]; then
    echo -e "${BLUE}Déplacement de '$module' vers '$REPO_PATH/modules'...${NC}"
    mv "./$module" "$REPO_PATH/modules/"
  elif [ -f "$REPO_PATH/modules/$module" ]; then
    echo -e "${GREEN}Le module '$module' est déjà dans le dossier 'modules'.${NC}"
  else
    echo -e "${YELLOW}Le fichier '$module' est introuvable dans le dépôt courant.${NC}"
  fi
done

# Rendre le script 'hashish.py' exécutable dans le répertoire du dépôt
if [ -f "./hashish.py" ]; then
  echo -e "${BLUE}Rend le script './hashish.py' exécutable dans le dépôt...${NC}"
  chmod +x "./hashish.py"
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

# Copier les modules vers le répertoire bin
echo -e "${BLUE}\nCopie des modules vers '$INSTALL_DIR' pour une utilisation globale...${NC}"
for module in "${MODULES[@]}"; do
  if [ -f "$REPO_PATH/modules/$module" ]; then
    echo -e "${BLUE}Copie de '$module' vers '$INSTALL_DIR/$module'...${NC}"
    cp "$REPO_PATH/modules/$module" "$INSTALL_DIR/$module"
    chmod +x "$INSTALL_DIR/$module" # Rendre les modules exécutables (même si ce sont des librairies)
  else
    echo -e "${YELLOW}Le module '$module' est introuvable dans le dépôt.${NC}"
  fi
done

# Tenter de rendre 'hashish' accessible globalement dans Termux
if [ -d "$INSTALL_DIR" ]; then
  if [ -f "$INSTALL_DIR/hashish" ]; then
    echo -e "${YELLOW}Un fichier 'hashish' existe déjà dans '$INSTALL_DIR'.${NC}"
    REPLACE="o"
    echo -e "${BLUE}Réponse automatique à la demande de remplacement : o${NC}"
    if [[ "$REPLACE" == "o" || "$REPLACE" == "O" ]]; then
      echo -e "${BLUE}Suppression de l'ancienne version de 'hashish' dans '$INSTALL_DIR'...${NC}"
      rm -f "$INSTALL_DIR/hashish"
      echo -e "${BLUE}Copie de './hashish.py' vers '$INSTALL_DIR/hashish'...${NC}"
      cp "./hashish.py" "$INSTALL_DIR/hashish"
      chmod +x "$INSTALL_DIR/hashish"
      if [ $? -eq 0 ]; then
        echo -e "${GREEN}'hashish' (la dernière version) est maintenant accessible globalement.${NC}"
      else
        echo -e "${RED}Erreur lors de la copie de 'hashish.py'. Assurez-vous d'avoir les permissions nécessaires.${NC}"
      fi
    else
      echo -e "${YELLOW}Opération annulée. Vous pouvez exécuter 'hashish' depuis le répertoire du dépôt avec 'python3 hashish.py' ou tenter de le rendre global manuellement.${NC}"
    fi
  else
    echo -e "${BLUE}Copie de './hashish.py' vers '$INSTALL_DIR/hashish'...${NC}"
    cp "./hashish.py" "$INSTALL_DIR/hashish"
    chmod +x "$INSTALL_DIR/hashish"
    if [ $? -eq 0 ]; then
      echo -e "${GREEN}'hashish' est maintenant accessible globalement.${NC}"
    else
      echo -e "${RED}Erreur lors de la copie de 'hashish.py'. Assurez-vous d'avoir les permissions nécessaires.${NC}"
    fi
  fi
else
  echo -e "${YELLOW}Impossible d'accéder à '$INSTALL_DIR' pour l'installation globale.${NC}"
  echo -e "${GREEN}Vous pouvez exécuter 'hashish' depuis le répertoire du dépôt avec 'python3 hashish.py'.${NC}"
fi

# Copie de banner.txt vers le répertoire bin
if [ -f "./banner.txt" ]; then
  echo -e "${BLUE}Copie de 'banner.txt' vers '$INSTALL_DIR'...${NC}"
  cp "./banner.txt" "$INSTALL_DIR/banner.txt"
else
  echo -e "${YELLOW}Le fichier 'banner.txt' est introuvable dans le dépôt.${NC}"
fi

echo -e "${GREEN}\nInstallation terminée !${NC}"
echo -e "${GREEN}Pour lancer le toolkit principal, tapez 'hashish'.${NC}"
echo -e "${GREEN}Vous devriez également pouvoir utiliser les modules directement (par exemple, 'python3 hashcracker.py').${NC}"

exit 0
