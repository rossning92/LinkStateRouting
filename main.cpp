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

using namespace std;
#define INFTY 2147483647;

class LSP{
public:
    int RouterID;
    int SeqNum;
    int TTL;
    map<string,int> ReachNet;
    LSP(int id,int seq){
        RouterID=id;
        SeqNum=seq;
        TTL=10;
    }
    void AddNet(string w,int b){
        ReachNet.insert(pair<string,int>(w,b));
    }
};

class Router{
public:
    static map<int, Router> Routers;
    
    int ID;
    string Net;
    int Cost;
    int LSPNum;
    int Tick;
    map<int, int> DirectConRouter; // map from : router ID => link cost
    map<int,pair<int,int>> ReceivedLSP;
    map<string,pair<int,int>>RoutingTable;
    vector<list<pair<string, int>>> NetGraph;
    
    
    // init a router with invalid values
    // must declare this function for std::map
    Router() {
        ID = -1;
        Cost = -1;
        LSPNum = 0;
        Tick = 0;
    }
    
    Router(int id,string net,int cost){
        ID=id;
        Net=net;
        Cost=cost;
        LSPNum=0;
        Tick=0;
        
        // XXX: 
        // NetGraph[0].push_back(pair<string,int>(net,cost));
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
    void ReceiveLSP(LSP lsp){
        LSP t=lsp;
        t.TTL--;
        if(t.TTL>0&&(ReceivedLSP.find(t.RouterID)==ReceivedLSP.end()||(ReceivedLSP.find(t.RouterID)!=ReceivedLSP.end()&&ReceivedLSP[lsp.RouterID].first<t.SeqNum))){
            ReceivedLSP[lsp.RouterID].first=t.SeqNum;
            ReceivedLSP[lsp.RouterID].second=Tick;
            
            //todo: how to send lsp?
            for (auto it = DirectConRouter.begin(); it != DirectConRouter.end(); it++) {
                int connRouterId = it->first;
                
                
            }
        }
    }
    
    static Router* GetRouterById(int routerId) {
        auto it = Routers.find(routerId);
        if (it == Routers.end()) return NULL;
        else return &it->second;
    }
    
    void OriginateLSP() {
        AddNum();
        LSP lsp(ID,LSPNum);
        lsp.AddNet(Net, Cost);
        AddTick();
        for(map<int,pair<int,int>>::iterator iter=ReceivedLSP.begin();iter!=ReceivedLSP.end();iter++){
            if(Tick-(iter->second.second)>=2){
                DirectConRouter[iter->first]=INFTY;
            }
        }
        for(map<int,int>::iterator it=DirectConRouter.begin();it!=DirectConRouter.end();it++){
			int routerId = it->first;
			Router* r = GetRouterById(routerId);
			assert(r);
			r->ReceiveLSP(lsp);
        }
    }
    
    static void InitRouters() {
        Router router;
        
        ifstream ifs("infile.dat");
		if (!ifs) {
			throw exception("Cannot open file infile.dat");
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
    
};

map<int, Router> Router::Routers;


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
				auto router = it->second;
				router.OriginateLSP();
			}
            
        } else if (key == 'q') {
            cout << "Bye!" << endl;
            break;
        } else if (key == 'p') {
            cout << "[print routing table]" << endl;
        } else if (key == 's') {
            cout << "[shutdown router]" << endl;
        } else if (key == 't') {
            cout << "[startup router]" << endl;
        }
    }
    
    cout << '1'<<endl;
    
    return 0;
}


