/**
 * @file BerSequence.cpp
 * @brief Class to store a SEQUENCE/SEQUENCE OF
 */

// Includes C/C++
#include <stdexcept>

// Own includes
#include "asn1/BerSequence.h"

namespace NetMan {

/**
 * @brief Constructor for a BER Sequence
 * @param tagOptions Tag options (optional)
 * @param tag Tag (optional)
 * @note Also valid for a BER Sequence of
 */
BerSequence::BerSequence(u8 tagOptions, u32 tag) : BerField(tagOptions, tag){

	children = std::vector<std::shared_ptr<BerField>>();
}

/**
 * @brief Add a child to a BerSequence
 * @param child Child to add
 */
void BerSequence::addChild(std::shared_ptr<BerField> child) {

	children.push_back(child);
}

/**
 * @brief Get total size of a BerSequence
 * @return Total size
 */
u32 BerSequence::getTotalSize() {

	u32 length = 0;
	for(u32 i = 0; i < children.size(); i++) {
		length += children[i]->getTotalSize();
	}

	this->setLength(length);

	return BerField::getTotalSize();
}

/**
 * @brief Parse sequence data
 * @param out Output buffer
 */
void BerSequence::parseData(u8 **out) {

	for(u32 i = 0; i < children.size(); i++) {
		children[i]->parseTag(out);
		children[i]->parseLength(out);
		children[i]->parseData(out);
	}
}

/**
 * @brief Destructor for a BerSequence
 */
BerSequence::~BerSequence() { }

/**
 * @brief Decode a BerSequence
 * @param data Input buffer
 * @param tagOptions Tag options (optional)
 * @param tag Tag (optional)
 * @return Sequence length
 */
u32 BerSequence::decode(u8 **data, u8 tagOptions, u32 tag, u8 *decodedTag) {
	
	u8 *in = *data;

	// Check tag
	if(in[0] != (tagOptions | tag) && tag != BER_TAG_ANY) {
		throw std::runtime_error("Not a SEQUENCE");
	}

	// Parse tag
	if(decodedTag) *decodedTag = in[0] &0x1F;

	// Return length
	*data += 1;
	try {
		return BerField::decodeLength(data);
	} catch (const std::runtime_error &e) {
		throw;
	}
}

/**
 * @brief Get a child
 * @param i Child index
 * @return Desired child
 */
std::shared_ptr<BerField> BerSequence::getChild(u16 i) {
	try {
		return this->children[i];
	} catch (const std::out_of_range &e) {
		throw;
	}
}

/**
 * @brief Print a SEQUENCE
 */
void BerSequence::print() {
	FILE *f = fopen("log.txt", "a+");
	fprintf(f, "SEQUENCE\n");
	fclose(f);
}

}
