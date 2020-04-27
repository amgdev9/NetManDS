/**
 * @file BerOid.cpp
 * @brief Class to hold an OBJECT IDENTIFIER
 */

// Includes C/C++
#include <stdlib.h>
#include <string.h>
#include <stdexcept>

// Own includes
#include "asn1/BerOid.h"

namespace NetMan {

/**
 * @brief Constructor for a BerOid
 * @param tagOptions Tag options (optional)
 * @param tag Tag (optional)
 */
BerOid::BerOid(u8 tagOptions, u32 tag) : BerField(tagOptions, tag){ }

/**
 * @brief Constructor for a BerOid
 * @param oidString String with the OID data
 * @param tagOptions Tag options (optional)
 * @param tag Tag (optional)
 */
BerOid::BerOid(const std::string &oidString, u8 tagOptions, u32 tag) : BerField(tagOptions, tag) {

	std::vector<u32> oid = std::vector<u32>();

	char oidTemp[256];
	memcpy(oidTemp, &oidString[0], oidString.length());
	oidTemp[oidString.length()] = '\0';

	char *oidToken = strtok(oidTemp, ".");
	u32 oidFirst = 0;
	bool firstFlag = false;

	while(oidToken) {

		if(!firstFlag) {
			oidFirst = atoi(oidToken) * 40;
			firstFlag = true;
		} else if(oid.size() == 0) {
			oidFirst += atoi(oidToken);
			oid.push_back(oidFirst);
		} else {
			oid.push_back(atoi(oidToken));
		}

		oidToken = strtok(NULL, ".");
	}

	try {
		this->parseOid(oid);
	} catch (const std::runtime_error &e) {
		throw;
	}
}

/**
 * @brief Constructor for a BerOid
 * @param oid Vector with the OID data
 * @param tagOptions Tag options (optional)
 * @param tag Tag (optional)
 */
BerOid::BerOid(const std::vector<u32> &oid, u8 tagOptions, u32 tag) : BerField(tagOptions, tag) {

    std::vector<u32> realOid = std::vector<u32>();
    if(oid.size() < 2) {
        throw std::runtime_error("OID length < 2");
    }
    realOid.push_back(oid[0] * 40 + oid[1]);
    for(u32 i = 2; i < oid.size(); i++) {
        realOid.push_back(oid[i]);
    }

	try {
		this->parseOid(realOid);
	} catch (const std::runtime_error &e) {
		throw;
	}
}

/**
 * @brief Parse an OID vector
 * @param oid OID to parse
 */
void BerOid::parseOid(std::vector<u32> &oid){

	this->oid = oid;

	if(this->oid.size() == 0) {
		throw std::runtime_error("OID is empty");
	}

	u32 length = 0;
	for(u32 i = 0; i < this->oid.size(); i++) {
		length ++;
		for(u8 j = 1; j <= 4; j++) {
			if((this->oid[i] >> (7 * j)) &0x7F) length ++;
			else j = 5;		// Exit the loop
		}
	}

	this->setLength(length);
}

/**
 * @brief Add a new element to the OID
 * @param n Element to be added
 */
void BerOid::addElement(u32 n) {
    this->oid.push_back(n);
    this->parseOid(this->oid);
}

/**
 * @brief Edit the last element of the OID
 * @param n New value for the last element
 */
void BerOid::editLastElement(u32 n) {
    this->oid[this->oid.size() - 1] = n;
    this->parseOid(this->oid);
}

/**
 * @brief Parse data to an output buffer
 * @param out Output buffer
 */
void BerOid::parseData(u8 **out) {

	u8 *data = *out;

	for(u32 i = 0; i < this->oid.size(); i++) {

		// Get number length
		u8 len = 1;
		for(u8 j = 1; j <= 4; j++) {
			if((this->oid[i] >> (7 * j)) &0x7F) len ++;
			else j = 5;			// Exit the loop
		}

		// Parse the number
		for(u8 j = 0; j < len; j++) {
			data[len - j - 1] = ((j > 0) << 7) | 		// Bit 7 up except the last iteration
								((this->oid[i] >> (7 * j)) &0x7F);
		}

		data += len;
	}

	// Update write pointer
	*out = data;
}

/**
 * @brief Destructor for a BerOid
 */
BerOid::~BerOid() { }

/**
 * @brief Decode an OBJECT IDENTIFIER
 * @param data Input buffer
 * @return A decoded BerOid
 */
std::shared_ptr<BerOid> BerOid::decode(u8** data) {

	// Skip tag
	*data += 1;

	try {
		// Get length
		u32 len = BerField::decodeLength(data);
		u8 *in = *data;

		// Fill a vector with oid data
		std::vector<u32> oid;
		u32 curOid = 0;
		for(u32 i = 0; i < len; i++) {

			curOid <<= 7;
			curOid |= in[i] &0x7F;

			if(!(in[i] &(1 << 7))) {
				oid.push_back(curOid);
				curOid = 0;
			}
		}

		*data += len;

		// Return the decoded OID
		std::shared_ptr<BerOid> berOid = std::make_shared<BerOid>();
		berOid->parseOid(oid);
		return berOid;
	} catch (const std::runtime_error &e) {
		throw;
	}
}

/**
 * @brief Print an OID
 * @return The OID representation
 */
std::string BerOid::print() {
	std::string text;
	char tmp[64];
	sprintf(tmp, "%ld.%ld", oid[0] / 40, oid[0] % 40);
	text.append(tmp);
	for(u32 i = 1; i < oid.size(); i++) {
		sprintf(tmp, ".%ld", oid[i]);
		text.append(tmp);
	}
	return text;
}

}
