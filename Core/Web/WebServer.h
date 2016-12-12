//
//  WebServer.h
//  
//
//  Created by Manuel Deneu on 28/08/14.
//
//

#ifndef ____WebServer__
#define ____WebServer__

#include "web/mongoose.h"

#include "../Internal/Thread.h"
#include "../Internal/AbstractController.h"
#include "../Internal/AbstractDelegate.h"
#include "../Internal/FileSystem.h"
#include "../Data/Database.h"
#include "../Scheduler/Dispatch.h"
#include "../Env_Variables.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class WebServerDelegate : public virtual AbstractDelegate
{
    friend class WebServer;
public:
    virtual ~WebServerDelegate();
    
protected:
    WebServerDelegate();
    virtual std::string getRequest( const std::string &ipAddress ,
                                    const int port,
                                    const std::string &addressPattern,
                                    const Database &arguments) = 0;
};



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


class WebServer : public Element,
                  public Dispatch::Task
{
public:
    
    enum { pollTimeMS       = 10 };
    enum { updateIntervalMS = 10 };
    
    WebServer( int portNum = 8080 );
    
    virtual ~WebServer();
    
    bool start();
    void doTask();
    
    void setDelegate(WebServerDelegate *delegate)
    {
        _delegate = delegate;
    }
    
    void setPort( int port);
    
    int getPort() const noexcept
    {
        return _port;
    }
    

    bool setWorkingDirectory( const std::string &dir);

    
    const std::string &getWorkingDirectory() const
    {
        return m_workingDirectory;
    }
    
    static const std::string getDecodedUrl( const std::string &buffer);
    
    // will return uri parameters' as a Database
    static std::unique_ptr<Database > getUriArguments( const std::string &uri);
    

#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr ) ;
    
    static const Variant staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall);
    
    static void init()
    {
        Element::addStaticCallbackForClassName( staticSelector, ClassNames::WebServer );
    }
#endif
private:
    
    void settingsChanged();
    
    std::string m_workingDirectory;
    
    /*
        will return s_noFilesContent if not found
     */
    const std::string getHtmlFile( const std::string & filename) const;
    
    // thread's entry point
    void run();
    
    mg_result send_reply(struct mg_connection *conn);
    
    int event(struct mg_connection *conn, enum mg_event ev);
    
    // main static event handler from mongoose' server
    static int ev_handler(struct mg_connection *conn, enum mg_event ev);
    
    WebServerDelegate  *_delegate;
    struct mg_server   *_server;
    int                 _port;
    
    std::string m_defaultContent; // index.html in working dir
    
    static std::string s_noFilesContent;
    static std::string s_404;
};

#endif /* defined(____WebServer__) */
