//
//  UIAttributesList.h
//  JSCenter
//
//  Created by Manuel Deneu on 12/12/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#ifndef UIAttributesList_h
#define UIAttributesList_h


#include <map>
#include <list>


typedef std::map< std::string, Variant> ValuesList;

/*
 
 error from list<CONST std::string> on gcc
 
 error: ‘const _Tp* __gnu_cxx::new_allocator< <template-parameter-1-1> >::address(__gnu_cxx::new_allocator< <template-parameter-1-1> >::const_reference) const [with _Tp = const std::basic_string<char>; __gnu_cxx::new_allocator< <template-parameter-1-1> >::const_pointer = const std::basic_string<char>*; __gnu_cxx::new_allocator< <template-parameter-1-1> >::const_reference = const std::basic_string<char>&]’ cannot be overloaded
 
 */
#if defined(__clang__)
typedef std::list<const std::string > ObservableAttributes;
#elif defined(__GNUC__) || defined(__GNUG__)
typedef std::list<std::string > ObservableAttributes;
#endif


typedef enum
{
    ValueAdded   = 0,
    ValueChanged = 1,
    ValueRemoved = 2
} ValueState;

class AttributesList;

class AttributeObserver
{
public:
    
    friend class AttributesList;
    virtual ~AttributeObserver()
    {
    }
protected:
    AttributeObserver()
    {
    }
    
    virtual void valueChanged( const AttributesList* object, const std::string &attribute, const ValueState &state) = 0;

    
};

class AttributesList
{
public:
    virtual ~AttributesList()
    {
        _attributeList.clear();
    }
    
    const Variant &getValueForKey( const std::string &attribute) const
    {
        try
        {
            return _attributeList.at( attribute );
        }
        catch (const std::out_of_range &err)
        {}
        
        return Variant::null();
        
    }
    
    inline bool hasValueForkey(const std::string &attribute) const
    {
        return _attributeList.find( attribute ) != _attributeList.cend();
    }
    
    inline bool addValueForKey( const std::string &attribute , const Variant &value)
    {
        const std::pair< ValuesList::iterator,bool> ret = _attributeList.insert( std::pair< std::string, Variant>( attribute, value));

        if (ret.second == true )
            sendNotification( attribute  , ValueAdded );
        
        return ret.second;
    }
    
    inline bool updateValueForKey( const std::string &attribute , const Variant &value)
    {
        if( !hasValueForkey( attribute ))
            return addValueForKey(attribute, value);
        
        const Variant &val = _attributeList.at(attribute);
        if( val != value )
        {
            _attributeList[ attribute ] = value;
        
            sendNotification( attribute  , ValueChanged );
            return true;
        }
        
        return false;
    }
    
    inline bool removeKey( const std::string &attribute )
    {
        const auto ret = _attributeList.erase( attribute );
        if( ret > 0)
        {
            sendNotification( attribute  , ValueRemoved );
            return true;
        }
        return  false;
    }
    
    inline size_t attributesSize() const
    {
        return _attributeList.size();
    }
    
    void addObserverForAttribute(AttributeObserver *obs , const std::string &attribute)
    {
        if( obs == nullptr)
            return;
        
        _observers[obs].push_back( attribute );
        

    }
    
    void removeObserver( AttributeObserver *obs)
    {
        _observers.erase( obs );
    }
    
    void removeAttributeForObserver(AttributeObserver *obs , const std::string &attribute)
    {
        auto it = _observers.find( obs);
        
        if( it != _observers.end() )
        {
            (*it).second.remove( attribute );
        }
        
    }
    
protected:
    AttributesList()
    {
        
    }
    
private:
    
    inline void sendNotification( const std::string &attr , const ValueState &state) const
    {

        for(  const auto &obs : _observers )
        {
            const ObservableAttributes list = obs.second;
            
            if( std::find( list.begin(), list.end(), attr) != list.end())
                obs.first->valueChanged( this , attr , state);
        }
        
    }
    std::map< std::string, Variant> _attributeList;
    
    std::map< AttributeObserver* , ObservableAttributes >  _observers; // not owned!
};

#endif /* UIAttributesList_h */
