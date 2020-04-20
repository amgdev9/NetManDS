/**
 * @file BerOctetString.cpp
 * @brief 
 */

// Includes C/C++
#include <string.h>
#include <stdexcept>

// Own includes
#include "asn1/BerOctetString.h"

namespace NetMan {

/**
 * @brief Constructor for a BerOctetString
 * @param value String to store
 * @param tagOptions Tag options (optional)
 * @param tag Tag (optional)
 */
BerOctetString::BerOctetString(const std::string &value, u8 tagOptions, u32 tag) : BerField(tagOptions, tag){

	try {
		this->text = value;
	} catch (const std::bad_alloc &e) {
		throw e;
	}

	this->setLength(value.length());
}

/**
 * @brief Parse octet string data
 * @param out Destination address
 * @note Only primitive BER encoding is supported for this type
 */
void BerOctetString::parseData(u8 **out) {
	memcpy(*out, &this->text[0], this->getLength());
	*out += this->getLength();
}

/**
 * @brief Destructor for a BerOctetString
 */
BerOctetString::~BerOctetString() { }

/**
 * @brief Decode an OCTET STRING from a buffer
 * @param data Input buffer
 * @return A decoded OCTET STRING
 */
std::shared_ptr<BerOctetString> BerOctetString::decode(u8 **data) {

	// Skip tag
	*data += 1;

	try {
		// Get length
		u32 len = BerField::decodeLength(data);

		// Retrieve value
		u8 *in = *data;
		std::string str;
		str.resize(len);
		memcpy(&str[0], in, len);

		// Update write pointer
		*data += len;

		// Return decoded string
		return std::make_shared<BerOctetString>(str);
	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Print an Octet String
 * @return The string representation
 */
std::string BerOctetString::print() {
	return text;
}

/**
 * @brief Get octet string value
 * @return Octet string value
 */
std::string &BerOctetString::getValue() {
	return this->text;
}

}
