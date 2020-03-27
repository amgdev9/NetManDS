// Includes C/C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>

// Includes 3DS
#include <3ds.h>

// Own includes
#include "Application.h"
#include "snmp/Snmpv2Pdu.h"
#include "snmp/Snmpv3Pdu.h"
#include "snmp/Snmpv3UserStore.h"
#include "syslog/SyslogPdu.h"
#include "socket/UdpSocket.h"
#include "ssh/SshHelper.h"

using namespace NetMan;

// Test scenarios
void ssh_test();
void syslog_test();
void snmpv1_test();
void snmpv3_test();

/**
 * @brief Main function
 * @param argc Number of arguments
 * @param argv Program arguments
 */
int main(int argc, char **argv) {

	Application &app = Application::getInstance();

	app.initialize();

	//ssh_test();
	//syslog_test();
	//snmpv1_test();
	//snmpv3_test();

	app.run();

	return 0;
}

/**
 * @brief Test the SSH stuff
 */
void ssh_test() {

	FILE *f = fopen("log.txt", "wb");
	fclose(f);

	try {
		std::shared_ptr<SshHelper> sshHelper = std::make_shared<SshHelper>();
		//sshHelper->connect(...);
		sshHelper->sendData("ls -l\n");
		if(sshHelper->recvData()) {
			f = fopen("log.txt", "a+");
			fprintf(f, sshHelper->getRecvData().c_str());
			fclose(f);
		}
		sshHelper->disconnect();
	} catch (const std::runtime_error &e) {
		f = fopen("log.txt", "a+");
		fprintf(f, e.what());
		fclose(f);
	}
}

/**
 * @brief Test the syslog stuff
 */
void syslog_test() {
	
	FILE *f = fopen("log.txt", "wb");
	fclose(f);

	std::shared_ptr<UdpSocket> sock = std::make_shared<UdpSocket>(30);
	sock->bindTo(SYSLOG_PORT);

	try {
		std::shared_ptr<SyslogPdu> syslogPdu = std::make_shared<SyslogPdu>();
		for(u8 i = 0; i < 10; i++) {
			syslogPdu->recvLog(sock);
			syslogPdu->print();
		}
	} catch (const std::runtime_error &e) {
		f = fopen("log.txt", "a+");
		fprintf(f, e.what());
		fclose(f);
	}
}

/**
 * @brief Test the SNMPv3 stuff
 */
void snmpv3_test() {

	Snmpv3UserStore &store = Snmpv3UserStore::getInstance();
	store.load("userStore.txt");

	std::shared_ptr<UdpSocket> sock = std::make_shared<UdpSocket>();

	FILE *f = fopen("sdmc:/log.txt", "wb");
	fprintf(f, "Initialized\n");
	fclose(f);

	try {

		// Pruebas SNMP GET
		std::shared_ptr<Snmpv3Pdu> pdu = std::make_shared<Snmpv3Pdu>("engineid", "", "manager");
		std::shared_ptr<BerOid> oid[7];
		oid[0] = std::make_shared<BerOid>("1.3.6.1.2.1.1.1.0");
		oid[1] = std::make_shared<BerOid>("1.3.6.1.2.1.1.2.0");
		oid[2] = std::make_shared<BerOid>("1.3.6.1.2.1.1.3.0");
		oid[3] = std::make_shared<BerOid>("1.3.6.1.2.1.1.4.0");
		oid[4] = std::make_shared<BerOid>("1.3.6.1.2.1.1.5.0");
		oid[5] = std::make_shared<BerOid>("1.3.6.1.2.1.1.6.0");
		oid[6] = std::make_shared<BerOid>("1.3.6.1.2.1.1.7.0");
		std::shared_ptr<BerNull> nullval = std::make_shared<BerNull>();
		for(u8 i = 0; i < 7; i++) {
			pdu->addVarBind(oid[i], nullval);
		}
		pdu->sendRequest(SNMPV2_GETREQUEST, sock, "127.0.0.1", SNMP_PORT);
		try{
			pdu->recvResponse(sock, "127.0.0.1", SNMP_PORT);
		} catch (std::runtime_error &e) {
			f = fopen("sdmc:/log.txt", "a+");
			fprintf(f, "Error: %s\n", e.what());
			fclose(f);
		}
		pdu->clear();
		for(u8 i = 0; i < 7; i++) {
			pdu->addVarBind(oid[i], nullval);
		}
		pdu->sendRequest(SNMPV2_GETREQUEST, sock, "127.0.0.1", SNMP_PORT);
		pdu->recvResponse(sock, "127.0.0.1", SNMP_PORT);

		// Pruebas SNMP GET BULK
		pdu->clear();
		oid[0] = std::make_shared<BerOid>("1.3.6.1.2.1.1.3");
		oid[1] = std::make_shared<BerOid>("1.3.6.1.2.1.1.1");
		oid[2] = std::make_shared<BerOid>("1.3.6.1.2.1.2.2");
		for(u8 i = 0; i < 3; i++) {
			pdu->addVarBind(oid[i], nullval);
		}
		pdu->sendBulkRequest(2, 5, sock, "127.0.0.1", SNMP_PORT);
		pdu->recvResponse(sock, "127.0.0.1", SNMP_PORT);

		// Pruebas SNMP TRAP e INFORM
		pdu = std::make_shared<Snmpv3Pdu>("engineid", "", "manager");
		sock = std::make_shared<UdpSocket>(30);
		sock->bindTo(SNMP_TRAP_PORT);
		pdu->clear();
		pdu->recvTrap(sock);
		pdu->clear();
		pdu->recvTrap(sock);

	} catch (const std::runtime_error &e) {
		f = fopen("sdmc:/log.txt", "a+");
		fprintf(f, "Error: %s\n", e.what());
		fclose(f);
	}
}

