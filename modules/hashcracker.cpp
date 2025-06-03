#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <thread>
#include <atomic>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <cstring>
#include <map>
#include <cmath>
#include <limits>
#include <mutex>
#include <random>
#include <filesystem>
#ifdef __linux__
#include <unistd.h>
#endif
#ifdef _OPENMP
#include <omp.h>
#endif
#include <openssl/evp.h>
#include <openssl/err.h>
#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define INFO_COLOR "\033[0;34m" // Added INFO_COLOR to match the bash script's INFO
#define WHITE   "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"
#define BOLD    "\033[1m"
#define FAINT   "\033[2m"
#define ITALIC  "\033[3m"
#define UNDERLINE "\033[4m"
#define BLINK   "\033[5m"
#define REVERSE "\033[7m"
#define HIDDEN  "\033[8m"
#define STRIKETHROUGH "\033[9m"
#define CR_WHITE  WHITE BOLD
#define CR_CYAN   CYAN BOLD
#define CR_RED    RED BOLD
#define CR_GREEN  GREEN BOLD
#define CR_YELLOW YELLOW BOLD
#define CR_BLUE   BLUE BOLD FAINT
#define CR_MAGENTA MAGENTA BOLD FAINT
#define CR_DARK_GRAY "\033[90m"
std::atomic<bool> g_hash_cracked_flag(false);
std::string g_found_password;
std::mutex g_cout_mutex;
std::atomic<long long> g_total_attempts_bruteforce(0);
std::string bytes_to_hex_string(const unsigned char* bytes, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
    }
    return ss.str();
}
std::string calculate_hash_openssl(const std::string& input, const EVP_MD* digest_type) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
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
std::string detect_hash_type_str(const std::string& hash_hex) {
    size_t len = hash_hex.length();
    if (len == 32) return "MD5";
    if (len == 40) return "SHA1";
    if (len == 64) return "SHA256";
    if (len == 96) return "SHA384";
    if (len == 128) return "SHA512";
    return "INCONNU";
}
const EVP_MD* get_openssl_digest_type(const std::string& hash_type_str) {
    if (hash_type_str == "MD5") return EVP_md5();
    if (hash_type_str == "SHA1") return EVP_sha1();
    if (hash_type_str == "SHA256") return EVP_sha256();
    if (hash_type_str == "SHA384") return EVP_sha384();
    if (hash_type_str == "SHA512") return EVP_sha512();
    return nullptr;
}
long double calculate_max_attempts(int charset_size, int min_len, int max_len) {
    long double total_attempts = 0;
    for (int len = min_len; len <= max_len; ++len) {
        if (charset_size > 0) {
            total_attempts += std::pow((long double)charset_size, (long double)len);
        } else {
            return 0;
        }
    }
    return total_attempts;
}
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
        ss << std::fixed << std::setprecision(2) << attempts / 1e18 << " E";
    }
    return ss.str();
}
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
double run_benchmark(const EVP_MD* digest_type, const std::string& charset, int num_threads) {
    std::cout << CR_CYAN << "\n[BENCHMARK] Running a quick benchmark to estimate speed..." << RESET << std::endl;
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
    std::string test_input_base = "bench_input_0";
    for (int j = 0; j < benchmark_attempts_per_thread * num_threads; ++j) {
        calculate_hash_openssl(test_input_base + std::to_string(j), digest_total_hashes++;
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
        try {
            return std::filesystem::current_path().string();
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << CR_RED << "[ERROR] Filesystem error: " << e.what() << RESET << std::endl;
            return "";
        }
    }
#else
    try {
        return std::filesystem::current_path().string();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << CR_RED << "[ERROR] Filesystem error: " << e.what() << RESET << std::endl;
        return "";
    }
#endif
}

// CORRECTION ICI pour la fonction reduce_hash
std::string reduce_hash(const std::string& hash, size_t target_len, const std::string& charset, int r_index) {
    if (charset.empty() || target_len == 0) return "";
    std::string reduced_string = "";

    // Utilisation de std::seed_seq avec une graine plus robuste
    std::vector<unsigned int> seed_data;
    for (char c : hash) {
        seed_data.push_back(static_cast<unsigned int>(c));
    }
    seed_data.push_back(static_cast<unsigned int>(r_index)); // Inclure r_index pour des étapes déterministes

    std::seed_seq seed_sequence(seed_data.begin(), seed_data.end());
    std::mt19937 generator(seed_sequence);
    std::uniform_int_distribution<> distribution(0, charset.length() - 1);

    for (size_t i = 0; i < target_len; ++i) {
        reduced_string += charset[distribution(generator)];
    }
    return reduced_string;
}

void set_cursor_position(int x, int y) {
    std::cout << "\033[" << y << ";" << x << "H";
}

void clear_line() {
    std::cout << "\033[2K\r";
}

void display_loading_animation(const std::string& message, int duration_ms, int num_frames = 4) {
    const std::vector<std::string> frames = {"-", "\\", "|", "/"};
    std::cout << message << " ";
    std::cout.flush();
    auto start_time = std::chrono::high_resolution_clock::now();
    int frame_index = 0;
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count() < duration_ms) {
        std::lock_guard<std::mutex> lock(g_cout_mutex);
        clear_line();
        std::cout << CR_DARK_GRAY << message << " " << frames[frame_index % num_frames] << RESET << std::flush;
        frame_index++;
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms / (num_frames * 10)));
    }
    clear_line();
    std::cout << CR_GREEN << message << " [DONE]" << RESET << std::endl;
}

