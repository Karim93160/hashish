#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm> // Pour std::transform
#include <thread>    // Pour std::this_thread::sleep_for
#include <atomic>    // Pour g_hash_cracked_flag et g_total_attempts_bruteforce
#include <iomanip>   // Pour std::setw, std::setfill, std::quoted
#include <sstream>   // Pour std::stringstream
#include <chrono>    // Pour la mesure du temps
#include <cstring>   // Pour strerror
#include <map>       // Pour std::map des charsets
#include <cmath>     // Pour pow (pour le calcul des combinaisons)
#include <limits>    // Pour std::numeric_limits
#include <mutex>     // Pour std::mutex (g_cout_mutex)
#include <random>    // Pour la fonction de réduction déterministe et la génération de chaînes
#include <filesystem> // Pour std::filesystem::current_path et path manipulation (C++17)

// --- Ajout spécifique pour obtenir le chemin de l'exécutable sous Linux ---
#ifdef __linux__
#include <unistd.h> // Pour readlink
#endif

// --- Ajout d'OpenMP ---
#ifdef _OPENMP
#include <omp.h>
#endif

// Pour OpenSSL
#include <openssl/evp.h> // Fonctions de hachage génériques (MD5, SHA1, SHA256, etc.)
#include <openssl/err.h> // Pour ERR_print_errors_fp (utile pour le débogage OpenSSL)

// --- Codes de couleurs ANSI pour le terminal ---
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CR_WHITE  WHITE BOLD // Renommé pour correspondre à vos conventions
#define CR_CYAN   CYAN BOLD // Renommé pour correspondre à vos conventions
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
#define CR_MAGENTA MAGENTA BOLD FAINT
#define CR_DARK_GRAY "\033[90m" // Gris foncé pour les infos moins importantes

// --- Variables globales pour le cracking multithread et l'état ---
std::atomic<bool> g_hash_cracked_flag(false);
std::string g_found_password;
std::mutex g_cout_mutex; // Mutex pour protéger les affichages sur la console (cout)
std::atomic<long long> g_total_attempts_bruteforce(0); // Compteur atomique pour la force brute

// --- Fonctions utilitaires ---

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
// Cette fonction s'appuie sur les implémentations d'OpenSSL, qui sont déjà
// hautement optimisées et peuvent utiliser des instructions assembleur (intrinsics)
// spécifiques au CPU si OpenSSL et le compilateur sont correctement configurés.
std::string calculate_hash_openssl(const std::string& input, const EVP_MD* digest_type) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
        // Gérer l'erreur plus verbosement si nécessaire, mais pour un appel fréquent, un simple retour peut suffire.
        // fprintf(stderr, "EVP_MD_CTX_new failed.\n"); // Pour un débogage plus précis
        return "";
    }

    // EVP_DigestInit_ex est optimisé par OpenSSL.
    if (1 != EVP_DigestInit_ex(mdctx, digest_type, nullptr)) {
        // ERR_print_errors_fp(stderr); // Pour un débogage plus précis
        EVP_MD_CTX_free(mdctx);
        return "";
    }
    // EVP_DigestUpdate est optimisé par OpenSSL pour les opérations de hachage.
    if (1 != EVP_DigestUpdate(mdctx, input.c_str(), input.length())) {
        // ERR_print_errors_fp(stderr); // Pour un débogage plus précis
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len;

    // EVP_DigestFinal_ex est optimisé par OpenSSL.
    if (1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len)) {
        // ERR_print_errors_fp(stderr); // For more precise debugging
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);
    return bytes_to_hex_string(digest, digest_len);
}

