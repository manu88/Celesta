//
//  Element.cpp
//  
//
//  Created by Manuel Deneu on 31/07/14.
//
//
#include <assert.h>
#include "Element.h"
#include "../Log/Log.h"
#include "../Env_Variables.h"

#include "../Data/StringOperations.h"

#ifdef ENABLE_ELEMENT_SELECTOR
std::list< Element*> Element::s_elementsList = std::list< Element*>();

std::map< std::string ,  StaticSelectorSignature >Element::s_staticCalls = std::map< std::string ,  StaticSelectorSignature >();

#endif

Element::element_id Element::s_elementsCount  = 0;
Element::element_id Element::s_elementNextId = 0;
std::vector< Element::element_id > Element::s_idPool = std::vector< Element::element_id >();

Element* Element::RootNode = nullptr;

/* *** *** *** *** *** *** */

Element::Element() :
_id            ( getNewId() ),
_typeInfos     ( None )
{
    s_elementsCount++;
    
    if( s_elementsCount == 1 )
    {
        initRootNode();
    }
    
    setParentElement( RootNode );
#ifdef ENABLE_ELEMENT_SELECTOR

    for (const Element *e : s_elementsList )
    {
        if(e->getElementId() == _id)
        {
            Log::log("2 identical ids :");
            Log::log("\t new Element : id = %llu " , _id);
            Log::log("\t aleady existing Element : id = %llu name '%s' class '%s' " , _id , e->getElementName().c_str() , e->className.c_str() );
            logElementsTree( true );

            assert(false);
        }
    }

#endif

    
    className = ClassNames::Element;

#ifdef ENABLE_ELEMENT_SELECTOR
    s_elementsList.push_back( this );

    _elementName = "element"+ std::to_string(_id);
    
#endif
    

}

