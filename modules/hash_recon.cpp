#include <iostream> // Pour les entrées/sorties (cin, cout)
#include <string>   // Pour manipuler les chaînes de caractères
#include <vector>   // Pour stocker des types de hachages
#include <algorithm> // Pour std::tolower (pour convertir en minuscules)

// Fonction pour convertir une chaîne en minuscules
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

// Fonction pour vérifier si une chaîne est hexadécimale
bool isHex(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!std::isxdigit(c)) { // isxdigit vérifie si le caractère est un chiffre hexadécimal
            return false;
        }
    }
    return true;
}

// Fonction principale d'analyse du hachage
void analyzeHash(const std::string& hash) {
    int length = hash.length();
    bool hex_chars = isHex(hash); // Vérifie si tous les caractères sont hexadécimaux

    std::cout << "\n--- Résultat de l'analyse ---\n";

    if (length == 32 && hex_chars) {
        std::cout << "Suggestion décisive : Hachage de type **MD5** probable.\n";
        std::cout << "Paramètres de cassage recommandés : Attaque par dictionnaire ou force brute pour des courtes longueurs de mots de passe (ex: 1-8 caractères alphanumériques).\n";
    } else if (length == 40 && hex_chars) {
        std::cout << "Suggestion décisive : Hachage de type **SHA-1** probable.\n";
        std::cout << "Paramètres de cassage recommandés : Attaque par dictionnaire ou force brute ciblée (ex: 1-10 caractères alphanumériques, ou variantes de mots de passe courants).\n";
    } else if (length == 64 && hex_chars) {
        std::cout << "Suggestion décisive : Hachage de type **SHA-256** probable.\n";
        std::cout << "Paramètres de cassage recommandés : Attaque par dictionnaire étendue, ou force brute avec des masques de caractères spécifiques (ex: si des majuscules/minuscules/chiffres/symboles sont attendus).\n";
    } else if (length == 128 && hex_chars) {
        std::cout << "Suggestion décisive : Hachage de type **SHA-512** probable.\n";
        std::cout << "Paramètres de cassage recommandés : Généralement très difficile à casser par force brute sans informations supplémentaires. Privilégiez l'attaque par dictionnaire avec de très grands fichiers ou des listes de fuites de données connues.\n";
    } else if (length == 32 && (hash[16] == ':' || hash[0] == '$')) { // Tentative de détection NTLM/LM, etc.
        // Cette détection est très simpliste et devrait être affinée pour des cas réels
        std::cout << "Suggestion décisive : Hachage de type **NTLM / LM** (ou format similaire avec sel) probable.\n";
        std::cout << "Paramètres de cassage recommandés : Outils spécifiques pour NTLM/LM, attaque par dictionnaire avec des listes de mots de passe Windows ou de type NTLM.\n";
    }
    // Tu peux ajouter d'autres conditions pour d'autres types de hachages ici
    // par exemple, pour bcrypt, scrypt, etc., qui ont des formats plus complexes et souvent un préfixe reconnaissable.
    else {
        std::cout << "Suggestion décisive : Type de hachage **indéterminé ou non pris en charge**.\n";
        std::cout << "Paramètres de cassage recommandés : Vérifiez la source du hachage. Essayez une recherche en ligne avec le format du hachage pour identifier son type. Le cassage par force brute général sera très long.\n";
    }
    std::cout << "--------------------------------\n";
}

int main() {
    std::string inputHash;

    std::cout << "Bienvenue dans Hash Recon ! Entrez le hachage à analyser :\n";
    std::cout << "Hachage : ";
    std::cin >> inputHash;

    // Convertir le hachage en minuscules pour faciliter la comparaison hexadécimale
    // et éviter les problèmes de casse pour isHex.
    inputHash = toLower(inputHash);

    analyzeHash(inputHash);

    return 0;
}

