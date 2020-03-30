// Includes C/C++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <arpa/inet.h>

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
#include "snmp/SnmpAgentScanner.h"
#include "snmp/MibLoader.h"

using namespace NetMan;

// Test scenarios
void ssh_test();
void syslog_test_udp();
void syslog_test_tcp();
void snmpv1_test();
void snmpv3_test();
void snmpagent_test();
void mibloader_test();

/**
 * @brief Main function
 * @param argc Number of arguments
 * @param argv Program arguments
 */
int main(int argc, char **argv) {

	Application &app = Application::getInstance();

	app.initialize();

	//snmpv1_test();
	//snmpv3_test();
	//syslog_test_udp();
	//syslog_test_tcp();
	//ssh_test();	// Edit sshHelper->connect() line
    //snmpagent_test();
    mibloader_test();

	app.run();

	return 0;
}

/**
 * @brief Test the MIB loader
 */
void mibloader_test() {

    FILE *f = fopen("log.txt", "wb");
	fclose(f);
    
    try {
        MibLoader &mibLoader = MibLoader::getInstance();
        mibLoader.loadSMI("mibs/SNMPv2-SMI.txt");
        std::shared_ptr<Mib> mib = mibLoader.load("mibs/IF-MIB.txt");
        mib->print();
        std::shared_ptr<BerOid> oid = mib->resolve("ifTestOwner");
        oid->print();
    } catch (const std::runtime_error &e) {
		f = fopen("log.txt", "a+");
		fprintf(f, e.what());
		fclose(f);
	}
}

/**
 * @brief Test the syslog stuff (TCP)
 */
void syslog_test_tcp() {
	
	FILE *f = fopen("log.txt", "wb");
	fclose(f);

	try {

	    std::shared_ptr<TcpSocket> sock = std::make_shared<TcpSocket>(10, 0);
	    sock->bindTo(SYSLOG_PORT);
	    sock->listenState(10);

		std::shared_ptr<SyslogPdu> syslogPdu = std::make_shared<SyslogPdu>();
		for(u8 i = 0; i < 3; i++) {
			f = fopen("log.txt", "a+");
			fprintf(f, "Connection\n");
			fclose(f);
			std::shared_ptr<TcpSocketInterface> conn = sock->acceptConnection();
			if(conn != nullptr) {
				syslogPdu->recvLog(conn);
				syslogPdu->print();
				syslogPdu->recvLog(conn);
				syslogPdu->print();
			}
		}
	} catch (const std::runtime_error &e) {
		f = fopen("log.txt", "a+");
		fprintf(f, e.what());
		fclose(f);
	}
}

/**
 * @brief Test the SNMP agent discovery
 */
void snmpagent_test() {

    FILE *f = fopen("log.txt", "wb");
	fclose(f);

    try {
		std::shared_ptr<SnmpAgentScanner> agentScanner = std::make_shared<SnmpAgentScanner>();
        agentScanner->scanAgents(inet_addr("192.168.100.1"), 254, SNMP_PORT, SNMPV1_VERSION, 15, 2);
        agentScanner->print();
	} catch (const std::runtime_error &e) {
		f = fopen("log.txt", "a+");
		fprintf(f, e.what());
		fclose(f);
	}
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
 * @brief Test the syslog stuff (UDP)
 */
void syslog_test_udp() {

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
		pdu->sendRequest(SNMPV2_GETREQUEST, sock, inet_addr("127.0.0.1"), SNMP_PORT);
		try{
			pdu->recvResponse(sock, inet_addr("127.0.0.1"), SNMP_PORT);
		} catch (std::runtime_error &e) {
			f = fopen("sdmc:/log.txt", "a+");
			fprintf(f, "Error: %s\n", e.what());
			fclose(f);
		}
		pdu->clear();
		for(u8 i = 0; i < 7; i++) {
			pdu->addVarBind(oid[i], nullval);
		}
		pdu->sendRequest(SNMPV2_GETREQUEST, sock, inet_addr("127.0.0.1"), SNMP_PORT);
		pdu->recvResponse(sock, inet_addr("127.0.0.1"), SNMP_PORT);

		// Pruebas SNMP GET BULK
		pdu->clear();
		oid[0] = std::make_shared<BerOid>("1.3.6.1.2.1.1.3");
		oid[1] = std::make_shared<BerOid>("1.3.6.1.2.1.1.1");
		oid[2] = std::make_shared<BerOid>("1.3.6.1.2.1.2.2");
		for(u8 i = 0; i < 3; i++) {
			pdu->addVarBind(oid[i], nullval);
		}
		pdu->sendBulkRequest(2, 5, sock, inet_addr("127.0.0.1"), SNMP_PORT);
		pdu->recvResponse(sock, inet_addr("127.0.0.1"), SNMP_PORT);

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
		pdu->sendRequest(SNMPV2_GETREQUEST, sock, inet_addr("127.0.0.1"), SNMP_PORT);
		pdu->recvResponse(sock, inet_addr("127.0.0.1"), SNMP_PORT);

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
		pdu->sendBulkRequest(2, 5, sock, inet_addr("127.0.0.1"), SNMP_PORT);
		pdu->recvResponse(sock, inet_addr("127.0.0.1"), SNMP_PORT);

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
