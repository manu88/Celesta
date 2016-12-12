//
//  Url.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 18/10/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//



#include "Url.h"
//#include <curl/curl.h> // after Url.h !!
#include "../Env_Variables.h"
#include "../Scheduler/Dispatch.h"

#include "../Internal/FileSystem.h"
#include "../Env_Variables.h"


void UrlDelegate::urlRequestDidProgressTo( Url* , float  )
{
    
}

// optional
void UrlDelegate::urlRequestDidEnd( Url* , int  )
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Url::s_pendingCount = 0;

Url::Url() :
Thread( ClassNames::Url ),
_threadedFetch ( true ),
_delegate    ( nullptr ),


_fileToWrite ( nullptr ),
_curlCode    ( -1 )
{
    
#ifdef ENABLE_ELEMENT_SELECTOR
    _progressTarget = nullptr;
    _endTarget      = nullptr;
#endif
    
    className = ClassNames::Url;
    setElementName( ClassNames::Url );
    
    _state = Unknown;
    
    addTypeInfo( Network );
}

Url::~Url()
{
    
}

size_t Url::write_data(void *ptr, size_t size, size_t nmemb, void *userData)
{
    Url *self = reinterpret_cast<Url*>(userData);
    
    size_t written = fwrite(ptr, size, nmemb, self->_fileToWrite );
    

    return written;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Url::older_progress(void *clientp, double dltotal, double dlnow, double /*ultotal*/, double /*ulnow*/)
{
    Url *self = reinterpret_cast<Url*>( clientp );
    
    if( self)
    {
        if  ( dltotal != 0 )
            self->progressChangedTo( static_cast<float>( dlnow/dltotal ) );
        
    }
    return 0;
}

int Url::progress_callback(void *clientp,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t /*ultotal*/,   curl_off_t /*ulnow*/)
{
    Url *self = reinterpret_cast<Url*>( clientp );

    if( self)
    {
        if  ( dltotal != 0 )
            self->progressChangedTo( static_cast<float>( dlnow/dltotal ) );

    }
    return 0; // ok
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Url::internalFetch()
{

    const std::string tempFile = FileSystem::correctPathIfNeeded(_tempPath) + _name;

    _fileToWrite = fopen( tempFile.c_str() ,"wb");
    
    if( !_fileToWrite )
    {
        _state = CopyError;
        
        return false;
    }
    
    /* Start download */
    
    s_pendingCount++;
    
    CURLcode res;
    CURL * curl = curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_URL, _url.c_str() );
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    
    /* Progress */
    
    const auto l = StringOperations::split( curl_version_info( CURLVERSION_NOW )->version, '.');
    const int version = std::stoi( l[0] )*100 +  std::stoi( l[1] );
    
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0 );
    
    if( version < 732) // old school progress callback
    {
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA , this );
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, older_progress );
    }
    else
    {
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
    }
    
    char *ct = NULL;
    
    _state = Fetching;
    
    res = curl_easy_perform(curl);

    fclose(_fileToWrite);
    
    if( res != CURLE_OK)
    {
        _state = DownloadError;
    }
    else // OK
    {
        _state = Downloaded;

        res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    
        if( ct != NULL)
            _mimeType = ct;
        
        const bool ret = writeDestFile();
        
        _curlCode = res;
        
        if( !ret )
            _state = CopyError;
        else
            _state = Copied;

        _state = ReadyToUse;
        
    }
    // clean

    curl_easy_cleanup( curl );
    
    didEnd( _state );
    
    s_pendingCount--;
    
    return (_state == ReadyToUse ) && (_curlCode == CURLE_OK);
}

/* Threaded fetch */
void Url::run()
{
    internalFetch();
}

bool Url::startFetchContent( )
{
    if( _threadedFetch )
        return startThread();
    
    return internalFetch();
}

