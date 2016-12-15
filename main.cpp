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

using namespace std;
#define INFTY 2147483647;

class LSP{
public:
    int RouterID;
    int SeqNum;
    int TTL;
    map<string,int> ReachableNetwork;
    LSP(int id,int seq){
        RouterID=id;
        SeqNum=seq;
        TTL=10;
    }
};

class ConnectedRouter
{
public:
	int RouterID;
	int Cost;
	
	ConnectedRouter(int routerId, int cost)
		: RouterID(routerId), Cost(cost) {}
};

class Router{
public:
    int ID;
    string Network;
    int Cost;
    int LSPNum;
    vector<ConnectedRouter> ConnectedRouters;
    

    // TODO: we don't need this
    vector <Router> ConnectedNetwork;

    
    list<pair<int, int>>NetworkGraph;
    map<int,int> ReceivedLSP;
    
    
    // init a router with invalid values
    // must declare this function for std::map
    Router() {
    	ID = -1;
    	Cost = -1;
    	LSPNum = 0;
	}
    
    Router(int id,string net,int cost){
        ID=id;
        Network=net;
        Cost=cost;
        LSPNum=0;
    }
    
    void AddNum(){
        LSPNum=LSPNum+1;
    }
    
    void ReceiveLSP(LSP lsp){
        LSP t=lsp;
        t.TTL--;
        if(t.TTL>0&&(ReceivedLSP.find(t.RouterID)==ReceivedLSP.end()||(ReceivedLSP.find(t.RouterID)!=ReceivedLSP.end()&&ReceivedLSP[lsp.RouterID]<t.SeqNum))){
            ReceivedLSP[lsp.RouterID]=t.SeqNum;
            for(int i=0;i<ConnectedNetwork.size();i++){
                ConnectedNetwork[i].ReceiveLSP(t);
            }
        }
    }
    
    LSP OriginateLSP(){
        AddNum();
        LSP lsp(ID,LSPNum);
        for(int i=0;i<ConnectedNetwork.size();i++){
            lsp.ReachableNetwork.insert(pair<string,int>(ConnectedNetwork[i].Network,ConnectedNetwork[i].Cost));
        }
        return lsp;
    }
    
};

map<int, Router> g_routers;

void initRouters(map<int, Router>& routers) {
	
	Router router;
	
	ifstream ifs("infile.dat");
	string line;
	while (!ifs.eof()) {
		getline(ifs, line);
		//cout << line << endl;
		
		
		if (line[0] != ' ') {
			
			// add router to router list
			if (router.ID >= 0) {
				routers[router.ID] = router;
			}
			
			// create new router
			router = Router();
			
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
			
			router.ID = routerId;
			router.Network = networkName;
			router.Cost = cost;
			//cout << "rid: " << routerId << " netName: " << networkName << endl;
			
		} else {
			
			stringstream ss(line);
			
			int neighborRouterId = -1;
			int cost;
			
			string word;
			while (ss >> word) {
				if (neighborRouterId < 0) {
					neighborRouterId = atoi(word.c_str());
				} else {
					cost = atoi(word.c_str());
				}
			}
			
			router.ConnectedRouters.push_back(
				ConnectedRouter(neighborRouterId, cost));
			//cout << "  neighbor: " << neighborRouterId << endl;
		}
	}
	
}

int main() {
	
	initRouters(g_routers);
	
    cout << '1'<<endl;
    return 0;
}


