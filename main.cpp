//
//  main.cpp
//  Link State Routing
//
//  Created by Yiwei Chen on 12/2/16.
//  Copyright © 2016 Project Group 9. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <cstdio>
#include <assert.h>
#include <set>
#include <limits.h>
#include <exception>


#define DEBUG_PRINT
#define INF 10000 // adding integer to too big INF will cause overflow


using namespace std;

class LSP{
public:
    int RouterID;
    int SeqNum;
    int TTL;

	map<int, int> ConnRouters; // connected router id => link cost
	string NetworkName;

    LSP(int id, int seq, string networkName){
        RouterID=id;
        SeqNum=seq;
        TTL=10;
		NetworkName = networkName;
    }
};

class Router{
public:
	typedef int ROUTER_ID;
	typedef int LINK_COST;

    static map<int, Router> Routers;
    
	bool IsOff;
    int ID;
    string Net;
    int Cost;
    int LSPNum;
    int Tick;
    map<int, int> DirectConRouter; // map from : router ID => link cost
    
	map<int, pair<int,int>> ReceivedLSP;
	map<string, pair<LINK_COST, ROUTER_ID>> RoutingTable;
	map<ROUTER_ID, map<ROUTER_ID, LINK_COST>> NetGraph;
	map<ROUTER_ID, string> RouterNetMap;
    
    // init a router with invalid values
    // must declare this function for std::map
    Router() {
        ID = -1;
        Cost = -1;
        LSPNum = 0;
        Tick = 0;
		IsOff = false;
    }
    
    Router(int id,string net,int cost){
        ID=id;
        Net=net;
        Cost=cost;
        LSPNum=0;
        Tick=0;
		IsOff = false;
    }
    void AddConRouter(int a,int b){
        DirectConRouter.insert(pair<int,int>(a,b));
    }
    void AddNum(){
        LSPNum=LSPNum+1;
    }
    void AddTick(){
        Tick=Tick+1;
    }

	/*
		fromRouterId: must use this ID to identify from which router the packet sent
	*/
    void ReceiveLSP(LSP lsp, int fromRouterId) {
		if (IsOff) return;
		
		lsp.TTL--;

		// indicates whether this lsp has been received
		bool receivedLsp = ReceivedLSP.find(lsp.RouterID) != ReceivedLSP.end() 
			&& ReceivedLSP[lsp.RouterID].first >= lsp.SeqNum;

		if (lsp.TTL > 0 && !receivedLsp) {
            ReceivedLSP[lsp.RouterID].first = lsp.SeqNum;
            ReceivedLSP[lsp.RouterID].second = Tick;
            
			bool updated = UpdateGraph(lsp);
			
			if (updated) {
				UpdateRoutingTable();
			}

            // forward current LSP
            for (auto it = DirectConRouter.begin(); it != DirectConRouter.end(); it++) {
                int connRouterId = it->first;
				Router* r = GetRouterById(connRouterId);
				assert(r);

				if (connRouterId != fromRouterId) {
					r->ReceiveLSP(lsp, ID);
				}
            }
        }
    }
    
    static Router* GetRouterById(int routerId) {
        auto it = Routers.find(routerId);
        if (it == Routers.end()) return NULL;
        else return &it->second;
    }
    
    void OriginateLSP() {

		if (IsOff) return;

		AddNum();
        LSP lsp(ID, LSPNum, Net);
		//lsp.ConnRouters = DirectConRouter;

		
 		AddTick();
		for (auto it = ReceivedLSP.begin(); it != ReceivedLSP.end(); it++){
            if (Tick - (it->second.second) > 2) {
				RemoveGraphNode(it->first);
				UpdateRoutingTable();
            }
        }

		for (auto it = DirectConRouter.begin(); it != DirectConRouter.end(); it++) {
			Router* r = GetRouterById(it->first);
			assert(r);
			if (!r->IsShutdown()) {
				lsp.ConnRouters[it->first] = it->second;
			}
		}


		// send out original LSP
        for(map<int,int>::iterator it=DirectConRouter.begin();it!=DirectConRouter.end();it++){
			int routerId = it->first;
			Router* r = GetRouterById(routerId);
			assert(r);

			if (it->second < INF) {
				r->ReceiveLSP(lsp, this->ID);
			}
        }
    }
    
