#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <algorithm>
#include <utility>
#include <limits>

#include <stdlib.h>
#include <string.h>

#include "OutgoingSocketManager.h"
#include "MyExceptions.h"
#include "structs.h"

#define INFTY std::numeric_limits<int>::max()

std::ofstream fout; 	//Logging file

int routerId;			//This router's id
const char* emuHost;	//Emulator host address
const char* emuPort;	//Emulator port
const char* routerPort;	//This router's port

int emuSock;			//UDP socket for emulator communications

struct circuit_DB *routerCDB;		//Initial circuit_DB for this router
std::map<int, circuit_DB> networkDB;//Link state database
std::map<int, bool> myLinks;		//Maintains mapping of routers to whether a HELLO has been received

std::map<int, std::set<int> > linkEndpoints; //Maps links to set of router endpoints (2)

std::string routerName;	//Name of this router for logging

//Just setting up logging
void initVars()
{
	std::ostringstream oss;
	oss << "router" << routerId << ".log";

	fout.open(oss.str().c_str());

	oss.str(std::string());

	oss << "R" << routerId;
	routerName = oss.str();
}

//Validate and load command line args
void parseArgs(int argc, char const *argv[])
{
	if(argc != 5)
	{
		std::cout << "Wrong number of arguments. " << std::endl;
		exit(1);
	}

	routerId = strtol(argv[1], NULL, 10);
	emuHost = argv[2];
	emuPort = argv[3];
	routerPort = argv[4];
}

//Receive at most size bytes from the emulator
//Return total received
int recvBytes(char* buf, int size)
{
	int status = recv(emuSock, buf, size, 0);
	if(status == 0)
	{
		throw DISCONNECTION_ERROR;
	}

	if(status < 0)
	{
		throw RECEIVE_BODY_ERROR;
	}

	return status;
}

//Send INIT packet to the emulator
void sendInit()
{
	struct pkt_INIT pinit;
	pinit.router_id = routerId;

	int pktSize = sizeof(struct pkt_INIT);
	char* sendBuff = (char*) malloc(pktSize);
	memcpy(sendBuff, (char*)&pinit, pktSize);

	if(send(emuSock, sendBuff, pktSize, 0) == -1)
	{
		throw SEND_ERROR;
	}

	fout << routerName << " sent an INIT" << std::endl;

	free(sendBuff);
}

//
//Below is the implementation of Dijkstra's for computing the RIB
//

//Returns true if router "router" is an endpoint of link "link"
bool routerConnectedToLink(int router, int link)
{
	return linkEndpoints[link].count(router) != 0;
}

//Returns true if the two routers are connected by a link
bool isAdjacent(int router1, int router2)
{
	//Check that we have info on both routers
	if(networkDB.count(router1) == 0 || networkDB.count(router2) == 0)
	{
		return false;
	}

	circuit_DB router1DB = networkDB[router1];

	for (int i = 0; i < router1DB.nbr_link; ++i)
	{
		link_cost linkCost = router1DB.linkcost[i];

		if(routerConnectedToLink(router2, linkCost.link))
		{
			return true;
		}
	}

	return false;
}

//Should only be called on adjacent nodes
int getCostForAdjacent(int router1, int router2)
{
	circuit_DB db = networkDB[router1];
	for (int i = 0; i < db.nbr_link; ++i)
	{
		link_cost linkCost = db.linkcost[i];

		if(routerConnectedToLink(router2, linkCost.link))
		{
			return linkCost.cost;
		}
	}

	return INFTY;
}

//Runs Dijkstra's algorithm and returns the mapping D
//The key being the destination router and the value being the (via router, minimum cost) pair
std::map<int, std::pair<int,int> > computeCostsToAllRouters(int source)
{
	int totalRouters = 0;
	std::set<int> N;
	std::map<int, std::pair<int,int> > D;

	//Initialization
	// N = {A}
	// for all nodes v
	// 	if v adjacent to A then 
	// 		D(v) = c(A,v) 
	// 	else
	// 		D(v) = infty

	N.insert(source);

	for (int v = 1; v <= NBR_ROUTER; ++v)
	{
		if(networkDB.count(v) != 0)
		{
			if(isAdjacent(source, v))
			{
				int cost = getCostForAdjacent(source, v);
				D[v] = std::make_pair(v, cost);
			}
			else
			{
				D[v] = std::make_pair(-1, INFTY);
			}

			totalRouters++;
		}
	}
	//done init

	while(N.size() != totalRouters) //until all of the routers are in N
	{
		// find w not in N such that D(w) is a minimum 
		// add w to N
		int min = INFTY;
		int w = -1;
		for (int v = 1; v <= NBR_ROUTER; ++v)
		{
			if(networkDB.count(v) != 0 && N.count(v) == 0)
			{
				int cost = D[v].second;
				if(cost < min)
				{
					min = cost;
					w = v;
				}
			}
		}

		N.insert(w);

		// update D(v) for all v adjacent to w and not in N:
		// 	D(v) = min( D(v), D(w) + c(w,v) )
		// /* new cost to v is either old cost to v or known shortest
		// path cost to w plus cost from w to v */
		for (int v = 1; v <= NBR_ROUTER; ++v)
		{
			if(networkDB.count(v) != 0)
			{
				if(isAdjacent(w, v) && N.count(v) == 0)
				{
					int Dv = D[v].second;
					int Dw = min;
					int Cwv = getCostForAdjacent(w, v);

					if(Dw + Cwv < Dv)
					{
						D[v] = std::make_pair(D[w].first, Dw + Cwv); //TODO is this right? Should it be w?
					}
				}
			}
		}
	}

	return D;
}

