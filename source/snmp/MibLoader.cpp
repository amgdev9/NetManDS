/**
 * @file MibLoader.cpp
 * @brief MIB loader
 */

// Includes C/C++
#include <fstream>
#include <string.h>
#include <unordered_map>

// Own includes
#include "snmp/MibLoader.h"

// List of tokens
static const char *tokenList[] = {
    "DEFINITIONS",
    "::=",
    "BEGIN",
    "IDENTIFIER",
    "{",
    "}",
    "END",
    "LAST-UPDATED",
    "ORGANIZATION",
    "CONTACT-INFO",
    "DESCRIPTION",
    "REVISION",
    "SYNTAX",
    "UNITS",
    "MAX-ACCESS",
    "STATUS",
    "REFERENCE",
    "INDEX",
    "AUGMENTS",
    "DEFVAL",
};

// Enumeration with all the tokens
enum tokenListEnum {
    TOKEN_DEFINITIONS = 0,
    TOKEN_EQUAL,
    TOKEN_BEGIN,
    TOKEN_IDENTIFIER,
    TOKEN_OPENBRACKET,
    TOKEN_CLOSEBRACKET,
    TOKEN_END,
    TOKEN_LAST_UPDATED,
    TOKEN_ORGANIZATION,
    TOKEN_CONTACT_INFO,
    TOKEN_DESCRIPTION,
    TOKEN_REVISION,
    TOKEN_SYNTAX,
    TOKEN_UNITS,
    TOKEN_MAX_ACCESS,
    TOKEN_STATUS,
    TOKEN_REFERENCE,
    TOKEN_INDEX,
    TOKEN_AUGMENTS,
    TOKEN_DEFVAL,
};

// List of specific tokens to be scanned
// If a token is not included here it is ignored
static const char *wantedTokens[] = {
    "OBJECT",
    "MODULE-IDENTITY",
    "OBJECT-IDENTITY",
    "OBJECT-TYPE",
    "OBJECT-GROUP",
    "MODULE-COMPLIANCE",
};

// Enumeration with all the wanted tokens
enum wantedTokensEnum {
    WTOKEN_OBJECT = 0,
    WTOKEN_MODULE_IDENTITY,
    WTOKEN_OBJECT_IDENTITY,
    WTOKEN_OBJECT_TYPE,
    WTOKEN_OBJECT_GROUP,
    WTOKEN_MODULE_COMPLIANCE,
};

