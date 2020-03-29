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
};

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
};

static const char *wantedTokens[] = {
    "OBJECT",
    "MODULE-IDENTITY",
    "OBJECT-IDENTITY",
    "OBJECT-TYPE",
    "OBJECT-GROUP",
    "MODULE-COMPLIANCE",
};

enum wantedTokensEnum {
    WTOKEN_OBJECT = 0,
    WTOKEN_MODULE_IDENTITY,
    WTOKEN_OBJECT_IDENTITY,
    WTOKEN_OBJECT_TYPE,
    WTOKEN_OBJECT_GROUP,
    WTOKEN_MODULE_COMPLIANCE,
};

static const char *forbiddenTokens[] = {
    "IMPORTS",
};

namespace NetMan {

MibLoader::MibLoader() { }

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
    auto mibOids = std::unordered_map<std::string, MibOid>();
    MibOid mibOid;
    std::string mibOidName;

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

        // Continue if last token has a forbidden word
        found = false;
        for(u32 i = 0; i < sizeof(forbiddenTokens) / sizeof(char*); i++) {
            if(compareToken(oldToken, forbiddenTokens[i])) {
                found = true;
                i = sizeof(forbiddenTokens) / sizeof(char*);
            }
        }
        if(found) continue;

        // Save "possible" expression name
        // If decoding fails, this won't be saved
        mibOidName = std::string(oldToken);

        try {
            // Decode expression type
            if(compareToken(token, wantedTokens[WTOKEN_OBJECT])) {
                getToken(f, token);
                if(compareToken(token, tokenList[TOKEN_IDENTIFIER])) {
                    checkToken(f, TOKEN_EQUAL);
                    decodeOID(f, token, &mibOid);
                    mibOid.macroType = MACRO_NONE;
                    mibOid.macroData = nullptr;
                    mibOids[mibOidName] = mibOid;
                }
            } else if(compareToken(token, wantedTokens[WTOKEN_MODULE_IDENTITY])) {
                std::shared_ptr<u8> moduleIdentityPtr = std::shared_ptr<u8>((u8*)new MibModuleIdentity());
                MibModuleIdentity *moduleIdentity = (MibModuleIdentity*)moduleIdentityPtr.get();
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
                
                fseek(f, -1, SEEK_CUR);     // TODO FIX
                checkToken(f, TOKEN_EQUAL);
                decodeOID(f, token, &mibOid);
                mibOid.macroType = MACRO_MODULE_IDENTITY;
                mibOid.macroData = moduleIdentityPtr;
                mibOids[mibOidName] = mibOid;
                
            } else if(compareToken(token, wantedTokens[WTOKEN_OBJECT_IDENTITY])) {
                // TODO
            } else if(compareToken(token, wantedTokens[WTOKEN_OBJECT_TYPE])) {
                // TODO
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

    f = fopen("log.txt", "wb");
    for(auto element : mibOids) {
        fprintf(f, "%s: { %s %ld }\n", element.first.c_str(), element.second.parentName.c_str(), element.second.value);
        switch(element.second.macroType) {
            case MACRO_MODULE_IDENTITY:
            {
                auto moduleIdentity = (MibModuleIdentity*)element.second.macroData.get();
                fprintf(f, "\tLastUpdated: %s\n", moduleIdentity->lastUpdated.c_str());
                fprintf(f, "\tOrganization: %s\n", moduleIdentity->organization.c_str());
                fprintf(f, "\tContactInfo: %s\n", moduleIdentity->contactInfo.c_str());
                fprintf(f, "\tDescription: %s\n", moduleIdentity->description.c_str());
                for(auto revision : moduleIdentity->revisions) {
                    fprintf(f, "\tRevisionDate: %s\n", revision.date.c_str());
                    fprintf(f, "\tRevision: %s\n", revision.description.c_str());
                }
            } break;
            default:
                break;
        }
    }
    fclose(f);

    return nullptr;
}

void MibLoader::decodeOID(FILE *f, char *token, MibOid *oid) {
    checkToken(f, TOKEN_OPENBRACKET);
    getToken(f, token);
    oid->parentName = std::string(token);
    getToken(f, token);
    oid->value = atoi(token);
    checkToken(f, TOKEN_CLOSEBRACKET);
}

bool MibLoader::compareToken(char *token, const char *compToken) {
    return (strcmp(token, compToken) == 0);
}

void MibLoader::checkToken(FILE *f, u32 compToken) {
    skipUntilReadable(f);
    
    u32 length = strlen(tokenList[compToken]);
    for(u32 i = 0; i < length; i++) {
        if((fgetc(f) != tokenList[compToken][i]) || feof(f)) {
            throw std::runtime_error(std::string("Expected ") + tokenList[compToken]);
        }
    }
}

void MibLoader::getQuotedString(FILE *f, char *str) {
    skipUntilReadable(f);

    u32 i = 0;
    if(fgetc(f) != '\"') {
        throw std::runtime_error("No quotation mark");
    }
    char c = fgetc(f);

    while(c != '\"') {
        if(i >= MIBLOADER_STR_SIZE - 1) {
            throw std::runtime_error("Error decoding quoted string");
        }
        str[i++] = c;
        c = fgetc(f);
    }

    str[i] = '\0';
}

void MibLoader::getToken(FILE *f, char *token) {
    skipUntilReadable(f);
    char c = fgetc(f);
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
    /*if(i == 0) {
        fseek(f, -1, SEEK_CUR);
    }*/
}

void MibLoader::skipUntilReadable(FILE *f) {
    bool readable = false;
    while(!readable && !feof(f)) {

        char c = fgetc(f);

        // Skip fillers
        if(c != ' ' && c != '\t' && c != '\n' && c != '\r' && !feof(f)) {
            fseek(f, -1, SEEK_CUR);

            // Skip comments
            char c1 = fgetc(f);
            char c2 = fgetc(f);
            if(c1 == '-' && c2 == '-') {
                while(fgetc(f) != '\n' && !feof(f));    // Go to next line
            } else {
                fseek(f, -2, SEEK_CUR);
                readable = true;    // No fillers and no comment
            }
        }
    }
}

MibLoader::~MibLoader() { }

}
