//
//  Url.hpp
//  JSCenter
//
//  Created by Manuel Deneu on 18/10/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef Url_hpp
#define Url_hpp

#include "../Internal/Element.h"
#include "../Internal/AbstractDelegate.h"
#include "../Internal/Thread.h"


#include <curl/curl.h> 

class Url; // forward

class UrlDelegate : public virtual AbstractDelegate
{
    friend class Url;
public:
    

    virtual ~UrlDelegate(){}
    
protected:
    UrlDelegate()
    {}
    
    // optional
    virtual void urlRequestDidProgressTo( Url* url, float percent );
    
    // optional
    virtual void urlRequestDidEnd( Url* url, int code );
};


class Url : public Element, public Thread
{
public:
    
    typedef enum
    {
        Unknown       = -1,
        
        Fetching      = 0,
        FetchError    = 1,
        
        DownloadError = 2,
        Downloaded    = 3,
        
        CopyError     = 4,
        Copied        = 5,
        
        ReadyToUse    = 10
        
    } UrlState;
    
    
    Url();
    virtual ~Url();
    
    void setDelegate( UrlDelegate *delegate)
    {
        _delegate = delegate;
    }
    
    void setThreadedFetch( bool useThread )
    {
        _threadedFetch = useThread;
    }
    
    bool isThreadedFetch() const
    {
        return _threadedFetch;
    }
    
    bool startFetchContent( );
    
    /* *** *** *** *** *** *** *** *** */
    
    const UrlState &getState() const
    {
        return _state;
    }
    
    /* *** *** *** *** *** *** *** *** */

    void setUrl( const std::string &url)
    {
        _url = url;
    }
    
    const std::string &getUrl() const
    {
        return _url;
    }
    
    /* *** *** *** *** *** *** *** *** */
    
    void setName( const std::string &name )
    {
        _name = name;
    }
    
    const std::string &getName() const
    {
        return _name;
    }
    
    /* *** *** *** *** *** *** *** *** */
    
    void setFilepath( const std::string path)
    {
        _pathDestination = path;
    }
    
    const std::string &getFilePath() const
    {
        return _pathDestination;
    }
    
    const std::string getRessourcePath() const
    {
        if (_state == ReadyToUse)
            return _completeRessoucePath;
        
        return DefaultsValues::Undefined;
    }
    
    /* *** *** *** *** *** *** *** *** */
    
    void setTempPath( const std::string &path)
    {
        _tempPath = path;
    }
    
    const std::string &getTempPath() const
    {
        return _tempPath;
    }
    
    static int getPendingCount()
    {
        return s_pendingCount;
    }
    
#ifdef ENABLE_ELEMENT_SELECTOR
    
    void setProgressCallbackSelector( const std::string selector, Element* target)
    {
        _progressSelector = selector;
        _endTarget = target;
    }
    
    void setEndCallbackSelector( const std::string selector, Element* target)
    {
        _progressSelector = selector;
        _endTarget = target;
    }
    
    
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
    
    static const Variant staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall);
    
    static void init()
    {
        addStaticCallbackForClassName( staticSelector, ClassNames::Url );
    }
#endif
    
private:
    
    enum { SendPercentEvery = 20  };
    
    
    void run();
    
    bool writeDestFile();
    bool internalFetch();
    
    void progressChangedTo( float progress);
    void didEnd ( int code );
    
    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *userData);
    

    static int progress_callback(void *clientp,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow);
    static int older_progress(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
    bool _threadedFetch; // default to true
    
    UrlDelegate *_delegate;
    
    std::string _name;            // named ressource
    std::string _url;             // url fetch
    std::string _tempPath;        // temp file
    std::string _pathDestination; // final path : file will be _pathDestination + _name +"."+ extensionFromMIME (e.x jpg)
    std::string _mimeType;        // mime to get extension from
    
    std::string _completeRessoucePath;
    FILE *_fileToWrite;
    
    int      _curlCode;
    UrlState _state;
    
    

#ifdef ENABLE_ELEMENT_SELECTOR
    std::string  _progressSelector;
    Element     *_progressTarget;
    
    std::string  _endSelector;
    Element     *_endTarget;
#endif

    static int s_pendingCount;

};


#endif /* Url_hpp */
