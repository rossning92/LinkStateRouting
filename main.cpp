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

class Router{
public:
    int ID;
    string Network;
    int Cost;
    int LSPNum;
    vector <Router> ConnectedNetwork;
    list<pair<int, int>>NetworkGraph;
    map<int,int> ReceivedLSP;
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
                .ReceiveLSP(t);
            }
        }
    }
    LSP OriginateLSP(){
        AddNum();
        LSP lsp(ID,LSPNum);
        for(map<string,int>::iterator it=ConnectedNetwork.begin();it!=ConnectedNetwork.end();it++){
            lsp.ReachableNetwork.insert(pair<string,int>(it->first,it->second));
        }
        return lsp;
    }
    
};




int main() {
    int i=12;
    cout << to_string(12)<<endl;
    return 0;
}