// Détecte le type de hash (MD5, SHA1, SHA256, SHA384, SHA512) basé sur la longueur de la chaîne hexadécimale
std::string detect_hash_type_str(const std::string& hash_hex) {
    size_t len = hash_hex.length();
    if (len == 32) return "MD5";
    if (len == 40) return "SHA1";
    if (len == 64) return "SHA256";
    if (len == 96) return "SHA384";
    if (len == 128) return "SHA512";
    return "INCONNU";
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

// Calcule le nombre maximal de tentatives pour une plage de longueurs et un charset
long double calculate_max_attempts(int charset_size, int min_len, int max_len) {
    long double total_attempts = 0;
    for (int len = min_len; len <= max_len; ++len) {
        // Utilise std::pow de manière sécurisée pour les grands nombres
        // Assurez-vous que charset_size n'est pas zéro pour éviter division par zéro
        if (charset_size > 0) {
            total_attempts += std::pow((long double)charset_size, (long double)len);
        } else {
            return 0; // Si le charset est vide, aucune tentative n'est possible
        }
    }
    return total_attempts;
}

// Fonction pour formater un nombre de tentatives pour une meilleure lisibilité (K, M, G, T, P, E)
std::string format_attempts(long double attempts) {
    std::stringstream ss;
    if (attempts < 1000) {
        ss << static_cast<long long>(attempts);
    } else if (attempts < 1e6) {
        ss << std::fixed << std::setprecision(2) << attempts / 1e3 << " K";
    } else if (attempts < 1e9) {
        ss << std::fixed << std::setprecision(2) << attempts / 1e6 << " M";
    } else if (attempts < 1e12) {
        ss << std::fixed << std::setprecision(2) << attempts / 1e9 << " G";
    } else if (attempts < 1e15) {
        ss << std::fixed << std::setprecision(2) << attempts / 1e12 << " T";
    } else if (attempts < 1e18) {
        ss << std::fixed << std::setprecision(2) << attempts / 1e15 << " P";
    } else {
        ss << std::fixed << std::setprecision(2) << attempts / 1e18 << " E"; // Exa
    }
    return ss.str();
}

// Fonction pour formater la durée de temps (jours, heures, minutes, secondes)
std::string format_time_duration(long double seconds) {
    if (seconds < 60) {
        return std::to_string(static_cast<int>(seconds)) + " seconds";
    }
    long long total_minutes = static_cast<long long>(seconds / 60);
    long long sec = static_cast<long long>(seconds) % 60;

    long long total_hours = total_minutes / 60;
    long long min = total_minutes % 60;

    long long total_days = total_hours / 24;
    long long hour = total_hours % 24;

    std::stringstream ss;
    if (total_days > 0) {
        ss << total_days << "d ";
    }
    if (total_hours > 0 || total_days > 0) {
        ss << hour << "h ";
    }
    ss << min << "m " << sec << "s";
    return ss.str();
}

// Fonction de benchmark pour estimer les H/s
double run_benchmark(const EVP_MD* digest_type, const std::string& charset, int num_threads) {
    std::cout << CR_CYAN << "\n[BENCHMARK] Running a quick benchmark to estimate speed..." << RESET << std::endl;
    // Réduire le nombre de tentatives pour un benchmark rapide, surtout sur mobile
    const int benchmark_attempts_per_thread = 50000;
    std::atomic<long long> benchmark_total_hashes(0);

    auto benchmark_start_time = std::chrono::high_resolution_clock::now();

    #ifdef _OPENMP
    #pragma omp parallel num_threads(num_threads)
    {
        std::string test_input_base = "bench_input_";
        test_input_base += std::to_string(omp_get_thread_num());
        for (int j = 0; j < benchmark_attempts_per_thread; ++j) {
            calculate_hash_openssl(test_input_base + std::to_string(j), digest_type);
            benchmark_total_hashes++;
        }
    }
    #else
    // Fallback séquentiel si OpenMP n'est pas activé
    std::string test_input_base = "bench_input_0";
    for (int j = 0; j < benchmark_attempts_per_thread * num_threads; ++j) {
        calculate_hash_openssl(test_input_base + std::to_string(j), digest_type);
        benchmark_total_hashes++;
    }
    #endif

    auto benchmark_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> benchmark_duration = benchmark_end_time - benchmark_start_time;

    double hashes_per_second = 0;
    if (benchmark_duration.count() > 0) {
        hashes_per_second = benchmark_total_hashes.load() / benchmark_duration.count();
    }

    std::cout << CR_GREEN << "[BENCHMARK] Estimated speed: " << std::fixed << std::setprecision(2) << hashes_per_second << " H/s" << RESET << std::endl;
    return hashes_per_second;
}

// Fonction utilitaire pour obtenir le chemin du répertoire de l'exécutable
std::string get_executable_dir() {
    std::string exec_path;
#ifdef __linux__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        exec_path = std::string(result, (size_t)count);
        return std::filesystem::path(exec_path).parent_path().string();
    } else {
        std::cerr << CR_RED << "[WARNING] Could not read /proc/self/exe. Falling back to current_path(). Error: " << strerror(errno) << RESET << std::endl;
        // Fallback si readlink échoue
        try {
            return std::filesystem::current_path().string();
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << CR_RED << "[ERROR] Filesystem error: " << e.what() << RESET << std::endl;
            return "";
        }
    }
#else
    // Pour les systèmes non-Linux, nous nous basons sur current_path()
    try {
        return std::filesystem::current_path().string();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << CR_RED << "[ERROR] Filesystem error: " << e.what() << RESET << std::endl;
        return "";
    }
#endif
}

// Fonction de réduction : transforme un hash en un mot de passe
// Cette fonction est CRUCIALE et DOIT être IDENTIQUE pour la génération et le cracking.
// Elle utilise le hash et l'index de la fonction de réduction (r_index) pour la déterminisme.
std::string reduce_hash(const std::string& hash, size_t target_len, const std::string& charset, int r_index) {
    if (charset.empty() || target_len == 0) return "";

    std::string reduced_string = "";
    // Combinaison du hash et de l'index pour un seed unique et déterministe
    // Le seed doit être le même à la génération et au crack pour le même hash et r_index.
    std::string seed_str = hash + std::to_string(r_index);
    std::hash<std::string> hasher;
    size_t seed_val = hasher(seed_str);

    std::mt19937 generator(static_cast<unsigned int>(seed_val)); // Utilise le seed_val pour un générateur reproductible
    std::uniform_int_distribution<> distribution(0, charset.length() - 1);

    for (size_t i = 0; i < target_len; ++i) {
        reduced_string += charset[distribution(generator)];
    }
    return reduced_string;
}