//
//End of Dijkstras's implementation
//

//Computes and logs the RIB for this router
void printRIB()
{
	std::map<int, std::pair<int,int> > costs = computeCostsToAllRouters(routerId);

	fout << "# RIB" << std::endl;

	for (int i = 1; i <= NBR_ROUTER; ++i)
	{
		if(networkDB.count(i) != 0)
		{
			fout << routerName << " -> " << "R" << i << " -> ";
			if(i == routerId) //If it is the local router
			{
				fout << "Local, 0" << std::endl;
			}
			else //Get info from Dijkstra's
			{
				if(costs[i].first != -1)
				{
					fout << costs[i].first << ", " << costs[i].second << std::endl;
				}
				else
				{
					fout << "N/A, " << "INFTY" << std::endl;
				}
			}	
		}
	}

	fout << std::endl;
}

//logs the link state database
void printTopology()
{
	fout << std::endl << "# Topology database" << std::endl;
	for (int i = 1; i <= NBR_ROUTER; ++i)
	{
		if(networkDB.count(i) != 0)
		{
			circuit_DB db = networkDB[i];
			fout << routerName << " -> R" << i << " nbr link " << db.nbr_link << std::endl;
			for (int j = 0; j < db.nbr_link; ++j)
			{
				fout << routerName << " -> R" << i << " link " << db.linkcost[j].link << " cost " << db.linkcost[j].cost <<  std::endl;
			}
		}
	}
	fout << std::endl;
}

//Receives the initial circuit_DB from the emulator
circuit_DB* recvCircuitDB()
{
	int dbSize = sizeof(struct circuit_DB);
	struct circuit_DB *cdb = (struct circuit_DB *) malloc(dbSize);

	recvBytes((char*)cdb, dbSize);

	fout << routerName << " received a circuit_DB: ";
	for (int i = 0; i < cdb->nbr_link; ++i)
	{
		fout << " link " << cdb->linkcost[i].link << " cost " << cdb->linkcost[i].cost << ", ";
	}
	fout << std::endl;

	return cdb;
}

//After receiving the initial circuit_DB, initialize all links
//in myLinks to be false. That is, have not yet received HELLO.
//Also updata linkEndpoints
void updateMyLinks()
{
	for (int i = 0; i < routerCDB->nbr_link; ++i)
	{
		int link = routerCDB->linkcost[i].link;
		myLinks[link] = false;
		linkEndpoints[link].insert(routerId);
	}
}

//Send a pkt_HELLO to each adjacent router
void sendGreetings()
{
	for(int i = 0; i < routerCDB->nbr_link; i++)
	{
		struct pkt_HELLO hello;
		hello.router_id = routerId;
		hello.link_id = routerCDB->linkcost[i].link;

		if(send(emuSock, &hello, sizeof(struct pkt_HELLO), 0) == -1)
		{
			throw SEND_ERROR;
		}

		fout << routerName << " sent greeting through link " << hello.link_id << std::endl;
	}
}

// 1.Send LSPUs as response to HELLO (one LSPDU for each pair in router_circuit_DB)
// 2. Log the sending of LSPDU
void sendLSPUs(int router, int link)
{
	for (int i = 0; i < routerCDB->nbr_link; ++i)
	{
		struct pkt_LSPDU lspdu;

		lspdu.sender = routerId;
		lspdu.router_id = routerId;
		lspdu.link_id = routerCDB->linkcost[i].link;
		lspdu.cost = routerCDB->linkcost[i].cost;
		lspdu.via = link;

		if(send(emuSock, &lspdu, sizeof(struct pkt_LSPDU), 0) == -1)
		{
			throw SEND_ERROR;
		}

		fout << routerName << " sent an LS PDU: sender " << lspdu.sender << ", router_id " << lspdu.router_id << ", link_id " << lspdu.link_id << ", cost " << lspdu.cost << ", via " << lspdu.via << std::endl;
	}

}

//Return the index of the link connected to this router
//return -1 if not found
int dBContains(circuit_DB &db, int link)
{
	int index = -1;

	for (int i = 0; i < db.nbr_link; ++i)
	{
		if(db.linkcost[i].link == link)
		{
			index = i;
			break;
		}
	}

	return index;
}

