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
    map<int,pair<string,int>> ReachNet;
    LSP(int id,int seq){
        RouterID=id;
        SeqNum=seq;
        TTL=10;
    }
    void AddNet(int a,string w,int b){
        pair<string,int>p(w,b);
        ReachNet.insert(pair<int,pair<string,int>>(a,p));
    }
};

/*class ConnectedRouter
 {
 public:
	int RouterID;
	int Cost;
	
	ConnectedRouter(int routerId, int cost)
 : RouterID(routerId), Cost(cost) {}
 };*/

class Router{
public:
    int ID;
    string Net;
    int Cost;
    int LSPNum;
    map <int,int> DirectConRouter;
    map<int,int> ReceivedLSP;
    map<string,pair<int,int>>RoutingTable;
    //to be considered
    list<pair<int, int>>NetGraph;
    
    
    //vector<ConnectedRouter> ConnectedRouters;
    
    
    // init a router with invalid values
    // must declare this function for std::map
    Router() {
        ID = -1;
        Cost = -1;
        LSPNum = 0;
    }
    
    Router(int id,string net,int cost){
        ID=id;
        Net=net;
        Cost=cost;
        LSPNum=0;
    }
    void AddConRouter(int a,int b){
        DirectConRouter.insert(pair<int,int>(a,b));
    }
    void AddNum(){
        LSPNum=LSPNum+1;
    }
    void ReceiveLSP(LSP lsp){
        LSP t=lsp;
        t.TTL--;
        if(t.TTL>0&&(ReceivedLSP.find(t.RouterID)==ReceivedLSP.end()||(ReceivedLSP.find(t.RouterID)!=ReceivedLSP.end()&&ReceivedLSP[lsp.RouterID]<t.SeqNum))){
            ReceivedLSP[lsp.RouterID]=t.SeqNum;
            //todo: how to sand lsp?
        }
    }
    
    LSP OriginateLSP(){
        AddNum();
        LSP lsp(ID,LSPNum);
        //
        return lsp;
    }
    
};


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
            router.Net = networkName;
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
            
            router.DirectConRouter.insert(pair<int,int>(neighborRouterId, cost));
            //cout << "  neighbor: " << neighborRouterId << endl;
        }
    }
    
}

int main() {
    map<int, Router> g_routers;
    initRouters(g_routers);
    
    cout <<'Routers Created!'<<endl;
    return 0;
}


