#include <iostream>  // For input/output (cin, cout)
#include <string>    // For string manipulation
#include <vector>    // For vector
#include <algorithm> // For std::transform, std::tolower
#include <cctype>    // For std::isxdigit, std::isdigit, std::isalpha
#include <set>       // For unique characters analysis (optional, but good for charset deduction)

// --- ANSI Escape Codes for Colors ---
// Ces codes permettent d'afficher du texte en couleur dans le terminal.
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

// Nouvelle fonction pour tenter de deviner le jeu de caractères potentiel du mot de passe
// basée sur l'analyse de la longueur du hash et des hachages eux-mêmes.
// Ceci reste une HEURISTIQUE et non une certitude.
void suggestCharsetAndLength(int length, const std::string& hashType) {
    // Les hachages sont toujours hexadécimaux, donc on ne peut pas déduire le charset
    // du mot de passe original directement à partir des caractères du HASH.
    // Les suggestions sont basées sur les PRATIQUES COURANTES de mots de passe
    // pour les différentes longueurs et types de hachages.

    // On va utiliser un peu d'heuristique pour les mots de passe TRÈS courts,
    // où le contenu du hachage peut parfois (mais rarement) donner un indice faible.
    // Par exemple, si un hachage MD5 très court provient d'un mot de passe "123",
    // le hachage résultant sera toujours hexadécimal, mais les hachages de courts
    // mots de passe numériques ou alphabétiques peuvent avoir des motifs subtils
    // (mais non fiables) en pratique.

    // Pour éviter le "flood", nous allons nous concentrer sur une seule suggestion principale.
    std::string charset_suggestion;
    std::string length_suggestion;

    if (hashType == "MD5") {
        if (length <= 8) { // Courts mots de passe
            charset_suggestion = "[a-z0-9]";
            length_suggestion = "1-8";
        } else if (length <= 12) { // Mots de passe de longueur moyenne
            charset_suggestion = "[a-zA-Z0-9]";
            length_suggestion = "8-12";
        } else { // Mots de passe plus longs ou complexes
            charset_suggestion = "[a-zA-Z0-9!@#$%^&*()_+-=[]{}|;:,.<>?/]"; // All common chars
            length_suggestion = "8+ (likely longer)";
        }
    } else if (hashType == "SHA-1") {
        if (length <= 10) {
            charset_suggestion = "[a-zA-Z0-9]";
            length_suggestion = "1-10";
        } else {
            charset_suggestion = "[a-zA-Z0-9!@#$%^&*()_+-=[]{}|;:,.<>?/]";
            length_suggestion = "10+ (likely longer)";
        }
    } else if (hashType == "SHA-256" || hashType == "SHA-512") {
        // Pour les SHA-256/512, les mots de passe sont généralement plus complexes et longs
        charset_suggestion = "[a-zA-Z0-9!@#$%^&*()_+-=[]{}|;:,.<>?/]"; // Très large
        length_suggestion = "8+ (typically 10-16+)";
    } else if (hashType == "NTLM / LM") {
        // NTLM/LM ont des spécificités
        charset_suggestion = "NTLM: [a-zA-Z0-9!@#$%^&*()] (common chars); LM: [a-zA-Z0-9] (uppercase insensitive, 7 chars per block)";
        length_suggestion = "NTLM: 1-128; LM: 7 chars per block";
    }
    // Pour d'autres types de hash, on pourrait ajouter des logiques spécifiques.
    // Par défaut, si le type de hash n'est pas reconnu par les conditions ci-dessus :
    else {
        charset_suggestion = "[a-zA-Z0-9!@#$%^&*()]"; // Charset générique large
        length_suggestion = "Unknown (try common ranges)";
    }

    // Affichage de la suggestion unique et concise.
    std::cout << RED << BOLD << "RECOMMANDED BY RECON:" << RESET << "\n";
    std::cout << YELLOW << "  -> Charset: " << charset_suggestion << RESET << "\n";
    std::cout << YELLOW << "  -> Length Range: " << length_suggestion << RESET << "\n";
}


// Fonction principale d'analyse de hachage
void analyzeHash(const std::string& hash) {
    int length = hash.length();
    bool hex_chars = isHex(hash); // Vérifie si tous les caractères sont hexadécimaux

    std::cout << "\n" << CYAN << "--- Analyse du Hash ---" << RESET << "\n";

    std::string detected_hash_type = "Undetermined";

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
    } else if (length == 32 && (hash.rfind(":::", 0) == 0 || hash[0] == '$' || hash.find("LM:", 0) == 0 || (length > 0 && hash.find(":", 0) != std::string::npos))) {
        // Tentative de détection NTLM/LM et autres formats avec ':' ou '$' au début.
        // NTLM peut être de 32 hex ou plus complexe avec des ":" pour user:hash
        // LM est 32 hex souvent avec "LM:"
        // D'autres comme bcrypt, scrypt commencent par '$2a$', '$2b$', '$6$', '$argon2id$' etc.
        detected_hash_type = "NTLM / LM (ou format spécial avec sel)";
        std::cout << "Type de Hash : " << YELLOW << BOLD << detected_hash_type << RESET << "\n";
    }
    // Ajoutez ici d'autres détections basées sur la structure si nécessaire
    // Ex: if (hash.rfind("$2a$", 0) == 0 || hash.rfind("$2b$", 0) == 0) { detected_hash_type = "Bcrypt"; }

    else {
        detected_hash_type = "Indéterminé ou non supporté";
        std::cout << "Type de Hash : " << RED << BOLD << detected_hash_type << RESET << "\n";
    }

    // Afficher la suggestion de paramètres seulement si le hash est déterminé ou supporté
    if (detected_hash_type != "Indéterminé ou non supporté") {
        suggestCharsetAndLength(length, detected_hash_type);
    } else {
        std::cout << RED << BOLD << "RECOMMANDED BY RECON:" << RESET << "\n";
        std::cout << YELLOW << "  -> Action: Vérifiez la source du hash. Essayez une recherche en ligne pour identifier son type." << RESET << "\n";
        std::cout << YELLOW << "  -> Note: La force brute générale sera très coûteuse en temps." << RESET << "\n";
    }

    std::cout << "\n" << CYAN << "-----------------------" << RESET << "\n";
}

// Fonction main pour les tests (à enlever si intégré comme module)
/*
int main() {
    std::string hash_input;
    std::cout << "Entrez le hash à analyser : ";
    std::cin >> hash_input;
    analyzeHash(hash_input);
    return 0;
}
*/
