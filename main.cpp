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
    int ID;
    string Net;
    int Cost;
    int LSPNum;
    int Tick;
    map <int,int> DirectConRouter;
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
        NetGraph[0].push_back(pair<string,int>(net,cost));
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
        }
    }
    
    void OriginateLSP(map<int,Router> routers){
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
            routers[it->first].ReceiveLSP(lsp);
        }
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
    }
    
}

int main() {
    map<int, Router> g_routers;
    initRouters(g_routers);
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
            
            //cout << 'c' << endl;
            
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


