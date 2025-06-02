#ifndef HASH_RECON_H
#define HASH_RECON_H

#include <string>

bool estNumerique(const std::string& str);
bool estAlphabetique(const std::string& str);
bool estAlphanumerique(const std::string& str);
bool estHexadecimal(const std::string& str);
void analyserTypeCaracteresHachage(const std::string& hash_hex);

#endif // HASH_RECON_H
