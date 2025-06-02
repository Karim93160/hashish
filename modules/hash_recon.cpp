#include "hash_recon.h" // Inclut les déclarations de nos fonctions
#include <cctype>       // For character checking functions (isdigit, isalpha, etc.)
#include <iostream>     // For input/output (std::cout)
#include <string>       // For std::string
#include <iomanip>      // For std::quoted (if you use it, otherwise you can remove it)

// Console color macros (copied from hashcracker.cpp)
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


// Function to check if a string consists only of digits
bool estNumerique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Function to check if a string consists only of alphabetic characters
bool estAlphabetique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    return true;
}

// Function to check if a string consists only of alphanumeric characters
bool estAlphanumerique(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalnum(c)) {
            return false;
        }
    }
    return true;
}

// Function to check if a string consists only of hexadecimal characters
bool estHexadecimal(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isxdigit(c)) {
            return false;
        }
    }
    return true;
}

// New function for interactive hash character type analysis
void analyserTypeCaracteresHachage(const std::string& hash_hex) {
    std::cout << "\n" << CR_BLUE << "--- [IN-DEPTH HASH ANALYSIS] -------------------" << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Hash to analyze: " << hash_hex << RESET << std::endl;

    bool est_numerique = estNumerique(hash_hex);
    bool est_alphabetique = estAlphabetique(hash_hex);
    bool est_alphanumerique = estAlphanumerique(hash_hex);
    bool est_hexadecimal = estHexadecimal(hash_hex);

    std::cout << CR_CYAN << "    HASH analysis results:" << RESET << std::endl;

    // Displaying results
    std::cout << CR_CYAN << "      - Only numeric (0-9): " << (est_numerique ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;
    std::cout << CR_CYAN << "      - Only alphabetic (a-z, A-Z): " << (est_alphabetique ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;
    
    // Highlighting the alphanumeric line if the HASH is alphanumeric
    if (est_alphanumerique) {
        std::cout << CR_GREEN << BOLD << "      - Only alphanumeric (letters or digits): YES, contains [a-z] and [0-9]!" << RESET << std::endl;
    } else {
        std::cout << CR_CYAN << "      - Only alphanumeric (letters or digits): " << CR_RED << "NO" << RESET << std::endl;
    }

    // Hexadecimal detection is important for the hash itself
    std::cout << CR_CYAN << "      - Only hexadecimal (0-9, a-f, A-F): " << (est_hexadecimal ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;

    if (est_hexadecimal) {
        std::cout << CR_YELLOW << "    [INFO] A hexadecimal hash is very common for MD5, SHA-1, SHA-256, etc." << RESET << std::endl;
    } else {
        std::cout << CR_YELLOW << "    [INFO] This hash does NOT appear to be purely hexadecimal. It could be a different encoding or corrupted." << RESET << std::endl;
    }

    // Adding a specific RECOMMENDATION for brute-force attacks if the hash is alphanumeric
    if (est_alphanumerique) {
        std::cout << "\n" << CR_GREEN << BOLD << "    [RECOMMENDATION] For brute-force attacks (option 2), consider an alphanumeric character set." << RESET << std::endl;
    } else if (est_numerique) {
        std::cout << "\n" << CR_YELLOW << "    [RECOMMENDATION] For brute-force attacks (option 2), a numeric character set might be appropriate." << RESET << std::endl;
    } else if (est_alphabetique) {
        std::cout << "\n" << CR_YELLOW << "    [RECOMMENDATION] For brute-force attacks (option 2), an alphabetic character set might be appropriate." << RESET << std::endl;
    }

    if (!est_numerique && !est_alphabetique && !est_hexadecimal && !est_alphanumerique) {
        std::cout << CR_YELLOW << "    [INFO] This hash contains special or unexpected characters. It might be differently encoded or corrupted." << RESET << std::endl;
    }
    
    std::cout << CR_BLUE << "--------------------------------------------------------" << RESET << std::endl;
}
