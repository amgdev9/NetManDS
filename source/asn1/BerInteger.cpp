/**
 * @file BerInteger.cpp
 * @brief 
 */

#include "asn1/BerInteger.h"

namespace NetMan {

BerInteger::BerInteger(s32 value) : BerField(BER_TAGCLASS_INTEGER, BER_TAG_INTEGER){

	this->value = value;

	u8 length = 1;						// 8 bits by default
	if(value > 0xFF){					// 16 bits
		length = 2;
	}else if(value > 0xFFFF){			// 24 bits
		length = 3;
	}else if(value > 0xFFFFFF){			// 32 bits
		length = 4;
	}

	this->setLength(length);
}

void BerInteger::parseData(u8 **out) {

	u8 *data = *out;

	if(this->value > 0xFFFFFF){					// 32 bits
		data[0] = (this->value >> 24) &0xFF;
		data[1] = (this->value >> 16) &0xFF;
		data[2] = (this->value >> 8) &0xFF;
		data[3] = this->value &0xFF;
	}else if(this->value > 0xFFFF){				// 24 bits
		data[0] = (this->value >> 16) &0xFF;
		data[1] = (this->value >> 8) &0xFF;
		data[2] = this->value &0xFF;
	}else if(this->value > 0xFF){					// 16 bits
		data[0] = (this->value >> 8) &0xFF;
		data[1] = this->value &0xFF;
	} else {								// 8 bits
		data[0] = this->value &0xFF;
	}

	// Advance write pointer
	*out += this->getLength();
}

}
