#include <iostream>  // For input/output (cin, cout)
#include <string>    // For string manipulation
#include <vector>    // For vector
#include <algorithm> // For std::transform, std::tolower
#include <cctype>    // For std::isxdigit, std::isdigit, std::isalpha
#include <set>       // For unique characters analysis (optional, but good for charset deduction)
#include <sstream>   // For std::stringstream (used in hex_to_string)
#include <iomanip>   // For std::hex, std::setw, std::setfill (used in hash functions)

// --- Cryptographic Libraries (conceptual inclusion) ---
// Pour calculer de "vrais" hachages, nous aurions besoin d'une bibliothèque cryptographique.
// Pour cet exemple, je vais implémenter des versions simplifiées ou utiliser des stubs
// pour montrer le concept. En production, on utiliserait OpenSSL, Crypto++ ou d'autres.
// Pour MD5, SHA-1, SHA-256, nous aurions besoin de ces implémentations.
// Je vais fournir des stubs qui retournent des exemples de hachages pour l'illustration.

// Pour un vrai projet, tu intégrerais des bibliothèques comme OpenSSL pour ces fonctions :
// #include <openssl/md5.h>
// #include <openssl/sha.h>

// --- ANSI Escape Codes for Colors ---
#define RESET   "\033[0m"       // Réinitialise la couleur à la valeur par défaut
#define RED     "\033[0;31m"    // Texte rouge
#define GREEN   "\033[0;32m"    // Texte vert
#define YELLOW  "\033[0;33m"    // Texte jaune
#define BLUE    "\033[0;34m"    // Texte bleu
#define MAGENTA "\033[0;35m"    // Texte magenta
#define CYAN    "\033[0;36m"    // Texte cyan
#define BOLD    "\033[1m"       // Texte en gras

// --- Fonctions Utilitaires ---

// Fonction pour convertir une chaîne en minuscules (utile pour la normalisation)
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

// Fonction pour vérifier si une chaîne est entièrement hexadécimale
bool isHex(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!std::isxdigit(c)) {
            return false;
        }
    }
    return true;
}

