
#ifndef HASH_RECON_H
#define HASH_RECON_H

#include <string>
#include <vector>
#include <openssl/md5.h>
#include <openssl/sha.h>

/**
 * @brief Classe pour la reconnaissance des types de hachages.
 *
 * Cette classe fournit des méthodes statiques pour identifier le type d'un hachage
 * donné en se basant sur sa longueur et des motifs potentiels.
 * Elle inclut également des fonctions utilitaires pour la conversion hexadécimale
 * et le calcul de hachages MD5 et SHA-256 (bien que les fonctions de calcul
 * ne soient pas directement utilisées pour la *reconnaissance* de type, elles
 * sont incluses pour des raisons de complétude et d'utilité générale dans un
 * contexte de hachage).
 */
class HashRecon {
public:
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
    static std::string recognizeHash(const std::string& hash);

    /**
     * @brief Vérifie si une chaîne est une représentation hexadécimale valide.
     * @param s La chaîne à vérifier.
     * @return true si la chaîne est hexadécimale, false sinon.
     */
    static bool isHex(const std::string& s);

    // Fonctions de hachage (pour d'autres usages, non directement pour la reconnaissance de type)
    /**
     * @brief Calcule le hachage MD5 d'une chaîne.
     * @param input La chaîne d'entrée.
     * @return Le hachage MD5 en format hexadécimal.
     */
    static std::string calculateMD5(const std::string& input);

    /**
     * @brief Calcule le hachage SHA-256 d'une chaîne.
     * @param input La chaîne d'entrée.
     * @return Le hachage SHA-256 en format hexadécimal.
     */
    static std::string calculateSHA256(const std::string& input);

    /**
     * @brief Calcule le hachage SHA-1 d'une chaîne.
     * @param input La chaîne d'entrée.
     * @return Le hachage SHA-1 en format hexadécimal.
     */
    static std::string calculateSHA1(const std::string& input);

    /**
     * @brief Calcule le hachage SHA-512 d'une chaîne.
     * @param input La chaîne d'entrée.
     * @return Le hachage SHA-512 en format hexadécimal.
     */
    static std::string calculateSHA512(const std::string& input);

    /**
     * @brief Calcule le hachage NTLM d'une chaîne (généralement le mot de passe en UTF-16LE Haché par MD4).
     * @param input La chaîne d'entrée (le mot de passe en clair).
     * @return Le hachage NTLM en format hexadécimal.
     * Note: Cette implémentation de NTLM est simplifiée et suppose un MD4 de l'UTF-16LE.
     * Pour un NTLM complet avec salage et autres, une bibliothèque plus robuste serait nécessaire.
     */
    static std::string calculateNTLM(const std::string& input);

private:
    /**
     * @brief Convertit un tableau d'octets en une chaîne hexadécimale.
     * @param bytes Le tableau d'octets.
     * @param len La longueur du tableau.
     * @return La chaîne hexadécimale.
     */
    static std::string bytesToHex(const unsigned char* bytes, size_t len);
};

#endif // HASH_RECON_H

