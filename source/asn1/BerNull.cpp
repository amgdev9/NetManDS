/**
 * @file BerNull.cpp
 * @brief Class to hold a NULL value
 */

// Includes C/C++
#include <stdexcept>

// Own includes
#include "asn1/BerNull.h"

namespace NetMan {

/**
 * @brief Constructor for a BerNull
 * @param tagOptions Tag options (optional)
 * @param tag Tag (optional)
 */
BerNull::BerNull(u8 tagOptions, u32 tag) : BerField(tagOptions, tag) { }

/**
 * @brief Parse data to an output buffer
 * @param out Output buffer
 */
void BerNull::parseData(u8 **out) { }

/**
 * @brief Decode a NULL value
 * @param data Output buffer
 * @return A decoded NULL value
 */
std::shared_ptr<BerNull> BerNull::decode(u8 **data) {

	// Check a length of zero
	u8 *in = *data;
	if(in[1] != 0) {
        throw std::runtime_error("Not a NULL with zero length");
    }

	// Update write pointer
	*data += 2;

	// Return decoded data
	return std::make_shared<BerNull>(in[0] >> 5, in[0] &0x1F);
}

/**
 * @brief Print a Null
 */
void BerNull::print() {
	FILE *f = fopen("log.txt", "a+");
	fprintf(f, "NULL\n");
	fclose(f);
}

/**
 * @brief Destructor for a BerNull
 */
BerNull::~BerNull() { }

}
