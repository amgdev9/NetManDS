/**
 * @file Snmpv3Pdu.cpp
 * @brief SNMPv3 PDU handler
 */

// Includes C/C++
#include <string.h>

// Own includes
#include "snmp/Snmpv3Pdu.h"
#include "snmp/Snmpv2Pdu.h"
#include "asn1/BerInteger.h"
#include "snmp/Snmpv3UserStore.h"
#include "Utils.h"

namespace NetMan {

// Static data
u32 Snmpv3Pdu::requestID = 0;

/**
 * @brief Constructor for a Snmpv3 PDU
 * @param engineID 		Engine ID
 * @param contextName 	Name of the SNMP context
 * @param userName 		User name
 */
Snmpv3Pdu::Snmpv3Pdu(const std::string &engineID, const std::string &contextName, const std::string &userName) {
    this->reqID = 0;
	this->contextName = contextName;
	secParams.msgAuthoritativeEngineID = engineID;
    secParams.msgAuthoritativeEngineBoots = 0;
    secParams.msgAuthoritativeEngineTime = 0;
    secParams.msgUserName = userName;
    secParams.msgAuthenticationParameters = "";
    secParams.msgPrivacyParameters = "";
	this->varBindList = nullptr;
}

/**
 * @brief Generate a SNMPv3 header
 * @param type			PDU type
 * @param reportable	Reportable flag set?
 * @param scopedPdu		Scoped PDU data
 * @return A sequence containing a SNMPv3 header
 */
std::shared_ptr<BerSequence> Snmpv3Pdu::generateHeader(u32 type, bool reportable, std::shared_ptr<BerField> scopedPdu) {

    try {
		std::shared_ptr<BerSequence> message = std::make_shared<BerSequence>();

		// Header (common to all SNMPv3 PDUs)
        u32 ver = SNMPV3_VERSION;
		std::shared_ptr<BerInteger> msgVersion = std::make_shared<BerInteger>(&ver, sizeof(u32), false);
        std::shared_ptr<BerSequence> msgGlobalData = std::make_shared<BerSequence>();

		// Prepare flags
		u8 flags = 0;
		Snmpv3UserStore &store = Snmpv3UserStore::getInstance();
		Snmpv3UserStoreEntry &user = store.getUser(secParams.msgUserName);
		if(reportable) flags |= SNMPV3_FLAG_REPORTABLE;
		if(type != SNMPV2_REPORT) {		// Reports are not secured
			if(user.authProto != SNMPV3_AUTHPROTO_NONE) flags |= SNMPV3_FLAG_AUTH;
			if(user.privProto != SNMPV3_PRIVPROTO_NONE) flags |= SNMPV3_FLAG_PRIV;
		}

        // Fill msgGlobalData
        this->reqID = ++Snmpv3Pdu::requestID;
        std::shared_ptr<BerInteger> msgID = std::make_shared<BerInteger>(&this->reqID, sizeof(u32), false);
        u32 maxSize = SNMP_MAX_PDU_SIZE;
        std::shared_ptr<BerInteger> msgMaxSize = std::make_shared<BerInteger>(&maxSize, sizeof(u32), false);
        std::shared_ptr<BerOctetString> msgFlags = std::make_shared<BerOctetString>(std::string(1, flags));
        u32 securityModel = SNMPV3_USM_MODEL;
        std::shared_ptr<BerInteger> msgSecurityModel = std::make_shared<BerInteger>(&securityModel, sizeof(u32), false);
        msgGlobalData->addChild(msgID);
        msgGlobalData->addChild(msgMaxSize);
        msgGlobalData->addChild(msgFlags);
        msgGlobalData->addChild(msgSecurityModel);

        // Fill msgSecurityParameters
        std::shared_ptr<BerPdu> securityParams = std::make_shared<BerPdu>();
        std::shared_ptr<BerSequence> securityParamsSeq = std::make_shared<BerSequence>();
        std::shared_ptr<BerOctetString> contextEngineID = std::make_shared<BerOctetString>(secParams.msgAuthoritativeEngineID);
        std::shared_ptr<BerInteger> engineBoots = std::make_shared<BerInteger>(&secParams.msgAuthoritativeEngineBoots, sizeof(u32), false);
        std::shared_ptr<BerInteger> engineTime = std::make_shared<BerInteger>(&secParams.msgAuthoritativeEngineTime, sizeof(u32), false);
        std::shared_ptr<BerOctetString> userName = std::make_shared<BerOctetString>(secParams.msgUserName);
        std::shared_ptr<BerOctetString> authParams = std::make_shared<BerOctetString>(secParams.msgAuthenticationParameters);
        std::shared_ptr<BerOctetString> privParams = std::make_shared<BerOctetString>(secParams.msgPrivacyParameters);

		// Add fields
        securityParamsSeq->addChild(contextEngineID);
        securityParamsSeq->addChild(engineBoots);
        securityParamsSeq->addChild(engineTime);
        securityParamsSeq->addChild(userName);
        securityParamsSeq->addChild(authParams);
        securityParamsSeq->addChild(privParams);
        securityParams->addField(securityParamsSeq);

		// Serialize security parameters as an OCTET STRING
        u32 securityParamsSize;
        std::unique_ptr<u8> serializedParams = securityParams->serialize(&securityParamsSize);
        std::shared_ptr<BerOctetString> msgSecurityParameters = std::make_shared<BerOctetString>(std::string((char*)serializedParams.get(), securityParamsSize));

        // Add each structure to the header
        message->addChild(msgVersion);
		message->addChild(msgGlobalData);
        message->addChild(msgSecurityParameters);
        message->addChild(scopedPdu);

		// Return the generated message
		return message;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Check a SNMPv3 header
 * @param ptr 			Data buffer
 * @param checkMsgID	Check message ID?
 * @param params		Security parameters (input)
 * @param sock			Socket used to generate reports to an agent
 * @param flags			SNMPv3 header flags
 * @return Encrypted PDU, or nullptr if it is not encrypted
 */
std::shared_ptr<BerOctetString> Snmpv3Pdu::checkHeader(u8 **ptr, bool checkMsgID, Snmpv3SecurityParams &params, std::shared_ptr<UdpSocket> sock, u8 *flags) {

    try {

		// Read response header
		BerSequence::decode(ptr);

		// Check version
		std::shared_ptr<BerInteger> version = BerInteger::decode(ptr, false);
		if(version->getValueU32() != SNMPV3_VERSION) {
			throw std::runtime_error("Not a SNMPv3 PDU");
		}
#ifdef SNMP_DEBUG
	version->print();
#endif

		// Check msgID
		BerSequence::decode(ptr);
		std::shared_ptr<BerInteger> msgID = BerInteger::decode(ptr, false);
		if(checkMsgID && msgID->getValueU32() != this->reqID) {
			throw std::runtime_error("msgID does not match");
		}
		if(!checkMsgID) {	// Update the requestID for the possible ACK being sent
			Snmpv3Pdu::requestID = msgID->getValueU32() - 1;
		}
#ifdef SNMP_DEBUG
	msgID->print();
#endif

		// Skip maxSize and flags
		BerInteger::decode(ptr, false);
		*flags = BerOctetString::decode(ptr)->getValue().c_str()[0];

		// Check msgSecurityModel
		std::shared_ptr<BerInteger> msgSecurityModel = BerInteger::decode(ptr, false);
		if(msgSecurityModel->getValueU32() != SNMPV3_USM_MODEL) {
			if(*flags &SNMPV3_FLAG_REPORTABLE) {
				Snmpv3Pdu::sendReportTo(sock, 0, 0, SNMPV3_SECMODEL_MISMATCH, this->secParams);
			}
			throw std::runtime_error("msgSecurityModel does not match");
		}
#ifdef SNMP_DEBUG
	msgSecurityModel->print();
#endif

		// Get securityParameters
		u8 *secParamsPtr = *ptr;		// Used if authentication is needed
		std::shared_ptr<BerOctetString> securityParams = BerOctetString::decode(ptr);
		secParamsPtr += securityParams->getTagAndLengthSize();	// Beginning of the OCTET STRING data

		// Check securityParameters
		u8 *paramsPtr = (u8*)securityParams->getValue().c_str();
		BerSequence::decode(&paramsPtr);
		
		// Check engineID
		std::shared_ptr<BerOctetString> engineID = BerOctetString::decode(&paramsPtr);
		params.msgAuthoritativeEngineID = engineID->getValue();
#ifdef SNMP_DEBUG
	engineID->print();
#endif

		// Check engineBoots
		std::shared_ptr<BerInteger> engineBoots = BerInteger::decode(&paramsPtr, false);
		params.msgAuthoritativeEngineBoots = engineBoots->getValueU32();
#ifdef SNMP_DEBUG
	engineBoots->print();
#endif

		// Check engineTime
		std::shared_ptr<BerInteger> engineTime = BerInteger::decode(&paramsPtr, false);
		params.msgAuthoritativeEngineTime = engineTime->getValueU32();
#ifdef SNMP_DEBUG
	engineTime->print();
#endif

		// Check userName
		std::shared_ptr<BerOctetString> userName = BerOctetString::decode(&paramsPtr);
		params.msgUserName = userName->getValue();
#ifdef SNMP_DEBUG
	userName->print();
#endif

		// Check authParams
		secParamsPtr += (paramsPtr - (u8*)securityParams->getValue().c_str());
		std::shared_ptr<BerOctetString> authParams = BerOctetString::decode(&paramsPtr);
		secParamsPtr += authParams->getTagAndLengthSize();
		if((*flags &SNMPV3_FLAG_AUTH) && authParams->getValue().length() != 12) {
			throw std::runtime_error("authParam is not 12 octets");
		}
		params.msgAuthenticationParameters = authParams->getValue();
#ifdef SNMP_DEBUG
	authParams->print();
#endif

		// If data must be authenticated, fill the authParams in the packet with zeros
		if(*flags &SNMPV3_FLAG_AUTH) {
			memset(secParamsPtr, 0, 12);
		}

		// Check privParams
		std::shared_ptr<BerOctetString> privParams = BerOctetString::decode(&paramsPtr);
		params.msgPrivacyParameters = privParams->getValue();
#ifdef SNMP_DEBUG
	privParams->print();
#endif

		// Get the encrypted PDU, if any
		if(*flags &SNMPV3_FLAG_PRIV) {
			return BerOctetString::decode(ptr);
		}

		// If the PDU was not encrypted...
		return nullptr;
	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Clear a SNMPv3 PDU for further usage
 */
void Snmpv3Pdu::clear() {
	BerPdu::clear();		// Varbindlist can't be here
	this->varBindList.reset();
}

/**
 * @brief Add a VarBind to the VarBindList
 * @param oid 	Object Identifier to be added
 * @param value	Value for this OID
 */
void Snmpv3Pdu::addVarBind(std::shared_ptr<BerOid> oid, std::shared_ptr<BerField> value) {
	try {
		if(this->varBindList == nullptr) this->varBindList = std::make_shared<BerSequence>();
		Snmpv1Pdu::addVarBind(this->varBindList, oid, value);
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Generate a scoped PDU
 * @param pdu	The corresponding SNMPv2 PDU
 * @return The generated scoped PDU
 */
std::shared_ptr<BerSequence> Snmpv3Pdu::generateScopedPdu(std::shared_ptr<BerSequence> pdu) {

	// Generate the scopedPDU
	std::shared_ptr<BerSequence> msgData = std::make_shared<BerSequence>();
    std::shared_ptr<BerOctetString> contextNameField = std::make_shared<BerOctetString>(contextName);
	std::shared_ptr<BerOctetString> contextEngineID = std::make_shared<BerOctetString>(secParams.msgAuthoritativeEngineID);
    msgData->addChild(contextEngineID);		// = msgAuthoritativeEngineID (?)
    msgData->addChild(contextNameField);
	msgData->addChild(pdu);

	// Return it
	return msgData;
}

/**
 * @brief Send a SNMPv3 request
 * @param type				Type of SNMP request
 * @param sock				Socket used for transmission
 * @param ip				Destination IP. If zero, it uses the last socket's origin IP
 * @param port				Destination port
 * @param nonRepeaters		Non-repeaters field for GetBulkRequest
 * @param maxRepetitions	Max-repetitions field for GetBulkRequest
 */
void Snmpv3Pdu::sendRequest(u32 type, std::shared_ptr<UdpSocket> sock, in_addr_t ip, u16 port, u32 nonRepeaters, u32 maxRepetitions) {

    try {
		
		// Check if the VarBindList is empty
		if(this->varBindList == nullptr) {
			throw std::runtime_error("Empty VarBindList");
		}

        // Generate a get-request
		std::shared_ptr<Snmpv2Pdu> snmpv2Pdu = std::make_shared<Snmpv2Pdu>("");
		snmpv2Pdu->varBindList = this->varBindList;
		std::shared_ptr<BerSequence> pdu = nullptr;
		if(type == SNMPV2_GETBULKREQUEST) {
			pdu = snmpv2Pdu->generateBulkRequest(nonRepeaters, maxRepetitions);
		} else {
			pdu = snmpv2Pdu->generateRequest(type);
		}
		std::shared_ptr<BerSequence> scopedPdu = this->generateScopedPdu(pdu);

		// Serialize the scoped PDU
		std::shared_ptr<BerPdu> serializerPdu = std::make_shared<BerPdu>();
		u32 serializedPduSize;
		serializerPdu->addField(scopedPdu);
        std::unique_ptr<u8> serializedPdu = serializerPdu->serialize(&serializedPduSize, 8);	// Because of CBC-DES alignment

		// Get the authentication and privacy protocols
		Snmpv3UserStore &userStore = Snmpv3UserStore::getInstance();
		Snmpv3UserStoreEntry &user = userStore.getUser(secParams.msgUserName);
		std::shared_ptr<Snmpv3PrivProto> privProto = nullptr;
		std::shared_ptr<Snmpv3AuthProto> authProto = nullptr;
		if(type != SNMPV2_REPORT) {		// Reports are not secured
			privProto = userStore.getPrivProto(user);
			authProto = userStore.getAuthProto(user);
		}

		// Encrypt the PDU, if needed
		std::shared_ptr<BerOctetString> encryptedPdu = nullptr;
		if(authProto != nullptr && privProto != nullptr) {
			std::shared_ptr<u8> userCryptKey = authProto->passwordToKey(user.privPass, this->secParams);
			encryptedPdu = privProto->encrypt(serializedPdu.get(), serializedPduSize, this->secParams, userCryptKey);
		}

		// Generate a header
		std::shared_ptr<BerSequence> message = nullptr;
		if(authProto != nullptr) {
			// Initialize the authentication parameter
			this->secParams.msgAuthenticationParameters = std::string(12, '\0');
		}
		if(privProto != nullptr) {
			message = this->generateHeader(type, true, encryptedPdu);
		} else {
			message = this->generateHeader(type, true, scopedPdu);
		}

		// Authenticate the whole message
		if(authProto != nullptr) {
			serializerPdu->clear();
			serializerPdu->addField(message);
        	serializedPdu = serializerPdu->serialize(&serializedPduSize);
			std::shared_ptr<u8> userAuthKey = authProto->passwordToKey(user.authPass, this->secParams);
			authProto->createHash(serializedPdu.get(), serializedPduSize, this->secParams, userAuthKey);

			// Regenerate the header
			Snmpv3Pdu::requestID --;
			if(privProto != nullptr) {
				message = this->generateHeader(type, true, encryptedPdu);
			} else {
				message = this->generateHeader(type, true, scopedPdu);
			}
		}

		// Send the PDU
		this->fields.push_back(message);
		BerPdu::send(sock, ip, port);

	} catch (const std::bad_alloc &e) {
		this->fields.clear();
		throw;
	} catch (const std::runtime_error &e) {
		this->fields.clear();
		throw;
	}

	// Clear data
	this->fields.clear();
	this->varBindList.reset();
}

/**
 * @brief Retrieve a SNMPv3 response
 * @param sock				Reception socket
 * @param ip				Expected source IP
 * @param port				Expected source port
 * @param expectedPduType	Expected PDU type
 * @return The received PDU type
 */
u8 Snmpv3Pdu::recvResponse(std::shared_ptr<UdpSocket> sock, in_addr_t ip, u16 port, u32 expectedPduType) {
    
    try {
		// Create recv buffer
		std::unique_ptr<u8> data(new u8[SNMP_MAX_PDU_SIZE]);

		// Receive packet data
		u32 packetSize = sock->recvPacket(data.get(), SNMP_MAX_PDU_SIZE, ip, port);
		u8 *ptr = data.get();

		// Read response header
		Snmpv3SecurityParams params;
		u8 flags;
		std::shared_ptr<BerOctetString> encryptedPdu = this->checkHeader(&ptr, port != 0, params, sock, &flags);
		bool reportable = flags &SNMPV3_FLAG_REPORTABLE;

		// Send report if username does not match
		Snmpv3UserStore &userStore = Snmpv3UserStore::getInstance();
		Snmpv3UserStoreEntry user;
		try {
			user = userStore.getUser(params.msgUserName);
		} catch (std::runtime_error &e) {
			if(reportable) {
				Snmpv3Pdu::sendReportTo(sock, 0, 0, SNMPV3_USERNAME_MISMATCH, secParams);
			}
			throw;
		}

		// Get the authentication and privacy protocols
		std::shared_ptr<Snmpv3PrivProto> privProto = userStore.getPrivProto(user);
		std::shared_ptr<Snmpv3AuthProto> authProto = userStore.getAuthProto(user);

		// Authenticate the PDU
		std::unique_ptr<u8> decryptedPdu = nullptr;
		if(flags &SNMPV3_FLAG_AUTH && authProto != nullptr) {

			// Check the authentication status
			std::shared_ptr<u8> userAuthKey = authProto->passwordToKey(user.authPass, params);
			bool authResult = authProto->authenticate(data.get(), packetSize, params, userAuthKey);
			if(!authResult) {
				if(reportable) {
					Snmpv3Pdu::sendReportTo(sock, 0, 0, SNMPV3_AUTH_WRONG, secParams);
				}
				throw std::runtime_error("Authentication failed");
			}

			// Decrypt the PDU
			if(flags &SNMPV3_FLAG_PRIV && privProto != nullptr) {
				try {
					std::shared_ptr<u8> userPrivKey = authProto->passwordToKey(user.privPass, params);
					decryptedPdu = privProto->decrypt(encryptedPdu, params, userPrivKey);
				} catch (const std::runtime_error &e) {
					if(reportable) {
						Snmpv3Pdu::sendReportTo(sock, 0, 0, SNMPV3_PRIV_WRONG, secParams);
					}
					throw;
				}
				ptr = decryptedPdu.get();	// Update the read pointer to the decrypted PDU
			}
		}

		// Skip msgData sequence
		BerSequence::decode(&ptr);

		// Skip contextEngineID
		BerOctetString::decode(&ptr);

		// Check contextName
		std::shared_ptr<BerOctetString> contextNameStr = BerOctetString::decode(&ptr);
		if(contextNameStr->getValue().compare(contextName) != 0) {
			throw std::runtime_error("contextName does not match");
		}
#ifdef SNMP_DEBUG
	contextNameStr->print();
#endif

		// Decode SNMP PDU
		u8 pduType;
		std::shared_ptr<BerSequence> vbList = Snmpv2Pdu::recvResponse(&ptr, port != 0, this->reqID, &pduType, SNMP_PDU_ANY);
		if(pduType != SNMPV2_REPORT) {
			if(pduType != expectedPduType && expectedPduType != SNMP_PDU_ANY) {
				throw std::runtime_error("Received undesired PDU");
			} else {

				// Send report if engineID does not match
				if(secParams.msgAuthoritativeEngineID.compare(params.msgAuthoritativeEngineID) != 0) {
					if(reportable) {
						Snmpv3Pdu::sendReportTo(sock, 0, 0, SNMPV3_ENGINEID_MISMATCH, secParams);
					}
					throw std::runtime_error("EngineID does not match");
				}
				this->varBindList = vbList;
			}
		} else {
			// Learn from the report
			secParams.msgAuthoritativeEngineID = params.msgAuthoritativeEngineID;
			secParams.msgAuthoritativeEngineBoots = params.msgAuthoritativeEngineBoots;
			secParams.msgAuthoritativeEngineTime = params.msgAuthoritativeEngineTime;
			std::string oid = std::static_pointer_cast<BerOid>(std::static_pointer_cast<BerSequence>(vbList->getChild(0))->getChild(0))->print();
			throw std::runtime_error("REPORT received: " + oid);
		}

		// Return the received PDU type
		return pduType;
	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Receive a trap or inform-request
 * @param sock Socket used for reception
 * @return If an inform was received
 */
bool Snmpv3Pdu::recvTrap(std::shared_ptr<UdpSocket> sock) {

	try {

        // Receive a trap or inform-request PDU
        u8 pduType = this->recvResponse(sock, INADDR_ANY, 0, SNMP_PDU_ANY);

        // Error if not a notification was received
        if(pduType != SNMPV2_TRAP && pduType != SNMPV2_INFORMREQUEST) {
            throw std::runtime_error("This is not a notification PDU");
        }

        // If it was an inform-request, send back the acknowledgement
        if(pduType == SNMPV2_INFORMREQUEST) {
            this->sendRequest(SNMPV2_GETRESPONSE, sock, 0, 0);    // Use inform-request origin IP-port as destination IP-port
            return true;
        }

        // A trap was received
        return false;
    } catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Wrapper to send a bulk-request
 * @param nonRepeaters		Non-repeaters field
 * @param maxRepetitions	Max-repetitions field
 * @param sock				Socket used for transmission
 * @param ip				Destination IP
 * @param port				Destination port
 */
void Snmpv3Pdu::sendBulkRequest(u32 nonRepeaters, u32 maxRepetitions, std::shared_ptr<UdpSocket> sock, in_addr_t ip, u16 port) {
	this->sendRequest(SNMPV2_GETBULKREQUEST, sock, ip, port, nonRepeaters, maxRepetitions);
}

/**
 * @brief Get a VarBind from a SNMPv3 PDU
 * @param i		VarBind index
 * @return The desired VarBind
 */
std::shared_ptr<BerField> Snmpv3Pdu::getVarBind(u16 i) {
    try {
		return std::static_pointer_cast<BerSequence>(this->varBindList->getChild(i))->getChild(1);
	} catch (const std::out_of_range &e){
		throw;
	}
}

/**
 * @brief Destructor for a SNMPv3 PDU
 */
Snmpv3Pdu::~Snmpv3Pdu() { }

/**
 * @brief Send a report PDU to some agent
 * @param sock		Socket used for transmission
 * @param ip		Destination IP. If zero, it uses the last socket's origin IP
 * @param port		Destination port
 * @param reasonOid	OID indicating the reason for the report
 * @param params	Security parameters
 */
void Snmpv3Pdu::sendReportTo(std::shared_ptr<UdpSocket> sock, in_addr_t ip, u16 port, const std::string &reasonOid, const Snmpv3SecurityParams &params) {
	try {
		std::shared_ptr<Snmpv3Pdu> pdu = std::make_shared<Snmpv3Pdu>(params.msgAuthoritativeEngineID, "", params.msgUserName);
		std::shared_ptr<BerOid> oid = std::make_shared<BerOid>(reasonOid);
		std::shared_ptr<BerNull> nullVal = std::make_shared<BerNull>();

		pdu->addVarBind(oid, nullVal);
		pdu->sendRequest(SNMPV2_REPORT, sock, ip, port);
	} catch (const std::runtime_error &e) {
		throw;
	} catch (const std::bad_alloc &e) {
		throw;
	}
}

/**
 * @brief Serialize a SNMPv3 trap into a JSON
 * @return The serialized trap
 */
std::shared_ptr<json_t> Snmpv3Pdu::serializeTrap() {
    
    auto root = std::shared_ptr<json_t>(json_object(), [=](json_t* data) { json_decref(data); });
    
    json_t *data = json_array();
    json_object_set_new(root.get(), "data", data);

    if(varBindList != nullptr) {
        for(u32 i = 0; i < varBindList->getNChildren(); i++) {
            auto child = std::static_pointer_cast<BerSequence>(this->varBindList->getChild(i));
            Utils::addJsonField(data, "OID: " + child->getChild(0)->print());
            Utils::addJsonField(data, "Value: " + child->getChild(1)->print());
        }
    }

    return root;
}

}
