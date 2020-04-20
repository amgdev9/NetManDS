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
#include "restconf/RestConfClient.h"
#include "restconf/YinHelper.h"
#include "Config.h"

using namespace NetMan;

// Test scenarios
void ssh_test();
void syslog_test_udp();
void syslog_test_tcp();
void snmpv1_test();
void snmpv3_test();
void snmpagent_test();
void mibloader_test();
void restconf_test();

/**
 * @brief Main function
 * @param argc Number of arguments
 * @param argv Program arguments
 */
int main(int argc, char **argv) {

	Application &app = Application::getInstance();

	app.initialize("main");

	//snmpv1_test();
	//snmpv3_test();
	//syslog_test_udp();
	//syslog_test_tcp();
	//ssh_test();	// Edit sshHelper->connect() line
    //snmpagent_test();
    //mibloader_test();
    //restconf_test();

	app.run();

	return 0;
}

/**
 * @brief Test the restconf stuff
 */
void restconf_test() {

    FILE *f = fopen("log.txt", "wb");
    fclose(f);

    auto& config = Config::getInstance().getData();
    
    try {
        std::shared_ptr<YinHelper> yinHelper = std::make_shared<YinHelper>("yin/ietf-interfaces.xml");
        tinyxml2::XMLElement *root = yinHelper->getRoot();
        tinyxml2::XMLElement *containerNode = root->FirstChildElement("container");
        tinyxml2::XMLElement *reqNode = containerNode->FirstChildElement("list");
        auto restConfClient = std::make_shared<RestConfClient>("https://ios-xe-mgmt.cisco.com:9443", "root", "D_Vay!_10&", config.rcTimeout);
        
        // Test 1 - Get all interfaces
        auto jsonData = restConfClient->request(yinHelper->getRestConfURL(reqNode), HTTPC_METHOD_GET);
        char *data = json_dumps(jsonData.get(), 0);
        f = fopen("log.txt", "a+");
        fprintf(f, "Test 1 - Get all interfaces\n");
        fprintf(f, "%s\n", yinHelper->getRestConfURL(reqNode).c_str());
        fprintf(f, data);
        fclose(f);
        free(data);

        // Test 2 - Create a new interface
        auto newInterface = std::shared_ptr<json_t>(json_object(), [=](json_t* data) { json_decref(data); });
        json_t *interface = json_object();
        json_t *ipConfig = json_object();
        json_t *addresses = json_array();
        json_t *address = json_object();
        json_object_set_new(newInterface.get(), "ietf-interfaces:interface", interface);
            json_object_set_new(interface, "name", json_string("Loopback100"));
            json_object_set_new(interface, "description", json_string("Added with RESTCONF"));
            json_object_set_new(interface, "type", json_string("iana-if-type:softwareLoopback"));
            json_object_set_new(interface, "enabled", json_true());
            json_object_set_new(interface, "ietf-ip:ipv4", ipConfig);
                json_object_set_new(ipConfig, "address", addresses);
                    json_array_append(addresses, address);
                        json_object_set_new(address, "ip", json_string("172.16.100.1"));
                        json_object_set_new(address, "netmask", json_string("255.255.255.0"));
        f = fopen("log.txt", "a+");
        fprintf(f, "\nTest 2 - Create a new interface\n");
        data = json_dumps(newInterface.get(), 0);
        fprintf(f, "%s\n", yinHelper->getRestConfContainerURL(containerNode).c_str());
        fprintf(f, data);
        fclose(f);
        free(data);
        restConfClient->request(yinHelper->getRestConfContainerURL(containerNode), HTTPC_METHOD_POST, newInterface);

        // Test 3 - Get created interface
        jsonData = restConfClient->request(yinHelper->getRestConfURL(reqNode, "Loopback100"), HTTPC_METHOD_GET);
        data = json_dumps(jsonData.get(), 0);
        f = fopen("log.txt", "a+");
        fprintf(f, "\nTest 3 - Get created interface\n");
        fprintf(f, data);
        fclose(f);
        free(data);

        // Test 4 - Delete created interface
        restConfClient->request(yinHelper->getRestConfURL(reqNode, "Loopback100"), HTTPC_METHOD_DELETE);
        f = fopen("log.txt", "a+");
        fprintf(f, "\nTest 4 - Delete created interface\n");
        fclose(f);

        // Test 5 - Get all interfaces (check we deleted the interface)
        jsonData = restConfClient->request(yinHelper->getRestConfURL(reqNode), HTTPC_METHOD_GET);
        data = json_dumps(jsonData.get(), 0);
        f = fopen("log.txt", "a+");
        fprintf(f, "\nTest 5 - Get all interfaces\n");
        fprintf(f, data);
        fclose(f);
        free(data);

    } catch(const std::bad_alloc &e) {
        f = fopen("log.txt", "a+");
		fprintf(f, e.what());
		fclose(f);
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

    auto& config = Config::getInstance().getData();

	try {

	    std::shared_ptr<TcpSocket> sock = std::make_shared<TcpSocket>(10, 0);
	    sock->bindTo(config.syslogPort);
	    sock->listenState(10);

		std::shared_ptr<SyslogPdu> syslogPdu = std::make_shared<SyslogPdu>();
		for(u8 i = 0; i < 3; i++) {
			f = fopen("log.txt", "a+");
			fprintf(f, "Connection\n");
			fclose(f);
			std::shared_ptr<TcpSocket> conn = sock->acceptConnection(2);
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
 * @brief Test the SNMP agent discovery
 */
void snmpagent_test() {

    FILE *f = fopen("log.txt", "wb");
	fclose(f);

    auto& config = Config::getInstance().getData();

    try {
		std::shared_ptr<SnmpAgentScanner> agentScanner = std::make_shared<SnmpAgentScanner>();
        agentScanner->scanAgents(inet_addr("192.168.100.1"), 254, config.snmpPort, SNMPV1_VERSION, 15, 2);
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

    auto& config = Config::getInstance().getData();

	std::shared_ptr<UdpSocket> sock = std::make_shared<UdpSocket>(30);
	sock->bindTo(config.syslogPort);

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

	Snmpv3UserStore::getInstance();

    auto& config = Config::getInstance().getData();
	std::shared_ptr<UdpSocket> sock = std::make_shared<UdpSocket>(config.udpTimeout);

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
		pdu->sendRequest(SNMPV2_GETREQUEST, sock, inet_addr("127.0.0.1"), config.snmpPort);
		try{
			pdu->recvResponse(sock, inet_addr("127.0.0.1"), config.snmpPort);
		} catch (std::runtime_error &e) {
			f = fopen("sdmc:/log.txt", "a+");
			fprintf(f, "Error: %s\n", e.what());
			fclose(f);
		}
		pdu->clear();
		for(u8 i = 0; i < 7; i++) {
			pdu->addVarBind(oid[i], nullval);
		}
		pdu->sendRequest(SNMPV2_GETREQUEST, sock, inet_addr("127.0.0.1"), config.snmpPort);
		pdu->recvResponse(sock, inet_addr("127.0.0.1"), config.snmpPort);

		// Pruebas SNMP GET BULK
		pdu->clear();
		oid[0] = std::make_shared<BerOid>("1.3.6.1.2.1.1.3");
		oid[1] = std::make_shared<BerOid>("1.3.6.1.2.1.1.1");
		oid[2] = std::make_shared<BerOid>("1.3.6.1.2.1.2.2");
		for(u8 i = 0; i < 3; i++) {
			pdu->addVarBind(oid[i], nullval);
		}
		pdu->sendBulkRequest(2, 5, sock, inet_addr("127.0.0.1"), config.snmpPort);
		pdu->recvResponse(sock, inet_addr("127.0.0.1"), config.snmpPort);

		// Pruebas SNMP TRAP e INFORM
		pdu = std::make_shared<Snmpv3Pdu>("engineid", "", "manager");
		sock = std::make_shared<UdpSocket>(30);
		sock->bindTo(config.trapv3Port);
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

    auto& config = Config::getInstance().getData();

	std::shared_ptr<UdpSocket> sock = std::make_shared<UdpSocket>(config.udpTimeout);

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
		pdu->sendRequest(SNMPV2_GETREQUEST, sock, inet_addr("127.0.0.1"), config.snmpPort);
		pdu->recvResponse(sock, inet_addr("127.0.0.1"), config.snmpPort);

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
		pdu->sendBulkRequest(2, 5, sock, inet_addr("127.0.0.1"), config.snmpPort);
		pdu->recvResponse(sock, inet_addr("127.0.0.1"), config.snmpPort);

		// Pruebas SNMP TRAP
		sock = std::make_shared<UdpSocket>(30);
		sock->bindTo(config.trapv3Port);
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
