#ifndef HASH_RECON_H
#define HASH_RECON_H

#include <string> // Nécessaire car les fonctions utilisent std::string

// Déclarations des fonctions de vérification de type de caractères
// Elles retournent true si la chaîne est composée uniquement du type spécifié, false sinon.
bool estNumerique(const std::string& str);
bool estAlphabetique(const std::string& str);
bool estAlphanumerique(const std::string& str);
bool estHexadecimal(const std::string& str);
void analyserTypeCaracteresHachage(const std::string& hash_hex); // <-- DÉCLARATION DE LA FONCTION D'ANALYSE

#endif // HASH_RECON_H