// --- Attaque par Dictionnaire ---
void perform_dictionary_attack(const std::string& target_hash, const EVP_MD* digest_type) {
    std::cout << "\n" << CR_BLUE << ">>> [SCANNING] Initiating Dictionary Attack..." << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Target Hash   : " << target_hash << RESET << std::endl;

    std::string wordlist_path_user;
    char choice;

    std::cout << CR_YELLOW << "\n Do you want to use the default wordlists (common.part.01 to common.part.10)?" << RESET << std::endl;
    std::cout << CR_CYAN << " (They should be in the 'wordlists/' directory relative to the executable)" << RESET << std::endl;
    std::cout << CR_YELLOW << " Enter 'Y' for default, or 'N' to specify a custom wordlist path: > " << RESET;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the rest of the line

    std::vector<std::string> wordlist_paths;
    std::string base_path = get_executable_dir(); // Obtient le répertoire de l'exécutable
    if (!base_path.empty() && base_path.back() != std::filesystem::path::preferred_separator) {
        base_path += std::filesystem::path::preferred_separator;
    }

    if (tolower(choice) == 'y') {
        std::cout << CR_DARK_GRAY << "    Using default wordlists..." << RESET << std::endl;
        std::string default_wordlist_dir = base_path + "wordlists" + std::filesystem::path::preferred_separator;
        for (int i = 1; i <= 10; ++i) {
            std::stringstream ss;
            ss << default_wordlist_dir << "common.part." << std::setw(2) << std::setfill('0') << i;
            wordlist_paths.push_back(ss.str());
        }
    } else {
        std::cout << CR_YELLOW << " [CUSTOM WORDLIST PATH] Enter full path to your wordlist > " << RESET;
        std::getline(std::cin, wordlist_path_user);
        wordlist_paths.push_back(wordlist_path_user);
    }

    if (!digest_type) {
        std::cerr << CR_RED << "[ERROR] Invalid OpenSSL digest type. Aborting." << RESET << std::endl;
        return;
    }

    long long total_attempts = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (const std::string& wordlist_path : wordlist_paths) {
        if (g_hash_cracked_flag.load()) break;

        std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;
        std::cout << CR_DARK_GRAY << "    Processing Wordlist: " << wordlist_path << RESET << std::endl;
        std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;

        std::ifstream wordlist_file(wordlist_path);
        if (!wordlist_file.is_open()) {
            std::cerr << CR_RED << "[ERROR] Failed to open wordlist: " << wordlist_path << " (" << strerror(errno) << ")" << RESET << std::endl;
            std::cerr << CR_RED << "[HINT] Check path and file permissions. If using default, ensure 'wordlists/' exists and contains the files." << RESET << std::endl;
            continue;
        }

        std::string word;
        while (std::getline(wordlist_file, word) && !g_hash_cracked_flag.load()) {
            if (!word.empty() && word.back() == '\r') {
                word.pop_back();
            }
            if (word.empty()) continue;

            total_attempts++;
            if (total_attempts % 100000 == 0) {
                std::lock_guard<std::mutex> lock(g_cout_mutex);
                std::cout << "\r" << CR_YELLOW << "[PROGRESS] Dictionary: " << format_attempts(total_attempts) << " words scanned. Current word: " << word.substr(0, std::min((size_t)20, word.length())) << "..." << std::flush << RESET;
            }

            std::string current_hash = calculate_hash_openssl(word, digest_type);
            if (current_hash == target_hash) {
                std::lock_guard<std::mutex> lock(g_cout_mutex);
                if (!g_hash_cracked_flag.load()) {
                    g_found_password = word;
                    g_hash_cracked_flag.store(true);
                    std::cout << "\r" << std::string(120, ' ') << "\r"; // Efface la ligne de progression
                    std::cout << CR_GREEN << "[SUCCESS] Hash Cracked! Password Found: " << BOLD << g_found_password << RESET << std::endl;
                }
                break;
            }
        }
        wordlist_file.close();
    }

    if (!g_hash_cracked_flag.load()) {
        std::cout << "\r" << std::string(80, ' ') << "\r";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    if (!g_hash_cracked_flag.load()) {
        std::cout << CR_YELLOW << "[INFO] Hash not cracked using dictionary. (" << format_attempts(total_attempts) << " attempts)" << RESET << std::endl;
    }
    std::cout << CR_BLUE << "[COMPLETED] Dictionary attack finished in " << std::fixed << std::setprecision(2) << duration.count() << " seconds." << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;
}

// --- Attaque par Force Brute (Multithread avec OpenMP) ---

// Fonction récursive pour générer et tester des combinaisons de force brute
void generate_combinations_recursive_omp(
    std::string current_combination,
    int target_length,
    const std::string& charset,
    const std::string& target_hash,
    const EVP_MD* digest_type)
{
    if (g_hash_cracked_flag.load()) {
        return;
    }

    if (current_combination.length() == target_length) {
        g_total_attempts_bruteforce++;

        if (g_total_attempts_bruteforce % 500000 == 0) { // Mettre à jour la ligne de progression toutes les 500 000 tentatives
            std::lock_guard<std::mutex> cout_lock(g_cout_mutex);
            std::cout << "\r" << CR_YELLOW << "[PROGRESS] Bruteforce: " << format_attempts(g_total_attempts_bruteforce) << " attempts. Testing: " << current_combination << std::flush << RESET;
        }

        std::string hashed_attempt = calculate_hash_openssl(current_combination, digest_type);
        if (hashed_attempt == target_hash) {
            std::lock_guard<std::mutex> lock(g_cout_mutex);
            if (!g_hash_cracked_flag.load()) {
                g_found_password = current_combination;
                g_hash_cracked_flag.store(true);
                std::cout << "\r" << std::string(120, ' ') << "\r"; // Efface la ligne de progression
                std::cout << CR_GREEN << "[SUCCESS] Hash Cracked! Password Found: " << BOLD << g_found_password << RESET << std::endl;
            }
        }
        return;
    }

    for (char c : charset) {
        if (g_hash_cracked_flag.load()) return;
        generate_combinations_recursive_omp(current_combination + c, target_length, charset, target_hash, digest_type);
    }
}

void perform_bruteforce_attack(
    const std::string& target_hash,
    const EVP_MD* digest_type,
    const std::string& charset_str,
    int min_len,
    int max_len)
{
    std::cout << "\n" << CR_BLUE << ">>> [SCANNING] Initiating Bruteforce Attack..." << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Target Hash    : " << target_hash << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Charset        : " << std::quoted(charset_str) << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Length Range   : " << min_len << "-" << max_len << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;

    if (!digest_type) {
        std::cerr << CR_RED << "[ERROR] Invalid OpenSSL digest type. Aborting." << RESET << std::endl;
        return;
    }
    if (charset_str.empty()) {
        std::cerr << CR_RED << "[ERROR] Charset is empty. Cannot perform bruteforce." << RESET << std::endl;
        return;
    }

    unsigned int num_threads_to_use = 1;
    #ifdef _OPENMP
    num_threads_to_use = omp_get_max_threads();
    if (num_threads_to_use == 0) num_threads_to_use = 2; // Fallback
    #else
    num_threads_to_use = std::thread::hardware_concurrency();
    if (num_threads_to_use == 0) num_threads_to_use = 2; // Fallback
    #endif

    // Exécuter le benchmark pour obtenir le H/s
    double hashes_per_second = run_benchmark(digest_type, charset_str, num_threads_to_use);
    if (hashes_per_second <= 0) {
        std::cerr << CR_RED << "[ERROR] Benchmark failed or returned zero H/s. Cannot estimate time." << RESET << std::endl;
        // Permettre de continuer mais sans estimation si l'utilisateur le souhaite
        char continue_without_estimation;
        std::cout << CR_YELLOW << "Continue without time estimation? (y/n) > " << RESET;
        std::cin >> continue_without_estimation;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (tolower(continue_without_estimation) != 'y') {
            std::cout << CR_YELLOW << "[INFO] Bruteforce attack cancelled by user." << RESET << std::endl;
            return;
        }
    }

    // Calcul et affichage du nombre maximal de tentatives et du temps estimé
    long double estimated_max_attempts = calculate_max_attempts(charset_str.length(), min_len, max_len);
    std::cout << CR_MAGENTA << "\n[ESTIMATION] Max attempts for full crack: " << format_attempts(estimated_max_attempts) << RESET << std::endl;

    if (hashes_per_second > 0) {
        long double estimated_time_seconds = estimated_max_attempts / hashes_per_second;
        std::cout << CR_MAGENTA << "[ESTIMATION] Estimated time for full crack: " << format_time_duration(estimated_time_seconds) << RESET << std::endl;
        std::cout << CR_MAGENTA << "             (This is an estimation and depends on CPU/GPU load and hash complexity)" << RESET << std::endl;
    }

    char confirm_choice;
    std::cout << CR_YELLOW << "Do you want to proceed with the bruteforce attack? (y/n) > " << RESET;
    std::cin >> confirm_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (tolower(confirm_choice) != 'y') {
        std::cout << CR_YELLOW << "[INFO] Bruteforce attack cancelled by user." << RESET << std::endl;
        return;
    }

    g_total_attempts_bruteforce = 0; // Réinitialiser avant de commencer l'attaque
    auto start_time = std::chrono::high_resolution_clock::now();

    std::cout << CR_DARK_GRAY << "    Using " << num_threads_to_use << " threads." << RESET << std::endl;

    for (int len = min_len; len <= max_len; ++len) {
        if (g_hash_cracked_flag.load()) break;
        {
            std::lock_guard<std::mutex> lock(g_cout_mutex);
            std::cout << "\n" << CR_CYAN << "[INFO] Testing passwords of length " << len << "..." << RESET << std::endl;
        }

        #ifdef _OPENMP
        #pragma omp parallel for shared(g_hash_cracked_flag, g_found_password, g_total_attempts_bruteforce, g_cout_mutex) schedule(dynamic) num_threads(num_threads_to_use)
        #endif
        for (size_t i = 0; i < charset_str.length(); ++i) {
            if (g_hash_cracked_flag.load()) {
                continue;
            }
            std::string initial_string(1, charset_str[i]);
            generate_combinations_recursive_omp(initial_string, len, charset_str, target_hash, digest_type);
        }

        if (g_hash_cracked_flag.load()) {
            std::cout << "\r" << std::string(80, ' ') << "\r";
            break;
        }
    }

    if (!g_hash_cracked_flag.load()) {
        std::cout << "\r" << std::string(80, ' ') << "\r";
        std::cout << CR_YELLOW << "[INFO] Hash not cracked using bruteforce." << RESET << std::endl;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    std::cout << CR_BLUE << "[COMPLETED] Bruteforce attack finished in " << std::fixed << std::setprecision(2) << duration.count() << " seconds. (" << format_attempts(g_total_attempts_bruteforce) << " attempts)" << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;
}

// --- Génération de Table Arc-en-Ciel ---
void generate_rainbow_table(
    const std::string& output_file,
    const EVP_MD* digest_type,
    const std::string& charset,
    int min_len, // Minimum length of passwords in the chains
    int max_len, // Maximum length of passwords in the chains
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
    // std::random_device rd; // Remplacé par seed basé sur le temps pour Termux ou si rd est faible
    std::mt19937 generator_initial_word(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> charset_dist(0, charset.length() - 1);
    std::uniform_int_distribution<> len_dist(min_len, max_len);

    auto start_time = std::chrono::high_resolution_clock::now();
    long long generated_chains_count = 0;

    for (long long i = 0; i < num_chains; ++i) {
        // Générer un mot de passe de départ aléatoire pour la chaîne
        int current_len_for_reduction = len_dist(generator_initial_word); // La longueur des mots générés par réduction
        std::string start_word = "";
        for (int k = 0; k < current_len_for_reduction; ++k) {
            start_word += charset[charset_dist(generator_initial_word)];
        }

        std::string current_word_in_chain = start_word;
        std::string current_hash_in_chain;

        // Parcourir la chaîne
        for (int j = 0; j < chain_length; ++j) {
            current_hash_in_chain = calculate_hash_openssl(current_word_in_chain, digest_type);
            if (j == chain_length - 1) { // Dernier élément de la chaîne, on le stocke
                break;
            }
            // Réduire le hash pour le prochain mot. La longueur cible est la longueur du mot de passe initial.
            current_word_in_chain = reduce_hash(current_hash_in_chain, current_len_for_reduction, charset, j);
        }

        // Stocker le mot de départ et le hash de fin de chaîne
        outfile << start_word << ":" << current_hash_in_chain << "\n";
        generated_chains_count++;

        if (generated_chains_count % 1000 == 0) {
            std::lock_guard<std::mutex> lock(g_cout_mutex);
            std::cout << "\r" << CR_YELLOW << "[PROGRESS] Generated " << format_attempts(generated_chains_count) << "/" << format_attempts(num_chains) << " chains. Last word: " << start_word << std::flush << RESET;
        }
    }

    outfile.close();
    std::cout << "\r" << std::string(80, ' ') << "\r"; // Efface la ligne de progression

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;

    std::cout << CR_GREEN << "[COMPLETED] Rainbow table generated in " << std::fixed << std::setprecision(2) << duration.count() << " seconds. (" << format_attempts(generated_chains_count) << " chains saved)" << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;
}

// --- Attaque par Table Arc-en-Ciel ---
void perform_rainbow_attack(const std::string& target_hash, const std::string& rainbow_table_path, const EVP_MD* digest_type, const std::string& charset_for_reduction, int chain_length, int assumed_min_len, int assumed_max_len) {
    std::cout << "\n" << CR_BLUE << ">>> [SCANNING] Initiating Rainbow Table Attack..." << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Target Hash   : " << target_hash << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Rainbow Table : " << rainbow_table_path << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Chain Length  : " << chain_length << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Charset Used  : " << std::quoted(charset_for_reduction) << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Assumed Pass Lengths: " << assumed_min_len << "-" << assumed_max_len << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;

    if (!digest_type) {
        std::cerr << CR_RED << "[ERROR] Invalid OpenSSL digest type. Aborting." << RESET << std::endl;
        return;
    }
    if (charset_for_reduction.empty()) {
        std::cerr << CR_RED << "[ERROR] Charset for reduction is empty. Cannot perform rainbow attack." << RESET << std::endl;
        return;
    }
    if (chain_length <= 0) {
        std::cerr << CR_RED << "[ERROR] Invalid chain length. Must be > 0." << RESET << std::endl;
        return;
    }
    if (assumed_min_len <= 0 || assumed_max_len < assumed_min_len) {
        std::cerr << CR_RED << "[ERROR] Invalid assumed password length range. Min length > 0 and Max length >= Min length." << RESET << std::endl;
        return;
    }

    std::ifstream rainbow_table_file(rainbow_table_path);
    if (!rainbow_table_file.is_open()) {
        std::cerr << CR_RED << "[ERROR] Failed to open rainbow table: " << rainbow_table_path << " (" << strerror(errno) << ")" << RESET << std::endl;
        std::cerr << CR_RED << "[HINT] Check path and file permissions. Make sure the table exists or generate one using option 4." << RESET << std::endl;
        return;
    }

    std::map<std::string, std::string> rainbow_map; // hash_fin_chaine -> motdepasse_debut_chaine
    std::string line;
    long long loaded_entries = 0;

    std::cout << CR_BLUE << "[LOADING] Loading rainbow table into memory. This may take a while for large tables..." << RESET << std::endl;
    auto load_start_time = std::chrono::high_resolution_clock::now();

    while (std::getline(rainbow_table_file, line)) {
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string start_word = line.substr(0, colon_pos);
            std::string end_hash = line.substr(colon_pos + 1);
            rainbow_map[end_hash] = start_word;
            loaded_entries++;
            if (loaded_entries % 100000 == 0) {
                 std::lock_guard<std::mutex> lock(g_cout_mutex);
                 std::cout << "\r" << CR_YELLOW << "[PROGRESS] Loaded " << format_attempts(loaded_entries) << " entries..." << std::flush << RESET;
            }
        }
    }
    rainbow_table_file.close();

    auto load_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> load_duration = load_end_time - load_start_time;
    std::cout << "\r" << std::string(80, ' ') << "\r"; // Clear progress line
    std::cout << CR_GREEN << "[LOADED] Rainbow table loaded. " << format_attempts(loaded_entries) << " entries in " << std::fixed << std::setprecision(2) << load_duration.count() << " seconds." << RESET << std::endl;

    std::cout << CR_BLUE << "\n[CRACKING] Starting rainbow table lookup..." << RESET << std::endl;
    auto crack_start_time = std::chrono::high_resolution_clock::now();

    // La logique de recherche : pour chaque longueur de mot de passe possible (générée lors de la création de la table)
    // et pour chaque position dans une chaîne (de 0 à chain_length-1),
    // on va simuler la remontée de la chaîne depuis le hash cible.
    bool cracked = false;

    for (int current_len_for_reduction = assumed_min_len; current_len_for_reduction <= assumed_max_len; ++current_len_for_reduction) {
        if (g_hash_cracked_flag.load()) break; // Si le hash est trouvé, sortir des boucles

        // La boucle 'i' représente la "position" possible du hash cible dans une chaîne arc-en-ciel.
        // Si le hash cible est H_k dans une chaîne, alors i = k.
        // On remonte ensuite jusqu'à la fin de la chaîne pour obtenir H_final.
        for (int i = 0; i < chain_length; ++i) {
            if (g_hash_cracked_flag.load()) break; // Vérification rapide

            std::string current_hash_in_walk = target_hash;
            std::string current_word_in_walk;

            // Appliquer les fonctions de réduction et de hachage jusqu'à la fin de la chaîne
            // Le "r_index" pour la fonction de réduction doit augmenter à chaque pas.
            // Le premier pas est `i` (position du hash cible dans la chaîne).
            for (int j = i; j < chain_length; ++j) {
                current_word_in_walk = reduce_hash(current_hash_in_walk, current_len_for_reduction, charset_for_reduction, j);
                current_hash_in_walk = calculate_hash_openssl(current_word_in_walk, digest_type);
            }

            // Maintenant, current_hash_in_walk contient le H_final de la chaîne potentielle.
            // On cherche ce H_final dans la table arc-en-ciel.
            if (rainbow_map.count(current_hash_in_walk)) {
                std::string start_word_from_table = rainbow_map[current_hash_in_walk];

                // Si on trouve un H_final correspondant, on a une chaîne candidate.
                // Il faut maintenant reconstruire la chaîne depuis le mot de départ trouvé
                // (start_word_from_table) jusqu'à la position `i` pour vérifier si
                // on retrouve le hash cible `target_hash`.
                std::string potential_password = start_word_from_table;
                for (int k = 0; k <= i; ++k) { // On va jusqu'à la position 'i'
                    std::string hashed_potential = calculate_hash_openssl(potential_password, digest_type);
                    if (hashed_potential == target_hash) {
                        // On a trouvé le mot de passe !
                        std::lock_guard<std::mutex> lock(g_cout_mutex);
                        if (!g_hash_cracked_flag.load()) {
                            g_found_password = potential_password;
                            g_hash_cracked_flag.store(true);
                            std::cout << "\r" << std::string(120, ' ') << "\r"; // Efface la ligne de progression
                            std::cout << CR_GREEN << "[SUCCESS] Hash Cracked! Password Found: " << BOLD << g_found_password << RESET << std::endl;
                        }
                        cracked = true;
                        break; // Sortir de la boucle interne
                    }
                    if (k < i) { // Ne pas réduire après le dernier hachage vérifié
                        potential_password = reduce_hash(hashed_potential, current_len_for_reduction, charset_for_reduction, k);
                    }
                }
                if (cracked) break; // Sortir de la boucle 'i'
            }

            if (i % 100 == 0) { // Mettre à jour la progression de la recherche arc-en-ciel
                std::lock_guard<std::mutex> lock(g_cout_mutex);
                std::cout << "\r" << CR_YELLOW << "[PROGRESS] Rainbow: Checking position " << i << "/" << chain_length << " (len: " << current_len_for_reduction << ")..." << std::flush << RESET;
            }
        }
    }
    std::cout << "\r" << std::string(100, ' ') << "\r"; // Clear progress line

    auto crack_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> crack_duration = crack_end_time - crack_start_time;

    if (!g_hash_cracked_flag.load()) {
        std::cout << CR_YELLOW << "[INFO] Hash not cracked using rainbow table." << RESET << std::endl;
    }
    std::cout << CR_BLUE << "[COMPLETED] Rainbow table attack finished in " << std::fixed << std::setprecision(2) << crack_duration.count() << " seconds." << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;
}


// --- Fonction principale ---
int main() {
    // Initialiser OpenSSL
    ERR_load_crypto_strings();
    OpenSSL_add_all_digests(); // Initialiser toutes les fonctions de hachage disponibles

    // Boucle principale du programme pour revenir au menu après chaque opération
    while (true) {
        // Efface l'écran pour un affichage propre au démarrage de chaque cycle
        std::cout << "\033[H\033[J";

        // Définir la largeur du terminal pour le centrage
        const int terminal_width = 80; // Largeur typique d'un terminal Termux

        // Bannière d'accueil personnalisée
        std::string title = "Hashcracker-V.CPP";
        std::string subtitle = "Karim"; // <--- MODIFIÉ ICI

        // Calcul pour centrer le titre principal
        int title_padding = (terminal_width - title.length()) / 2;
        std::cout << std::string(title_padding, ' ') << CR_RED BOLD << title << RESET << std::endl;

        // Calcul pour centrer le sous-titre
        int subtitle_padding = (terminal_width - (std::string("by ") + subtitle).length()) / 2;
        std::cout << std::string(subtitle_padding, ' ') << CR_CYAN << "by " << FAINT ITALIC << subtitle << RESET << std::endl;

        // Lignes de séparation plus visuelles
        std::cout << CR_BLUE << std::string(terminal_width, '=') << RESET << std::endl;
        std::cout << CR_MAGENTA << "\n    [INFO] Welcome to Hashcracker-V.CPP! " << RESET << std::endl;
        std::cout << CR_MAGENTA << "    [INFO] Your ultimate hash cracking and generation tool." << RESET << std::endl;
        std::cout << CR_BLUE << std::string(terminal_width, '=') << RESET << std::endl;

        std::string input_hash_hex;
        std::cout << CR_YELLOW << "\n [TARGET HASH] Enter hash to crack (or 'exit' to quit) > " << RESET;
        std::cin >> input_hash_hex;
        std::transform(input_hash_hex.begin(), input_hash_hex.end(), input_hash_hex.begin(), ::tolower);

        if (input_hash_hex == "exit") {
            std::cout << CR_BLUE << "\n==========================================================" << RESET << std::endl;
            std::cout << CR_CYAN << "  [GOODBYE] Exiting Hashcracker. See you soon! " << RESET << std::endl;
            std::cout << CR_BLUE << "==========================================================" << RESET << std::endl;
            break; // Sortir de la boucle principale et terminer le programme
        }

        std::string detected_type_str = detect_hash_type_str(input_hash_hex);
        const EVP_MD* digest_algo = get_openssl_digest_type(detected_type_str);

        std::cout << CR_DARK_GRAY << "   [ANALYSIS] Hash Type Detected: " << detected_type_str << RESET << std::endl;

        if (detected_type_str == "INCONNU" || digest_algo == nullptr) {
            std::cerr << CR_RED << "[ERROR] Unknown or unsupported hash type. Supported: MD5, SHA1, SHA256, SHA384, SHA512." << RESET << std::endl;
            std::cout << CR_YELLOW << "Press Enter to return to main menu..." << RESET;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue; // Revenir au début de la boucle principale
        }

        std::cout << CR_BLUE << "\n--- [ATTACK SELECTION] --------------------------------" << RESET << std::endl;
        std::cout << CR_CYAN << " 1. Dictionary Attack (Wordlist)" << RESET << std::endl;
        std::cout << CR_CYAN << " 2. Bruteforce Attack (Character Set)" << RESET << std::endl;
        std::cout << CR_CYAN << " 3. Rainbow Table Attack (Pre-calculated table)" << RESET << std::endl;
        std::cout << CR_CYAN << " 4. Generate Rainbow Table" << RESET << std::endl;
        int attack_choice;
        std::cout << CR_YELLOW << " [SELECT ATTACK (1/2/3/4)] > " << RESET;
        std::cin >> attack_choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer after int input

        // Réinitialiser les drapeaux et variables globales pour chaque nouvelle tentative de crack
        g_hash_cracked_flag.store(false);
        g_found_password = "";
        g_total_attempts_bruteforce = 0;

        if (attack_choice == 1) {
            perform_dictionary_attack(input_hash_hex, digest_algo);
        } else if (attack_choice == 2) {
            std::cout << CR_BLUE << "\n--- [BRUTEFORCE PARAMETERS] ---------------------------" << RESET << std::endl;

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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
                std::cerr << CR_RED << "[ERROR] Charset is empty. Cannot perform bruteforce." << RESET << std::endl;
                std::cout << CR_YELLOW << "Press Enter to return to main menu..." << RESET;
                std::cin.get();
                continue;
            }

            int min_l, max_l;
            std::cout << CR_YELLOW << " [MIN LENGTH] Enter minimum password length > " << RESET;
            std::cin >> min_l;
            std::cout << CR_YELLOW << " [MAX LENGTH] Enter maximum password length > " << RESET;
            std::cin >> max_l;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

            if (min_l <= 0 || max_l < min_l) {
                std::cerr << CR_RED << "[ERROR] Invalid length range. Min length > 0 and Max length >= Min length." << RESET << std::endl;
                std::cout << CR_YELLOW << "Press Enter to return to main menu..." << RESET;
                std::cin.get();
                continue;
            }

            perform_bruteforce_attack(input_hash_hex, digest_algo, charset_input, min_l, max_l);

        } else if (attack_choice == 3) { // Rainbow Table Attack
            std::string base_path = get_executable_dir();
            if (!base_path.empty() && base_path.back() != std::filesystem::path::preferred_separator) {
                base_path += std::filesystem::path::preferred_separator;
            }
            std::string rainbow_table_file_path = base_path + "rainbow.txt"; // Chemin par défaut

            std::cout << CR_DARK_GRAY << "  [INFO] Default Rainbow Table path set to: " << rainbow_table_file_path << RESET << std::endl;

            // Définition des jeux de caractères prédéfinis pour la réduction (doit correspondre à la génération)
            std::map<int, std::pair<std::string, std::string>> predefined_charsets;
            predefined_charsets[1] = {"Lowercase letters (a-z)", "abcdefghijklmnopqrstuvwxyz"};
            predefined_charsets[2] = {"Lowercase + Digits (a-z, 0-9)", "abcdefghijklmnopqrstuvwxyz0123456789"};
            predefined_charsets[3] = {"Lowercase + Uppercase (a-z, A-Z)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
            predefined_charsets[4] = {"Lowercase + Uppercase + Digits (a-z, A-Z, 0-9)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
            predefined_charsets[5] = {"All common characters (a-z, A-Z, 0-9, !@#$%)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%"};
            predefined_charsets[6] = {"Numbers only (0-9)", "0123456789"};

            std::cout << CR_CYAN << "   Choose the charset used during table generation:" << RESET << std::endl;
            for (const auto& pair : predefined_charsets) {
                std::cout << CR_CYAN << "   " << pair.first << ". " << pair.second.first << RESET << std::endl;
            }
            std::cout << CR_CYAN << "   Or enter 'C' for Custom charset" << RESET << std::endl;
            std::cout << CR_YELLOW << " [REDUCTION CHARSET CHOICE] Enter choice (1-" << predefined_charsets.size() << " or C) > " << RESET;

            std::string charset_choice_str_reduction;
            std::cin >> charset_choice_str_reduction;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

            std::string reduction_charset;
            std::transform(charset_choice_str_reduction.begin(), charset_choice_str_reduction.end(), charset_choice_str_reduction.begin(), ::toupper);

            if (charset_choice_str_reduction == "C") {
                std::cout << CR_YELLOW << " [CUSTOM REDUCTION CHARSET] Enter the custom charset used for reduction (MUST MATCH GENERATION!) > " << RESET;
                std::getline(std::cin, reduction_charset);
            } else {
                try {
                    int choice_num = std::stoi(charset_choice_str_reduction);
                    if (predefined_charsets.count(choice_num)) {
                        reduction_charset = predefined_charsets[choice_num].second;
                        std::cout << CR_DARK_GRAY << "   Selected Charset: " << predefined_charsets[choice_num].first << RESET << std::endl;
                    } else {
                        std::cerr << CR_RED << "[ERROR] Invalid charset choice. Using empty charset." << RESET << std::endl;
                        reduction_charset = "";
                    }
                } catch (const std::invalid_argument& e) {
                    std::cerr << CR_RED << "[ERROR] Invalid input for charset choice. Using empty charset." << RESET << std::endl;
                    reduction_charset = "";
                } catch (const std::out_of_range& e) {
                    std::cerr << CR_RED << "[ERROR] Charset choice out of range. Using empty charset." << RESET << std::endl;
                    reduction_charset = "";
                }
            }

            if (reduction_charset.empty()) {
                std::cerr << CR_RED << "[ERROR] Reduction charset is empty. Cannot perform rainbow attack." << RESET << std::endl;
                std::cout << CR_YELLOW << "Press Enter to return to main menu..." << RESET;
                std::cin.get();
                continue;
            }

            int chain_len, assumed_min_len, assumed_max_len;
            std::cout << CR_YELLOW << " [CHAIN LENGTH] Enter the chain length used when generating the table (MUST MATCH GENERATION!) > " << RESET;
            std::cin >> chain_len;
            std::cout << CR_YELLOW << " [MIN PASSWORD LENGTH] Enter the minimum password length used during generation (MUST MATCH GENERATION!) > " << RESET;
            std::cin >> assumed_min_len;
            std::cout << CR_YELLOW << " [MAX PASSWORD LENGTH] Enter the maximum password length used during generation (MUST MATCH GENERATION!) > " << RESET;
            std::cin >> assumed_max_len;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

            if (chain_len <= 0 || assumed_min_len <= 0 || assumed_max_len < assumed_min_len) {
                std::cerr << CR_RED << "[ERROR] Invalid parameters for rainbow attack. Please check values." << RESET << std::endl;
                std::cout << CR_YELLOW << "Press Enter to return to main menu..." << RESET;
                std::cin.get();
                continue;
            }

            perform_rainbow_attack(input_hash_hex, rainbow_table_file_path, digest_algo, reduction_charset, chain_len, assumed_min_len, assumed_max_len);

        } else if (attack_choice == 4) { // Générer une table arc-en-ciel
            std::cout << CR_BLUE << "\n--- [RAINBOW TABLE GENERATION PARAMETERS] -------------" << RESET << std::endl;

            std::string output_filename = "rainbow.txt"; // Nom par défaut
            std::cout << CR_YELLOW << " [OUTPUT FILE] Enter desired output filename (e.g., my_rainbow_table.txt). Default: rainbow.txt > " << RESET;
            std::string temp_filename;
            std::getline(std::cin, temp_filename);
            if (!temp_filename.empty()) {
                output_filename = temp_filename;
            }

            std::string base_path = get_executable_dir();
            if (!base_path.empty() && base_path.back() != std::filesystem::path::preferred_separator) {
                base_path += std::filesystem::path::preferred_separator;
            }
            std::string full_output_path = base_path + output_filename;

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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::string charset_gen_input;
            std::transform(charset_choice_str.begin(), charset_choice_str.end(), charset_choice_str.begin(), ::toupper);

            if (charset_choice_str == "C") {
                std::cout << CR_YELLOW << " [CUSTOM CHARACTER SET] Enter your custom charset > " << RESET;
                std::getline(std::cin, charset_gen_input);
            } else {
                try {
                    int choice_num = std::stoi(charset_choice_str);
                    if (predefined_charsets.count(choice_num)) {
                        charset_gen_input = predefined_charsets[choice_num].second;
                        std::cout << CR_DARK_GRAY << "   Selected Charset: " << predefined_charsets[choice_num].first << RESET << std::endl;
                    } else {
                        std::cerr << CR_RED << "[ERROR] Invalid charset choice. Using empty charset." << RESET << std::endl;
                        charset_gen_input = "";
                    }
                } catch (const std::invalid_argument& e) {
                    std::cerr << CR_RED << "[ERROR] Invalid input for charset choice. Using empty charset." << RESET << std::endl;
                    charset_gen_input = "";
                } catch (const std::out_of_range& e) {
                    std::cerr << CR_RED << "[ERROR] Charset choice out of range. Using empty charset." << RESET << std::endl;
                    charset_gen_input = "";
                }
            }

            if (charset_gen_input.empty()) {
                std::cerr << CR_RED << "[ERROR] Charset is empty. Cannot generate rainbow table." << RESET << std::endl;
                std::cout << CR_YELLOW << "Press Enter to return to main menu..." << RESET;
                std::cin.get();
                continue;
            }

            int min_len_gen, max_len_gen;
            std::cout << CR_YELLOW << " [MIN LENGTH] Enter minimum password length for chains > " << RESET;
            std::cin >> min_len_gen;
            std::cout << CR_YELLOW << " [MAX LENGTH] Enter maximum password length for chains > " << RESET;
            std::cin >> max_len_gen;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (min_len_gen <= 0 || max_len_gen < min_len_gen) {
                std::cerr << CR_RED << "[ERROR] Invalid length range. Min length > 0 and Max length >= Min length." << RESET << std::endl;
                std::cout << CR_YELLOW << "Press Enter to return to main menu..." << RESET;
                std::cin.get();
                continue;
            }

            long long num_chains_gen;
            std::cout << CR_YELLOW << " [NUMBER OF CHAINS] Enter number of chains to generate (e.g., 1000000) > " << RESET;
            std::cin >> num_chains_gen;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            int chain_length_gen;
            std::cout << CR_YELLOW << " [CHAIN LENGTH] Enter length of each chain (e.g., 10000) > " << RESET;
            std::cin >> chain_length_gen;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::string hash_type_gen_str;
            std::cout << CR_YELLOW << " [HASH TYPE] Enter hash type (MD5, SHA1, SHA256, SHA512). MUST MATCH TARGET! > " << RESET;
            std::cin >> hash_type_gen_str;
            std::transform(hash_type_gen_str.begin(), hash_type_gen_str.end(), hash_type_gen_str.begin(), ::toupper);

            const EVP_MD* digest_algo_gen = get_openssl_digest_type(hash_type_gen_str);
            if (digest_algo_gen == nullptr) {
                std::cerr << CR_RED << "[ERROR] Unsupported hash type for generation. Supported: MD5, SHA1, SHA256, SHA384, SHA512." << RESET << std::endl;
                std::cout << CR_YELLOW << "Press Enter to return to main menu..." << RESET;
                std::cin.get();
                continue;
            }

            // Estimation de la taille du fichier
            // (Longueur min du mot de passe + taille du hash en hex + 2 (pour ':' et '\n')) * nombre de chaînes
            long double estimated_file_size_bytes = (long double)num_chains_gen * ((long double)min_len_gen + (EVP_MD_size(digest_algo_gen) * 2) + 2);
            std::cout << CR_MAGENTA << "\n[INFO] Generating a table of " << format_attempts(num_chains_gen) << " chains, each " << chain_length_gen << " steps long." << RESET << std::endl;
            std::cout << CR_MAGENTA << "       This will result in a file size of approximately "
                      << std::fixed << std::setprecision(2)
                      << estimated_file_size_bytes / (1024.0 * 1024.0 * 1024.0) // Convertir en Go
                      << " GB (estimation, can vary based on actual password lengths)." << RESET << std::endl;
            std::cout << CR_YELLOW << "       (Remember the actual size might be smaller if passwords are shorter than max_len_gen, or larger if max_len_gen is significantly used.)" << RESET << std::endl;

            char confirm_choice_gen;
            std::cout << CR_YELLOW << "Do you want to proceed with rainbow table generation? (y/n) > " << RESET;
            std::cin >> confirm_choice_gen;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (tolower(confirm_choice_gen) != 'y') {
                std::cout << CR_YELLOW << "[INFO] Rainbow table generation cancelled by user." << RESET << std::endl;
            } else {
                generate_rainbow_table(full_output_path, digest_algo_gen, charset_gen_input, min_len_gen, max_len_gen, num_chains_gen, chain_length_gen);
            }

        }
        else {
            std::cerr << CR_RED << "[ERROR] Invalid attack choice. Please select 1, 2, 3, or 4." << RESET << std::endl;
        }

        std::cout << CR_BLUE << "\n==========================================================" << RESET << std::endl;
        if (g_hash_cracked_flag.load()) {
            std::cout << CR_GREEN << "  [CRACK COMPLETE] Password found! Returning to main menu. " << RESET << std::endl;
        } else {
            std::cout << CR_CYAN << "  [MODULE COMPLETE] Operation finished. Returning to main menu. " << RESET << std::endl;
        }
        std::cout << CR_BLUE << "==========================================================" << RESET << std::endl;
        std::cout << CR_YELLOW << "Press Enter to continue..." << RESET;
        std::cin.get(); // Attend que l'utilisateur appuie sur Entrée pour revenir au menu
    } // Fin de la boucle while(true)

    // Libérer les ressources OpenSSL
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
