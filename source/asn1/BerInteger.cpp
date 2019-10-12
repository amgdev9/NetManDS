/**
 * @file BerInteger.cpp
 * @brief 
 */

#include "asn1/BerInteger.h"

namespace NetMan {

/**
 * @brief Constructor for a Integer field
 * @param value Value to be stored
 */
BerInteger::BerInteger(s32 value, u8 tagOptions, u32 tag) : BerField(tagOptions, tag){

	this->value = value;

	u8 length = 1;													// 8 bits by default
	if(value < INTEGER_MIN(24) || value > INTEGER_MAX(24)) {		// 32 bits
		length = 4;
	} else if(value < INTEGER_MIN(16) || value > INTEGER_MAX(16)) {	// 24 bits
		length = 3;
	} else if(value < INTEGER_MIN(8) || value > INTEGER_MAX(8)) {	// 16 bits
		length = 2;
	}

	this->setLength(length);
}

void BerInteger::parseData(u8 **out) {

	u8 *data = *out;

	switch(this->getLength()) {
		case 2:
			((s16*)data)[0] = (s16)this->value;
			break;
		case 3:
			{
				u8 *v = (u8*)&this->value;
				data[0] = v[0];
				data[1] = v[1];
				data[2] = v[2];
			}
			break;
		case 4:
			((s32*)data)[0] = this->value;
			break;
		default:
			((s8*)data)[0] = (s8)this->value;
	}

	// Advance write pointer
	*out += this->getLength();
}

}
