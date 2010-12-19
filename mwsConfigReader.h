#ifndef __mwsConfigReader__
#define __mwsConfigReader__

class mwsConfigReader {
   private:
     std::map<std::string ,std::map<std::string , std::string > > data;
     std::string filename;
     bool readconf(std::string ,std::map<std::string ,std::map<std::string , std::string > > &);
   public :
     mwsConfigReader(std::string filen);  
     std::string getAttrib(std::string scope,std::string attrib);
     std::map<std::string ,std::string> getScope(std::string scope);
     std::map<std::string ,std::map<std::string , std::string > > getAll(); 
};

#endif
