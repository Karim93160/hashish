#include <iostream> // For input/output (cin, cout)
#include <string>   // For string manipulation
#include <vector>   // For storing hash types (though not explicitly used for that here)
#include <algorithm> // For std::tolower (to convert to lowercase)
#include <cctype>   // For std::isxdigit

// Function to convert a string to lowercase
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

// Function to check if a string is hexadecimal
bool isHex(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!std::isxdigit(c)) { // isxdigit checks if the character is a hexadecimal digit
            return false;
        }
    }
    return true;
}

// Main hash analysis function
void analyzeHash(const std::string& hash) {
    int length = hash.length();
    bool hex_chars = isHex(hash); // Check if all characters are hexadecimal

    std::cout << "\n--- Analysis Result ---\n";

    if (length == 32 && hex_chars) {
        std::cout << "Decisive Suggestion: Probable **MD5** hash.\n";
        std::cout << "Recommended Cracking Parameters:\n";
        std::cout << "  - Attack Type: Dictionary Attack or Bruteforce (for shorter lengths).\n";
        std::cout << "  - Charset (Bruteforce): Common characters like lowercase letters `[a-z]`, digits `[0-9]`, and possibly some symbols `[.,!?]`. Consider `[a-z0-9]` as a good starting point.\n";
        std::cout << "  - Length: 1-8 characters (typically).\n";
        std::cout << "  - Note: MD5 is vulnerable to collision attacks, but for password cracking, dictionary attacks are often more efficient.\n";
    } else if (length == 40 && hex_chars) {
        std::cout << "Decisive Suggestion: Probable **SHA-1** hash.\n";
        std::cout << "Recommended Cracking Parameters:\n";
        std::cout << "  - Attack Type: Dictionary Attack or Targeted Bruteforce.\n";
        std::cout << "  - Charset (Bruteforce): `[a-zA-Z0-9]` or `[a-zA-Z0-9!@#$%^&*]` if symbols are expected.\n";
        std::cout << "  - Length: 1-10 characters (typically).\n";
        std::cout << "  - Note: SHA-1 is considered cryptographically broken for digital signatures but still requires significant effort for password cracking, often relying on large dictionaries or known password patterns.\n";
    } else if (length == 64 && hex_chars) {
        std::cout << "Decisive Suggestion: Probable **SHA-256** hash.\n";
        std::cout << "Recommended Cracking Parameters:\n";
        std::cout << "  - Attack Type: Extended Dictionary Attack or Bruteforce with specific masks.\n";
        std::cout << "  - Charset (Bruteforce): `[a-zA-Z0-9!@#$%^&*()]` (all common characters) or custom based on expected complexity.\n";
        std::cout << "  - Length: Minimum 8 characters, often longer (e.g., 8-16+ characters).\n";
        std::cout << "  - Note: SHA-256 is generally strong. Bruteforce is extremely time-consuming without significant computational resources (GPUs) or prior knowledge about the password structure.\n";
    } else if (length == 128 && hex_chars) {
        std::cout << "Decisive Suggestion: Probable **SHA-512** hash.\n";
        std::cout << "Recommended Cracking Parameters:\n";
        std::cout << "  - Attack Type: Dictionary Attack with very large wordlists or leaked databases. Pure bruteforce is generally infeasible.\n";
        std::cout << "  - Charset (Bruteforce): Not recommended for pure bruteforce due to computational cost.\n";
        std::cout << "  - Length: Typically 10+ characters.\n";
        std::cout << "  - Note: SHA-512 is very strong. Focus on credential stuffing, social engineering, or large-scale dictionary attacks. Rainbow tables are generally impractical for SHA-512 due to their size.\n";
    } else if (length == 32 && (hash.rfind(":::", 0) == 0 || hash[0] == '$' || hash.find("LM:", 0) == 0)) {
        // More robust check for NTLM/LM formats
        std::cout << "Decisive Suggestion: Probable **NTLM / LM** hash (or similar salted format).\n";
        std::cout << "Recommended Cracking Parameters:\n";
        std::cout << "  - Attack Type: Specific NTLM/LM cracking tools (e.g., Hashcat, John the Ripper) are highly recommended.\n";
        std::cout << "  - Charset (Bruteforce): For LM, often `[a-zA-Z0-9]` with specific length considerations (e.g., 7 characters per block). For NTLM, full ASCII or common passwords.\n";
        std::cout << "  - Length: Varies by type (LM 7 chars blocks, NTLM up to 128 chars).\n";
        std::cout << "  - Note: LM hashes are notoriously weak. NTLM is stronger but can be cracked if passwords are simple or commonly used. These are typically Windows-specific hashes.\n";
    }
    // You can add other conditions for other hash types here
    // for example, for bcrypt, scrypt, etc., which have more complex formats and often a recognizable prefix.
    else {
        std::cout << "Decisive Suggestion: Hash type **undetermined or unsupported**.\n";
        std::cout << "Recommended Cracking Parameters:\n";
        std::cout << "  - Action: Verify the hash source. Try an online search with the hash format to identify its type.\n";
        std::cout << "  - Note: General bruteforce will be very time-consuming. Focus on identifying the hash type first to apply appropriate cracking strategies.\n";
    }
    std::cout << "--------------------------------\n";
}
