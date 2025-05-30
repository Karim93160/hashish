#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm> // Pour std::transform
#include <chrono>    // Pour la mesure du temps
#include <iomanip>   // Pour std::setw, std::setfill
#include <openssl/evp.h> // Fonctions de hachage OpenSSL
#include <openssl/err.h> // Pour ERR_print_errors_fp
#include <random>    // Pour std::mt19937 et std::uniform_int_distribution
#include <limits>    // Pour std::numeric_limits
#include <map>       // Pour std::map
#include <thread>    // Pour std::this_thread::sleep_for

// --- Codes de couleurs ANSI pour le terminal ---
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLD    "\033[1m"       /* Bold */
#define FAINT   "\033[2m"       /* Faint/Dim */
#define ITALIC  "\033[3m"       /* Italic */
#define UNDERLINE "\033[4m"     /* Underline */
#define BLINK   "\033[5m"       /* Blink */
#define REVERSE "\033[7m"       /* Reverse */
#define HIDDEN  "\033[8m"       /* Hidden */
#define STRIKETHROUGH "\033[9m" /* Strikethrough */

// Couleurs personnalisées pour l'ambiance Cracker Mood
#define CR_RED    RED BOLD
#define CR_GREEN  GREEN BOLD
#define CR_YELLOW YELLOW BOLD
#define CR_BLUE   BLUE BOLD FAINT
#define CR_CYAN   CYAN BOLD
#define CR_MAGENTA MAGENTA BOLD FAINT
#define CR_WHITE  WHITE BOLD
#define CR_DARK_GRAY "\033[90m" // Gris foncé pour les infos moins importantes

// --- Fonctions utilitaires (reprises de hashcracker.cpp) ---

// Convertit un tableau d'octets (digest binaire) en une chaîne hexadécimale
std::string bytes_to_hex_string(const unsigned char* bytes, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
    }
    return ss.str();
}

// Calcule le hachage d'une chaîne d'entrée en utilisant un type de digest OpenSSL spécifié
std::string calculate_hash_openssl(const std::string& input, const EVP_MD* digest_type) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
        // Gérer l'erreur de création du contexte
        return "";
    }

    if (1 != EVP_DigestInit_ex(mdctx, digest_type, nullptr)) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }
    if (1 != EVP_DigestUpdate(mdctx, input.c_str(), input.length())) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;

    if (1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len)) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);
    return bytes_to_hex_string(digest, digest_len);
}

// Obtient le pointeur vers la fonction de digest OpenSSL correspondant au type de hash
const EVP_MD* get_openssl_digest_type(const std::string& hash_type_str) {
    if (hash_type_str == "MD5") return EVP_md5();
    if (hash_type_str == "SHA1") return EVP_sha1();
    if (hash_type_str == "SHA256") return EVP_sha256();
    if (hash_type_str == "SHA384") return EVP_sha384();
    if (hash_type_str == "SHA512") return EVP_sha512();
    return nullptr;
}

// Fonction de réduction : transforme un hash en un mot de passe
// La complexité de cette fonction de réduction est cruciale pour l'efficacité de la table arc-en-ciel.
// Pour une "vraie" table arc-en-ciel, la fonction de réduction devrait varier avec l'index de la fonction
// dans la chaîne, pour éviter les "collisions" (chaînes qui se rejoignent).
// Ici, nous utilisons une simplification basée sur le charset et la longueur cible.
// `r_index` est important : il simule une variation de la fonction de réduction.
std::string reduce_hash(const std::string& hash, size_t target_len, const std::string& charset, int r_index) {
    if (charset.empty() || target_len == 0) return "";

    std::string reduced_string = "";
    // Pour cet exemple, nous allons utiliser une méthode simple basée sur le hash
    // et l'index de la fonction de réduction (r_index) pour déterminer les caractères.
    // Une implémentation réelle serait plus robuste et utiliserait par exemple
    // des bits spécifiques du hash après XOR avec l'index de réduction.

    // Utilisons un générateur de nombres pseudo-aléatoires déterministe basé sur le hash et r_index
    // pour "simuler" la réduction. Cela garantit que la même entrée produit la même sortie.
    std::vector<unsigned int> seed_data;
    for (char c : hash) { seed_data.push_back(static_cast<unsigned int>(c)); }
    seed_data.push_back(static_cast<unsigned int>(r_index));

    std::seed_seq seed_sequence(seed_data.begin(), seed_data.end());
    std::mt19937 generator(seed_sequence);
    std::uniform_int_distribution<> distribution(0, charset.length() - 1);

    for (size_t i = 0; i < target_len; ++i) {
        reduced_string += charset[distribution(generator)];
    }
    return reduced_string;
}

