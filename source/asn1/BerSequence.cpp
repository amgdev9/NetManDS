/**
 * @file BerSequence.cpp
 * @brief 
 */

#include "asn1/BerSequence.h"

namespace NetMan {

/**
 * @brief Constructor for a BER Sequence
 * @note Also valid for a BER Sequence of
 */
BerSequence::BerSequence(u8 tagOptions, u32 tag) : BerField(tagOptions, tag){

	children = std::vector<BerField*>();
}

void BerSequence::addChild(BerField *child) {

	children.push_back(child);
}

u32 BerSequence::getTotalSize() {

	u32 length = 0;
	for(u32 i = 0; i < children.size(); i++) {
		length += children[i]->getTotalSize();
	}

	this->setLength(length);

	return BerField::getTotalSize();
}

void BerSequence::parseData(u8 **out) {

	for(u32 i = 0; i < children.size(); i++) {
		children[i]->parseTag(out);
		children[i]->parseLength(out);
		children[i]->parseData(out);
	}
}

BerSequence::~BerSequence() {

	for(u32 i = 0; i < children.size(); i++) {
		delete children[i];
	}
}

}
