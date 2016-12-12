//
//  Env_Variables.h
//  JSCenter
//
//  Created by Manuel Deneu on 01/10/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef Env_Variables_h
#define Env_Variables_h

#include <string>

/*
    Variables, litterals used in the software are defined here
 */


namespace DefaultsValues
{
    static const std::string DefaultFont            = "Source Sans Pro Regular";
    static const std::string DefaultApplicationName = "Application";
    
    static const std::string Undefined              = "undefined";
    
    static const std::string IdentifierPrefix       = "com.JSCenter.";
    
}

namespace SystemFolders
{
    static const std::string JsHeaders = "JS_Dir";
    static const std::string Fonts     = "FONTS_Dir";
}

/* Params names */
namespace Parameters
{
    static const std::string ApplicationName = "AppName";
}

namespace TimerIdentifiers
{
    static const std::string QuitSignal          = "QuitSignal";
    static const std::string ApplicationDidStart = "ApplicationDidStart";
}

namespace ClassNames
{
    /* Base */
    static const std::string Element             = "Element";
    static const std::string Thread              = "Thread";
    
    static const std::string Root                = "Root";

    static const std::string Dispatch            = "Dispatch";
    static const std::string Timer               = "Timer";
    static const std::string Chrono              = "Chrono";

    static const std::string Event               = "Event";
    
    static const std::string IPCSender           = "IPCSender";
    static const std::string IPCListener         = "IPCListener";
    
    /* Apps */
    static const std::string ApplicationBase     = "ApplicationBase";
    static const std::string FileManager         = "FileManager";
    /* Web */
    static const std::string WebServer           = "WebServer";
    static const std::string Url                 = "Url";
    
    /* Net */
    static const std::string NetworkController   = "NetworkController";
        
    /* GX classes */
    static const std::string DisplayController   = "DisplayController";
    
    static const std::string GXElement           = "GXElement";
    static const std::string GXAnimation         = "GXAnimation";
    static const std::string GXAsyncPainter      = "GXAsyncPainter";
    static const std::string GXVideo             = "GXVideo";
    static const std::string GXImage             = "GXImage";
    static const std::string GXGlyph             = "GXGlyph";
    static const std::string GXFont              = "GXFont";
    static const std::string GXPath              = "GXPath";
    static const std::string GXScene             = "GXScene";
    static const std::string GXTextRenderer      = "GXTextRenderer";
    static const std::string Painter             = "Painter";
    
    static const std::string GXTouchController   = "GXTouchController";
    
    /* IO */
    static const std::string InterfaceController = "InterfaceController";
    static const std::string SerialEvent         = "SerialEvent";

    /* Parsers */
    static const std::string JSON_Document       = "JSONDocument";

    
    /* UI */
    
    static const std::string UIButton     = "UIButton";
    static const std::string UIWindow     = "UIWindow";
    static const std::string UISlider     = "UISlider";
    static const std::string UIImage      = "UIImage";
    static const std::string UIStoryBoard = "UIStoryBoard";
    static const std::string UILabel      = "UILabel";
    static const std::string UIModalAlert = "UIModalAlert";
    static const std::string UIVideo      = "UIVideo";
}




#endif /* Env_Variables_h */