//Update the link state database with the new router, link, and cost
bool updateNetworkDB(int router, int link, int cost)
{
	bool changed = false;

	linkEndpoints[link].insert(router); //Update link endpoints

	if(networkDB.count(router) != 0) //if this router is in the db
	{
		circuit_DB &db = networkDB[router]; //get the db for this router
		
		int index = dBContains(db, link);
		if(index != -1) //check if it has this link
		{
			//check if the cost changed, otherwise do nothing
			if(db.linkcost[index].cost != cost)
			{
				changed = true;
				db.linkcost[index].cost = cost;
			}
		}
		else //if it doesnt have the link, add it
		{
			changed = true;
			int i = db.nbr_link;
			db.linkcost[i].link = link;
			db.linkcost[i].cost = cost;
			db.nbr_link++;
		}
	}
	else //if it is not in the db, then add it.
	{
		changed = true;

		struct circuit_DB db;
		db.nbr_link = 1;
		db.linkcost[0].cost = cost;
		db.linkcost[0].link = link;

		networkDB[router] = db;
	}

	return changed;
}

//forward and log PDU on neighboring links
//(except the one that send the LS PDU and those from which the router did not receive a HELLO packet yet)
void forwardPDU(struct pkt_LSPDU *lspdu)
{
	for (int i = 0; i < routerCDB->nbr_link; ++i)
	{
		int nbrLink = routerCDB->linkcost[i].link;

		if(myLinks[nbrLink] != false) //make sure we have received a hello
		{
			if(nbrLink != lspdu->via) //make sure it isn't the one that sent it
			{
				//update the pdu values
				struct pkt_LSPDU newPDU = *lspdu;
				newPDU.sender = routerId;
				newPDU.via = nbrLink;

				//Send the new pdu
				if(send(emuSock, &newPDU, sizeof(struct pkt_LSPDU), 0) == -1)
				{
					throw SEND_ERROR;
				}

				fout << routerName << " sent an LS PDU: sender " << newPDU.sender << ", router_id " << newPDU.router_id << ", link_id " << newPDU.link_id << ", cost " << newPDU.cost << ", via " << newPDU.via << std::endl;
			}
		}

	}
}

// 1. Log HELLO receipt
// 2. Update gotHello field in Mylinks
void handleHello(struct pkt_HELLO *hello)
{
	fout << routerName << " received HELLO from " <<  hello->router_id << " over link " << hello->link_id << std::endl;

	//update myLinks
	myLinks[hello->link_id] = true;

	//Send and log one lspu for each entry in routerCDB
	sendLSPUs(hello->router_id, hello->link_id);

}

// 1. Log the receipt of LSPDU
// 2. Update router_network_DB using the router ID, link ID and cost in the PDU
void handleLSPDU(struct pkt_LSPDU *lspdu)
{
	fout << routerName << " received an LS PDU: sender " << lspdu->sender << ", router_id " << lspdu->router_id << ", link_id " << lspdu->link_id << ", cost " << lspdu->cost << ", via " << lspdu->via << std::endl;
	
	//update networkDB
	//Did any link cost change?
	// no -> return
	// yes -> log networkDB, forward and log PDU on neighboring links (except the one that send the LS PDU and those from which the router did not receive a HELLO packet yet)
	if(updateNetworkDB(lspdu->router_id, lspdu->link_id, lspdu->cost))
	{
		printTopology();
		printRIB();
		forwardPDU(lspdu);
	}
	else
	{
		return;
	}
}

//Waits to receive either a pkt_HELLO or a pkt_LSPDU
//Once one is received, the appropriate handler is called
void handleMessage()
{
	int helloSize = sizeof(struct pkt_HELLO);
	int lspduSize = sizeof(struct pkt_LSPDU);
	char* buf = (char*) malloc(lspduSize);

	//Receive message of either type and check size to determine type
	int recvdSize = recvBytes(buf, lspduSize);

	if(recvdSize == helloSize)
	{
		handleHello((pkt_HELLO*) buf);
	}
	else if(recvdSize == lspduSize)
	{
		handleLSPDU((pkt_LSPDU*) buf);
	}
	else
	{
		free(buf);
		throw RUNTIME_LOGIC_ERROR;
	}

	free(buf);
}

int main(int argc, char const *argv[])
{
	try
	{
	parseArgs(argc,argv);
	initVars();

	emuSock = OutgoingSocketManager::connectTo(emuHost, emuPort);

	sendInit();

	routerCDB = recvCircuitDB();
	networkDB[routerId] = *routerCDB; //update networkDB
	updateMyLinks();

	printTopology();

	sendGreetings();

	while(true)
	{
		handleMessage();
	}

	free(routerCDB);
	
	}
	catch(MY_EXCEPTION e)
	{
		printException(e);
	}

	fout.close();	

	return 0;
}





