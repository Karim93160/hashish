#include "hash_recon.h" // Inclut les déclarations de nos fonctions
#include <cctype>       // Pour les fonctions de vérification de caractères (isdigit, isalpha, etc.)
#include <iostream>     // Pour les entrées/sorties (std::cout, std::cin)
#include <string>       // Pour std::string
#include <iomanip>      // Pour std::quoted

// Définition des macros de couleur pour la console (copiées de hashcracker.cpp)
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


// Fonction pour vérifier si une chaîne est composée uniquement de chiffres
bool estNumerique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Fonction pour vérifier si une chaîne est composée uniquement de lettres
bool estAlphabetique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    return true;
}

// Fonction pour vérifier si une chaîne est composée uniquement de caractères alphanumériques
bool estAlphanumerique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalnum(c)) {
            return false;
        }
    }
    return true;
}

// Fonction pour vérifier si une chaîne est composée uniquement de caractères hexadécimaux
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
void analyserTypeCaracteresHachage(const std::string& hash_hex) {
    std::cout << "\n" << CR_BLUE << "--- [ANALYSE APPROFONDIE DU HACHAGE] -------------------" << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Hachage à analyser : " << hash_hex << RESET << std::endl;

    bool is_numeric = estNumerique(hash_hex);
    bool is_alpha = estAlphabetique(hash_hex);
    bool is_alphanum = estAlphanumerique(hash_hex);
    bool is_hex = estHexadecimal(hash_hex);

    std::cout << CR_CYAN << "    Résultats de l'analyse :" << RESET << std::endl;
    std::cout << CR_CYAN << "      - Uniquement numérique (0-9) : " << (is_numeric ? CR_GREEN "OUI" : CR_RED "NON") << RESET << std::endl;
    std::cout << CR_CYAN << "      - Uniquement alphabétique (a-z, A-Z) : " << (is_alpha ? CR_GREEN "OUI" : CR_RED "NON") << RESET << std::endl;
    std::cout << CR_CYAN << "      - Uniquement alphanumérique (lettres ou chiffres) : " << (is_alphanum ? CR_GREEN "OUI" : CR_RED "NON") << RESET << std::endl;
    std::cout << CR_CYAN << "      - Uniquement hexadécimal (0-9, a-f, A-F) : " << (is_hex ? CR_GREEN "OUI" : CR_RED "NON") << RESET << std::endl;

    if (is_hex) {
        std::cout << CR_YELLOW << "    [INFO] Un hachage hexadécimal est très courant pour MD5, SHA-1, SHA-256, etc." << RESET << std::endl;
    }
    if (is_numeric && !is_hex) { // Si c'est numérique mais pas hexadécimal (ex: "123" est numérique et hex, "G" est hex mais pas num)
        std::cout << CR_YELLOW << "    [INFO] Ce hachage ne contient que des chiffres. Cela peut suggérer un mot de passe ou un identifiant numérique." << RESET << std::endl;
    }
    if (is_alpha && !is_hex) {
        std::cout << CR_YELLOW << "    [INFO] Ce hachage ne contient que des lettres. Cela peut indiquer un mot de passe simple ou un identifiant textuel." << RESET << std::endl;
    }
    if (!is_numeric && !is_alpha && !is_hex && !is_alphanum) {
        std::cout << CR_YELLOW << "    [INFO] Ce hachage contient des caractères spéciaux ou inattendus. Il pourrait être encodé différemment ou corrompu." << RESET << std::endl;
    }
    
    std::cout << CR_BLUE << "--------------------------------------------------------" << RESET << std::endl;
}
