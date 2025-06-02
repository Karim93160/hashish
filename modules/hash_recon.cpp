#include "hash_recon.h" // Inclut les déclarations de nos fonctions
#include <cctype>       // Pour les fonctions de vérification de caractères (isdigit, isalpha, etc.)
#include <iostream>     // Pour l'entrée/sortie (std::cout)
#include <string>       // Pour std::string
#include <iomanip>      // Pour std::quoted (si tu l'utilises, sinon tu peux l'enlever)

// Macros de couleur pour une meilleure lisibilité dans la console
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define FAINT   "\033[2m"
#define ITALIC  "\033[3m"
#define CR_RED    RED BOLD
#define CR_GREEN  GREEN BOLD
#define CR_YELLOW YELLOW BOLD
#define CR_BLUE   BLUE BOLD FAINT
#define CR_CYAN   CYAN BOLD
#define CR_MAGENTA MAGENTA BOLD FAINT
#define CR_DARK_GRAY "\033[90m"


// Fonction pour vérifier si une chaîne est uniquement numérique
bool estNumerique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Fonction pour vérifier si une chaîne est uniquement alphabétique
bool estAlphabetique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    return true;
}

// Fonction pour vérifier si une chaîne est uniquement alphanumérique
bool estAlphanumerique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalnum(c)) {
            return false;
        }
    }
    return true;
}

// Fonction pour vérifier si une chaîne est uniquement hexadécimale
bool estHexadecimal(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isxdigit(c)) {
            return false;
        }
    }
    return true;
}

// Nouvelle fonction pour l'analyse interactive du type de caractères du hachage
// Le nom de la fonction est bien "analyserTypeCaracteresHachage"
void analyserTypeCaracteresHachage(const std::string& hash_hex) { 
    std::cout << "\n" << CR_BLUE << "--- [ANALYSE APPROFONDIE DU HACHAGE] -------------------" << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Hachage à analyser: " << hash_hex << RESET << std::endl;

    bool est_numerique = estNumerique(hash_hex);
    bool est_alphabetique = estAlphabetique(hash_hex);
    bool est_alphanumerique = estAlphanumerique(hash_hex);
    bool est_hexadecimal = estHexadecimal(hash_hex);

    std::cout << CR_CYAN << "    Résultats de l'analyse du HACHAGE:" << RESET << std::endl;

    // Affichage des résultats
    std::cout << CR_CYAN << "      - Uniquement numérique (0-9): " << (est_numerique ? CR_GREEN "OUI" : CR_RED "NON") << RESET << std::endl;
    std::cout << CR_CYAN << "      - Uniquement alphabétique (a-z, A-Z): " << (est_alphabetique ? CR_GREEN "OUI" : CR_RED "NON") << RESET << std::endl;
    
    // Mise en évidence si le hachage est alphanumérique
    if (est_alphanumerique) {
        std::cout << CR_GREEN << BOLD << "      - Uniquement alphanumérique (lettres ou chiffres): OUI, contient [a-z] et [0-9]!" << RESET << std::endl;
    } else {
        std::cout << CR_CYAN << "      - Uniquement alphanumérique (lettres ou chiffres): " << CR_RED << "NON" << RESET << std::endl;
    }

    // Détection hexadécimale
    std::cout << CR_CYAN << "      - Uniquement hexadécimal (0-9, a-f, A-F): " << (est_hexadecimal ? CR_GREEN "OUI" : CR_RED "NON") << RESET << std::endl;

    if (est_hexadecimal) {
        std::cout << CR_YELLOW << "    [INFO] Un hachage hexadécimal est très courant pour MD5, SHA-1, SHA-256, etc." << RESET << std::endl;
    } else {
        std::cout << CR_YELLOW << "    [INFO] Ce hachage ne semble PAS être purement hexadécimal. Il pourrait s'agir d'un encodage différent ou d'un hachage corrompu." << RESET << std::endl;
    }

    // Recommandations pour les attaques par force brute
    if (est_alphanumerique) {
        std::cout << "\n" << CR_GREEN << BOLD << "    [RECOMMANDATION] Pour les attaques par force brute (option 2), envisage un jeu de caractères alphanumérique." << RESET << std::endl;
    } else if (est_numerique) {
        std::cout << "\n" << CR_YELLOW << "    [RECOMMANDATION] Pour les attaques par force brute (option 2), un jeu de caractères numérique pourrait être approprié." << RESET << std::endl;
    } else if (est_alphabetique) {
        std::cout << "\n" << CR_YELLOW << "    [RECOMMANDATION] Pour les attaques par force brute (option 2), un jeu de caractères alphabétique pourrait être approprié." << RESET << std::endl;
    }

    if (!est_numerique && !est_alphabetique && !est_hexadecimal && !est_alphanumerique) {
        std::cout << CR_YELLOW << "    [INFO] Ce hachage contient des caractères spéciaux ou inattendus. Il pourrait être encodé différemment ou corrompu." << RESET << std::endl;
    }
    
    std::cout << CR_BLUE << "--------------------------------------------------------" << RESET << std::endl;
}
