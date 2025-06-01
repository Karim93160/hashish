#include "hash_recon.h"
#include <iostream>
#include <iomanip> // Pour std::hex et std::setw
#include <sstream> // Pour std::stringstream
#include <algorithm> // Pour std::all_of
#include <cctype>    // Pour std::isxdigit

// Inclusion de OpenSSL pour les fonctions de hachage
#include <openssl/md4.h> // Pour NTLM (MD4)

/**
 * @brief Convertit un tableau d'octets en une chaîne hexadécimale.
 * @param bytes Le tableau d'octets.
 * @param len La longueur du tableau.
 * @return La chaîne hexadécimale.
 */
std::string HashRecon::bytesToHex(const unsigned char* bytes, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

/**
 * @brief Vérifie si une chaîne est une représentation hexadécimale valide.
 * @param s La chaîne à vérifier.
 * @return true si la chaîne est hexadécimale, false sinon.
 */
bool HashRecon::isHex(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    // Vérifie si tous les caractères sont des chiffres hexadécimaux
    return std::all_of(s.begin(), s.end(), [](char c){
        return std::isxdigit(static_cast<unsigned char>(c));
    });
}

/**
 * @brief Reconnaît le type d'un hachage donné.
 *
 * Tente d'identifier le type de hachage en fonction de sa longueur
 * et, potentiellement, de son format (hexadécimal).
 *
 * @param hash La chaîne de caractères représentant le hachage à reconnaître.
 * @return Une chaîne de caractères décrivant le type de hachage reconnu,
 * ou "Inconnu" si le type ne peut pas être déterminé.
 */
std::string HashRecon::recognizeHash(const std::string& hash) {
    if (!isHex(hash)) {
        return "Non-hexadécimal ou format invalide";
    }

    size_t len = hash.length();

    switch (len) {
        case 32:
            return "MD5 / NTLM"; // NTLM est un MD4 de 16 octets, donc 32 caractères hexadécimaux
        case 40:
            return "SHA-1";
        case 56:
            return "SHA-224 / SHA3-224";
        case 64:
            return "SHA-256 / SHA3-256";
        case 96:
            return "SHA-384 / SHA3-384";
        case 128:
            return "SHA-512 / SHA3-512";
        default:
            return "Inconnu (longueur: " + std::to_string(len) + " caractères)";
    }
}

/**
 * @brief Calcule le hachage MD5 d'une chaîne.
 * @param input La chaîne d'entrée.
 * @return Le hachage MD5 en format hexadécimal.
 */
std::string HashRecon::calculateMD5(const std::string& input) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), digest);
    return bytesToHex(digest, MD5_DIGEST_LENGTH);
}

/**
 * @brief Calcule le hachage SHA-256 d'une chaîne.
 * @param input La chaîne d'entrée.
 * @return Le hachage SHA-256 en format hexadécimal.
 */
std::string HashRecon::calculateSHA256(const std::string& input) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), digest);
    return bytesToHex(digest, SHA256_DIGEST_LENGTH);
}

/**
 * @brief Calcule le hachage SHA-1 d'une chaîne.
 * @param input La chaîne d'entrée.
 * @return Le hachage SHA-1 en format hexadécimal.
 */
std::string HashRecon::calculateSHA1(const std::string& input) {
    unsigned char digest[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), digest);
    return bytesToHex(digest, SHA_DIGEST_LENGTH);
}

/**
 * @brief Calcule le hachage SHA-512 d'une chaîne.
 * @param input La chaîne d'entrée.
 * @return Le hachage SHA-512 en format hexadécimal.
 */
std::string HashRecon::calculateSHA512(const std::string& input) {
    unsigned char digest[SHA512_DIGEST_LENGTH];
    SHA512(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), digest);
    return bytesToHex(digest, SHA512_DIGEST_LENGTH);
}

/**
 * @brief Calcule le hachage NTLM d'une chaîne (généralement le mot de passe en UTF-16LE Haché par MD4).
 * @param input La chaîne d'entrée (le mot de passe en clair).
 * @return Le hachage NTLM en format hexadécimal.
 * Note: Cette implémentation de NTLM est simplifiée et suppose un MD4 de l'UTF-16LE.
 * Pour un NTLM complet avec salage et autres, une bibliothèque plus robuste serait nécessaire.
 */
std::string HashRecon::calculateNTLM(const std::string& input) {
    // Convertir la chaîne d'entrée en UTF-16LE
    // Pour une implémentation complète, il faudrait gérer correctement l'encodage des caractères.
    // Ici, nous faisons une conversion simplifiée : chaque caractère devient un octet + un octet nul.
    std::string utf16le_input;
    for (char c : input) {
        utf16le_input += c;
        utf16le_input += '\0'; // Ajoute un octet nul pour représenter l'UTF-16LE simple
    }

    unsigned char digest[MD4_DIGEST_LENGTH];
    MD4(reinterpret_cast<const unsigned char*>(utf16le_input.c_str()), utf16le_input.length(), digest);
    return bytesToHex(digest, MD4_DIGEST_LENGTH);
}