void simulate_typing(const std::string& text, int delay_ms, bool new_line = true, bool bold = false) {
    std::string color_code = CR_CYAN;
    if (bold) {
        color_code = BOLDWHITE;
    }
    std::cout << color_code;
    for (char c : text) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    std::cout << RESET;
    if (new_line) {
        std::cout << std::endl;
    }
}

void draw_progress_bar(long long current, long double total, int width = 50, const std::string& prefix = "", const std::string& suffix = "") {
    std::lock_guard<std::mutex> lock(g_cout_mutex);
    clear_line();
    if (total <= 0) {
        std::cout << prefix << "[ERROR: Total attempts is 0]" << suffix << std::flush;
        return;
    }
    double progress = static_cast<double>(current) / total;
    int filled_width = static_cast<int>(width * progress);
    std::cout << prefix << "[";
    std::cout << CR_GREEN;
    for (int i = 0; i < filled_width; ++i) {
        std::cout << "#";
    }
    std::cout << CR_DARK_GRAY;
    for (int i = filled_width; i < width; ++i) {
        std::cout << "-";
    }
    std::cout << RESET;
    std::cout << "] " << static_cast<int>(progress * 100.0) << "% " << suffix << std::flush;
}

void display_matrix_effect(int lines, int delay_ms) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()_+-=[]{}|;':\",.<>/?`~";
    for (int i = 0; i < lines; ++i) {
        for (int j = 0; j < 80; ++j) {
            int rand_char_idx = std::uniform_int_distribution<>(0, chars.length() - 1)(generator);
            int color_choice = std::uniform_int_distribution<>(0, 10)(generator);
            if (color_choice < 7) {
                std::cout << CR_GREEN;
            } else if (color_choice < 9) {
                std::cout << FAINT << CR_DARK_GRAY;
            } else {
                std::cout << CR_WHITE;
            }
            std::cout << chars[rand_char_idx];
        }
        std::cout << RESET << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }
    std::cout << "\033[H\033[J";
}

