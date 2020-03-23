/**
 * @file BerField.cpp
 * @brief BER field
 */

// Includes C/C++
#include <stdexcept>

// Own includes
#include "asn1/BerField.h"
#include "asn1/BerInteger.h"
#include "asn1/BerNull.h"
#include "asn1/BerOctetString.h"
#include "asn1/BerOid.h"
#include "snmp/Snmpv2Pdu.h"

namespace NetMan {

/**
 * @brief Constructor for a BER Field
 * @param tagOptions Tag class and if it is a structured field
 * @param tag Tag ID
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
			if((tag >> (7 * i)) &0x7F) this->tagSize ++;
		}
	}
}

/**
 * @brief Set field length
 * @param length Field length
 */
void BerField::setLength(u32 length) {

	this->length = length;

	// Calculate length field size
	this->lengthSize = 1;
	if(length > 127){
		this->lengthSize ++;
		for(u8 i = 1; i <= 3; i++) {
			if((length >> (i << 3)) &0xFF) this->lengthSize ++;
		}
	}
}

/**
 * @brief Parse the field tag
 * @param out Output buffer
 */
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

/**
 * @brief Parse the field length
 * @param out Output buffer
 */
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

/**
 * @brief Retrieve field total size
 * @return Field total size, including tag, length and payload size
 */
u32 BerField::getTotalSize() {
	return (this->tagSize + this->lengthSize + this->length);
}

/**
 * @brief Decode a field length
 * @param data Input buffer
 * @return Decoded field length
 * @note Only short and long length are supported
 */
u32 BerField::decodeLength(u8 **data) {

	u8 *in = *data;
	u32 len = in[0];
	*data += 1;
	if(len &(1 << 7)) {			// Long length
		try {
			u32 l;
			BerInteger::decodeIntegerValue(data, len &0x7F, false, (u8*)&l, sizeof(u32));
			len = l;
		} catch(const std::runtime_error &e) {
			throw e;
		}
	}

	return len;
}

/**
 * @brief Decode a BER field
 * @param data Input buffer
 * @return A decoded BER field
 */
std::shared_ptr<BerField> BerField::decode(u8 **data) {

	try {
		switch((*data)[0]) {
			// SNMPv1+
			case (BER_TAG_INTEGER | BER_TAGCLASS_INTEGER):
				return BerInteger::decode(data, true);
			case (SNMPV1_TAG_COUNTER | SNMPV1_TAGCLASS_COUNTER):
			case (SNMPV1_TAG_GAUGE | SNMPV1_TAGCLASS_GAUGE):
			case (SNMPV1_TAG_TIMETICKS | SNMPV1_TAGCLASS_TIMETICKS):
				return BerInteger::decode(data, false);
			case (BER_TAG_NULL | BER_TAGCLASS_NULL):
				return BerNull::decode(data);
			case (BER_TAG_OCTETSTRING | BER_TAGCLASS_OCTETSTRING):
			case (SNMPV1_TAG_NETWORKADDRESS | SNMPV1_TAGCLASS_NETWORKADDRESS):
			case (SNMPV1_TAG_OPAQUE | SNMPV1_TAGCLASS_OPAQUE):
				return BerOctetString::decode(data);
			case (BER_TAG_OID | BER_TAGCLASS_OID):
				return BerOid::decode(data);
			// SNMPv2+
			case (SNMPV2_TAG_NOSUCHOBJECT | SNMPV2_TAGCLASS_VALUE_EXCEPTION):
			case (SNMPV2_TAG_NOSUCHINSTANCE | SNMPV2_TAGCLASS_VALUE_EXCEPTION):
			case (SNMPV2_TAG_ENDOFMIBVIEW | SNMPV2_TAGCLASS_VALUE_EXCEPTION):
				return BerNull::decode(data);
			case (SNMPV2_TAG_COUNTER64 | SNMPV2_TAGCLASS_COUNTER64):
				return BerInteger::decode(data, false);
		}
	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}

	throw std::runtime_error("Unknown BER field");
	return nullptr;
}

}
