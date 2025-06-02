#ifndef HASH_RECON_H
#define HASH_RECON_H

#include <string>

// Définitions de couleurs pour la console
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

// Fonctions d'analyse de type de caractères (pour le mot de passe retrouvé)
bool estNumerique(const std::string& str);
bool estAlphabetique(const std::string& str);
bool estAlphanumerique(const std::string& str);
bool estHexadecimal(const std::string& str); // Utile pour vérifier le hash lui-même

// Fonctions de hachage
std::string md5(const std::string& str);
std::string sha256(const std::string& str);

// Fonction d'analyse principale (pour le hash lui-même, comme tu l'avais initialement)
void analyserTypeCaracteresHachage(const std::string& hash_hex);

// Nouvelle fonction pour la force brute et l'analyse du mot de passe original
void crackerEtAnalyserMotDePasse(const std::string& hash_cible, const std::string& type_hash, int max_longueur);

#endif // HASH_RECON_H
