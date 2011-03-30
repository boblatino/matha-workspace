#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <map>
#include "mwsConfigReader.h"

using namespace std;

mwsConfigReader::mwsConfigReader(mws_string filen){
    filename = filen;
    readconf(filename,data);
}

bool mwsConfigReader::readconf(mws_string filen, mws_scopemap &data){
    ifstream fin(filen.c_str());
    mws_string passa;
    mws_string awd;
    int pos = -1;
    if(fin){   
        mws_string line;
        mws_string name;
        int uni = 0;
        while (getline(fin, line)) {
            int pose=-1;
            uni = 0;
            pos = line.find("[");
            pose = line.find("]");
            if(pos < 0){
                continue;
            }
            if(pose == 0){
                pose = line.size();
            }
            istringstream names(line.substr(pos+1,pose -1));
            names >> name;
            mws_attrmap values;
            pos = -1;
            while (getline(fin, line)) {
                pos = line.find(":");
                if(pos < 0 ){
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

mws_scopemap mwsConfigReader::getAll(){
    return data;
}

mws_attrmap mwsConfigReader::getScope(mws_string scope){
    return data[scope];
}

mws_string mwsConfigReader::getAttrib(mws_string scope,mws_string attrib){
    return data[scope][attrib];
}
