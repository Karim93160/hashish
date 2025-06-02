#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <random>
#include <limits>
#include <map>
#include <thread>
#include <cerrno>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"
#define FAINT   "\033[2m"
#define ITALIC  "\033[3m"
#define UNDERLINE "\033[4m"
#define BLINK   "\033[5m"
#define REVERSE "\033[7m"
#define HIDDEN  "\033[8m"
#define STRIKETHROUGH "\033[9m"

#define CR_RED    RED BOLD
#define CR_GREEN  GREEN BOLD
#define CR_YELLOW YELLOW BOLD
#define CR_BLUE   BLUE BOLD FAINT
#define CR_CYAN   CYAN BOLD
#define CR_MAGENTA MAGENTA BOLD FAINT
#define CR_WHITE  WHITE BOLD
#define CR_DARK_GRAY "\033[90m"

std::string bytes_to_hex_string(const unsigned char* bytes, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
    }
    return ss.str();
}

// Comme pour hashcracker.cpp, cette fonction s'appuie sur les optimisations
// d'OpenSSL, qui utilise déjà des implémentations assembleur/intrinsics.
std::string calculate_hash_openssl(const std::string& input, const EVP_MD* digest_type) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
        std::cerr << CR_RED << "[ERROR_OPENSSL] Failed to create EVP_MD_CTX." << RESET << std::endl;
        return "";
    }
    if (1 != EVP_DigestInit_ex(mdctx, digest_type, nullptr)) {
        ERR_print_errors_fp(stderr);
        EVP_MD_CTX_free(mdctx);
        return "";
    }
    if (1 != EVP_DigestUpdate(mdctx, input.c_str(), input.length())) {
        ERR_print_errors_fp(stderr);
        EVP_MD_CTX_free(mdctx);
        return "";
    }
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;
    if (1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len)) {
        ERR_print_errors_fp(stderr);
        EVP_MD_CTX_free(mdctx);
        return "";
    }
    EVP_MD_CTX_free(mdctx);
    return bytes_to_hex_string(digest, digest_len);
}

const EVP_MD* get_openssl_digest_type(const std::string& hash_type_str) {
    if (hash_type_str == "MD5") return EVP_md5();
    if (hash_type_str == "SHA1") return EVP_sha1();
    if (hash_type_str == "SHA256") return EVP_sha256();
    if (hash_type_str == "SHA384") return EVP_sha384();
    if (hash_type_str == "SHA512") return EVP_sha512();
    return nullptr;
}

