/**
 * @file BerInteger.cpp
 * @brief Class to hold an Integer
 */

// Includes C/C++
#include <string.h>
#include <stdexcept>

// Own includes
#include "asn1/BerInteger.h"

namespace NetMan {

/**
 * @brief Constructor for a Integer field
 * @param value Value to be stored
 * @param len Length of the value to be stored
 * @param sign Signed integer?
 * @param tagOptions Specific tag scope (optional)
 * @param tag Tag value (optional)
 */
BerInteger::BerInteger(void *value, u8 len, bool sign, u8 tagOptions, u32 tag) : BerField(tagOptions, tag){

	if(len != 4 && len != 8) {
		throw std::runtime_error("Invalid integer length (4 or 8): " + std::to_string(len));
	}
	this->value = std::unique_ptr<u8>(new u8[len]);
	memcpy(this->value.get(), value, len);

	this->setLength(len);

	this->sign = sign;
}

/**
 * @brief Parse integer data
 * @param out Output buffer
 */
void BerInteger::parseData(u8 **out) {

	u8 *data = *out;
	u32 len = this->getLength();

	for(u32 i = 0; i < len; i++) {
		data[i] = this->value.get()[len - i - 1];
	}

	// Advance write pointer
	*out += len;
}

/**
 * @brief Decode an integer value
 * @param data Input buffer
 * @param len Length of the integer value
 * @param sign Interpret as signed integer?
 * @param dest Destination buffer
 * @param maxlen Maximum length of destination buffer
 */
void BerInteger::decodeIntegerValue(u8 **data, u8 len, bool sign, u8 *dest, u8 maxlen) {

	u8 *in = *data;

	// Check max length (32 or 64 bits)
	if(maxlen != 4 && maxlen != 8) {
		throw std::runtime_error("Invalid maximum length (4 or 8): " + std::to_string(maxlen));
	}

	// Check length
    if(len > maxlen) {
        throw std::runtime_error("Invalid length for INTEGER" + std::to_string(maxlen << 3) + ": " + std::to_string(len));
    }

	// Copy integer
	for(u8 i = 0; i < len; i++) {
		dest[i] = in[len - i - 1];
	}

	// Fill remaining bytes
	if((in[0] &(1 << 7)) && sign) {
		memset(&dest[len], 0xFF, maxlen - len);
	} else {
		memset(&dest[len], 0, maxlen - len);
	}

	// Update write pointer
	*data += len;
}

/**
 * @brief Decode an Integer
 * @param data Data pointer
 * @param sign Interpret as signed integer?
 * @return A decoded BerInteger
 */
std::shared_ptr<BerInteger> BerInteger::decode(u8 **data, bool sign) {

	// Skip tag
	*data += 1;
	
	try {
		// Get length
		u32 len = BerField::decodeLength(data);

		// Return decoded integer
		u8 maxlen = sizeof(s32);
		if(len > 4) maxlen = sizeof(s64);
		u8 *val = new u8[maxlen];
		BerInteger::decodeIntegerValue(data, len, sign, val, maxlen);
		std::shared_ptr<BerInteger> ptr = std::make_shared<BerInteger>(val, maxlen, sign);
		delete [] val;
		return ptr;
	} catch (const std::runtime_error &e) {
		throw e;
	}
}

/**
 * @brief Print an Integer
 */
void BerInteger::print() {
	FILE *f = fopen("sdmc:/log.txt", "a+");
	fprintf(f, "INTEGER: ");
	if(sign) {
		if(this->getLength() == sizeof(s64)) {
			fprintf(f, "%lld", *(s64*)this->value.get());
		} else {
			fprintf(f, "%ld", *(s32*)this->value.get());
		}
	} else {
		if(this->getLength() == sizeof(u64)) {
			fprintf(f, "%llu", *(u64*)this->value.get());
		} else {
			fprintf(f, "%lu", *(u32*)this->value.get());
		}
	}
	fprintf(f, "\n");
	fclose(f);
}

/**
 * @brief Get an Integer as u32
 * @return The integer as u32
 */
u32 BerInteger::getValueU32() {
	if(this->sign) throw std::runtime_error("Not unsigned");
	if(this->getLength() == 4) return *(u32*)this->value.get();
	return *(u64*)this->value.get();
}

/**
 * @brief Get an Integer as s32
 * @return The integer as s32
 */
s32 BerInteger::getValueS32() {
	if(!this->sign) throw std::runtime_error("Not signed");
	if(this->getLength() == 4) return *(s32*)this->value.get();
	return *(s64*)this->value.get();
}

/**
 * @brief Get an Integer as u64
 * @return The integer as u64
 */
u64 BerInteger::getValueU64() {
	if(this->sign) throw std::runtime_error("Not unsigned");
	if(this->getLength() == 4) return *(u32*)this->value.get();
	return *(u64*)this->value.get();
}

/**
 * @brief Get an Integer as s64
 * @return The integer as s64
 */
s64 BerInteger::getValueS64() {
	if(!this->sign) throw std::runtime_error("Not signed");
	if(this->getLength() == 4) return *(s32*)this->value.get();
	return *(s64*)this->value.get();
}

}
