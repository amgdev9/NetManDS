/**
 * @file BerField.cpp
 * @brief 
 */

#include "asn1/BerField.h"

namespace NetMan {

/**
 * @brief Constructor for a BER Field
 * @param tagOptions Tag class and if it is a structured field
 * @param tag Tag ID
 * @param length Length of the data (undefined length is not supported)
 */
BerField::BerField(u8 tagOptions, u32 tag) {

	this->tagOptions = tagOptions;
	this->tag = tag;
	this->length = 0;
	this->lengthSize = 1;

	// Calculate tag size
	this->tagSize = 1;
	if(tag >= 31){
		this->tagSize ++;
		for(u8 i = 1; i <= 4; i++) {
			if(tag >> (7 * i)) this->tagSize ++;
		}
	}
}

void BerField::setLength(u32 length) {

	this->length = length;

	// Calculate length field's size
	this->lengthSize = 1;
	if(length > 127){
		this->lengthSize ++;
		for(u8 i = 1; i <= 3; i++) {
			if(length >> (i << 3)) this->lengthSize ++;
		}
	}
}

void BerField::parseTag(u8 **out) {

	u8 *data = *out;
	if(this->tag < 31) {							// Short tag
		data[0] = this->tagOptions | this->tag;
	} else {										// Long tag
		data[0] = this->tagOptions | 0b11111;
		for(u8 i = 1; i < this->tagSize; i++) {
			data[this->tagSize - i] = ((i < (this->tagSize - 1)) << 7) | 		// Bit 7 up except the last iteration
									  ((this->tag >> (7 * (i - 1))) &0x7F);
		}
	}

	// Advance the write pointer
	*out += this->tagSize;
}

void BerField::parseLength(u8 **out) {

	u8 *data = *out;
	if(this->length <= 127) {							// Short size
		data[0] = this->length;
	} else {											// Long size
		data[0] = (1 << 7) | (this->lengthSize - 1);	// Number of bytes which compound the length
		for(u8 i = 1; i < this->lengthSize; i++) {
			data[this->lengthSize - i] = (this->length >> ((i - 1) << 3)) &0xFF;
		}
	}

	// Advance the write pointer
	*out += this->lengthSize;
}

u32 BerField::getTotalSize() {
	return (this->tagSize + this->lengthSize + this->length);
}

static BerField *BerField::decode(u8 **data) {
	// TODO Decode according to tag
}

}