    static void InitRouters() {
        Router router;
        
        ifstream ifs("infile.dat");
		if (!ifs) {
			throw "Cannot open file infile.dat";
		}

        string line;
        while (!ifs.eof()) {

            getline(ifs, line);
			//cout << line << endl;

            // read <routerId, networkName, networkCost>
            if (line[0] != ' ') {

                // create new router
                stringstream ss(line);
                
                int routerId = -1;
                string networkName;
                int cost = 1; // default value of cost
                
                string word;
                while (ss >> word) {
                    if (routerId < 0) {
                        routerId = atoi(word.c_str());
                    } else if (networkName.empty()) {
                        networkName = word;
                    } else {
                        cost = atoi(word.c_str());
                    }
                }
                router = Router(routerId,networkName,cost);
                //cout << "rid: " << routerId << " netName: " << networkName << endl;
                
            } else {
                
                stringstream ss(line);
                
                int neighborRouterId = -1;
                int cost=1;
                
                string word;
                while (ss >> word) {
                    if (neighborRouterId < 0) {
                        neighborRouterId = atoi(word.c_str());
                    } else {
                        cost = atoi(word.c_str());
                    }
                }
                router.AddConRouter(neighborRouterId, cost);
                //cout << "  neighbor: " << neighborRouterId << endl;
            }

			// add router to router list
			if (ifs.peek() == EOF || ifs.peek() != ' ') {
				assert(router.ID >= 0);
				Routers[router.ID] = router;
			}
        }
    }
    

	void PrintRoutingTable()
	{
		if (IsOff) {
			cout << endl << endl;
			cout << "[ERROR] Router #" << ID << " is shutdown" << endl;
			cout << endl << endl;
			return;
		}

		cout << endl << endl;
		cout << "[ ROUTING TABLE FOR ROUTER #" << ID << " ]" << endl;
		for (auto it = RoutingTable.begin(); it != RoutingTable.end(); it++) {
			cout << it->first << " - ";
			cout << it->second.first << " - ";
			cout << it->second.second << endl;
		}
		cout << endl;
	}

	void Shutdown()
	{
		IsOff = true;
		cout << "Router #" << ID << " has been shutdown" << endl;
	}

	void Startup()
	{
		IsOff = false;
		cout << "Router #" << ID << " starts up" << endl;
	}

	bool IsShutdown() {
		return IsOff;
	}

private:

	// returns true if graph was updated
	bool UpdateGraph(const LSP& lsp)
	{
		bool updated = false;

		// save network name
		if (RouterNetMap.find(lsp.RouterID) == RouterNetMap.end()) {
			updated = true;
			RouterNetMap[lsp.RouterID] = lsp.NetworkName;
		}

		for (auto it = lsp.ConnRouters.begin(); it != lsp.ConnRouters.end(); it++) {
			int rid = it->first; // connected router id
			int cost = it->second;

			if (AddGraphEdge(lsp.RouterID, rid, cost)) {
				updated = true;
			}
		}

		return updated;
	}

	// returns -1 if edge doesn't exist
	LINK_COST GetGraphEdge(ROUTER_ID r0, ROUTER_ID r1) {
		auto it1 = NetGraph.find(r0);
		if (it1 == NetGraph.end()) return -1;
		
		auto it2 = it1->second.find(r1);
		if (it2 == it1->second.end()) return -1;

		return it2->second;
	}

	// returns true if new edge was added or new cost value was updated
	bool AddGraphEdge(ROUTER_ID r0, ROUTER_ID r1, LINK_COST cost) {
		if (GetGraphEdge(r0, r1) == cost) {
			return false;
		} else {
			NetGraph[r0][r1] = cost;
			NetGraph[r1][r0] = cost;
			return true;
		}
	}

