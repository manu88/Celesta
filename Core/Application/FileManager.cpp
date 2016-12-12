//
//  FileManager.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 29/10/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "FileManager.h"
#include "../Env_Variables.h"
#include "../Internal/FileSystem.h"

FileManager::FileManager(const std::string &rootFolder ):
Task( UpdateEvery, ClassNames::FileManager ),
_rootFolder ( FileSystem::correctPathIfNeeded( rootFolder ) )
{
    className = ClassNames::FileManager;
    
    _fileList = FileSystem::getFilesListFromFolder( _rootFolder, true);
    
}

FileManager::~FileManager()
{}

void FileManager::doTask()
{
    const auto list = FileSystem::getFilesListFromFolder( _rootFolder, true);

    if( list != _fileList )
    {
        Dispatch::getInstance()->pushNotification( new FileManager::Notification( this ));   
    }
    

    
    _fileList = list;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::string FileManager::getRessourceNamed( const std::string &name , const std::string &extention /*= ""*/) const
{
    std::string ret;
    for( const auto &file : FileSystem::getFilesListFromFolder( _rootFolder , true ) )
    {
        
        const std::string _name = FileSystem::getFileName( file );

        if( _name == name  && (extention.empty() || ( !extention.empty() && FileSystem::getFileExtension(file) == extention  ) ))
        {
            const std::string path = FileSystem::getPathRelativeTo(file, _rootFolder );
            
            return path;
        }
    }
    
    return ret;
}

bool FileManager::ressourceNamedExists( const std::string &name) const
{
    return !getRessourceNamed( name ).empty();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FileManager::createDirectory( const std::string &path)
{
    return FileSystem::createDir(_rootFolder + path );
}

bool FileManager::deleteDirectory( const std::string &path)
{
    return FileSystem::removeDir( _rootFolder + path );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FileManager::createFile( const std::string &file)
{
    return FileSystem::createFile( _rootFolder +file );
}

bool FileManager::deleteFile( const std::string &file)
{
    return FileSystem::removeFile( _rootFolder + file );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::string FileManager::readFile( const std::string &file ) const
{
    return FileSystem::getFileText( _rootFolder + file );
}

bool FileManager::writeFile( const std::string &file , const std::string &data)
{
    return FileSystem::setFileText( _rootFolder + file, data);
}

bool FileManager::appendToFile( const std::string &file , const std::string &data)
{
    return FileSystem::appendTextToFile( _rootFolder + file, data);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant FileManager::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall)
{
    if ( validCall )
        *validCall = true;
    
    if( selector == "getRessourceNamed" )
    {
        std::string ret;
        
        if( arguments.size() == 1)
            ret = getRessourceNamed(arguments.at(0).getString() );
        else if( arguments.size() == 2 )
            ret = getRessourceNamed(arguments.at(0).getString(), arguments.at(1).getString() );
        
        return Variant( ret );
    }
    else if( selector == "createDirectory")
    {
        return  Variant( createDirectory( arguments.at(0).getString() ));
    }
    else if (selector == "deleteDirectory")
    {
        return  Variant( deleteDirectory( arguments.at(0).getString() ));
    }
    else if( selector == "createFile")
    {
        return  Variant( createFile( arguments.at(0).getString() ));
    }
    else if (selector == "deleteFile")
    {
        return  Variant( deleteFile( arguments.at(0).getString() ));
    }
    
    if ( validCall )
        *validCall = false;

    
    return Element::performSelectorWithArguments(selector, arguments , validCall );
    
}
#endif
