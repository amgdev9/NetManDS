/**
 * @file BerOid.cpp
 * @brief 
 */

#include <stdlib.h>
#include <string.h>
#include <stdexcept>

#include "asn1/BerOid.h"

namespace NetMan {

BerOid::BerOid(const char *oidString, u8 tagOptions, u32 tag) : BerField(tagOptions, tag){

	this->oid = std::vector<u32>();

	char oidTemp[256];
	strcpy(oidTemp, oidString);

	char *oidToken = strtok(oidTemp, ".");
	u32 oidFirst = 0;

	while(oidToken) {

		if(!oidFirst) {
			oidFirst = atoi(oidToken) * 40;
		} else if(this->oid.size() == 0) {
			oidFirst += atoi(oidToken);
			this->oid.push_back(oidFirst);
		} else {
			this->oid.push_back(atoi(oidToken));
		}

		oidToken = strtok(NULL, ".");
	}

	if(this->oid.size() < 2) {
		throw std::runtime_error("OID is < 2 elements");
	}

	u32 length = 0;
	for(u32 i = 0; i < this->oid.size(); i++) {
		length ++;
		for(u8 j = 1; j <= 4; j++) {
			if(this->oid[i] >> (7 * j)) length ++;
		}
	}

	this->setLength(length);
}

void BerOid::parseData(u8 **out) {

	u8 *data = *out;

	for(u32 i = 0; i < this->oid.size(); i++) {

		u8 len = 1;
		for(u8 j = 1; j <= 4; j++) {
			if(this->oid[i] >> (7 * j)) len ++;
			else j = 5;			// Exit the loop
		}

		for(u8 j = 0; j < len; j++) {
			data[len - j - 1] = ((j < (len - 1)) << 7) | 		// Bit 7 up except the last iteration
								((this->oid[i] >> (7 * j)) &0x7F);
		}

		data += len;
	}

	// Update write pointer
	*out = data;
}

BerOid::~BerOid() {

}

static BerOid *BerOid::decode(u8** data) {
	// TODO
}

}
