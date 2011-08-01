#include <iostream>
#include <fstream>
#include <sstream>
#include "mwsConfigReader.h"

using namespace std;

mwsConfigReader::mwsConfigReader(string filen){
    filename = filen;
    readconf(filename,data);
}

bool mwsConfigReader::readconf(string filen,map<string,map<string ,string> > &data){
   ifstream fin(filen.c_str());
   string passa;
   string awd;
   int pos = -1;
   if(fin){
        string line;
        string name;
      while (getline(fin, line)) {
        int pose=-1;
        pos = line.find("[");
        pose = line.find("]");
        if(pos < 0){
            continue;
        }
        if(pose < 0){
            pose = line.size();
        }
        istringstream names(line.substr(pos+1,pose -1));
        names >> name;
        map<string ,string> values;
        pos = -1;
        while (getline(fin, line)) {
            pos = line.find(":");
            if(pos < 0){
                break;
            }
            istringstream attrib(line.substr(0,pos));
            istringstream value(line.substr(pos + 1));
            attrib >> passa;
            value >> awd;
            values[passa] = awd;
        }
        data[name] = values;
     }
   fin.close();
   return true;
   }else{
       return false;
   }
}

map<string ,map<string , string > > mwsConfigReader::getAll(){
        return data;
}

map<string ,string > mwsConfigReader::getScope(string scope){
    return data[scope];
}

string mwsConfigReader::getAttrib(string scope,string attrib){
    return data[scope][attrib];
}
