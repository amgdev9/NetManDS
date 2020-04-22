/**
 * @file SyslogPdu.cpp
 * @brief Syslog PDU processing
 */

// Includes C/C++
#include <string.h>

// Own includes
#include "syslog/SyslogPdu.h"
#include "Utils.h"

namespace NetMan {

/**
 * @brief Constructor for a SyslogPdu
 */
SyslogPdu::SyslogPdu() {
    this->elements = std::vector<SyslogElement>();
}

/**
 * @brief Destructor for a SyslogPdu
 */
SyslogPdu::~SyslogPdu() { }

/**
 * @brief Decode a syslog PDU
 * @param data      Data buffer
 * @param dataSize  Data size
 */
void SyslogPdu::decodeLog(std::shared_ptr<u8> data, u32 dataSize) {

	try {
		u8 *ptr = data.get();

		// Read priority
		checkCharacter(&ptr, SYSLOG_LT);
		this->priority = getNumber(&ptr, 1, 3, 0, 191, false);
#ifdef SYSLOG_DEBUG
		fprintf(f, "\nPriority: %d\n", this->priority);
		fflush(f);
#endif
		checkCharacter(&ptr, SYSLOG_GT);

		// Read version
		this->version = getNumber(&ptr, 1, 1, 1, 9, true);
		this->subversion = getNumber(&ptr, 0, 2, 0, 99, false);
#ifdef SYSLOG_DEBUG
		fprintf(f, "Version: %d.%d\n", this->version, this->subversion);
		fflush(f);
#endif
		checkCharacter(&ptr, SYSLOG_SP);

		// Read timestamp
        memset(&this->timeStamp, 0, sizeof(SyslogTimeStamp));
		if(checkAlternative(&ptr, SYSLOG_NILVALUE)) {
			this->timeStamp.year = getNumber(&ptr, 4, 4, 0, 9999, false);
			checkCharacter(&ptr, SYSLOG_DASH);
			this->timeStamp.month = getNumber(&ptr, 2, 2, 1, 12, false);
			checkCharacter(&ptr, SYSLOG_DASH);
			this->timeStamp.day = getNumber(&ptr, 2, 2, 1, 31, false);
			checkCharacter(&ptr, SYSLOG_T);
#ifdef SYSLOG_DEBUG
			fprintf(f, "Date: %d-%d-%d\n", this->timeStamp.day, this->timeStamp.month, this->timeStamp.year);
			fflush(f);
#endif
			this->timeStamp.hour = getNumber(&ptr, 2, 2, 0, 23, false);
			checkCharacter(&ptr, SYSLOG_COLON);
			this->timeStamp.minute = getNumber(&ptr, 2, 2, 0, 59, false);
			checkCharacter(&ptr, SYSLOG_COLON);
			this->timeStamp.second = getNumber(&ptr, 2, 2, 0, 59, false);
			if(!checkAlternative(&ptr, SYSLOG_DOT)) {
				this->timeStamp.fracsecond = getNumber(&ptr, 1, 6, 0, 999999, false);
			} else {
				this->timeStamp.fracsecond = 0;
			}
#ifdef SYSLOG_DEBUG
			fprintf(f, "Time: %d:%d:%d.%ld\n", this->timeStamp.hour, this->timeStamp.minute, this->timeStamp.second, this->timeStamp.fracsecond);
			fflush(f);
#endif
			// Read time shift
			if(checkAlternative(&ptr, SYSLOG_Z)) {
				char plusminus = ptr[0];
				ptr ++;
				if(plusminus == SYSLOG_PLUS) this->timeStamp.hour_offset = 1;
				else this->timeStamp.hour_offset = -1;
				this->timeStamp.hour_offset *= getNumber(&ptr, 2, 2, 0, 23, false);
				checkCharacter(&ptr, SYSLOG_COLON);
				this->timeStamp.minute_offset = getNumber(&ptr, 2, 2, 0, 59, false);
			} else {
				this->timeStamp.hour_offset = 0;
				this->timeStamp.minute_offset = 0;
			}
		} else {
			memset(&this->timeStamp, 0, sizeof(SyslogTimeStamp));
		}
#ifdef SYSLOG_DEBUG
		fprintf(f, "Time offset: %d:%d\n", this->timeStamp.hour_offset, this->timeStamp.minute_offset);
		fflush(f);
#endif
		checkCharacter(&ptr, SYSLOG_SP);

		// Read some strings
		this->hostname = getString(&ptr, 1, 255, " ");
#ifdef SYSLOG_DEBUG
		fprintf(f, "Hostname: %s\n", this->hostname.c_str());
		fflush(f);
#endif
		this->appname = getString(&ptr, 1, 48, " ");
#ifdef SYSLOG_DEBUG
		fprintf(f, "Appname: %s\n", this->appname.c_str());
		fflush(f);
#endif
		this->procid = getString(&ptr, 1, 128, " ");
#ifdef SYSLOG_DEBUG
		fprintf(f, "ProcID: %s\n", this->procid.c_str());
		fflush(f);
#endif
		this->msgid = getString(&ptr, 1, 32, " ");
#ifdef SYSLOG_DEBUG
		fprintf(f, "MsgID: %s\n", this->msgid.c_str());
		fflush(f);
#endif

		// Read structured data
		this->elements.clear();
		if(checkAlternative(&ptr, SYSLOG_NILVALUE)) {

			// Check if there are more elements
			while(ptr[0] == SYSLOG_OPENBRACKET) {

				// Read an element
				SyslogElement element;
				checkCharacter(&ptr, SYSLOG_OPENBRACKET);
				element.name = getString(&ptr, 1, 32, "= ]\"");
#ifdef SYSLOG_DEBUG
				fprintf(f, "ElementName: %s\n", element.name.c_str());
				fflush(f);
#endif

				// Read params
				ptr --;		// Don't skip last string's separator
				element.params = std::vector<SyslogElementParam>();
				while(ptr[0] != SYSLOG_CLOSEBRACKET) {
					SyslogElementParam param;
					checkCharacter(&ptr, SYSLOG_SP);
					param.name = getString(&ptr, 1, 32, "=");
#ifdef SYSLOG_DEBUG
					fprintf(f, "ParamName: %s\n", param.name.c_str());
					fflush(f);
#endif
					checkCharacter(&ptr, SYSLOG_QUOT);
					param.value = getString(&ptr, 0, SYSLOG_MAX_STRING_LENGTH, "\"", true, 0, 0xFF);
#ifdef SYSLOG_DEBUG
					fprintf(f, "ParamValue: %s\n", param.value.c_str());
					fflush(f);
#endif
					element.params.push_back(param);
				}

				checkCharacter(&ptr, SYSLOG_CLOSEBRACKET);
				this->elements.push_back(element);
			}
		}

		// Read optional message
		if((u32)(ptr - data.get()) < dataSize) {
			checkCharacter(&ptr, SYSLOG_SP);

			if(ptr[0] == 0xEF && ptr[1] == 0xBB && ptr[2] == 0xBF) {
				ptr += 3;
				u16 messageLength = dataSize - (ptr - data.get());
				this->message = getString(&ptr, 0, messageLength, "", false, 0, 0xFF);
			} else {
				u16 messageLength = dataSize - (ptr - data.get());
				this->message = getString(&ptr, 0, messageLength, "", false, 0, 0xFF);
			}

#ifdef SYSLOG_DEBUG
			fprintf(f, "Message: %s\n", this->message.c_str());
			fflush(f);
#endif
		} else {
            this->message = "";
        }
	} catch (const std::runtime_error &e) {
#ifdef SYSLOG_DEBUG
		fprintf(f, e.what());
		fclose(f);
#endif
		throw;
	} catch (const std::bad_alloc &e) {
#ifdef SYSLOG_DEBUG
		fprintf(f, e.what());
		fclose(f);
#endif
		throw;
	}

#ifdef SYSLOG_DEBUG
	fclose(f);
#endif
}

/**
 * @brief Receive a syslog PDU from a TCP stream
 * @param sock  TCP socket to use for reception
 */
void SyslogPdu::recvLog(std::shared_ptr<TcpSocket> sock) {

	// If using TCP, read first the "packet" length from the flow
	u32 packetSize = SYSLOG_MAX_PDU_SIZE;
	char n = 0;
	char sizeString[8];
	u8 sizeIndex = 0;
	do {
		sock->recvData(&n, 1);
		sizeString[sizeIndex++] = n;
	} while(n >= '0' && n <= '9');
	sizeString[sizeIndex] = '\0';
	packetSize = atoi(sizeString);

	try {

		// Create recv buffer
		std::shared_ptr<u8> data(new u8[packetSize]);

		// Receive packet data
		u32 dataSize = sock->recvData(data.get(), packetSize);

		// Decode the log PDU
		this->decodeLog(data, dataSize);

	} catch (const std::bad_alloc &e) {
		throw;
	} catch (const std::runtime_error &e) {
		throw;
	}
}

/**
 * @brief Receive a log message from a UDP socket
 * @param sock	UDP Socket to use for reception
 */
void SyslogPdu::recvLog(std::shared_ptr<UdpSocket> sock) {

    try {

		// Create recv buffer
		std::shared_ptr<u8> data(new u8[SYSLOG_MAX_PDU_SIZE]);

		// Receive packet data
		u32 dataSize = sock->recvPacket(data.get(), SYSLOG_MAX_PDU_SIZE);
		
		// Decode PDU data
		this->decodeLog(data, dataSize);

	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Check if the next character is not the specified one
 * @param ptr	Buffer pointer
 * @param c 	Specified character
 * @return Whether the next character in the buffer is the specified one
 */
bool SyslogPdu::checkAlternative(u8 **ptr, char c) {

	if((char)**ptr != c) {
		return true;
	}

	*ptr += 1;
	return false;
}

/**
 * @brief Check if the next character is the specified one
 * @param ptr	Buffer pointer
 * @param c		Specified character
 */
void SyslogPdu::checkCharacter(u8 **ptr, char c) {

	if((char)**ptr != c) {
		throw std::runtime_error("No match: " + std::string(1, c) + " vs " + std::string(1, (char)**ptr));
	}

	*ptr += 1;
}

/**
 * @brief Retrieve a number from the buffer
 * @param ptr			Buffer pointer
 * @param mindigits		Minimum number of digits
 * @param maxdigits		Maximum number of digits
 * @param minrange		Minimum range of values
 * @param maxrange		Maximum range of values
 * @param nonzero		The number can be zero?
 * @return The retrieved number
 */
u32 SyslogPdu::getNumber(u8 **ptr, u8 mindigits, u8 maxdigits, u32 minrange, u32 maxrange, bool nonzero) {

	// Create a temp string
	char digitstring[SYSLOG_MAX_DIGITS + 1];
	memset(digitstring, 0, SYSLOG_MAX_DIGITS + 1);
	char *data = (char*)*ptr;

	// Copy the number
	u8 count;
	for(count = 0; count < maxdigits; count++) {
		if(!nonzero && (data[count] < '0' || data[count] > '9')) break;
		if(nonzero && (data[count] < '1' || data[count] > '9')) break;
		digitstring[count] = data[count];
	}

	// Check the minimum digits
	if(count < mindigits) {
		throw std::runtime_error("Less than mindigits");
	}

	// Convert the string to an integer
	*ptr += count;
	u32 number = strtol(digitstring, NULL, 10);

	// Check number range
	if(number < minrange || number > maxrange) {
		throw std::runtime_error("Number out of range");
	}

	// Return the number
	return number;
}

/**
 * @brief Get a string from a buffer
 * @param ptr		Buffer pointer
 * @param minlength	Minimum string length
 * @param maxlength	Maximum string length
 * @param separator	List of characters indicating end og the string
 * @param escape	Allow escaped characters?
 * @param mincode	Minimum ASCII code
 * @param maxcode	Maximum ASCII code
 * @return The retrieved string
 */
std::string SyslogPdu::getString(u8 **ptr, u16 minlength, u16 maxlength, const char *separator, bool escape, u8 mincode, u8 maxcode) {

	// Variables
	u16 index = 0;
	u16 bufferLength = 0;
	u8 separatorLen = strlen(separator);
	u8 *data = *ptr;
	std::unique_ptr<char> stringBuffer = std::unique_ptr<char>(new char[maxlength]);

	// Loop until the string end is reached
	bool stringEnds = true;
	do {

		// Check every separator for a coincidence
		stringEnds = false;
		for(u8 i = 0; i < separatorLen; i++) {
			stringEnds |= (data[index] == separator[i]);
		}

		if(!stringEnds) {

			// Check ASCII range
			if(data[index] < mincode || data[index] > maxcode) {
				throw std::runtime_error("String in wrong alphabet");
			}

			// Escape characters, if needed
			if(data[index] == '\\' && escape) {
				index ++;
			}

			// Save the next character of the string
			stringBuffer.get()[bufferLength++] = data[index++];
		}
	} while(!stringEnds && bufferLength < maxlength);

	// Check the string length
	if(bufferLength < minlength || bufferLength > maxlength) {
		throw std::runtime_error("Wrong string length");
	}

	// Return the retrived string
	*ptr += index + 1;		// Separator
	return std::string(stringBuffer.get(), bufferLength);
}

/**
 * @brief Serialize a Syslog into a JSON
 * @return The serialized syslog
 */
std::shared_ptr<json_t> SyslogPdu::serialize() {

    auto root = std::shared_ptr<json_t>(json_object(), [=](json_t* data) { json_decref(data); });
    
    json_t *fields = json_array();
    json_object_set_new(root.get(), "data", fields);
    Utils::addJsonField(fields, "Priority: " + std::to_string(priority));
    Utils::addJsonField(fields, "Version: " + std::to_string(version) + "." + std::to_string(subversion));
    Utils::addJsonField(fields, "Date: " + std::to_string(timeStamp.day) + "-" + std::to_string(timeStamp.month) + "-" + std::to_string(timeStamp.year));
    Utils::addJsonField(fields, "Time: " + std::to_string(timeStamp.hour) + ":" + std::to_string(timeStamp.minute) + ":" + std::to_string(timeStamp.second) + "." + std::to_string(timeStamp.fracsecond));
    Utils::addJsonField(fields, "Time offset: " + std::to_string(timeStamp.hour_offset) + ":" + std::to_string(timeStamp.minute_offset));
    Utils::addJsonField(fields, "Hostname: " + hostname);
    Utils::addJsonField(fields, "Appname: " + appname);
    Utils::addJsonField(fields, "ProcID: " + procid);
    Utils::addJsonField(fields, "MsgID: " + msgid);

    for(SyslogElement element : this->elements) {
		Utils::addJsonField(fields, "ElementName: " + element.name);
		for(SyslogElementParam param : element.params) {
			Utils::addJsonField(fields, "ParamName: " + param.name);
			Utils::addJsonField(fields, "ParamValue: " + param.value);
		}
	}

    Utils::addJsonField(fields, message);
    return root;
}

/**
 * @brief Print information about the retrieved Syslog
 */
void SyslogPdu::print() {
	
	FILE *f = fopen("log.txt", "a+");
	fprintf(f, "\nPriority: %d\n", this->priority);
	fprintf(f, "Version: %d.%d\n", this->version, this->subversion);
	fprintf(f, "Date: %d-%d-%d\n", this->timeStamp.day, this->timeStamp.month, this->timeStamp.year);
	fprintf(f, "Time: %d:%d:%d.%ld\n", this->timeStamp.hour, this->timeStamp.minute, this->timeStamp.second, this->timeStamp.fracsecond);
	fprintf(f, "Time offset: %d:%d\n", this->timeStamp.hour_offset, this->timeStamp.minute_offset);
	fprintf(f, "Hostname: %s\n", this->hostname.c_str());
	fprintf(f, "Appname: %s\n", this->appname.c_str());
	fprintf(f, "ProcID: %s\n", this->procid.c_str());
	fprintf(f, "MsgID: %s\n", this->msgid.c_str());

	for(SyslogElement element : this->elements) {
		fprintf(f, "ElementName: %s\n", element.name.c_str());
		for(SyslogElementParam param : element.params) {
			fprintf(f, "ParamName: %s\n", param.name.c_str());
			fprintf(f, "ParamValue: %s\n", param.value.c_str());
		}
	}

	fprintf(f, "Message: %s\n", this->message.c_str());

	fclose(f);
}

}
