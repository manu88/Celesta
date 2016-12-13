/*
 * Celesta
 *
 * Copyright (c) 2014 Manuel Deneu
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
#ifndef ____Element__
#define ____Element__

#include <algorithm>

#include <assert.h>
#include <list>
#include <map>

#include "Object.h"
#include "../Env_Variables.h"

#include "AttributesList.h"

//! Element
/********************************************//**
*
*   Base class for any type of Element
*   Its main goal is to provide an _unique_ ID to Elements such as GXElement (GXVideo , GXImage, etc.) so
*   that each instance can be retreived easily.
*
*                                                                 
*   \sa GpioEvent OscEvent
*
*
***********************************************/

namespace ElementAttributes
{
    static const std::string Name = "Name";
    static const std::string State = "State";
}

#ifdef ENABLE_ELEMENT_SELECTOR
typedef std::function<const Variant (const std::string& , const VariantList&, bool *validCall) > StaticSelectorSignature;
#endif

class Element : public virtual Object , public AttributesList
{
public:
    
    typedef enum
    {
        None     =   0,        // 0
        
        Audio    = ( 1 << 0 ), // 1
        Video    = ( 1 << 1 ), // 2
        
        Network  = ( 1 << 2 ), // 4
        
        Hardware = ( 1 << 3 ), // 8
        
        Input    = ( 1 << 4 ), // 16
        Output   = ( 1 << 5 ), // 32
        
        File     = ( 1 << 6 ), // 64
        System   = ( 1 << 7 )  // 128
        
    } TypeInfos;
    
    static Element *RootNode; // set initRootNode bellow;

    typedef uint64_t element_id;
    
    virtual ~Element();
    
    element_id getElementId() const noexcept
    {
        return _id;
    }

    //! if the name already exists, a num will be added and false will be returned
    bool setElementName(const std::string &name) noexcept
    {
        bool ret = true;
        
        if( (getElementByName(name) != nullptr) &&(getElementByName(name) != this) )
        {
            std::string n = name ;
            int count = 0;
            while (getElementByName( n ) != nullptr)
            {
                n = name;
                n+= std::to_string(count); // string's ctor may throw
                
                count++;
            }
            
            _elementName = n;
            
            ret = false;
        }
        
        _elementName = name;
        
        addValueForKey( ElementAttributes::Name , Variant( _elementName ));
        
        return ret;
    }
    
    const std::string &getElementName() const noexcept
    {
        return _elementName;
    }
    
    const Element* getParentElement() const noexcept
    {
        return _parentElement;
    }
    
    void setParentElement( const Element* element) noexcept
    {
        _parentElement = element;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    const TypeInfos &getTypeInfos() const noexcept
    {
        return _typeInfos;
    }
    
    bool hasTypeInfosFlag( const TypeInfos &module) const noexcept
    {
        return (_typeInfos & module) ;
    }
    
    std::vector<const Element*> getChildrenConformingToType( const TypeInfos &typeflags ) const
    {
        std::vector<const Element*> ret;
        
        for (const Element *c : getAllChildren() )
        {
            if( c->getTypeInfos() == typeflags )
                ret.push_back( c );
            

        }
        
        return ret;
    }
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

    static std::vector<const Element*> getParentsList( const Element* element)
    {
        std::vector<const Element*> list;
        const Element *el = element;
        
        while ( el != nullptr)
        {
            list.push_back( el );
            el = el->getParentElement();
            
        }
        return list;
    }
    
    //! return something like 'Root.ClassName1.ClassName2.12
    std::string getIdentifier() const
    {
        std::string ret;
        
        bool first = true;
        for (const Element *e : getParentsList( this ) )
        {
            if( first)
            {
                first = false;
            }
            else
                ret.insert(0, "." );
            
            ret.insert(0, e->getElementName() /* + "."*/ );
        }
        
        return ret;
    }
    
    std::vector<const Element*> getChildren() const;
    std::vector<const Element*> getAllChildren() const;

    static void logElementsTree( bool additionalInfos );
    
#ifdef ENABLE_ELEMENT_SELECTOR
    
    virtual const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr ) ;
    
    static Element* getElementByName(const std::string &elementName ) noexcept
    {        
        const auto it = std::find_if(s_elementsList.begin(), s_elementsList.end(), FindElementByName( elementName ));
        
        if( it != s_elementsList.end())
            return *it;
        
        return nullptr;
    }
    
    static Element* getElementById( element_id id )
    {
        const auto it = std::find_if(s_elementsList.begin(), s_elementsList.end(), FindElementById( id ));
        
        if( it != s_elementsList.end())
            return *it;
        
        return nullptr;
    }
    
