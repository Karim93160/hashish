#include "hash_recon.h" // Includes our function declarations
#include <cctype>       // For character checking functions (isdigit, isalpha, etc.)
#include <iostream>     // For input/output (std::cout, std::cin)
#include <string>       // For std::string
#include <iomanip>      // For std::quoted

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
bool isNumeric(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Function to check if a string consists only of alphabetic characters
bool isAlphabetic(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    return true;
}

// Function to check if a string consists only of alphanumeric characters
bool isAlphanumeric(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isalnum(c)) {
            return false;
        }
    }
    return true;
}

// Function to check if a string consists only of hexadecimal characters
bool isHexadecimal(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isxdigit(c)) {
            return false;
        }
    }
    return true;
}

// New function for interactive hash character type analysis
void analyzeHashCharacterType(const std::string& hash_hex) {
    std::cout << "\n" << CR_BLUE << "--- [IN-DEPTH HASH ANALYSIS] -------------------" << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Hash to analyze: " << hash_hex << RESET << std::endl;

    bool is_numeric = isNumeric(hash_hex);
    bool is_alpha = isAlphabetic(hash_hex);
    bool is_alphanum = isAlphanumeric(hash_hex); // This variable is the result of the HASH analysis, not the plaintext password.
    bool is_hex = isHexadecimal(hash_hex);

    std::cout << CR_CYAN << "    HASH analysis results:" << RESET << std::endl; // Clarification for the user

    // Displaying results
    std::cout << CR_CYAN << "      - Only numeric (0-9): " << (is_numeric ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;
    std::cout << CR_CYAN << "      - Only alphabetic (a-z, A-Z): " << (is_alpha ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;
    
    // Highlighting the alphanumeric line if the HASH is alphanumeric
    // Important note: this analysis always concerns the HASH (e.g., 268041af5c99b9588f8ab2b600f4aec3),
    // not the original password "hash77". However, since the original password was
    // alphanumeric, this HASH line will also be alphanumeric if it only contains
    // letters and numbers (which is the case for hexadecimal, which is a subset).
    // If you truly want to analyze the type of the *original password* as it would be,
    // the tool would need access to the original password, which is not the case here.
    // The analysis here is on the ENTERED HASH.
    
    // We highlight the alphanumeric line because it most often reflects
    // the character type you would find in a plaintext password, even if here
    // we apply it to the HASH itself.
    if (is_alphanum) {
        std::cout << CR_GREEN << BOLD << "      - Only alphanumeric (letters or digits): YES, contains [a-z] and [0-9]!" << RESET << std::endl;
    } else {
        std::cout << CR_CYAN << "      - Only alphanumeric (letters or digits): " << CR_RED << "NO" << RESET << std::endl;
    }

    // Hexadecimal detection is important for the hash itself, but we make it less visually "aggressive"
    std::cout << CR_CYAN << "      - Only hexadecimal (0-9, a-f, A-F): " << (is_hex ? CR_GREEN "YES" : CR_RED "NO") << RESET << std::endl;

    if (is_hex) {
        std::cout << CR_YELLOW << "    [INFO] A hexadecimal hash is very common for MD5, SHA-1, SHA-256, etc." << RESET << std::endl;
    } else {
        std::cout << CR_YELLOW << "    [INFO] This hash does NOT appear to be purely hexadecimal. It could be a different encoding or corrupted." << RESET << std::endl;
    }

    // Adding a specific RECOMMENDATION for brute-force attacks if the hash is alphanumeric
    if (is_alphanum) {
        std::cout << "\n" << CR_GREEN << BOLD << "    [RECOMMENDATION] For brute-force attacks (option 2), consider an alphanumeric character set." << RESET << std::endl;
    } else if (is_numeric) {
        std::cout << "\n" << CR_YELLOW << "    [RECOMMENDATION] For brute-force attacks (option 2), a numeric character set might be appropriate." << RESET << std::endl;
    } else if (is_alpha) {
        std::cout << "\n" << CR_YELLOW << "    [RECOMMENDATION] For brute-force attacks (option 2), an alphabetic character set might be appropriate." << RESET << std::endl;
    }


    if (!is_numeric && !is_alpha && !is_hex && !is_alphanum) {
        std::cout << CR_YELLOW << "    [INFO] This hash contains special or unexpected characters. It might be differently encoded or corrupted." << RESET << std::endl;
    }
    
    std::cout << CR_BLUE << "--------------------------------------------------------" << RESET << std::endl;
}