// Génère la table arc-en-ciel
void generate_rainbow_table(
    const std::string& output_file,
    const EVP_MD* digest_type,
    const std::string& charset,
    int min_len,
    int max_len,
    long long num_chains,
    int chain_length
) {
    std::cout << CR_BLUE << "\n>>> [GENERATOR] Initiating Rainbow Table Generation..." << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Output File    : " << output_file << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Hash Type      : " << EVP_MD_name(digest_type) << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Charset        : " << std::quoted(charset) << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Length Range   : " << min_len << "-" << max_len << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Number of Chains: " << num_chains << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Chain Length   : " << chain_length << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;

    if (!digest_type) {
        std::cerr << CR_RED << "[ERROR] Invalid OpenSSL digest type. Aborting." << RESET << std::endl;
        return;
    }
    if (charset.empty()) {
        std::cerr << CR_RED << "[ERROR] Charset is empty. Cannot generate rainbow table." << RESET << std::endl;
        return;
    }
    if (min_len <= 0 || max_len < min_len) {
        std::cerr << CR_RED << "[ERROR] Invalid length range. Min length > 0 and Max length >= Min length." << RESET << std::endl;
        return;
    }
    if (num_chains <= 0 || chain_length <= 0) {
        std::cerr << CR_RED << "[ERROR] Number of chains and chain length must be greater than 0." << RESET << std::endl;
        return;
    }

    std::ofstream outfile(output_file);
    if (!outfile.is_open()) {
        std::cerr << CR_RED << "[ERROR] Failed to open output file: " << output_file << " (" << strerror(errno) << ")" << RESET << std::endl;
        return;
    }

    // Utilisation d'un générateur de nombres aléatoires pour choisir les mots de départ des chaînes
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> charset_dist(0, charset.length() - 1);
    std::uniform_int_distribution<> len_dist(min_len, max_len);

    auto start_time = std::chrono::high_resolution_clock::now();
    long long generated_chains = 0;

    for (long long i = 0; i < num_chains; ++i) {
        // Générer un mot de passe de départ aléatoire pour la chaîne
        int current_len = len_dist(generator);
        std::string start_word = "";
        for (int k = 0; k < current_len; ++k) {
            start_word += charset[charset_dist(generator)];
        }

        std::string current_word = start_word;
        std::string current_hash;

        // Parcourir la chaîne
        for (int j = 0; j < chain_length; ++j) {
            current_hash = calculate_hash_openssl(current_word, digest_type);
            if (j == chain_length - 1) { // Dernier élément de la chaîne, on le stocke
                break;
            }
            current_word = reduce_hash(current_hash, current_len, charset, j); // Réduire le hash pour le prochain mot
        }
        
        // Stocker le mot de départ et le hash de fin de chaîne
        outfile << start_word << ":" << current_hash << "\n";
        generated_chains++;

        if (generated_chains % 1000 == 0) {
            std::cout << "\r" << CR_YELLOW << "[PROGRESS] Generated " << generated_chains << "/" << num_chains << " chains. Last word: " << start_word << std::flush << RESET;
        }
    }

    outfile.close();
    std::cout << "\r" << std::string(80, ' ') << "\r"; // Efface la ligne de progression

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    std::cout << CR_GREEN << "[COMPLETED] Rainbow table generated in " << std::fixed << std::setprecision(2) << duration.count() << " seconds. (" << generated_chains << " chains saved)" << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;
}


