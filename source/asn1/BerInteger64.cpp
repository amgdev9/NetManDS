/**
 * @file BerInteger64.cpp
 * @brief Class to hold an Integer64
 */

// Includes C/C++
#include <stdexcept>

// Own includes
#include "asn1/BerInteger.h"
#include "asn1/BerInteger64.h"

namespace NetMan {

/**
 * @brief Constructor for a Integer64 field
 * @param value Value to be stored
 * @param tagOptions Specific tag scope (optional)
 * @param tag Tag value (optional)
 */
BerInteger64::BerInteger64(s64 value, u8 tagOptions, u32 tag) : BerField(tagOptions, tag){

	this->value = value;

	u8 length = 1;													// 8 bits by default
    for(u8 i = 1; i < 8; i++) {
        if(value < INTEGER_MIN(8 * i) || value > INTEGER_MAX(8 * i)) {
            length ++;
        } else {
            i = 8;      // Exit the loop
        }
    }

	this->setLength(length);
}

/**
 * @brief Parse integer data
 * @param out Output buffer
 */
void BerInteger64::parseData(u8 **out) {

	u8 *data = *out;
	u32 len = this->getLength();

    u8 *v = (u8*)&this->value;
	for(u8 i = 0; i < len; i++) {
		data[i] = v[len - i - 1];
	}

	// Advance write pointer
	*out += this->getLength();
}

/**
 * @brief Decode an integer value
 * @param data Input buffer
 * @param len Length of the integer value
 * @return The decoded integer value
 */
s64 BerInteger64::decodeIntegerValue(u8 **data, u8 len) {

	u8 *in = *data;
	s64 value = 0;

    if(len > 8) {
        throw std::runtime_error("Invalid length for INTEGER64: " + std::to_string(len));
    }

	u8 *v = (u8*)&value;
	for(u8 i = 0; i < len; i++) {
		v[i] = in[len - i - 1];
	}

    // Fill remaning bytes, if negative
	if(in[0] &(1 << 7)) {
        for(u8 i = len; i < 8; i++) {
            v[i] = 0xFF;
        }
    }

	// Update write pointer
	*data += len;

	// Return decoded value
	return value;
}

/**
 * @brief Decode an Integer
 * @param data Data pointer
 * @return A decoded BerInteger
 */
std::shared_ptr<BerInteger64> BerInteger64::decode(u8 **data) {

	// Skip tag
	*data += 1;
	
	try {
		// Get length
		u32 len = BerField::decodeLength(data);

		// Return decoded integer
		return std::make_shared<BerInteger64>(BerInteger64::decodeIntegerValue(data, len));
	} catch (const std::runtime_error &e) {
		throw e;
	}
}

/**
 * @brief Print an Integer
 */
void BerInteger64::print() {
	FILE *f = fopen("sdmc:/log.txt", "a+");
	fprintf(f, "INTEGER: %lld\n", this->value);
	fclose(f);
}

/**
 * @brief Get an Integer value
 * @return The integer value
 */
s64 BerInteger64::getValue() {
	return this->value;
}

}
