#include "hash_recon.h"
#include <cctype>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream> // Pour std::stringstream
#include <vector>  // Pour std::vector dans SHA-256
#include <algorithm> // Pour std::transform dans la génération de mots de passe
#include <cstring>   // Pour memcpy

// =======================================================
// [ PARTIE 1 ] Fonctions d'analyse de type de caractères
// =======================================================

bool estNumerique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool estAlphabetique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    return true;
}

bool estAlphanumerique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalnum(c)) {
            return false;
        }
    }
    return true;
}

bool estHexadecimal(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isxdigit(c)) {
            return false;
        }
    }
    return true;
}

void analyserTypeCaracteresHachage(const std::string& hash_hex) {
    std::cout << "\n" << CR_BLUE << "--- [IN-DEPTH HASH ANALYSIS] -------------------" << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Hash to analyze: " << hash_hex << RESET << std::endl;
    bool est_numerique = estNumerique(hash_hex);
    bool est_alphabetique = estAlphabetique(hash_hex);
    bool est_alphanumerique = estAlphanumerique(hash_hex);
    bool est_hexadecimal = estHexadecimal(hash_hex);

    std::cout << CR_CYAN << "    HASH analysis results:" << RESET << std::endl;
    std::cout << CR_CYAN << "      - Only numeric (0-9): " << (est_numerique ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;
    std::cout << CR_CYAN << "      - Only alphabetic (a-z, A-Z): " << (est_alphabetique ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;
    if (est_alphanumerique) {
        std::cout << CR_GREEN << BOLD << "      - Only alphanumeric (letters or digits): YES, contains [a-z] and [0-9]!" << RESET << std::endl;
    } else {
        std::cout << CR_CYAN << "      - Only alphanumeric (letters or digits): " << CR_RED << "NO" << RESET << std::endl;
    }
    std::cout << CR_CYAN << "      - Only hexadecimal (0-9, a-f, A-F): " << (est_hexadecimal ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;

    if (est_hexadecimal) {
        std::cout << CR_YELLOW << "    [INFO] A hexadecimal hash is very common for MD5, SHA-1, SHA-256, etc." << RESET << std::endl;
    } else {
        std::cout << CR_YELLOW << "    [INFO] This hash does NOT appear to be purely hexadecimal. It could be a different encoding or corrupted." << RESET << std::endl;
    }

    if (est_alphanumerique) {
        std::cout << "\n" << CR_GREEN << BOLD << "    [RECOMMENDATION] For brute-force attacks, consider an alphanumeric character set." << RESET << std::endl;
    } else if (est_numerique) {
        std::cout << "\n" << CR_YELLOW << "    [RECOMMENDATION] For brute-force attacks, a numeric character set might be appropriate." << RESET << std::endl;
    } else if (est_alphabetique) {
        std::cout << "\n" << CR_YELLOW << "    [RECOMMENDATION] For brute-force attacks, an alphabetic character set might be appropriate." << RESET << std::endl;
    }

    if (!est_numerique && !est_alphabetique && !est_hexadecimal && !est_alphanumerique) {
        std::cout << CR_YELLOW << "    [INFO] This hash contains special or unexpected characters. It might be differently encoded or corrupted." << RESET << std::endl;
    }
    std::cout << CR_BLUE << "--------------------------------------------------------" << RESET << std::endl;
}

// =======================================================
// [ PARTIE 2 ] Implémentations des fonctions de Hachage
// =======================================================

// Implémentation de MD5 (code précédent copié ici)
namespace { // Namespace anonyme pour masquer les fonctions d'aide internes
    inline unsigned int rotate_left(unsigned int x, int n) {
        return (x << n) | (x >> (32 - n));
    }
    inline unsigned int F(unsigned int x, unsigned int y, unsigned int z) { return (x & y) | (~x & z); }
    inline unsigned int G(unsigned int x, unsigned int y, unsigned int z) { return (x & z) | (y & ~z); }
    inline unsigned int H(unsigned int x, unsigned int y, unsigned int z) { return x ^ y ^ z; }
    inline unsigned int I(unsigned int x, unsigned int y, unsigned int z) { return y ^ (~z | x); }

    void MD5Transform(unsigned int state[4], const unsigned char block[64]) {
        unsigned int a = state[0], b = state[1], c = state[2], d = state[3];
        unsigned int x[16];
        for (int i = 0; i < 16; ++i) {
            x[i] = ((unsigned int)block[i * 4]) | ((unsigned int)block[i * 4 + 1] << 8) | ((unsigned int)block[i * 4 + 2] << 16) | ((unsigned int)block[i * 4 + 3] << 24);
        }
#define FF(a, b, c, d, x, s, ac) { a += F(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
        FF(a, b, c, d, x[0],  7, 0xd76aa478); FF(d, a, b, c, x[1], 12, 0xe8c7b756); FF(c, d, a, b, x[2], 17, 0x242070db); FF(b, c, d, a, x[3], 22, 0xc1bdceee);
        FF(a, b, c, d, x[4],  7, 0xf57c0faf); FF(d, a, b, c, x[5], 12, 0x4787c62a); FF(c, d, a, b, x[6], 17, 0xa8304613); FF(b, c, d, a, x[7], 22, 0xfd469501);
        FF(a, b, c, d, x[8],  7, 0x698098d8); FF(d, a, b, c, x[9], 12, 0x8b44f7af); FF(c, d, a, b, x[10], 17, 0xffff5bb1); FF(b, c, d, a, x[11], 22, 0x895cd7be);
        FF(a, b, c, d, x[12], 7, 0x6b901122); FF(d, a, b, c, x[13], 12, 0xfd987193); FF(c, d, a, b, x[14], 17, 0xa679438e); FF(b, c, d, a, x[15], 22, 0x49b40821);
#undef FF
#define GG(a, b, c, d, x, s, ac) { a += G(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
        GG(a, b, c, d, x[1],  5, 0xf61e2562); GG(d, a, b, c, x[6],  9, 0xc040b340); GG(c, d, a, b, x[11], 14, 0x265e5a51); GG(b, c, d, a, x[0], 20, 0xe9b6c7aa);
        GG(a, b, c, d, x[5],  5, 0xd62f105d); GG(d, a, b, c, x[10],  9, 0x02441453); GG(c, d, a, b, x[15], 14, 0xd8a1e681); GG(b, c, d, a, x[4], 20, 0xe7d3fbc8);
        GG(a, b, c, d, x[9],  5, 0x21e1cde6); GG(d, a, b, c, x[14],  9, 0xc33707d6); GG(c, d, a, b, x[3], 14, 0xf4d50d87); GG(b, c, d, a, x[8], 20, 0xe23049b4);
        GG(a, b, c, d, x[13],  5, 0xa9e3e905); GG(d, a, b, c, x[2],  9, 0xfcefa3f8); GG(c, d, a, b, x[7], 14, 0x676f02d9); GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);
#undef GG
#define HH(a, b, c, d, x, s, ac) { a += H(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
        HH(a, b, c, d, x[5],  4, 0xfffa3942); HH(d, a, b, c, x[8], 11, 0x8771f681); HH(c, d, a, b, x[11], 16, 0x6d9d6122); HH(b, c, d, a, x[14], 23, 0xfde5380c);
        HH(a, b, c, d, x[1],  4, 0xa4beea44); HH(d, a, b, c, x[4], 11, 0x4bdecfa9); HH(c, d, a, b, x[7], 16, 0xf6bb4baf); HH(b, c, d, a, x[10], 23, 0xbebfbc70);
        HH(a, b, c, d, x[13],  4, 0x289b7ec6); HH(d, a, b, c, x[0], 11, 0xeaa127fa); HH(c, d, a, b, x[3], 16, 0xd4ef3085); HH(b, c, d, a, x[6], 23, 0x04881d05);
        HH(a, b, c, d, x[9],  4, 0xd9d4d039); HH(d, a, b, c, x[12], 11, 0xe6db99e5); HH(c, d, a, b, x[15], 16, 0x1fa27cf8); HH(b, c, d, a, x[2], 23, 0xc4ac5665);
#undef HH
#define II(a, b, c, d, x, s, ac) { a += I(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
        II(a, b, c, d, x[0],  6, 0xf4292244); II(d, a, b, c, x[7], 10, 0x432aff97); II(c, d, a, b, x[14], 15, 0xab9423a7); II(b, c, d, a, x[5], 21, 0xfc93a039);
        II(a, b, c, d, x[12],  6, 0x655b59c3); II(d, a, b, c, x[3], 10, 0x8f0ccc92); II(c, d, a, b, x[10], 15, 0xffeff47d); II(b, c, d, a, x[1], 21, 0x85845dd1);
        II(a, b, c, d, x[8],  6, 0x6fa87e4f); II(d, a, b, c, x[15], 10, 0xfe2ce6e0); II(c, d, a, b, x[6], 15, 0xa3014314); II(b, c, d, a, x[13], 21, 0x4e0811a1);
        II(a, b, c, d, x[4],  6, 0xf7537e82); II(d, a, b, c, x[11], 10, 0xbd3af235); II(c, d, a, b, x[2], 15, 0x2ad7d2bb); II(b, c, d, a, x[9], 21, 0xeb86d391);
#undef II
        state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    }

    void MD5Init(unsigned int state[4]) {
        state[0] = 0x67452301; state[1] = 0xEFCDAB89; state[2] = 0x98BADCFE; state[3] = 0x10325476;
    }

    void MD5Update(unsigned int state[4], unsigned long long& count, unsigned char buffer[64], const unsigned char* input, unsigned int inputLen) {
        unsigned int i, index, partLen;
        index = (unsigned int)((count >> 3) & 0x3F);
        count += ((unsigned long long)inputLen) << 3;
        partLen = 64 - index;
        if (inputLen >= partLen) {
            memcpy(&buffer[index], input, partLen);
            MD5Transform(state, buffer);
            for (i = partLen; i + 63 < inputLen; i += 64) {
                MD5Transform(state, &input[i]);
            }
            index = 0;
        } else { i = 0; }
        memcpy(&buffer[index], &input[i], inputLen - i);
    }

    void MD5Final(unsigned char digest[16], unsigned int state[4], unsigned long long count, unsigned char buffer[64]) {
        unsigned char bits[8]; unsigned int index, padLen;
        for (int i = 0; i < 8; ++i) { bits[i] = (unsigned char)((count >> (i * 8)) & 0xFF); }
        index = (unsigned int)((count >> 3) & 0x3f);
        padLen = (index < 56) ? (56 - index) : (120 - index);
        MD5Update(state, count, buffer, (const unsigned char*)"\x80", 1);
        MD5Update(state, count, buffer, (const unsigned char*)"", padLen);
        MD5Update(state, count, buffer, bits, 8);
        for (int i = 0; i < 4; ++i) {
            digest[i * 4] = (unsigned char)(state[i] & 0xFF);
            digest[i * 4 + 1] = (unsigned char)((state[i] >> 8) & 0xFF);
            digest[i * 4 + 2] = (unsigned char)((state[i] >> 16) & 0xFF);
            digest[i * 4 + 3] = (unsigned char)((state[i] >> 24) & 0xFF);
        }
    }
} // Fin du namespace anonyme pour MD5

std::string md5(const std::string& str) {
    unsigned int state[4]; unsigned long long count = 0; unsigned char buffer[64]; unsigned char digest[16];
    MD5Init(state);
    MD5Update(state, count, buffer, (const unsigned char*)str.c_str(), str.length());
    MD5Final(digest, state, count, buffer);
    std::stringstream ss;
    for (int i = 0; i < 16; ++i) { ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i]; }
    return ss.str();
}

// Implémentation de SHA-256 (code précédent copié ici)
namespace { // Namespace anonyme pour masquer les fonctions d'aide internes
    inline unsigned int rotate_right(unsigned int val, int bits) { return (val >> bits) | (val << (32 - bits)); }
    inline unsigned int Ch(unsigned int x, unsigned int y, unsigned int z) { return (x & y) ^ (~x & z); }
    inline unsigned int Maj(unsigned int x, unsigned int y, unsigned int z) { return (x & y) ^ (x & z) ^ (y & z); }
    inline unsigned int Sigma0(unsigned int x) { return rotate_right(x, 2) ^ rotate_right(x, 13) ^ rotate_right(x, 22); }
    inline unsigned int Sigma1(unsigned int x) { return rotate_right(x, 6) ^ rotate_right(x, 11) ^ rotate_right(x, 25); }
    inline unsigned int sigma0(unsigned int x) { return rotate_right(x, 7) ^ rotate_right(x, 18) ^ (x >> 3); }
    inline unsigned int sigma1(unsigned int x) { return rotate_right(x, 17) ^ rotate_right(x, 19) ^ (x >> 10); }

    static const unsigned int k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    void sha256_transform(unsigned int state[8], const unsigned char block[64]) {
        unsigned int a = state[0], b = state[1], c = state[2], d = state[3];
        unsigned int e = state[4], f = state[5], g = state[6], h = state[7];
        unsigned int w[64];
        for (int i = 0; i < 16; ++i) {
            w[i] = ((unsigned int)block[i * 4]) << 24 | ((unsigned int)block[i * 4 + 1]) << 16 | ((unsigned int)block[i * 4 + 2]) << 8 | ((unsigned int)block[i * 4 + 3]);
        }
        for (int i = 16; i < 64; ++i) { w[i] = sigma1(w[i - 2]) + w[i - 7] + sigma0(w[i - 15]) + w[i - 16]; }
        for (int i = 0; i < 64; ++i) {
            unsigned int temp1 = h + Sigma1(e) + Ch(e, f, g) + k[i] + w[i];
            unsigned int temp2 = Sigma0(a) + Maj(a, b, c);
            h = g; g = f; f = e; e = d + temp1; d = c; c = b; b = a; a = temp1 + temp2;
        }
        state[0] += a; state[1] += b; state[2] += c; state[3] += d;
        state[4] += e; state[5] += f; state[6] += g; state[7] += h;
    }
} // Fin du namespace anonyme pour SHA-256

std::string sha256(const std::string& str) {
    unsigned int state[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    unsigned long long total_bits = str.length() * 8;
    std::string padded_str = str;
    padded_str += (char)0x80;
    unsigned int initial_len = padded_str.length();
    unsigned int padding_len = 0;
    if ((initial_len * 8) % 512 != 448) {
        padding_len = (512 - ((initial_len * 8) % 512) - 64) / 8;
        if (padding_len < 0) padding_len += 64;
    } else { padding_len = 64 / 8; }
    padded_str.append(padding_len, '\0');
    for (int i = 0; i < 8; ++i) { padded_str += (char)((total_bits >> (56 - i * 8)) & 0xFF); }
    for (size_t i = 0; i < padded_str.length(); i += 64) {
        sha256_transform(state, (const unsigned char*)padded_str.substr(i, 64).c_str());
    }
    std::stringstream ss;
    for (int i = 0; i < 8; ++i) { ss << std::hex << std::setw(8) << std::setfill('0') << state[i]; }
    return ss.str();
}

// =======================================================
// [ PARTIE 3 ] Logique de force brute et analyse du mot de passe
// =======================================================

// Fonction pour générer la prochaine chaîne dans l'ordre lexicographique
bool next_permutation_chars(std::string& s, const std::string& charset) {
    int n = s.length();
    if (n == 0) return false;

    for (int i = n - 1; i >= 0; --i) {
        size_t current_char_index = charset.find(s[i]);
        if (current_char_index == std::string::npos) {
            // Devrait pas arriver si le charset est bien géré
            return false;
        }

        if (current_char_index < charset.length() - 1) {
            s[i] = charset[current_char_index + 1];
            return true;
        } else {
            s[i] = charset[0]; // Réinitialise le caractère à la première du charset
        }
    }
    return false; // Tous les caractères ont été réinitialisés, la longueur doit augmenter
}

void crackerEtAnalyserMotDePasse(const std::string& hash_cible, const std::string& type_hash, int max_longueur) {
    std::cout << "\n" << CR_MAGENTA << "--- [PASSWORD CRACKING & ANALYSIS] -------------------" << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Target Hash: " << hash_cible << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Hash Algorithm: " << type_hash << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Max Password Length: " << max_longueur << RESET << std::endl;

    std::string numeric_charset = "0123456789";
    std::string lower_alpha_charset = "abcdefghijklmnopqrstuvwxyz";
    std::string upper_alpha_charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string alpha_charset = lower_alpha_charset + upper_alpha_charset;
    std::string alphanumeric_charset = numeric_charset + alpha_charset;

    std::vector<std::pair<std::string, std::string>> charsets_to_try;
    charsets_to_try.push_back({"Numeric (0-9)", numeric_charset});
    charsets_to_try.push_back({"Alphabetic (a-Z)", alpha_charset});
    charsets_to_try.push_back({"Alphanumeric (a-Z, 0-9)", alphanumeric_charset}); // <-- Ligne corrigée
    // Tu peux ajouter d'autres jeux de caractères ici (ex: avec caractères spéciaux)

    std::string mot_de_passe_trouve = "";
    std::string type_mdp_trouve = "Not Found";
    bool found = false;

    for (const auto& charset_entry : charsets_to_try) {
        if (found) break; // Arrêter si le mot de passe est déjà trouvé

        std::cout << CR_CYAN << "\n    Attempting with character set: " << charset_entry.first << RESET << std::endl;
        const std::string& current_charset = charset_entry.second;

        for (int len = 1; len <= max_longueur; ++len) {
            if (found) break; // Arrêter si le mot de passe est déjà trouvé
            std::cout << CR_DARK_GRAY << "      Trying length: " << len << RESET << std::endl;

            std::string current_password(len, current_charset[0]); // Commence avec la première combinaison

            // Boucle de génération et de test de mots de passe
            do {
                std::string generated_hash;

                // Calcule le hash en fonction du type d'algorithme spécifié
                if (type_hash == "MD5") {
                    generated_hash = md5(current_password);
                } else if (type_hash == "SHA256") {
                    generated_hash = sha256(current_password);
                }
                // else if (type_hash == "SHA1") { ... } // Ajouter d'autres algorithmes ici
                else {
                    std::cerr << CR_RED << "Error: Unsupported hash algorithm '" << type_hash << "'" << RESET << std::endl;
                    return;
                }

                if (generated_hash == hash_cible) {
                    mot_de_passe_trouve = current_password;
                    type_mdp_trouve = charset_entry.first;
                    found = true;
                    break;
                }
                // IMPORTANT: Gérer les très longues itérations, tu peux ajouter un compteur
                // et afficher des "..." pour montrer la progression ou ajouter une limite de temps.

            } while (next_permutation_chars(current_password, current_charset));
        }
    }

    std::cout << "\n" << CR_YELLOW << "--- [CRACKING RESULTS] -------------------------" << RESET << std::endl;
    if (found) {
        std::cout << CR_GREEN << BOLD << "    SUCCESS! Password Found: " << mot_de_passe_trouve << RESET << std::endl;
        std::cout << CR_GREEN << BOLD << "    Original Password Type: " << type_mdp_trouve << RESET << std::endl;

        // Utilise tes fonctions existantes pour une analyse plus fine du mot de passe retrouvé
        std::cout << "\n" << CR_CYAN << "    Detailed analysis of found password (" << mot_de_passe_trouve << "):" << RESET << std::endl;
        std::cout << CR_CYAN << "      - Only numeric (0-9): " << (estNumerique(mot_de_passe_trouve) ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;
        std::cout << CR_CYAN << "      - Only alphabetic (a-z, A-Z): " << (estAlphabetique(mot_de_passe_trouve) ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;
        std::cout << CR_CYAN << "      - Only alphanumeric (letters or digits): " << (estAlphanumerique(mot_de_passe_trouve) ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;

    } else {
        std::cout << CR_RED << BOLD << "    Password Not Found within the specified parameters (length/character sets)." << RESET << std::endl;
    }
    std::cout << CR_MAGENTA << "--------------------------------------------------------" << RESET << std::endl;
}