Element::~Element()
{
#ifdef ENABLE_ELEMENT_SELECTOR
    s_elementsList.erase(std::find( s_elementsList.begin(), s_elementsList.end(), this) );
#endif
    
    addId();
    s_elementsCount--;
    
    if ( s_elementsCount == 0 )
        s_elementNextId = 0;

    else if (s_elementsCount == 1)
        releaseRootNode();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::vector<const Element*> Element::getChildren() const
{
    std::vector<const Element*> ret;

    for (const Element* el : getElementList() )
    {
        if ( StringOperations::beginWith( el->getIdentifier(), getIdentifier() ) )
        {
            if( el->getParentElement() == this )
                ret.push_back( el );
        }
    }
    return ret;
}

void Element::deleteChildren() const
{
    
    for (Element* el : getElementList() )
    {
        if ( StringOperations::beginWith( el->getIdentifier(), getIdentifier() ) )
        {
            if ( el->getParentElement() == this )
                delete  el;
        }
    }

}

std::vector<const Element*> Element::getAllChildren() const
{
    std::vector<const Element*> ret;
    
    for (const Element* el : getElementList() )
    {
        if ( StringOperations::beginWith( el->getIdentifier(), getIdentifier() ) )
            ret.push_back( el );
    }
    return ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void Element::logElementsTree( bool additionalInfos )
{

    
    std::function<void(const Element*,int&)> func = [&func , additionalInfos] (const Element*e , int &deep) ->void
    {
        std::string output ;
        
        for (int i = 0; i< deep ; i++)
            output+= "\t";
        
        output += "Element ID " + std::to_string(e->getElementId()) + " -Name " + e->getElementName() + " -Class " + e->className.c_str();
        Log::log("%s",output.c_str() );
        
        if( additionalInfos )
        {
            
            const std::string AudioType     = "Audio";
            const std::string VideoType     = "Video";
            const std::string NetworkType   = "Network";
            const std::string HardwareType  = "Hardware";
            const std::string InputType     = "Input";
            const std::string OutputType    = "Output";
            const std::string FileType      = "File";
            const std::string SystemType    = "System";
            
            output = "";
        
            for (int i = 0; i< deep+1 ; i++)
                output+= "\t";
            
            output+="TypeInfos = ";// + std::to_string( e->getTypeInfos() );
            
            if( e->hasTypeInfosFlag( Audio ) )
                output += AudioType + "-";
            
            if( e->hasTypeInfosFlag( Video ) )
                output += VideoType + "-";
            
            if( e->hasTypeInfosFlag( Network ) )
                output += NetworkType + "-";
            
            if( e->hasTypeInfosFlag( Hardware ) )
                output += HardwareType + "-";
            
            if( e->hasTypeInfosFlag( Input ) )
                output += InputType + "-";
            
            if( e->hasTypeInfosFlag( Output ) )
                output += OutputType + "-";
            
            if( e->hasTypeInfosFlag( File ) )
                output += FileType + "-";
            
            if( e->hasTypeInfosFlag( System ) )
                output += SystemType + "-";
            
            Log::log("%s",output.c_str() );
            
        }
        
        
        deep++;
        for (const Element*ee : e->getChildren() )
        {
            func(ee, deep);
        }
        deep--;
    };
    
    int deep = 0;
    
    func(Element::RootNode , deep); // recursive call

}

#ifdef ENABLE_ELEMENT_SELECTOR

void Element::dumpElement( bool /*additionalInfos*/  )
{
    Log::log("------- Start Elements DUMP  ------- ");
    for (const Element *e : Element::getElementList() )
    {
        Log::log("---------");
        Log::log("Element Id %i", e->getElementId() );
        Log::log("\tName '%s'",e->getElementName().c_str() );
        Log::log("\tClass '%s' ", e->className.c_str() );
        
        if( e->getParentElement() )
            Log::log("\tParent '%s'" , e->getParentElement( )->getElementName().c_str() );
        
        Log::log("\tIdentifier %s" , e->getIdentifier().c_str() );

    }
    
    Log::log("------- End Elements DUMP  ------- ");
}
//#ifdef ENABLE_ELEMENT_SELECTOR
const Variant Element::staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall)
{
    if( validCall )
        *validCall = true;
    
    if( selector == "LogElementTree")
    {
        logElementsTree( arguments.at(0).getBool() );
        return Variant::null();
    }
    
    else if( selector == "getElementById")
    {
        const Element *el = getElementById( arguments.at(0).getUInt64() );
        
        return  el != nullptr ? Variant(el->getElementId()  ) : Variant::null();
        
    }
    else if( selector == "getElementByName")
    {
        const Element *el = getElementByName( arguments.at(0).getString() );
        
        return  el != nullptr ? Variant(el->getElementId()  ) : Variant::null();
        
    }
    
    if( validCall )
        *validCall = false;
    
    return Variant::null();
}

//#ifdef ENABLE_ELEMENT_SELECTOR
const Variant Element::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall )
{
    if( validCall )
        *validCall = true;
    
    if ( selector == "getElementId")
        return Variant (  getElementId() );
    
    else if ( selector == "getElementName")
        return getElementName();
    
    else if ( selector == "setElementName")
        return Variant(setElementName( arguments.at(0).getString() ));
    
    else if ( selector == "getClassName")
        return Variant( getClassName() );
    
    else if( selector == "getIdentifier")
        return getIdentifier();
    
    else if( selector == "getParentElementId")
    {
        if( getParentElement() )
            return Variant( getParentElement()->getElementId() );
        
        return Variant::null();
    }
    
    else if( selector == "getChildren" )
    {
        VariantList a;
        for (const auto e : getChildren())
            a.push_back( Variant( (Variant::CastUIntAs) e->getElementId() ) );

        return a;
    }
    
    else if( selector == "dumpElements")
    {
        dumpElement( arguments.at(0).getBool() );
        getChildren();
        return Variant::null();
    }
    
    else if( selector == "conformsTo")
    {
        for (const Variant &i : arguments )
        {
            if( !hasTypeInfosFlag( static_cast<TypeInfos>(i.getInt() ) ))
                return Variant( false );
        }
        return Variant(  true );
    }
    
    else if( selector == "addObserverForAttribute")
    {
        AttributeObserver* obs = dynamic_cast<AttributeObserver*>(Element::getElementById( arguments.at(0).getUInt64() ));
        addObserverForAttribute( obs , arguments.at(1).getString() );
        
        return Variant::null();
    }
    
    else if( selector == "getChildrenConformingToType")
    {
        const auto list = getChildrenConformingToType(static_cast<TypeInfos>( arguments.at(0).getInt() ) );
        
        
        VariantList ret;
        
        for (const Element *e : list )
            ret.push_back( Variant( e->getElementId() ) );
        
        return ret;

    }
    
    if( validCall )
        *validCall = false;
    
    
    return Variant::null();
}

//#ifdef ENABLE_ELEMENT_SELECTOR
bool Element::addStaticCallbackForClassName( const StaticSelectorSignature &function , const std::string &className)
{
    const auto it = s_staticCalls.find( className );
    
    if( it == s_staticCalls.end() )
    {
        s_staticCalls.insert( s_staticCalls.end(), std::pair<std::string, StaticSelectorSignature> (className, function) );
        return true;
    }
    
    return false;
}



#endif /*ENABLE_ELEMENT_SELECTOR*/





