//
//  Value.h
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Value__
#define __Value__
#include <typeinfo>
#include <vector>
#include <stdexcept>      // std::out_of_range
#include <algorithm>


class Variant;
class ValueImpl;
class VariantList;

typedef std::vector<uint8_t> BytesList;
typedef std::pair<std::string , Variant> DataPair;

#ifdef USE_JAVA_INTERPRETER
class CScriptVar;
#endif


#include "../GXDataType/GXGeometry.h"
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */




class Variant
{
public:
    
    //typedef int CastLongAs;
    typedef int CastUIntAs;


    static Variant &null() noexcept;

    explicit Variant();
    explicit Variant( int val );
    explicit Variant( unsigned int val );
    explicit Variant( int64_t val);
    explicit Variant( uint64_t val);
    explicit Variant( float val );
    explicit Variant( double val );
    
    explicit Variant( long val);
    explicit Variant( unsigned long val);
    Variant( const std::string &val );

    explicit Variant( const std::string &name , const Variant &val);
    // Added to prevent litterals c—strings from being implicitly converted to bool
    explicit Variant( const char* val );
    
    explicit Variant( bool val );

    Variant( std::initializer_list< Variant > args);
    Variant( const VariantList &list );

    Variant( const std::vector<std::string> &args);
    
    Variant( const BytesList &args);
    
    /* For GX types*/

    explicit Variant( const GXRect & rect);
    explicit Variant( const GXPoint & point);
    explicit Variant( const GXSize & size);
    /**/
    
#ifdef USE_JAVA_INTERPRETER

    static Variant create( CScriptVar *var);
    static VariantList createList( CScriptVar *var);
#endif
    

    /* **** **** */
    
    /* copy & assignment ctors */
    
    Variant ( const Variant &val );
    Variant& operator=(Variant const& copy);

    ~Variant();
    
    friend std::ostream& operator<<( std::ostream& os, const Variant& val );

    /* get val */
    bool        getBool() const;
    int         getInt() const;
    
    int64_t     getInt64() const;
    uint64_t    getUInt64() const;
    float       getFloat() const;
    double      getDouble() const;
    const       std::string getString() const;
    
    long            getLong() const;
    unsigned long   getULong() const;
    
    const VariantList getList() const;
    VariantList getList();

    const std::vector< uint8_t > getByteArray() const;
    
    const DataPair &getPair() const;
    
    //! careful! The type will not be checked, and reinterpret_cast may fail!
    template <typename T> T getValue() const;
    
    template <typename T> void setValue(const T &val) const;
    
    const std::type_info& getType() const
    {
        return typeid( this );
    }
    
    template <typename T>  bool isType() const;
    
    /* test type */
    
    bool isInt()    const noexcept;
    bool isInt64()  const noexcept;
    bool isUInt64()  const noexcept;
    bool isFloat()  const noexcept;
    bool isDouble() const noexcept;
    bool isLong()   const noexcept;
    bool isULong()   const noexcept;
    bool isBool()   const noexcept;
    
    inline bool isNumeric() const
    {
        return   isBool()
              || isInt()
              || isInt64()
              || isUInt64()
              || isFloat()
              || isDouble()
              || isLong()
              || isULong();

    }
    
    bool isString() const noexcept;
    bool isList() const noexcept;
    bool isNull() const noexcept;
    
    bool isByteArray() const noexcept;
    
    bool isDataPair() const noexcept;
    
    /**/

    /* Relationals Operators */
    
    bool operator==( const Variant& rhs) const;
    bool operator!=( const Variant& rhs) const;
    bool operator==( const void* ptr) const; // use this to test against nullptr
    

    
    

protected:
    
    mutable ValueImpl* _variant;
    
/*
private:
    static Variant _null;
*/
};

std::ostream& operator<<( std::ostream& os, const Variant& val);

/*  Class wrapper for std::vector -> catches errors for at() methods
    ( and returns Variant::null() )  so its guarranted( Maybe? ) noexcept*/

class VariantList
{
public:
    typedef typename std::vector< Variant >::iterator iterator;
    typedef typename std::vector< Variant >::const_iterator const_iterator;
    
    VariantList()
    {}
    
    VariantList( std::initializer_list< Variant > l):
    _l( l )
    {}
    
    VariantList( const_iterator begin , const_iterator end ):
    _l( begin , end)
    {}
    
    /**/
    
    iterator begin() noexcept {return _l.begin();}
    const_iterator begin() const noexcept {return _l.begin();}
    const_iterator cbegin() const noexcept {return _l.cbegin();}
    iterator end() noexcept {return _l.end();}
    const_iterator end() const  noexcept {return _l.end();}
    const_iterator cend() const  noexcept {return _l.cend();}
    
    /**/
    
    inline bool empty() const noexcept
    {
        return _l.empty();
    }
    
    inline size_t size() const noexcept
    {
        return _l.size();
    }
    
    /**/
    
    inline void push_back( const Variant &v)
    {
        _l.push_back( v );
    }
    
    inline void clear()
    {
        _l.clear();
    }
    
    /**/
    
    inline Variant &at(size_t n) noexcept
    {
        try
        {
            return _l.at(n);
        }
        catch( const std::out_of_range& oor)
        {
            printf("catched out_of_range %s\n" , oor.what() );
        }
        
        return Variant::null();
    }
    
    inline const Variant &at(size_t n) const noexcept
    {
        try
        {
            return _l.at(n);
        }
        catch( const std::out_of_range& oor)
        {
            printf("catched out_of_range %s\n" , oor.what() );
        }
        
        return Variant::null();
    }
    
    inline const Variant &find( const Variant &value) const
    {
        const auto it = std::find(_l.begin(), _l.end(), value);
        
        if( it != _l.end() )
            return *it;
     

        return Variant::null();
    }
    
    inline bool remove( const Variant &value)
    {
        const auto it = std::find(_l.begin(), _l.end(), value);
        
        if( it != _l.end() )
        {
            _l.erase( it );
            
            return true;
        }
        return false;
    }
    
    
    
private:
    std::vector< Variant > _l;
};


#endif /* defined(__MediaCenter__Value__) */