/**
 * @brief Test the SNMPv1 stuff
 */
void snmpv1_test() {
	std::shared_ptr<UdpSocket> sock = std::make_shared<UdpSocket>();

	FILE *f = fopen("sdmc:/log.txt", "wb");
	fprintf(f, "Initialized\n");
	fclose(f);

	try {

		// Pruebas SNMP GET
		std::shared_ptr<Snmpv2Pdu> pdu = std::make_shared<Snmpv2Pdu>("public");
		std::shared_ptr<BerOid> oid[7];
		oid[0] = std::make_shared<BerOid>("1.3.6.1.2.1.1.1.0");
		oid[1] = std::make_shared<BerOid>("1.3.6.1.2.1.1.2.0");
		oid[2] = std::make_shared<BerOid>("1.3.6.1.2.1.1.3.0");
		oid[3] = std::make_shared<BerOid>("1.3.6.1.2.1.1.4.0");
		oid[4] = std::make_shared<BerOid>("1.3.6.1.2.1.1.5.0");
		oid[5] = std::make_shared<BerOid>("1.3.6.1.2.1.1.6.0");
		oid[6] = std::make_shared<BerOid>("1.3.6.1.2.1.1.7.0");
		std::shared_ptr<BerNull> nullval = std::make_shared<BerNull>();
		for(u8 i = 0; i < 7; i++) {
			pdu->addVarBind(oid[i], nullval);
		}
		pdu->sendRequest(SNMPV2_GETREQUEST, sock, "127.0.0.1");
		pdu->recvResponse(sock, "127.0.0.1", SNMP_PORT);

		// Pruebas SNMP SET
		/*pdu->clear();
		std::shared_ptr<BerOctetString> val[3];
		val[0] = std::make_shared<BerOctetString>("Texto 1");
		val[1] = std::make_shared<BerOctetString>("Esto funciona perfecto");
		val[2] = std::make_shared<BerOctetString>("No hace falta que lo pruebes mas");
		pdu->addVarBind(oid[3], val[0]);
		pdu->addVarBind(oid[4], val[1]);
		pdu->addVarBind(oid[5], val[2]);
		pdu->sendRequest(SNMPV1_SETREQUEST, sock, "127.0.0.1");
		pdu->recvResponse(sock, "127.0.0.1");*/

		// Pruebas SNMP GET BULK
		pdu->clear();
		oid[0] = std::make_shared<BerOid>("1.3.6.1.2.1.1.3");
		oid[1] = std::make_shared<BerOid>("1.3.6.1.2.1.1.1");
		oid[2] = std::make_shared<BerOid>("1.3.6.1.2.1.2.2");
		for(u8 i = 0; i < 3; i++) {
			pdu->addVarBind(oid[i], nullval);
		}
		pdu->sendBulkRequest(2, 5, sock, "127.0.0.1");
		pdu->recvResponse(sock, "127.0.0.1", SNMP_PORT);

		// Pruebas SNMP TRAP
		sock = std::make_shared<UdpSocket>(30);
		sock->bindTo(SNMP_TRAP_PORT);
		pdu->clear();
		pdu->recvTrap(sock);
		pdu->clear();
		pdu->recvTrap(sock);

	} catch (const std::runtime_error &e) {
		f = fopen("sdmc:/log.txt", "a+");
		fprintf(f, "Error: %s\n", e.what());
		fclose(f);
	}
}