namespace NetMan {

/**
 * @brief Constructor for a MIB loader
 */
MibLoader::MibLoader() {
    smiMib = nullptr;
}

/**
 * @brief Load the SMI MIB
 * @param path      Path to the SMI MIB
 * @note It is used for resolving OIDs
 */
void MibLoader::loadSMI(const std::string &path) {
    try {
        smiMib = load(path);
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
}

/**
 * @brief Load a MIB, scanning the wanted tokens
 * @param path  Path to the MIB file
 * @return The loaded MIB
 */
std::shared_ptr<Mib> MibLoader::load(const std::string &path) {

    // Open file
    FILE *f = fopen(path.c_str(), "rb");
    if(f == NULL) {
        throw std::runtime_error(path + " can't be opened");
    }

    // Variables
    char token[MIBLOADER_TOKEN_SIZE];
    char oldToken[MIBLOADER_TOKEN_SIZE];
    memset(token, 0, MIBLOADER_TOKEN_SIZE);

    // Create the OID tree
    auto mibOidsPtr = std::make_shared<std::unordered_map<std::string, std::shared_ptr<MibOid>>>();
    std::unordered_map<std::string, std::shared_ptr<MibOid>> &mibOids = *mibOidsPtr.get();
    std::shared_ptr<MibOid> mibOid = std::shared_ptr<MibOid>(new MibOid);
    std::string mibOidName;

    // Append the SMI tree, if it has been loaded
    if(smiMib != nullptr) {
        auto &smiMibTree = *smiMib->getOidTree();
        for(auto it = smiMibTree.begin(), it_end = smiMibTree.end(); it != it_end; ++it) {
            mibOids.insert(*it);
        }
    }

    // Create a buffer for string parsing
    std::unique_ptr<char> quotedString = nullptr;
    try {
        quotedString = std::unique_ptr<char>(new char[MIBLOADER_STR_SIZE]);
    } catch (const std::bad_alloc &e) {
        throw;
    }

    // Check document start
    getToken(f, token);
    try {
        checkToken(f, TOKEN_DEFINITIONS);
        checkToken(f, TOKEN_EQUAL);
        checkToken(f, TOKEN_BEGIN);
    } catch (const std::runtime_error &e) {
        throw;
    }

    // Read until end of file
    while(!feof(f)) {

        // Get a token
        strcpy(oldToken, token);
        getToken(f, token);

        // Continue if it is not a looked word
        bool found = false;
        for(u32 i = 0; i < sizeof(wantedTokens) / sizeof(char*); i++) {
            if(compareToken(token, wantedTokens[i])) {
                found = true;
                i = sizeof(wantedTokens) / sizeof(char*);
            }
        }
        if(!found) continue;

        // Save "possible" expression name
        // If decoding fails, this won't be saved
        mibOidName = std::string(oldToken);

        try {
            // Decode an OBJECT IDENTIFIER
            if(compareToken(token, wantedTokens[WTOKEN_OBJECT])) {
                getToken(f, token);
                if(compareToken(token, tokenList[TOKEN_IDENTIFIER])) {
                    decodeOID(f, token, mibOid.get());
                    mibOid = addOid(mibOids, mibOidName, mibOid, MACRO_NONE, nullptr);
                }
            } 
            
            // Decode a MODULE-IDENTITY
            else if(compareToken(token, wantedTokens[WTOKEN_MODULE_IDENTITY])) {

                std::shared_ptr<u8> moduleIdentityPtr = std::shared_ptr<u8>((u8*)new MibModuleIdentity());
                MibModuleIdentity *moduleIdentity = (MibModuleIdentity*)moduleIdentityPtr.get();

                // Read compulsory fields
                checkToken(f, TOKEN_LAST_UPDATED);
                getQuotedString(f, quotedString.get());
                moduleIdentity->lastUpdated = std::string(quotedString.get());
                checkToken(f, TOKEN_ORGANIZATION);
                getQuotedString(f, quotedString.get());
                moduleIdentity->organization = std::string(quotedString.get());
                checkToken(f, TOKEN_CONTACT_INFO);
                getQuotedString(f, quotedString.get());
                moduleIdentity->contactInfo = std::string(quotedString.get());
                checkToken(f, TOKEN_DESCRIPTION);
                getQuotedString(f, quotedString.get());
                moduleIdentity->description = std::string(quotedString.get());

                // Read MIB revisions
                getToken(f, token);
                while(compareToken(token, tokenList[TOKEN_REVISION])) {
                    MibRevision revision;
                    getQuotedString(f, quotedString.get());
                    revision.date = std::string(quotedString.get());
                    checkToken(f, TOKEN_DESCRIPTION);
                    getQuotedString(f, quotedString.get());
                    revision.description = std::string(quotedString.get());
                    moduleIdentity->revisions.push_back(revision);
                    getToken(f, token);
                }
                
                fseek(f, -1, SEEK_CUR);     // getToken() reads an additional character to skip ::=, { ...
                decodeOID(f, token, mibOid.get());
                mibOid = addOid(mibOids, mibOidName, mibOid, MACRO_MODULE_IDENTITY, moduleIdentityPtr);
            } else if(compareToken(token, wantedTokens[WTOKEN_OBJECT_IDENTITY])) {
                // TODO
            } 
            
            // Decode an OBJECT-TYPE
            else if(compareToken(token, wantedTokens[WTOKEN_OBJECT_TYPE])) {

                std::shared_ptr<u8> objectTypePtr = std::shared_ptr<u8>((u8*)new MibObjectType());
                MibObjectType *objectType = (MibObjectType*)objectTypePtr.get();

                // Read syntax field
                objectType->syntax = std::string();
                checkToken(f, TOKEN_SYNTAX);
                getToken(f, token);
                while(!compareToken(token, tokenList[TOKEN_UNITS]) && !compareToken(token, tokenList[TOKEN_MAX_ACCESS]) && !feof(f)) {
                    objectType->syntax.append(std::string(token) + " ");
                    getToken(f, token);
                }

                // Read units
                if(compareToken(token, tokenList[TOKEN_UNITS])) {
                    getQuotedString(f, quotedString.get());
                    objectType->units = std::string(quotedString.get());
                    getToken(f, token);
                }

                // Read max-access
                if(!compareToken(token, tokenList[TOKEN_MAX_ACCESS])) {
                    throw std::runtime_error("Expected MAX-ACCESS");
                }
                getToken(f, token);
                objectType->maxAccess = std::string(token);

                // Read status
                checkToken(f, TOKEN_STATUS);
                getToken(f, token);
                objectType->status = std::string(token);

                // Read description
                checkToken(f, TOKEN_DESCRIPTION);
                getQuotedString(f, quotedString.get());
                objectType->description = std::string(quotedString.get());

                // Read reference
                getToken(f, token);
                if(compareToken(token, tokenList[TOKEN_REFERENCE])) {
                    getQuotedString(f, quotedString.get());
                    objectType->reference = std::string(quotedString.get());
                    getToken(f, token);
                }

                // Read index
                if(compareToken(token, tokenList[TOKEN_INDEX]) || compareToken(token, tokenList[TOKEN_AUGMENTS])) {
                    getQuotedString(f, quotedString.get(), '{', '}');
                    objectType->indexPart = std::string(quotedString.get());
                    getToken(f, token);
                }

                // Read default value
                if(compareToken(token, tokenList[TOKEN_DEFVAL])) {
                    getQuotedString(f, quotedString.get(), '{', '}');
                    objectType->defaultValue = std::string(quotedString.get());
                    getToken(f, token);
                }

                fseek(f, -1, SEEK_CUR);
                decodeOID(f, token, mibOid.get());
                mibOid = addOid(mibOids, mibOidName, mibOid, MACRO_OBJECT_TYPE, objectTypePtr);
            } else if(compareToken(token, wantedTokens[WTOKEN_OBJECT_GROUP])) {
                // TODO
            } else if(compareToken(token, wantedTokens[WTOKEN_MODULE_COMPLIANCE])) {
                // TODO
            }
        } catch (const std::bad_alloc &e) {
            throw;
        } catch (const std::runtime_error &e) {
            // Continue reading the file
        }
    }

    // Close file
    fclose(f);

    // Return the loaded MIB
    return std::make_shared<Mib>(mibOidsPtr);
}

/**
 * @brief Add an OID to a MIB tree
 * @param oidMap    OID tree
 * @param name      OID name
 * @param oid       OID structure
 * @param macroType Embedded macro type
 * @param macroData Embedded macro data
 * @return A new MibOid for further loading
 */
std::shared_ptr<MibOid> MibLoader::addOid(std::unordered_map<std::string, std::shared_ptr<MibOid>> &oidMap, const std::string &name, std::shared_ptr<MibOid> oid, MibMacroType macroType, std::shared_ptr<u8> macroData) {
    oid->macroType = macroType;
    oid->macroData = macroData;
    oidMap[name] = oid;
    
    auto it = oidMap.find(oid->parentName);
    if(it != oidMap.end()) {
        it->second->children[name] = oid;
    }

    return std::shared_ptr<MibOid>(new MibOid);
}


/**
 * @brief Decode an OID from the MIB file
 * @param f     File handle
 * @param token Buffer to hold tokens
 * @param oid   Where to save the OID (output)
 */
void MibLoader::decodeOID(FILE *f, char *token, MibOid *oid) {
    checkToken(f, TOKEN_EQUAL);
    checkToken(f, TOKEN_OPENBRACKET);
    getToken(f, token);
    oid->parentName = std::string(token);
    getToken(f, token);
    oid->value = atoi(token);
    checkToken(f, TOKEN_CLOSEBRACKET);
}

/**
 * @brief Check if a token is present
 * @param f             File handle
 * @param compToken     One of tokenListEnum
 */
void MibLoader::checkToken(FILE *f, u32 compToken) {
    char c = skipUntilReadable(f);
    
    u32 length = strlen(tokenList[compToken]);
    for(u32 i = 0; i < length; i++) {
        if((c != tokenList[compToken][i]) || feof(f)) {
            throw std::runtime_error(std::string("Expected ") + tokenList[compToken]);
        }
        c = fgetc(f);
    }
}

/**
 * @brief Get a quoted string
 * @param f     File handle
 * @param str   Buffer to hold the string (output)
 * @param start Start delimiter
 * @param end   End delimiter
 */
void MibLoader::getQuotedString(FILE *f, char *str, char start, char end) {
    char c = skipUntilReadable(f);

    u32 i = 0;
    if(c != start) {
        throw std::runtime_error("No quotation mark");
    }
    c = fgetc(f);

    while(c != end) {
        if(i >= MIBLOADER_STR_SIZE - 1) {
            throw std::runtime_error("Error decoding quoted string");
        }
        str[i++] = c;
        c = fgetc(f);
    }

    str[i] = '\0';
}

/**
 * @brief Get a token from the MIB file
 * @param f     File handle
 * @param token Output buffer to hold the token
 */
void MibLoader::getToken(FILE *f, char *token) {
    char c = skipUntilReadable(f);
    u16 i = 0;
    char lastC = 0;
    while(((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '-')) && !feof(f) && i < MIBLOADER_TOKEN_SIZE - 1) {
        token[i++] = c;
        lastC = c;
        c = fgetc(f);
        if(c == '-' && lastC == '-') {  // Comment found
            i--;
            while(fgetc(f) != '\n' && !feof(f));    // Go to next line
            break;
        }
    }
    token[i] = '\0';
    
    // We don't go back 1 character so
    // we can skip ::=, { ... if needed
}

/**
 * @brief Skip file content until readable data is found
 * @param f     File handle
 * @return The next readable character
 */
char MibLoader::skipUntilReadable(FILE *f) {

    while(!feof(f)) {

        char c = fgetc(f);

        // Skip fillers
        if(c != ' ' && c != '\t' && c != '\n' && c != '\r' && !feof(f)) {

            // Skip comments
            char c2 = fgetc(f);
            if(c == '-' && c2 == '-') {
                while(fgetc(f) != '\n' && !feof(f));    // Go to next line
            } else {
                fseek(f, -1, SEEK_CUR);
                return c;    // No fillers and no comment
            }
        }
    }

    return 0;
}

/**
 * @brief Destructor for a MIB loader
 */
MibLoader::~MibLoader() { }

/**
 * @brief Get the singleton instance for a MibLoader
 * @return The singleton instance
 */
MibLoader &MibLoader::getInstance() {

	static MibLoader mibLoader;
	return mibLoader;
}

}