// Fonction pour convertir un tableau d'octets en chaîne hexadécimale
std::string bytesToHex(const unsigned char* bytes, size_t len) {
    std::stringstream ss;
    for (size_t i = 0; i < len; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

// --- Fonctions de Hachage (STUBS / SIMPLIFIÉES pour illustration) ---
// EN PRODUCTION, UTILISE DE VRAIES LIBRAIRIES CRYPTOGRAPHIQUES !

// Simule un hachage MD5. En réalité, ceci utiliserait une bibliothèque comme OpenSSL.
std::string calculateMD5(const std::string& input) {
    // Pour cet exemple, nous allons utiliser une simple simulation.
    // Une vraie implémentation MD5 serait beaucoup plus complexe.
    // L'idée est de montrer comment on générerait un hash pour le comparer.
    // Exemple de hash MD5 pour "password": 5f4dcc3b5aa765d61d8327deb882cf99
    // Exemple de hash MD5 pour "12345": 827ccb0eea8a706c4c34a16891f84e7b
    if (input == "password") return "5f4dcc3b5aa765d61d8327deb882cf99";
    if (input == "12345") return "827ccb0eea8a706c4c34a16891f84e7b";
    if (input == "P4$$w0rd") return "31a483a995e69e061ddf38a59f518e6e"; // Example with mixed case and symbols
    if (input == "admin") return "21232f297a57a5a743894a0e4a801fc3";
    // Si l'entrée n'est pas dans nos exemples, on retourne un hash générique
    // qui imite la longueur d'un MD5.
    return "00000000000000000000000000000000"; // Dummy MD5
}

// Simule un hachage SHA-1.
std::string calculateSHA1(const std::string& input) {
    // Exemple SHA-1 pour "password": 5baa61e4c9b93f3f0682250b6cf8331b7ee68fd8
    // Exemple SHA-1 pour "12345": 8cb22cc7728f323a07804473855a95217498c8c5
    if (input == "password") return "5baa61e4c9b93f3f0682250b6cf8331b7ee68fd8";
    if (input == "12345") return "8cb22cc7728f323a07804473855a95217498c8c5";
    if (input == "P4$$w0rd") return "cd87e1a3a6962327c515a676b7e5e347e3a9f0ee";
    if (input == "admin") return "d033e22ae348fbc94d84f86d6283735db1520e58";
    return "0000000000000000000000000000000000000000"; // Dummy SHA-1
}

// Simule un hachage SHA-256.
std::string calculateSHA256(const std::string& input) {
    // Exemple SHA-256 pour "password": 5e884898da28047151d0e56f8dc6292773603d0d6aabbd62a11ef721d1542d8d
    if (input == "password") return "5e884898da28047151d0e56f8dc6292773603d0d6aabbd62a11ef721d1542d8d";
    if (input == "12345") return "c7b1122a36d2ef29d0f411516e87f8f94f923e4d9c4902127267a57a6e14713e";
    if (input == "P4$$w0rd") return "a2c890731557088198f395632298c614b9866385d8525e985b967812f2c8d2a6";
    if (input == "admin") return "8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918";
    return "0000000000000000000000000000000000000000000000000000000000000000"; // Dummy SHA-256
}

// Simule un hachage SHA-512.
std::string calculateSHA512(const std::string& input) {
    // Les hashs SHA-512 sont très longs, voici un exemple d'une partie seulement.
    // Exemple SHA-512 pour "password": b109f3bbbc244eb82441917ed06d618b90087ad2885947235a96ff1b033d17ff...
    if (input == "password") return "b109f3bbbc244eb82441917ed06d618b90087ad2885947235a96ff1b033d17ffc0c6b1b4d34293f7fbc410425a81a9c5123d47fb8914b3dc04c0049e755b38d3";
    return "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"; // Dummy SHA-512
}

// Fonction pour tenter de deviner le jeu de caractères potentiel du mot de passe
// basée sur l'analyse de la longueur du hash et des hachages eux-mêmes.
// Ceci reste une HEURISTIQUE et non une certitude.
void suggestCharsetAndLength(int length, const std::string& hashType, const std::string& inputHash) {
    std::cout << RED << BOLD << "RECOMMANDED BY RECON:" << RESET << "\n";

    // Les hachages sont toujours hexadécimaux, donc on ne peut pas déduire le charset
    // du mot de passe original directement à partir des caractères du HASH.
    // Les suggestions sont basées sur les PRATIQUES COURANTES de mots de passe
    // pour les différentes longueurs et types de hachages.

    std::string charset_suggestion;
    std::string length_suggestion;
    std::string attack_strategy_suggestion;

    if (hashType == "MD5") {
        std::cout << YELLOW << "  -> Type d'attaque recommandé pour MD5 (souvent rapide pour courts mots de passe):" << RESET << "\n";
        if (length <= 8) { // Mots de passe très courts (e.g., 1-8 caractères)
            charset_suggestion = "[0-9] (numérique pur)";
            length_suggestion = "1-8";
            attack_strategy_suggestion = "Brute force rapide sur nombres, puis minuscules [a-z]. Attaque par dictionnaire sur mots de passe très communs.";
            std::cout << YELLOW << "    -> Teste des exemples simples : " << RESET << "\n";
            std::vector<std::string> test_passwords = {"12345", "password", "admin", "test", "root"};
            for (const auto& pw : test_passwords) {
                if (calculateMD5(pw) == inputHash) {
                    std::cout << GREEN << "      -> Hash correspond à '" << pw << "'! (MD5)" << RESET << "\n";
                    return; // Trouvé, on arrête les suggestions détaillées
                }
            }
        } else if (length <= 12) { // Mots de passe de longueur moyenne (e.g., 8-12 caractères)
            charset_suggestion = "[a-zA-Z0-9] (alphanumérique mixte)";
            length_suggestion = "8-12";
            attack_strategy_suggestion = "Brute force avec majuscules/minuscules et chiffres. Attaque par dictionnaire avec mutations.";
        } else { // Mots de passe plus longs ou complexes
            charset_suggestion = "[a-zA-Z0-9!@#$%^&*()_+-=[]{}|;:,.<>?/] (caractères spéciaux inclus)";
            length_suggestion = "12+ (potentiellement très long)";
            attack_strategy_suggestion = "Attaque par dictionnaire étendue, puis force brute avec des masques si possible (e.g., ?l?d?s pour minuscules, chiffres, symboles).";
        }
    } else if (hashType == "SHA-1") {
        std::cout << YELLOW << "  -> Type d'attaque recommandé pour SHA-1 (plus résistant que MD5 mais toujours vulnérable) :" << RESET << "\n";
        if (length <= 10) {
            charset_suggestion = "[a-zA-Z0-9] (alphanumérique mixte)";
            length_suggestion = "1-10";
            attack_strategy_suggestion = "Brute force sur petits charsets. Dictionnaire.";
        } else {
            charset_suggestion = "[a-zA-Z0-9!@#$%^&*()_+-=[]{}|;:,.<>?/] (caractères spéciaux inclus)";
            length_suggestion = "10+ (généralement plus de 12)";
            attack_strategy_suggestion = "Attaque par dictionnaire robuste, utilisation de règles de mutation, puis force brute ciblée.";
        }
    } else if (hashType == "SHA-256" || hashType == "SHA-512") {
        std::cout << YELLOW << "  -> Type d'attaque recommandé pour SHA-256/SHA-512 (très résistant à la force brute directe) :" << RESET << "\n";
        charset_suggestion = "[a-zA-Z0-9!@#$%^&*()_+-=[]{}|;:,.<>?/] (caractères spéciaux très probables)";
        length_suggestion = "8+ (typiquement 12-20+ pour des mots de passe robustes)";
        attack_strategy_suggestion = "Attaque par dictionnaire avancée avec des listes de mots de passe très volumineuses. Rainbow tables (moins efficaces pour SHA-256/512). Si possible, utilisation de GPUs pour une force brute distribuée.";
    } else if (hashType == "NTLM / LM (ou format spécial avec sel)") {
        std::cout << YELLOW << "  -> Type d'attaque recommandé pour NTLM/LM (spécifique aux systèmes Windows) :" << RESET << "\n";
        charset_suggestion = "NTLM: [a-zA-Z0-9!@#$%^&*()] (commun); LM: [a-zA-Z0-9] (majuscules insensibles, max 7 caractères par bloc)";
        length_suggestion = "NTLM: 1-128; LM: 7 chars par bloc";
        attack_strategy_suggestion = "Outils spécialisés comme Hashcat/John the Ripper avec des listes de mots de passe Windows spécifiques et des règles NTLM/LM.";
    }
    else {
        charset_suggestion = "[a-zA-Z0-9!@#$%^&*()]"; // Charset générique large
        length_suggestion = "Inconnu (essayez des plages courantes)";
        attack_strategy_suggestion = "Vérifiez la source du hash. Essayez une recherche en ligne pour identifier son type. La force brute générale sera très coûteuse en temps.";
    }

    std::cout << YELLOW << "  -> Charset suggéré: " << charset_suggestion << RESET << "\n";
    std::cout << YELLOW << "  -> Longueur suggérée: " << length_suggestion << RESET << "\n";
    std::cout << YELLOW << "  -> Stratégie d'attaque: " << attack_strategy_suggestion << RESET << "\n";

    // Si le hash est court et est un MD5, on peut tenter de le "deviner" avec quelques très petits mots de passe
    // C'est la seule forme de "calcul réel" que l'on peut raisonnablement inclure ici,
    // en générant des hashs connus et en les comparant.
    if (hashType == "MD5" && length <= 8) { // Pour les MD5 très courts
        std::cout << YELLOW << "  -> Vérification de quelques mots de passe numériques/alphanumériques très courts pour MD5 : " << RESET << "\n";
        std::vector<std::string> common_short_passwords = {"1", "12", "123", "1234", "12345", "test", "pass", "admin"};
        for (const auto& pw : common_short_passwords) {
            if (calculateMD5(pw) == inputHash) {
                std::cout << GREEN << "      -> Hash correspond à '" << pw << "'! (MD5)" << RESET << "\n";
            }
        }
    }
}


// Fonction principale d'analyse de hachage
void analyzeHash(const std::string& hash) {
    int length = hash.length();
    bool hex_chars = isHex(hash); // Vérifie si tous les caractères sont hexadécimaux

    std::cout << "\n" << CYAN << "--- Analyse du Hash ---" << RESET << "\n";

    std::string detected_hash_type = "Undetermined";

    // Normalisation de la chaîne de hachage pour faciliter la détection
    std::string normalized_hash = toLower(hash);

    // Détection basée sur la longueur et la nature hexadécimale
    if (length == 32 && hex_chars) {
        detected_hash_type = "MD5";
        std::cout << "Type de Hash : " << GREEN << BOLD << "MD5" << RESET << "\n";
    } else if (length == 40 && hex_chars) {
        detected_hash_type = "SHA-1";
        std::cout << "Type de Hash : " << GREEN << BOLD << "SHA-1" << RESET << "\n";
    } else if (length == 64 && hex_chars) {
        detected_hash_type = "SHA-256";
        std::cout << "Type de Hash : " << GREEN << BOLD << "SHA-256" << RESET << "\n";
    } else if (length == 128 && hex_chars) {
        detected_hash_type = "SHA-512";
        std::cout << "Type de Hash : " << GREEN << BOLD << "SHA-512" << RESET << "\n";
    }
    // Détection de formats spécifiques (NTLM/LM, bcrypt, scrypt, etc.)
    // Ces formats sont souvent préfixés ou ont une structure particulière.
    else if (normalized_hash.rfind(":::", 0) == 0 || // Possible NTLM format (user:::hash)
               (length == 32 && normalized_hash.find("lm:", 0) == 0) || // LM hash (partie du NTLM)
               (length > 0 && normalized_hash.find(":", 0) != std::string::npos && hex_chars) // Peut-être un hash avec sel ou user:hash, mais si c'est hex, c'est probablement NTLM pur ou avec sel
               ) {
        detected_hash_type = "NTLM / LM (ou format spécial avec sel)";
        std::cout << "Type de Hash : " << YELLOW << BOLD << detected_hash_type << RESET << "\n";
    }
    // Ajout de détections pour des formats de hachage courants (bcrypt, scrypt, Argon2)
    else if (normalized_hash.rfind("$2a$", 0) == 0 || normalized_hash.rfind("$2b$", 0) == 0 || normalized_hash.rfind("$2y$", 0) == 0) {
        detected_hash_type = "Bcrypt";
        std::cout << "Type de Hash : " << MAGENTA << BOLD << detected_hash_type << RESET << "\n";
    }
    else if (normalized_hash.rfind("$s0$", 0) == 0 || normalized_hash.rfind("$s1$", 0) == 0 || normalized_hash.rfind("$s2$", 0) == 0) {
        detected_hash_type = "Scrypt";
        std::cout << "Type de Hash : " << MAGENTA << BOLD << detected_hash_type << RESET << "\n";
    }
    else if (normalized_hash.rfind("$argon2i$", 0) == 0 || normalized_hash.rfind("$argon2id$", 0) == 0) {
        detected_hash_type = "Argon2";
        std::cout << "Type de Hash : " << MAGENTA << BOLD << detected_hash_type << RESET << "\n";
    }
    else {
        detected_hash_type = "Indéterminé ou non supporté";
        std::cout << "Type de Hash : " << RED << BOLD << detected_hash_type << RESET << "\n";
    }

    std::cout << "Longueur du Hash : " << BOLD << length << " caractères" << RESET << "\n";
    std::cout << "Tous les caractères sont hexadécimaux : " << (hex_chars ? GREEN : RED) << (hex_chars ? "Oui" : "Non") << RESET << "\n";

    // Afficher la suggestion de paramètres seulement si le hash est déterminé ou supporté
    if (detected_hash_type != "Indéterminé ou non supporté") {
        suggestCharsetAndLength(length, detected_hash_type, normalized_hash);
    } else {
        std::cout << RED << BOLD << "RECOMMANDED BY RECON:" << RESET << "\n";
        std::cout << YELLOW << "  -> Action: Vérifiez la source du hash. Essayez une recherche en ligne pour identifier son type." << RESET << "\n";
        std::cout << YELLOW << "  -> Note: La force brute générale sera très coûteuse en temps pour un hash non identifié." << RESET << "\n";
    }

    std::cout << "\n" << CYAN << "-----------------------" << RESET << "\n";
}

// Fonction main pour les tests (à décommenter pour tester en standalone)
/*
int main() {
    std::string hash_input;
    std::cout << "Entrez le hash à analyser : ";
    std::cin >> hash_input;
    analyzeHash(hash_input);

    // Exemples de test pour les fonctions de hachage (pour illustrer)
    // std::cout << "\n--- Exemples de Hachage ---\n";
    // std::cout << "MD5 de 'password': " << calculateMD5("password") << "\n";
    // std::cout << "SHA-1 de 'password': " << calculateSHA1("password") << "\n";
    // std::cout << "SHA-256 de 'password': " << calculateSHA256("password") << "\n";
    // std::cout << "SHA-512 de 'password': " << calculateSHA512("password") << "\n";
    // std::cout << "---------------------------\n";

    return 0;
}
*/
