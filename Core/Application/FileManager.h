//
//  FileManager.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 29/10/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef FileManager_hpp
#define FileManager_hpp

#include <stdio.h>

#include "../Internal/Element.h"
#include "../Scheduler/Dispatch.h"


class FileManager : public Element , public Dispatch::Task
{
public:
    
    enum { UpdateEvery  = 1000 };
    
    class Notification : public Event
    {
        
    public:
        Notification( Element *sender):
        Event( Event_FileManager , sender )
        {}
        
        ~Notification() {}
    };
    
    FileManager( const std::string &rootFolder);
    virtual ~FileManager();
    
    const std::string getRessourceNamed( const std::string &name , const std::string &extention = "" ) const;
    bool ressourceNamedExists( const std::string &name) const;
    
    bool createDirectory( const std::string &path);
    bool deleteDirectory( const std::string &path);
    
    bool createFile( const std::string &file);
    bool deleteFile( const std::string &file);
    
    const std::string readFile( const std::string &file ) const;
    bool writeFile( const std::string &file , const std::string &data);
    bool appendToFile( const std::string &file , const std::string &data);

    /**/
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
private:

    void doTask();
    
    std::string _rootFolder;
    
    std::vector< std::string > _fileList;
    
};

#endif /* FileManager_hpp */