std::string reduce_hash(const std::string& hash, size_t target_len, const std::string& charset, int r_index) {
    if (charset.empty() || target_len == 0) {
        return "";
    }
    std::string reduced_string = "";
    // Seed basé sur le hash et l'index de réduction pour la reproductibilité.
    // L'utilisation de std::seed_seq avec des entiers provenant du hash
    // assure une meilleure distribution du seed.
    std::vector<unsigned int> seed_data;
    for (char c : hash) {
        seed_data.push_back(static_cast<unsigned int>(c));
    }
    seed_data.push_back(static_cast<unsigned int>(r_index));
    std::seed_seq seed_sequence(seed_data.begin(), seed_data.end());
    std::mt19937 generator(seed_sequence);
    std::uniform_int_distribution<> distribution(0, charset.length() - 1);
    for (size_t i = 0; i < target_len; ++i) {
        reduced_string += charset[distribution(generator)];
    }
    return reduced_string;
}

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
        std::cerr << CR_RED << "[ERROR] Invalid length range. Min length > 0 and Max length >= Min length. Aborting." << RESET << std::endl;
        return;
    }
    if (num_chains <= 0 || chain_length <= 0) {
        std::cerr << CR_RED << "[ERROR] Number of chains and chain length must be greater than 0. Aborting." << RESET << std::endl;
        return;
    }
    std::ofstream outfile(output_file);
    if (!outfile.is_open()) {
        std::cerr << CR_RED << "[ERROR] Failed to open output file: " << output_file << " (" << strerror(errno) << ")" << RESET << std::endl;
        return;
    }
    // Utilisation de std::chrono::system_clock::now().time_since_epoch().count()
    // pour un seed plus robuste sur toutes les plateformes.
    std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> charset_dist(0, charset.length() - 1);
    std::uniform_int_distribution<> len_dist(min_len, max_len);
    auto start_time = std::chrono::high_resolution_clock::now();
    long long generated_chains = 0;
    for (long long i = 0; i < num_chains; ++i) {
        int current_len = len_dist(generator);
        std::string start_word = "";
        for (int k = 0; k < current_len; ++k) {
            start_word += charset[charset_dist(generator)];
        }
        std::string current_word = start_word;
        std::string current_hash;
        for (int j = 0; j < chain_length; ++j) {
            current_hash = calculate_hash_openssl(current_word, digest_type);
            if (current_hash.empty()) {
                std::cerr << CR_RED << "[ERROR] Hash calculation failed for word: " << current_word << ". Skipping chain." << RESET << std::endl;
                start_word = ""; // Marquer comme chaîne à ignorer
                break;
            }
            if (j == chain_length - 1) {
                break;
            }
            current_word = reduce_hash(current_hash, current_len, charset, j);
            if (current_word.empty()) {
                std::cerr << CR_RED << "[ERROR] Reduction failed for hash: " << current_hash << ". Skipping chain." << RESET << std::endl;
                start_word = ""; // Marquer comme chaîne à ignorer
                break;
            }
        }
        if (!start_word.empty()) { // Seulement écrire si la chaîne n'a pas été ignorée
            outfile << start_word << ":" << current_hash << "\n";
            generated_chains++;
        }
        if (generated_chains > 0 && generated_chains % 1000 == 0) {
            std::cout << "\r" << CR_YELLOW << "[PROGRESS] Generated " << generated_chains << "/" << num_chains << " chains. Last start word: " << start_word << std::flush << RESET;
        }
    }
    outfile.close();
    std::cout << "\r" << std::string(80, ' ') << "\r";
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << CR_GREEN << "[COMPLETED] Rainbow table generated in " << std::fixed << std::setprecision(2) << duration.count() << " seconds. (" << generated_chains << " chains saved)" << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;
}