    static const Variant performStaticSelectorWithArguments( const std::string &className,
                                                             const std::string &selector ,
                                                             const VariantList  &arguments ,
                                                             bool *validCall = nullptr ,
                                                             bool *validClass = nullptr
                                                            )
    {

        
        if( validCall)
            *validCall = false;
        
        const auto it = s_staticCalls.find( className);
        
        if( it != s_staticCalls.end() )
        {
            if( validClass)
                *validClass = true;
            
            return it->second( selector , arguments , validCall);
        }
        
        if( validClass)
            *validClass = false;
            
        
        return Variant::null();
    }
    
        
    /* *** */
    
    static const Variant performSelectorOnElement( const element_id elementId ,
                                                  const std::string &selector ,
                                                  const VariantList  &arguments ,
                                                  bool *validCall = nullptr ,
                                                  bool *validElement = nullptr
                                                  )
    {
        return performSelectorOnElement( getElementById( elementId )->getElementName() ,
                                         selector,
                                         arguments ,
                                         validCall ,
                                         validElement
                                        );
    }
        
    /* *** */
    
    static const Variant performSelectorOnElement( const std::string &elementName ,
                                                   const std::string &selector ,
                                                   const VariantList  &arguments ,
                                                   bool *validCall = nullptr ,
                                                   bool *validElement = nullptr
                                                  )
    {
        Element* element = getElementByName( elementName);

        if( element != nullptr )
        {
            if (validElement)
                *validElement = true;
            
            return element->performSelectorWithArguments( selector , arguments , validCall);
        }
        
        else if (validElement)
            *validElement = false;

        return Variant::null();
    }
    
    /* *** */

    static size_t getElementsCount()
    {
        return s_elementsList.size();
    }
    
    static std::list< Element*> &getElementList()
    {
        return s_elementsList;
    }
    
    
    static void dumpElement( bool additionalInfos );

#endif
    
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
protected:
    Element();
    
    void deleteChildren() const;
    
    void addTypeInfo( const TypeInfos &flag ) noexcept
    {
        _typeInfos = ( TypeInfos ) ( _typeInfos | flag );
    }
    
#ifdef ENABLE_ELEMENT_SELECTOR
    /*

     function signature:
         static const Variant staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall);

    Add to call stack with:
        Element::addStaticCallbackForClassName( staticSelector, ClassNames::Element );
     
     Your method should return this :
     return Element::staticSelector(selector, arguments, validCall );
     */
    static bool addStaticCallbackForClassName( const StaticSelectorSignature &function ,const std::string &className);
    
    static const Variant staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall);
    
    static void init()
    {
        Element::addStaticCallbackForClassName( staticSelector, ClassNames::Element );
    }
#endif
    
private:
    
    const Element *_parentElement;
    element_id     _id;
    std::string    _elementName;
    
    TypeInfos      _typeInfos;
    
    /* *** *** */
    
    static element_id s_elementsCount;
    static element_id s_elementNextId;
    static std::vector< element_id > s_idPool;
    
    static element_id getNewId() /*noexcept*/
    {
        if( s_idPool.empty() )
            s_idPool.push_back( s_elementNextId++ );
            
        element_id i = s_idPool.back();
        s_idPool.pop_back();
        
        return i;
    }
    
    inline void addId()
    {
        s_idPool.push_back( _id );
    }
    
    #ifdef ENABLE_ELEMENT_SELECTOR
    
        static std::list< Element*> s_elementsList;
    
        struct FindElementByName
        {
            std::string elementName;
            
            FindElementByName( const std::string &name) noexcept
            : elementName( name)
            {
            }
            
            bool operator()(Element *element) const noexcept
            {
                return elementName == element->_elementName;
            }
        };
    
        struct FindElementById
        {
            element_id elementId;
            
            FindElementById( element_id id) : elementId( id){}
            
            bool operator()(Element *element) const
            {
                return elementId == element->_id;
            }
        };

    static std::map< std::string , StaticSelectorSignature >  s_staticCalls;
    
    #endif // defined ENABLE_ELEMENT_SELECTOR

    static void initRootNode()
    {
        assert( RootNode == nullptr );
        
        RootNode = new Element();
        RootNode->setElementName( ClassNames::Root );
        RootNode->className = ClassNames::Root;
        RootNode->setParentElement( nullptr );
    }
    
    static void releaseRootNode()
    {
        assert( RootNode != nullptr );
        assert( RootNode->getChildren().size() == 0 );
        
        delete RootNode;
        RootNode = nullptr;
        assert( s_elementsList.empty() );
    }
};

#endif /* defined(____Element__) */
