/*
 * Celesta
 *
 * Copyright (c) 2016 Manuel Deneu
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

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