void Url::progressChangedTo( float progress)
{
    static int count = 0;
    
    if( count++ > SendPercentEvery )
    {
        count = 0;
        if( _delegate )
            _delegate->urlRequestDidProgressTo( this, progress );
        
        if( !_progressSelector.empty() )
        {
            if( _threadedFetch)
                Dispatch::getInstance()->performAsync(this, _progressTarget , _progressSelector, {Variant( progress ) } );
            else
                _progressTarget->performSelectorWithArguments(_progressSelector, {Variant( progress ) }  );

        }
    }
}

void Url::didEnd ( int code )
{
    if( _delegate )
        _delegate->urlRequestDidEnd( this , code);
    
    if( !_endSelector.empty() )
    {
        if( _threadedFetch)
            Dispatch::getInstance()->performAsync(this,_endTarget , _endSelector, {Variant( code ) } );
        else
            _endTarget->performSelectorWithArguments(_endSelector, {Variant( code ) });
    }
}

bool Url::writeDestFile()
{
    const std::string tempFile = FileSystem::correctPathIfNeeded(_tempPath) + _name;
    const std::string ext = FileSystem::getExtensionWithMIME(_mimeType);
/*
    printf("\n");
    printf("MIME Type : '%s' \n" , _mimeType.c_str());
    printf("Destination : '%s'\n" , _pathDestination.c_str() );
    printf("Extension : '%s' \n" , ext.c_str() );
    printf("Name '%s'\n" , _name.c_str() );
*/
    _completeRessoucePath  = FileSystem::correctPathIfNeeded(_pathDestination ) + _name + "." + ext;
    
//    printf("\n Dest pat '%s'" , _completeRessoucePath.c_str() );
    
    if( FileSystem::fileExists( _completeRessoucePath ))
    {
//        printf("\n File Already exists");
        FileSystem::removeFile( _completeRessoucePath );
    }
    
    bool ret = FileSystem::copyFile( tempFile , _completeRessoucePath  );
    
    if( ret )
    {
        ret = FileSystem::removeFile(tempFile );
        
        if( ret == false )
            printf("\n ERROR REMOVE TEMP");
    }
    else
        printf("\n ERROR COPY TO DEST");
    
    return ret;
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant Url::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall )
{
    
    if( validCall)
        *validCall = true;
    
    if( selector == "startFetchContent")
        return Variant( startFetchContent());
    
    else if (selector == "setThreadedFetch")
    {
        setThreadedFetch( arguments.at(0).getBool() );
        return Variant::null();
    }
    
    else if( selector == "isThreadedFetch")
    {
        return  Variant( isThreadedFetch() );
    }
    
    else if( selector =="getState")
    {
        return Variant( (int) getState() );
    }
    
    else if( selector == "getUrl")
        return Variant( getUrl() );
    
    else if( selector == "setUrl")
    {
        setUrl( arguments.at(0).getString() );
        return Variant::null();
    }
    
    else if( selector == "setName")
    {
        setName( arguments.at(0).getString() );
        return Variant::null();
    }
    else if( selector == "getName")
    {
        return Variant( getName() );
    }
    
    else if( selector == "setFolder")
    {
        setFilepath(arguments.at(0).getString() );
        return Variant::null();
    }
    
    else if( selector == "getFolder")
    {
        return Variant( getFilePath() );
    }
    
    else if( selector == "getRessourcePath")
    {
        return Variant( getRessourcePath() );
    }
    
    else if ( selector == "setPercentCallback")
    {
        _progressSelector = arguments.at(0).getString();
        _progressTarget = Element::getElementById( arguments.at(1).getUInt64() );
        
        return Variant( _progressTarget != nullptr );
    }
    
    else if (selector == "setEndCallback")
    {
        _endSelector = arguments.at(0).getString();
        _endTarget = Element::getElementById( arguments.at(1).getUInt64() );
        
        return Variant( _progressTarget != nullptr );
    }
    
    if( validCall)
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments , validCall );
}

/*static*/ const Variant Url::staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall)
{
    
    if (validCall)
        *validCall = true;

    if( selector == "getPendingCount" )
    {
        return Variant( getPendingCount() );
    }
    
    if (validCall)
        *validCall = false;
    return Element::performStaticSelectorWithArguments(ClassNames::Element, selector, arguments , validCall );
}


#endif