void perform_dictionary_attack(const std::string& target_hash, const EVP_MD* digest_type) {
    std::cout << "\n" << CR_BLUE << ">>> [SCANNING] Initiating Dictionary Attack..." << RESET << std::endl;
    std::cout << CR_DARK_GRAY << "    Target Hash   : " << target_hash << RESET << std::endl;
    std::string wordlist_path_user;
    char choice;
    std::cout << CR_YELLOW << "\n Do you want to use the default wordlists (common.part.01 to common.part.10)?" << RESET << std::endl;
    std::cout << CR_CYAN << " (They should be in the 'wordlists/' directory relative to the executable)" << RESET << std::endl;
    simulate_typing(" Enter 'Y' for default, or 'N' to specify a custom wordlist path: > ", 10, false);
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::vector<std::string> wordlist_paths;
    std::string base_path = get_executable_dir();
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
        simulate_typing(" [CUSTOM WORDLIST PATH] Enter full path to your wordlist > ", 10, false);
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
            if (total_attempts % 50000 == 0) {
                std::lock_guard<std::mutex> lock(g_cout_mutex);
                clear_line();
                std::cout << CR_YELLOW << "[PROGRESS] Dictionary: " << format_attempts(total_attempts) << " words scanned. Current: " << word.substr(0, std::min((size_t)30, word.length())) << "..." << std::flush << RESET;
            }
            std::string current_hash = calculate_hash_openssl(word, digest_type);
            if (current_hash == target_hash) {
                std::lock_guard<std::mutex> lock(g_cout_mutex);
                if (!g_hash_cracked_flag.load()) {
                    g_found_password = word;
                    g_hash_cracked_flag.store(true);
                    std::cout << "\r" << std::string(120, ' ') << "\r";
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
        if (g_total_attempts_bruteforce % 500000 == 0) {
            long double estimated_max = calculate_max_attempts(charset.length(), target_length, target_length);
            // CONCATENATION FIX
            draw_progress_bar(g_total_attempts_bruteforce, estimated_max, 50, CR_YELLOW + std::string("[BRUTEFORCE] "), " (" + current_combination.substr(0, std::min((size_t)10, current_combination.length())) + "...) " + RESET);
        }
        std::string hashed_attempt = calculate_hash_openssl(current_combination, digest_type);
        if (hashed_attempt == target_hash) {
            std::lock_guard<std::mutex> lock(g_cout_mutex);
            if (!g_hash_cracked_flag.load()) {
                g_found_password = current_combination;
                g_hash_cracked_flag.store(true);
                std::cout << "\r" << std::string(120, ' ') << "\r";
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
    if (num_threads_to_use == 0) num_threads_to_use = 2;
    #else
    num_threads_to_use = std::thread::hardware_concurrency();
    if (num_threads_to_use == 0) num_threads_to_use = 2;
    #endif
    double hashes_per_second = run_benchmark(digest_type, charset_str, num_threads_to_use);
    if (hashes_per_second <= 0) {
        std::cerr << CR_RED << "[ERROR] Benchmark failed or returned zero H/s. Cannot estimate time." << RESET << std::endl;
        char continue_without_estimation;
        simulate_typing("Continue without time estimation? (y/n) > ", 10, false);
        std::cin >> continue_without_estimation;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (tolower(continue_without_estimation) != 'y') {
            std::cout << CR_YELLOW << "[INFO] Bruteforce attack cancelled by user." << RESET << std::endl;
            return;
        }
    }
    long double estimated_max_attempts = calculate_max_attempts(charset_str.length(), min_len, max_len);
    std::cout << CR_MAGENTA << "\n[ESTIMATION] Max attempts for full crack: " << format_attempts(estimated_max_attempts) << RESET << std::endl;
    if (hashes_per_second > 0) {
        long double estimated_time_seconds = estimated_max_attempts / hashes_per_second;
        std::cout << CR_MAGENTA << "[ESTIMATION] Estimated time for full crack: " << format_time_duration(estimated_time_seconds) << RESET << std::endl;
        std::cout << CR_MAGENTA << "             (This is an estimation and depends on CPU/GPU load and hash complexity)" << RESET << std::endl;
    }
    char confirm_choice;
    simulate_typing("Do you want to proceed with the bruteforce attack? (y/n) > ", 10, false);
    std::cin >> confirm_choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (tolower(confirm_choice) != 'y') {
        std::cout << CR_YELLOW << "[INFO] Bruteforce attack cancelled by user." << RESET << std::endl;
        return;
    }
    g_total_attempts_bruteforce = 0;
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
    std::mt19937 generator_initial_word(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<> charset_dist(0, charset.length() - 1);
    std::uniform_int_distribution<> len_dist(min_len, max_len);
    auto start_time = std::chrono::high_resolution_clock::now();
    long long generated_chains_count = 0;
    for (long long i = 0; i < num_chains; ++i) {
        int current_len_for_reduction = len_dist(generator_initial_word);
        std::string start_word = "";
        for (int k = 0; k < current_len_for_reduction; ++k) {
            start_word += charset[charset_dist(generator_initial_word)];
        }
        std::string current_word_in_chain = start_word;
        std::string current_hash_in_chain;
        for (int j = 0; j < chain_length; ++j) {
            current_hash_in_chain = calculate_hash_openssl(current_word_in_chain, digest_type);
            if (j == chain_length - 1) {
                break;
            }
            current_word_in_chain = reduce_hash(current_hash_in_chain, current_len_for_reduction, charset, j);
        }
        outfile << start_word << ":" << current_hash_in_chain << "\n";
        generated_chains_count++;
        if (generated_chains_count % 1000 == 0) {
            // CONCATENATION FIX
            draw_progress_bar(generated_chains_count, num_chains, 50, CR_YELLOW + std::string("[GENERATION] "), " (" + start_word.substr(0, std::min((size_t)15, start_word.length())) + "...) " + RESET);
        }
    }
    outfile.close();
    std::cout << "\r" << std::string(80, ' ') << "\r";
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    std::cout << CR_GREEN << "[COMPLETED] Rainbow table generated in " << std::fixed << std::setprecision(2) << duration.count() << " seconds. (" << format_attempts(generated_chains_count) << " chains saved)" << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;
}
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
    std::map<std::string, std::string> rainbow_map;
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
                 clear_line();
                 std::cout << CR_YELLOW << "[LOADING] Loaded " << format_attempts(loaded_entries) << " entries..." << std::flush << RESET;
            }
        }
    }
    rainbow_table_file.close();
    std::cout << "\r" << std::string(80, ' ') << "\r";
    auto load_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> load_duration = load_end_time - load_start_time;
    std::cout << CR_GREEN << "[LOADED] Rainbow table loaded. " << format_attempts(loaded_entries) << " entries in " << std::fixed << std::setprecision(2) << load_duration.count() << " seconds." << RESET << std::endl;
    std::cout << CR_BLUE << "\n[CRACKING] Starting rainbow table lookup..." << RESET << std::endl;
    auto crack_start_time = std::chrono::high_resolution_clock::now();
    bool cracked = false;
    for (int current_len_for_reduction = assumed_min_len; current_len_for_reduction <= assumed_max_len; ++current_len_for_reduction) {
        if (g_hash_cracked_flag.load()) break;
        for (int i = 0; i < chain_length; ++i) {
            if (g_hash_cracked_flag.load()) break;
            std::string current_hash_in_walk = target_hash;
            std::string current_word_in_walk;
            for (int j = i; j < chain_length; ++j) {
                current_word_in_walk = reduce_hash(current_hash_in_walk, current_len_for_reduction, charset_for_reduction, j);
                current_hash_in_walk = calculate_hash_openssl(current_word_in_walk, digest_type);
            }
            if (rainbow_map.count(current_hash_in_walk)) {
                std::string start_word_from_table = rainbow_map[current_hash_in_walk];
                std::string potential_password = start_word_from_table;
                for (int k = 0; k <= i; ++k) {
                    std::string hashed_potential = calculate_hash_openssl(potential_password, digest_type);
                    if (hashed_potential == target_hash) {
                        std::lock_guard<std::mutex> lock(g_cout_mutex);
                        if (!g_hash_cracked_flag.load()) {
                            g_found_password = potential_password;
                            g_hash_cracked_flag.store(true);
                            std::cout << "\r" << std::string(120, ' ') << "\r";
                            std::cout << CR_GREEN << "[SUCCESS] Hash Cracked! Password Found: " << BOLD << g_found_password << RESET << std::endl;
                        }
                        cracked = true;
                        break;
                    }
                    if (k < i) {
                        potential_password = reduce_hash(hashed_potential, current_len_for_reduction, charset_for_reduction, k);
                    }
                }
                if (cracked) break;
            }
            if (i % 100 == 0) {
                // CONCATENATION FIX
                draw_progress_bar(i, chain_length, 50, CR_YELLOW + std::string("[CRACKING] "), " (Pos: " + std::to_string(i) + "/" + std::to_string(chain_length) + ", Len: " + std::to_string(current_len_for_reduction) + ") " + RESET);
            }
        }
    }
    std::cout << "\r" << std::string(100, ' ') << "\r";
    auto crack_end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> crack_duration = crack_end_time - crack_start_time;
    if (!g_hash_cracked_flag.load()) {
        std::cout << CR_YELLOW << "[INFO] Hash not cracked using rainbow table." << RESET << std::endl;
    }
    std::cout << CR_BLUE << "[COMPLETED] Rainbow table attack finished in " << std::fixed << std::setprecision(2) << crack_duration.count() << " seconds." << RESET << std::endl;
    std::cout << CR_BLUE << "---------------------------------------" << RESET << std::endl;
}
int main() {
    ERR_load_crypto_strings();
    OpenSSL_add_all_digests();
    display_matrix_effect(10, 20);
    while (true) {
        std::cout << "\033[H\033[J";
        const int terminal_width = 80;
        std::string title = "Hashcracker-V.CPP";
        std::string subtitle = "Karim";
        int title_padding = (terminal_width - title.length()) / 2;
        std::cout << std::string(title_padding, ' ') << CR_RED BOLD << title << RESET << std::endl;
        int subtitle_padding = (terminal_width - (std::string("by ") + subtitle).length()) / 2;
        std::cout << std::string(subtitle_padding, ' ') << CR_CYAN << "by " << FAINT ITALIC << subtitle << RESET << std::endl;
        std::cout << CR_BLUE << std::string(terminal_width, '=') << RESET << std::endl;
        simulate_typing("   [INFO] INITIALIZING SECURE MODULES...", 30);
        simulate_typing("   [INFO] Welcome to Hashcracker-V.CPP! ", 20);
        simulate_typing("   [INFO] Your ultimate hash cracking and generation tool.", 20);
        std::cout << CR_BLUE << std::string(terminal_width, '=') << RESET << std::endl;
        std::string input_hash_hex;
        simulate_typing("\n [TARGET HASH] Enter hash to crack (or 'exit' to quit) > ", 15, false);
        std::cin >> input_hash_hex;
        std::transform(input_hash_hex.begin(), input_hash_hex.end(), input_hash_hex.begin(), ::tolower);
        if (input_hash_hex == "exit") {
            std::cout << CR_BLUE << "\n==========================================================" << RESET << std::endl;
            simulate_typing("  [GOODBYE] TERMINATING ALL PROCESSES. See you soon! ", 30, true, true);
            std::cout << CR_BLUE << "==========================================================" << RESET << std::endl;
            break;
        }
        std::string detected_type_str = detect_hash_type_str(input_hash_hex);
        const EVP_MD* digest_algo = get_openssl_digest_type(detected_type_str);
        simulate_typing("   [ANALYSIS] Analyzing hash signature...", 20);
        std::cout << CR_DARK_GRAY << "   [ANALYSIS] Hash Type Detected: " << detected_type_str << RESET << std::endl;
        if (detected_type_str == "INCONNU" || digest_algo == nullptr) {
            simulate_typing("[ERROR] Unknown or unsupported hash type. Supported: MD5, SHA1, SHA256, SHA384, SHA512.", 20, true, true);
            simulate_typing("Press Enter to return to main menu...", 10);
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            continue;
        }
        std::cout << CR_BLUE << "\n--- [ATTACK SELECTION] --------------------------------" << RESET << std::endl;
        simulate_typing(" 1. Dictionary Attack (Wordlist)", 10);
        simulate_typing(" 2. Bruteforce Attack (Character Set)", 10);
        simulate_typing(" 3. Rainbow Table Attack (Pre-calculated table)", 10);
        simulate_typing(" 4. Generate Rainbow Table", 10);
        int attack_choice;
        simulate_typing(" [SELECT ATTACK (1/2/3/4)] > ", 15, false);
        std::cin >> attack_choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        g_hash_cracked_flag.store(false);
        g_found_password = "";
        g_total_attempts_bruteforce = 0;
        if (attack_choice == 1) {
            simulate_typing("\n>>> [MODULE ACTIVATED] Initiating Dictionary Attack Sequence...", 25, true, true);
            perform_dictionary_attack(input_hash_hex, digest_algo);
        } else if (attack_choice == 2) {
            simulate_typing("\n>>> [MODULE ACTIVATED] Initiating Bruteforce Protocol...", 25, true, true);
            std::cout << CR_BLUE << "\n--- [BRUTEFORCE PARAMETERS] ---------------------------" << RESET << std::endl;
            std::map<int, std::pair<std::string, std::string>> predefined_charsets;
            predefined_charsets[1] = {"Lowercase letters (a-z)", "abcdefghijklmnopqrstuvwxyz"};
            predefined_charsets[2] = {"Lowercase + Digits (a-z, 0-9)", "abcdefghijklmnopqrstuvwxyz0123456789"};
            predefined_charsets[3] = {"Lowercase + Uppercase (a-z, A-Z)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
            predefined_charsets[4] = {"Lowercase + Uppercase + Digits (a-z, A-Z, 0-9)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
            predefined_charsets[5] = {"All common characters (a-z, A-Z, 0-9, !@#$%)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%"};
            predefined_charsets[6] = {"Numbers only (0-9)", "0123456789"};
            simulate_typing("   Choose a predefined charset or enter your own:", 10);
            for (const auto& pair : predefined_charsets) {
                // CONCATENATION FIX
                simulate_typing(std::string("   ") + pair.first + ". " + pair.second.first, 5);
            }
            simulate_typing("   Or enter 'C' for Custom charset", 10);
            simulate_typing(" [CHARACTER SET CHOICE] Enter choice (1-" + std::to_string(predefined_charsets.size()) + " or C) > ", 15, false);
            std::string charset_choice_str;
            std::cin >> charset_choice_str;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string charset_input;
            std::transform(charset_choice_str.begin(), charset_choice_str.end(), charset_choice_str.begin(), ::toupper);
            if (charset_choice_str == "C") {
                simulate_typing(" [CUSTOM CHARACTER SET] Enter your custom charset > ", 15, false);
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
                simulate_typing("Press Enter to return to main menu...", 10);
                std::cin.get();
                continue;
            }
            int min_l, max_l;
            simulate_typing(" [MIN LENGTH] Enter minimum password length > ", 15, false);
            std::cin >> min_l;
            simulate_typing(" [MAX LENGTH] Enter maximum password length > ", 15, false);
            std::cin >> max_l;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (min_l <= 0 || max_l < min_l) {
                std::cerr << CR_RED << "[ERROR] Invalid length range. Min length > 0 and Max length >= Min length." << RESET << std::endl;
                simulate_typing("Press Enter to return to main menu...", 10);
                std::cin.get();
                continue;
            }
            perform_bruteforce_attack(input_hash_hex, digest_algo, charset_input, min_l, max_l);
        } else if (attack_choice == 3) {
            simulate_typing("\n>>> [MODULE ACTIVATED] Deploying Rainbow Table Lookup...", 25, true, true);
            std::string base_path = get_executable_dir();
            if (!base_path.empty() && base_path.back() != std::filesystem::path::preferred_separator) {
                base_path += std::filesystem::path::preferred_separator;
            }
            std::string rainbow_table_file_path = base_path + "rainbow.txt";
            std::cout << CR_DARK_GRAY << "  [INFO] Default Rainbow Table path set to: " << rainbow_table_file_path << RESET << std::endl;
            std::map<int, std::pair<std::string, std::string>> predefined_charsets;
            predefined_charsets[1] = {"Lowercase letters (a-z)", "abcdefghijklmnopqrstuvwxyz"};
            predefined_charsets[2] = {"Lowercase + Digits (a-z, 0-9)", "abcdefghijklmnopqrstuvwxyz0123456789"};
            predefined_charsets[3] = {"Lowercase + Uppercase (a-z, A-Z)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
            predefined_charsets[4] = {"Lowercase + Uppercase + Digits (a-z, A-Z, 0-9)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};
            predefined_charsets[5] = {"All common characters (a-z, A-Z, 0-9, !@#$%)", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%"};
            predefined_charsets[6] = {"Numbers only (0-9)", "0123456789"};
            simulate_typing("   Choose the charset used during table generation:", 10);
            for (const auto& pair : predefined_charsets) {
                // CONCATENATION FIX
                simulate_typing(std::string("   ") + pair.first + ". " + pair.second.first, 5);
            }
            simulate_typing("   Or enter 'C' for Custom charset", 10);
            simulate_typing(" [REDUCTION CHARSET CHOICE] Enter choice (1-" + std::to_string(predefined_charsets.size()) + " or C) > ", 15, false);
            std::string charset_choice_str_reduction;
            std::cin >> charset_choice_str_reduction;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string reduction_charset;
            std::transform(charset_choice_str_reduction.begin(), charset_choice_str_reduction.end(), charset_choice_str_reduction.begin(), ::toupper);
            if (charset_choice_str_reduction == "C") {
                simulate_typing(" [CUSTOM REDUCTION CHARSET] Enter the custom charset used for reduction (MUST MATCH GENERATION!) > ", 15, false);
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
                simulate_typing("Press Enter to return to main menu...", 10);
                std::cin.get();
                continue;
            }
            int chain_len, assumed_min_len, assumed_max_len;
            simulate_typing(" [CHAIN LENGTH] Enter the chain length used when generating the table (MUST MATCH GENERATION!) > ", 15, false);
            std::cin >> chain_len;
            simulate_typing(" [MIN PASSWORD LENGTH] Enter the minimum password length used during generation (MUST MATCH GENERATION!) > ", 15, false);
            std::cin >> assumed_min_len;
            simulate_typing(" [MAX PASSWORD LENGTH] Enter the maximum password length used during generation (MUST MATCH GENERATION!) > ", 15, false);
            std::cin >> assumed_max_len;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (chain_len <= 0 || assumed_min_len <= 0 || assumed_max_len < assumed_min_len) {
                std::cerr << CR_RED << "[ERROR] Invalid parameters for rainbow attack. Please check values." << RESET << std::endl;
                simulate_typing("Press Enter to return to main menu...", 10);
                std::cin.get();
                continue;
            }
            perform_rainbow_attack(input_hash_hex, rainbow_table_file_path, digest_algo, reduction_charset, chain_len, assumed_min_len, assumed_max_len);
        } else if (attack_choice == 4) {
            simulate_typing("\n>>> [MODULE ACTIVATED] Initiating Rainbow Table Generation Protocol...", 25, true, true);
            std::cout << CR_BLUE << "\n--- [RAINBOW TABLE GENERATION PARAMETERS] -------------" << RESET << std::endl;
            std::string output_filename = "rainbow.txt";
            simulate_typing(" [OUTPUT FILE] Enter desired output filename (e.g., my_rainbow_table.txt). Default: rainbow.txt > ", 15, false);
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
            simulate_typing("   Choose a predefined charset or enter your own:", 10);
            for (const auto& pair : predefined_charsets) {
                // CONCATENATION FIX
                simulate_typing(std::string("   ") + pair.first + ". " + pair.second.first, 5);
            }
            simulate_typing("   Or enter 'C' for Custom charset", 10);
            simulate_typing(" [CHARACTER SET CHOICE] Enter choice (1-" + std::to_string(predefined_charsets.size()) + " or C) > ", 15, false);
            std::string charset_choice_str;
            std::cin >> charset_choice_str;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string charset_gen_input;
            std::transform(charset_choice_str.begin(), charset_choice_str.end(), charset_choice_str.begin(), ::toupper);
            if (charset_choice_str == "C") {
                simulate_typing(" [CUSTOM CHARACTER SET] Enter your custom charset > ", 15, false);
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
                simulate_typing("Press Enter to return to main menu...", 10);
                std::cin.get();
                continue;
            }
            int min_len_gen, max_len_gen;
            simulate_typing(" [MIN LENGTH] Enter minimum password length for chains > ", 15, false);
            std::cin >> min_len_gen;
            simulate_typing(" [MAX LENGTH] Enter maximum password length for chains > ", 15, false);
            std::cin >> max_len_gen;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (min_len_gen <= 0 || max_len_gen < min_len_gen) {
                std::cerr << CR_RED << "[ERROR] Invalid length range. Min length > 0 and Max length >= Min length." << RESET << std::endl;
                simulate_typing("Press Enter to return to main menu...", 10);
                std::cin.get();
                continue;
            }
            long long num_chains_gen;
            simulate_typing(" [NUMBER OF CHAINS] Enter number of chains to generate (e.g., 1000000) > ", 15, false);
            std::cin >> num_chains_gen;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            int chain_length_gen;
            simulate_typing(" [CHAIN LENGTH] Enter length of each chain (e.g., 10000) > ", 15, false);
            std::cin >> chain_length_gen;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string hash_type_gen_str;
            simulate_typing(" [HASH TYPE] Enter hash type (MD5, SHA1, SHA256, SHA512). MUST MATCH TARGET! > ", 15, false);
            std::cin >> hash_type_gen_str;
            std::transform(hash_type_gen_str.begin(), hash_type_gen_str.end(), hash_type_gen_str.begin(), ::toupper);
            const EVP_MD* digest_algo_gen = get_openssl_digest_type(hash_type_gen_str);
            if (digest_algo_gen == nullptr) {
                std::cerr << CR_RED << "[ERROR] Unsupported hash type for generation. Supported: MD5, SHA1, SHA256, SHA384, SHA512." << RESET << std::endl;
                simulate_typing("Press Enter to return to main menu...", 10);
                std::cin.get();
                continue;
            }
            long double estimated_file_size_bytes = (long double)num_chains_gen * ((long double)min_len_gen + (EVP_MD_size(digest_algo_gen) * 2) + 2);
            simulate_typing("\n[INFO] Generating a table of " + format_attempts(num_chains_gen) + " chains, each " + std::to_string(chain_length_gen) + " steps long.", 10);
            std::cout << CR_MAGENTA << "       This will result in a file size of approximately "
                      << std::fixed << std::setprecision(2)
                      << estimated_file_size_bytes / (1024.0 * 1024.0 * 1024.0)
                      << " GB (estimation, can vary based on actual password lengths)." << RESET << std::endl;
            std::cout << CR_YELLOW << "       (Remember the actual size might be smaller if passwords are shorter than max_len_gen, or larger if max_len_gen is significantly used.)" << RESET << std::endl;
            char confirm_choice_gen;
            simulate_typing("Do you want to proceed with rainbow table generation? (y/n) > ", 15, false);
            std::cin >> confirm_choice_gen;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (tolower(confirm_choice_gen) != 'y') {
                simulate_typing("[INFO] Rainbow table generation cancelled by user.", 20);
            } else {
                generate_rainbow_table(full_output_path, digest_algo_gen, charset_gen_input, min_len_gen, max_len_gen, num_chains_gen, chain_length_gen);
            }
        }
        else {
            simulate_typing("[ERROR] Invalid attack choice. Please select 1, 2, 3, or 4.", 20, true, true);
        }
        std::cout << CR_BLUE << "\n==========================================================" << RESET << std::endl;
        if (g_hash_cracked_flag.load()) {
            simulate_typing("  [CRACK COMPLETE] Password found! Returning to main menu. ", 30, true, true);
        } else {
            simulate_typing("  [MODULE COMPLETE] Operation finished. Returning to main menu. ", 20);
        }
        std::cout << CR_BLUE << "==========================================================" << RESET << std::endl;
        simulate_typing("Press Enter to continue...", 10);
        std::cin.get();
    }
    EVP_cleanup();
    ERR_free_strings();
    return 0;
}