int main() {
    // Efface l'écran pour un affichage propre au démarrage
    std::cout << "\033[H\033[J";

    std::cout << CR_RED BOLD << "   H A S H C R A C K E R - R A I N B O W G E N " << RESET << std::endl;
    std::cout << CR_CYAN << "   ------------------------------------------- " << RESET << std::endl;
    std::cout << CR_BLUE << "==========================================================" << RESET << std::endl;
    std::cout << CR_MAGENTA << "\n  [INFO] This tool generates Rainbow Tables. " << RESET << std::endl;
    std::cout << CR_MAGENTA << "  [WARNING] Generation can be very time and resource intensive." << RESET << std::endl;
    std::cout << CR_BLUE << "==========================================================" << RESET << std::endl;

    std::string output_filename;
    std::cout << CR_YELLOW << "\n [OUTPUT FILE] Enter desired output filename (e.g., my_rainbow_table.txt) > " << RESET;
    std::cin >> output_filename;

    // Définir le chemin de sortie dans le sous-dossier `modules` de `hashish`
    // Assurez-vous que le chemin est correct pour Termux, en partant de `pwd`
    std::string rainbow_table_path = "./rainbow.txt"; // Par défaut, dans le répertoire courant (modules/)
                                                       // Le script installer.sh le déplacera si nécessaire.


    std::cout << CR_BLUE << "\n--- [PARAMETERS] --------------------------------------" << RESET << std::endl;
    
    // Définition des jeux de caractères prédéfinis
    std::map<int, std::pair<std::string, std::string>> predefined_charsets;
    predefined_charsets[1] = {"Lowercase letters (a-z)", "abcdefghijklmnopqrstuvwxyz"};
    predefined_charsets[2] = {"Lowercase + Digits (a-z, 0-9)", "abcdefghijklmnopqrstuvwxyz0123456789"};
    predefined_charsets[3] = {"Lowercase + Uppercase (a-z, A-Z)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    predefined_charsets[4] = {"Lowercase + Uppercase + Digits (a-z, A-Z, 0-9)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
    predefined_charsets[5] = {"All common characters (a-z, A-Z, 0-9, !@#$%)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%"};
    predefined_charsets[6] = {"Numbers only (0-9)", "0123456789"};

    std::cout << CR_CYAN << "   Choose a predefined charset or enter your own:" << RESET << std::endl;
    for (const auto& pair : predefined_charsets) {
        std::cout << CR_CYAN << "   " << pair.first << ". " << pair.second.first << RESET << std::endl;
    }
    std::cout << CR_CYAN << "   Or enter 'C' for Custom charset" << RESET << std::endl;
    std::cout << CR_YELLOW << " [CHARACTER SET CHOICE] Enter choice (1-" << predefined_charsets.size() << " or C) > " << RESET;
    
    std::string charset_choice_str;
    std::cin >> charset_choice_str;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the rest of the line

    std::string charset_input;
    std::transform(charset_choice_str.begin(), charset_choice_str.end(), charset_choice_str.begin(), ::toupper);

    if (charset_choice_str == "C") {
        std::cout << CR_YELLOW << " [CUSTOM CHARACTER SET] Enter your custom charset > " << RESET;
        std::getline(std::cin, charset_input);
    } else {
        try {
            int choice_num = std::stoi(charset_choice_str);
            if (predefined_charsets.count(choice_num)) {
                charset_input = predefined_charsets[choice_num].second;
                std::cout << CR_DARK_GRAY << "   Selected Charset: " << predefined_charsets[choice_num].first << RESET << std::endl;
            } else {
                std::cerr << CR_RED << "[ERROR] Invalid charset choice. Using empty charset." << RESET << std::endl;
                charset_input = "";
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << CR_RED << "[ERROR] Invalid input for charset choice. Using empty charset." << RESET << std::endl;
            charset_input = "";
        } catch (const std::out_of_range& e) {
            std::cerr << CR_RED << "[ERROR] Charset choice out of range. Using empty charset." << RESET << std::endl;
            charset_input = "";
        }
    }
    
    if (charset_input.empty()) {
        std::cerr << CR_RED << "[ERROR] Charset is empty. Cannot generate rainbow table." << RESET << std::endl;
        return 1;
    }

    int min_len, max_len;
    std::cout << CR_YELLOW << " [MIN LENGTH] Enter minimum password length for chains > " << RESET;
    std::cin >> min_len;
    std::cout << CR_YELLOW << " [MAX LENGTH] Enter maximum password length for chains > " << RESET;
    std::cin >> max_len;

    long long num_chains;
    std::cout << CR_YELLOW << " [NUMBER OF CHAINS] Enter number of chains to generate (e.g., 1000000) > " << RESET;
    std::cin >> num_chains;

    int chain_length;
    std::cout << CR_YELLOW << " [CHAIN LENGTH] Enter length of each chain (e.g., 10000) > " << RESET;
    std::cin >> chain_length;

    std::string hash_type_str;
    std::cout << CR_YELLOW << " [HASH TYPE] Enter hash type (MD5, SHA1, SHA256, SHA512) > " << RESET;
    std::cin >> hash_type_str;
    std::transform(hash_type_str.begin(), hash_type_str.end(), hash_type_str.begin(), ::toupper);

    const EVP_MD* digest_algo = get_openssl_digest_type(hash_type_str);
    if (digest_algo == nullptr) {
        std::cerr << CR_RED << "[ERROR] Unsupported hash type. Supported: MD5, SHA1, SHA256, SHA384, SHA512." << RESET << std::endl;
        return 1;
    }
    
    std::cout << CR_MAGENTA << "\n[INFO] Generating a table of " << num_chains << " chains, each " << chain_length << " steps long." << RESET << std::endl;
    std::cout << CR_MAGENTA << "       This will result in a file size of approximately " 
              << std::fixed << std::setprecision(2) 
              << (num_chains * (min_len + (EVP_MD_size(digest_algo) * 2) + 2)) / (1024.0 * 1024.0) // word + hash + colon + newline
              << " MB (estimation, can vary based on actual password lengths)." << RESET << std::endl;


    char confirm_choice;
    std::cout << CR_YELLOW << "Do you want to proceed with rainbow table generation? (y/n) > " << RESET;
    std::cin >> confirm_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the rest of the line

    if (tolower(confirm_choice) != 'y') {
        std::cout << CR_YELLOW << "[INFO] Rainbow table generation cancelled by user." << RESET << std::endl;
        return 0;
    }

    generate_rainbow_table(output_filename, digest_algo, charset_input, min_len, max_len, num_chains, chain_length);

    std::cout << CR_BLUE << "\n==========================================================" << RESET << std::endl;
    std::cout << CR_CYAN << "  [MODULE COMPLETE] Rainbow Table Generation finished. " << RESET << std::endl;
    std::cout << CR_BLUE << "==========================================================" << RESET << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