int main() {
    // Initialiser OpenSSL
    ERR_load_crypto_strings();
    OpenSSL_add_all_digests(); // Initialiser toutes les fonctions de hachage disponibles

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
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << CR_BLUE << "\n--- [PARAMETERS] --------------------------------------" << RESET << std::endl;
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
    std::getline(std::cin, charset_choice_str);
    std::transform(charset_choice_str.begin(), charset_choice_str.end(), charset_choice_str.begin(), ::toupper);
    std::string charset_input;
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
                std::cerr << CR_RED << "[ERROR] Invalid charset choice. Using empty charset. Please restart." << RESET << std::endl;
                // Libérer les ressources OpenSSL avant de quitter
                EVP_cleanup();
                ERR_free_strings();
                return 1;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << CR_RED << "[ERROR] Invalid input for charset choice. Using empty charset. Please restart." << RESET << std::endl;
            // Libérer les ressources OpenSSL avant de quitter
            EVP_cleanup();
            ERR_free_strings();
            return 1;
        } catch (const std::out_of_range& e) {
            std::cerr << CR_RED << "[ERROR] Charset choice out of range. Using empty charset. Please restart." << RESET << std::endl;
            // Libérer les ressources OpenSSL avant de quitter
            EVP_cleanup();
            ERR_free_strings();
            return 1;
        }
    }
    if (charset_input.empty()) {
        std::cerr << CR_RED << "[ERROR] Charset is empty. Cannot generate rainbow table. Please restart." << RESET << std::endl;
        // Libérer les ressources OpenSSL avant de quitter
        EVP_cleanup();
        ERR_free_strings();
        return 1;
    }
    int min_len, max_len;
    while (true) {
        std::cout << CR_YELLOW << " [MIN LENGTH] Enter minimum password length for chains (>0) > " << RESET;
        std::cin >> min_len;
        if (std::cin.fail() || min_len <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << CR_RED << "[ERROR] Invalid input. Please enter a positive integer for minimum length." << RESET << std::endl;
        } else {
            break;
        }
    }
    while (true) {
        std::cout << CR_YELLOW << " [MAX LENGTH] Enter maximum password length for chains (>= MIN LENGTH) > " << RESET;
        std::cin >> max_len;
        if (std::cin.fail() || max_len < min_len) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << CR_RED << "[ERROR] Invalid input. Please enter an integer greater than or equal to minimum length." << RESET << std::endl;
        } else {
            break;
        }
    }
    long long num_chains;
    while (true) {
        std::cout << CR_YELLOW << " [NUMBER OF CHAINS] Enter number of chains to generate (>0) > " << RESET;
        std::cin >> num_chains;
        if (std::cin.fail() || num_chains <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << CR_RED << "[ERROR] Invalid input. Please enter a positive integer for number of chains." << RESET << std::endl;
        } else {
            break;
        }
    }
    int chain_length;
    while (true) {
        std::cout << CR_YELLOW << " [CHAIN LENGTH] Enter length of each chain (>0) > " << RESET;
        std::cin >> chain_length;
        if (std::cin.fail() || chain_length <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << CR_RED << "[ERROR] Invalid input. Please enter a positive integer for chain length." << RESET << std::endl;
        } else {
            break;
        }
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string hash_type_str;
    const EVP_MD* digest_algo = nullptr;
    while (digest_algo == nullptr) {
        std::cout << CR_YELLOW << " [HASH TYPE] Enter hash type (MD5, SHA1, SHA256, SHA384, SHA512) > " << RESET;
        std::getline(std::cin, hash_type_str);
        std::transform(hash_type_str.begin(), hash_type_str.end(), hash_type_str.begin(), ::toupper);
        digest_algo = get_openssl_digest_type(hash_type_str);
        if (digest_algo == nullptr) {
            std::cerr << CR_RED << "[ERROR] Unsupported hash type. Please enter one of: MD5, SHA1, SHA256, SHA384, SHA512." << RESET << std::endl;
        }
    }
    double avg_len = (min_len + max_len) / 2.0;
    double estimated_hash_len = EVP_MD_size(digest_algo) * 2;
    double estimated_line_length = avg_len + estimated_hash_len + 2;
    double estimated_file_size_mb = (num_chains * estimated_line_length) / (1024.0 * 1024.0);
    std::cout << CR_MAGENTA << "\n[INFO] Generating a table of " << num_chains << " chains, each " << chain_length << " steps long." << RESET << std::endl;
    std::cout << CR_MAGENTA << "       This will result in an estimated file size of "
              << std::fixed << std::setprecision(2) << estimated_file_size_mb
              << " MB." << RESET << std::endl;
    std::cout << CR_MAGENTA << "       (Estimation can vary based on actual password lengths and hash collisions.)" << RESET << std::endl;
    char confirm_choice;
    std::cout << CR_YELLOW << "Do you want to proceed with rainbow table generation? (y/n) > " << RESET;
    std::cin >> confirm_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (tolower(confirm_choice) != 'y') {
        std::cout << CR_YELLOW << "[INFO] Rainbow table generation cancelled by user." << RESET << std::endl;
        // Libérer les ressources OpenSSL avant de quitter
        EVP_cleanup();
        ERR_free_strings();
        return 0;
    }
    generate_rainbow_table(output_filename, digest_algo, charset_input, min_len, max_len, num_chains, chain_length);
    std::cout << CR_BLUE << "\n==========================================================" << RESET << std::endl;
    std::cout << CR_CYAN << "  [MODULE COMPLETE] Rainbow Table Generation finished. " << RESET << std::endl;
    std::cout << CR_BLUE << "==========================================================" << RESET << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Libérer les ressources OpenSSL
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