	void UpdateRoutingTable()
	{
		// TODO: update routing table by running Dijkstra algorithm
		auto& edges = NetGraph[ID];

		// keys are sorted ascendingly by link cost
		map<ROUTER_ID, LINK_COST> distTo;
		map<ROUTER_ID, ROUTER_ID> predNode; // predecessor node of current shortest path to ROUTER_ID

		// initialize distTo
		for (auto it = NetGraph.begin(); it != NetGraph.end(); it++) {
			ROUTER_ID rid = it->first;
			if (rid == ID) continue;
			distTo[rid] = INF;
		}
		for (auto it = edges.begin(); it != edges.end(); it++) {
			ROUTER_ID rid = it->first;
			LINK_COST cost = it->second;
			distTo[rid] = cost;
			predNode[rid] = ID;
		}

		set<ROUTER_ID> solved;
		while (solved.size() < distTo.size()) {

			// find router with minimal distance
			ROUTER_ID rx = -1;
			LINK_COST dx = INF;
			for (auto it = distTo.begin(); it != distTo.end(); it++) {
				bool unsolved = solved.find(it->first) == solved.end();
				if (unsolved && it->second < dx) {
					dx = it->second;
					rx = it->first;
					solved.insert(rx);
				}
			}

			// if all routers are non-reachable
			if (rx == -1) break;
			
			// relax all edges from rx
			for (auto it = distTo.begin(); it != distTo.end(); it++) {
				if (solved.find(it->first) != solved.end()) continue;
				
				ROUTER_ID ry = it->first;
				LINK_COST dy = it->second;
				LINK_COST xy = GetGraphEdge(rx, ry);
				if (xy == -1) continue;

				if (dx + xy < dy) {
					it->second = dx + xy; // update dy
					predNode[ry] = rx;
				}
			}
		}

		RoutingTable.clear();

		// print for debug
#ifdef DEBUG_PRINT
		cout << "Router#" << ID << ": ";
#endif
		for (auto it = distTo.begin(); it != distTo.end(); it++) {
			
#ifdef DEBUG_PRINT
			cout << it->first << ",";
			if (it->second == INF) {
				cout << "-";
			} else {
				cout << it->second;
			}
#endif
			
			// print second node in path
			int nextHop = -1;
			
			if (it->second < INF) {
				list<ROUTER_ID> path;
				ROUTER_ID rid = it->first;
				path.push_front(rid);
				while (true) {
					if (predNode.find(rid) == predNode.end()) break;
					rid = predNode[rid];

					path.push_front(rid);
					if (rid == ID) break;
				}

				assert(path.size() > 1);
				nextHop = *(++path.begin());
			}

#ifdef DEBUG_PRINT
			cout << ",";
			if (it->second == INF) {
				cout << "-";
			} else {
				cout << nextHop;
			}
			cout << " | ";
#endif

			if (it->second < INF) {
				if (RouterNetMap.find(it->first) != RouterNetMap.end()) {
					const string& net = RouterNetMap[it->first];
					RoutingTable[net] = pair<int, int>(it->second, nextHop);
				}
			}

		}

#ifdef DEBUG_PRINT
		cout << endl;
#endif
	}

	void RemoveGraphNode(int routerId)
	{
		NetGraph.erase(routerId);
		for (auto it = NetGraph.begin(); it != NetGraph.end(); it++) {
			it->second.erase(routerId);
		}
	}

};

map<int, Router> Router::Routers;


Router* GetRouterByUserInput()
{
	cout << "Please input router ID: ";
	int rid = -1;
	cin >> rid;


	Router* r = Router::GetRouterById(rid);
	if (!r) {
		cout << "ERROR: Wrong router id" << endl;
	}

	return r;
}

int main() {
    Router::InitRouters();
    
    cout << "Routers Created!" << endl;
    
    while (true) {
        
        cout << "+-------------------------------------+\n"
			<< "| C: continue                         |\n"
			<< "| Q: quit                             |\n"
			<< "| P: print routing table by router-id |\n"
			<< "| S: shutdown router by router-id     |\n"
			<< "| T: start up router by router-id     |\n"
			<< "|                                     |\n"
			<< "| Please press key to continue...     |\n"
			<< "+-------------------------------------+\n";
        
        char key;
        cin >> key;
        if (key == 'c') {
            
			for (auto it = Router::Routers.begin(); it != Router::Routers.end(); it++) {
				auto& router = it->second;
				router.OriginateLSP();
			}
            
        } else if (key == 'q') {
            cout << "Bye!" << endl;
            break;
        } else if (key == 'p') {
			Router* r = GetRouterByUserInput();
			if (!r) continue;
			r->PrintRoutingTable();
        } else if (key == 's') {
			Router* r = GetRouterByUserInput();
			if (!r) continue;
			r->Shutdown();
        } else if (key == 't') {
			Router* r = GetRouterByUserInput();
			if (!r) continue;
			r->Startup();
        }
    }
    
    return 0;
}
