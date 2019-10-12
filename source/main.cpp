#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "Application.h"
#include "Snmpv1Pdu.h"

using namespace NetMan;

int main(int argc, char **argv) {

	Application *app = Application::getInstance();

	app->initialize();

	UdpSocket *sock = new UdpSocket();

	Snmpv1Pdu *pdu = new Snmpv1Pdu();
	pdu->addVarBind("1.2.3.4");
	pdu->getRequest(sock, "192.168.100.1");
	delete pdu;
	delete sock;

	app->run();

	return 0;
}
