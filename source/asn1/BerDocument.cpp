/**
 * @file BerDocument.cpp
 * @brief 
 */

#include "asn1/BerDocument.h"

namespace NetMan {

BerDocument::BerDocument() {
	this->fields = std::vector<BerField*>();
}

void BerDocument::addField(BerField *field) {
	this->fields.push_back(field);
}

const std::vector<BerField*> &BerDocument::getFields() {
	return this->fields;
}

BerDocument::~BerDocument() {

	for(u32 i = 0; i < this->fields.size(); i++) {
		delete fields[i];
	}
}

}
