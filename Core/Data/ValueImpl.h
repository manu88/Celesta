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

#ifndef _ValueImpl_h
#define _ValueImpl_h

#include <sstream>

#include "../Env_Variables.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* Base class for Value*/

class ValueImpl : public Object
{
    friend class Variant;
public:
    
    enum {
        T_NULL ,   // 0
        
        T_BOOL ,   // 1
        
        T_INT ,    // 2
        
        T_FLOAT ,  // 3
        T_DOUBLE , // 4
        
        T_STRING , // 5
        T_LIST,    // 6
        T_BYTES,   // 7
        
        T_64,      // 8
        T_U64,     // 9
        
        T_LONG,    // 10
        T_ULONG,   // 11
        
        T_PAIR,    // 12

         };
    
    int retain()
    {
        return GBRetain(_gbPtr);
        //return ++_refCount;
    }
    
    int release()
    {
        GBRelease(_gbPtr);
        return GBObjectGetRefCount(_gbPtr);
        //return --_refCount;
    }
    
    int refCount() const
    {
        return GBObjectGetRefCount(_gbPtr);
        //return _refCount;
    }

    virtual ~ValueImpl()
    {}
    
    /* **** **** **** **** **** **** **** **** **** **** **** */
    
    virtual bool               getBool() const = 0;
    
    virtual int                getInt() const = 0;

    virtual int64_t            getInt64() const = 0;
    virtual uint64_t           getUInt64() const = 0;
    
    virtual float              getFloat() const = 0;
    
    virtual long               getLong() const = 0;
    virtual unsigned long      getULong() const = 0;
    
    virtual double             getDouble() const = 0;
    virtual const std::string  getString() const = 0;
    virtual const VariantList  &getList() const = 0;
    virtual       VariantList  &getList() = 0;
    
    virtual const std::vector< uint8_t > getByteArray() const = 0;
    virtual const DataPair &getPair() const = 0;
    
    /* **** **** **** **** **** **** **** **** **** **** **** */
    
    inline bool isInt() const noexcept
    {
        return _type == T_INT;
    }
    
    inline bool isFloat() const noexcept
    {
        return _type == T_FLOAT;
    }
    
    inline bool isLong() const noexcept
    {
        return _type == T_LONG;
    }
    
    inline bool isULong() const noexcept
    {
        return _type == T_ULONG;
    }
    
    inline bool isDouble() const noexcept
    {
        return _type == T_DOUBLE;
    }
    inline bool isString() const noexcept
    {
        return _type == T_STRING;
    }
    inline bool isBool() const noexcept
    {
        return _type == T_BOOL;
    }
    inline bool isList() const noexcept
    {
        return _type == T_LIST;
    }
    inline bool isNull() const noexcept
    {
        return _type == T_NULL;
    }
    inline bool isByteArray() const noexcept
    {
        return _type == T_BYTES;
    }
    
    inline bool isInt64() const noexcept
    {
        return _type == T_64;
    }
    
    inline bool isUInt64() const noexcept
    {
        return _type == T_U64;
    }

    
    inline bool isDataPair() const noexcept
    {
        return _type == T_PAIR;
    }
    
    inline int getType() const noexcept
    {
        return _type;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** */
    
protected:

    int _type;
    
    int _refCount;
    
    ValueImpl( int type ): _type( type )
    {
        _refCount = 1;
        className = "ValueImpl";
    }
    
    ValueImpl( const ValueImpl &other) = delete;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

template <typename T>
class Value : public ValueImpl
{
public:
        
    /**/
    
    Value(T value , int type) :
    ValueImpl(type),
    _data( value )
    {
        
    }
    
    Value(const Value &other) = delete;
    
    ~Value()
    {
    }
    
    const T &getValue() const
    {
        return _data;
    }
    
    T &getValue()
    {
        return _data;
    }

    void setValue( T val)
    {
        _data = val;
    }

    
    /* Get value */
    
    const std::string getString() const
    {
        if ( isString() )
            return (reinterpret_cast<const Value< std::string >*>(this) )->getValue();
        
        else if ( isFloat() )
            return std::to_string( (reinterpret_cast<const Value< float >*>(this) )->getValue() );
        
        else if ( isDouble() )
            return std::to_string( (reinterpret_cast<const Value< double >*>(this) )->getValue() );
        
        else if ( isBool() )
            return std::to_string( (reinterpret_cast<const Value< bool >*>(this) )->getValue() );
        
        else if ( isInt() )
            return std::to_string( (reinterpret_cast<const Value< int >*>(this) )->getValue() );
   
        return DefaultsValues::Undefined;
    }
    
    bool getBool() const
    {
        if (isBool() )
            return (reinterpret_cast<const Value< bool >*>(this) )->getValue();

        else if (isInt() )
            return (reinterpret_cast<const Value< int >*>(this) )->getValue()!= 0;
        
        else if ( isString() )
            return getIntFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isFloat() )
            return (int) (reinterpret_cast<const Value< float >*>(this) )->getValue();
        
        else  if (isDouble() )
            return (int) (reinterpret_cast<const Value< double >*>(this) )->getValue();
        
        return false; // interpret as int
    }
    
    int getInt() const
    {
        if (isInt() )
            return (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        else if (isBool() )
            return (reinterpret_cast<const Value< bool >*>(this) )->getValue();
        
        else if ( isString() )
            return getIntFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isFloat() )
            return (int) (reinterpret_cast<const Value< float >*>(this) )->getValue();
        
        else  if (isDouble() )
            return (int) (reinterpret_cast<const Value< double >*>(this) )->getValue();
        
        return 0;
    }
    
    int64_t getInt64() const
    {
        if ( isInt64() )
            return (reinterpret_cast<const Value< int64_t >*>(this) )->getValue();
        
        return static_cast<int64_t>( getInt() );
    }
    
    uint64_t getUInt64() const
    {
        if ( isUInt64() )
            return (reinterpret_cast<const Value< uint64_t >*>(this) )->getValue();
        
        return static_cast<uint64_t>( getInt() );
    }
    
    float getFloat() const
    {
        if (isFloat() )
            return (reinterpret_cast<const Value< float >*>(this) )->getValue();
        
        if (isDouble() )
            return (float)(reinterpret_cast<const Value< double >*>(this) )->getValue();
        
        else if ( isString() )
            return getFloatFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isInt() || isBool() )
            return (float) (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        return 0.f;
        
    }
    
    double getDouble() const
    {
        if (isDouble() )
            return (reinterpret_cast<const Value< double >*>(this) )->getValue();
        
        else if ( isString() )
            return getDoubleFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isInt() || isBool() )
            return (double) (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        else  if (isFloat() )
            return (double) (reinterpret_cast<const Value< float >*>(this) )->getValue();
        
        return 0.f;
        
    }
    
    long getLong() const
    {
        if( isLong() )
            return (reinterpret_cast<const Value< long >*>(this) )->getValue();
        
        else if ( isString() )
            return getLongFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        return 0.f;
    }
    
    unsigned long getULong() const
    {
        if( isULong() )
            return (reinterpret_cast<const Value< unsigned long >*>(this) )->getValue();
        
        else if ( isString() )
            return getULongFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        return 0.f;
    }
    
    const VariantList  &getList() const
    {
        return (reinterpret_cast<const Value< VariantList >*>(this) )->getValue();
    }
    
    VariantList  &getList()
    {
        return (reinterpret_cast< Value < VariantList > * >( this ))->getValue();
    }
    
    const std::vector< uint8_t > getByteArray() const
    {
        
        if( isByteArray() )
        {
            return ( reinterpret_cast< const Value <BytesList > * >( this ) )->getValue();
        }
        
        else if (isString() )
        {
            std::vector< uint8_t> ret;
            
            for (const auto &c : getString() )
            {
                ret.push_back( ( uint8_t ) c);
            }
            return ret;
        }

        
        return BytesList();
    }

    const DataPair &getPair() const
    {
        //if ( isDataPair() )
        return ( reinterpret_cast< const Value <DataPair > * >( this ) )->getValue();
    }

    /* **** **** **** **** **** **** **** **** **** **** */

    template<typename Type>
    bool isType() const
    {
        return ( typeid( _data ) == typeid ( Type ) );
    }
    

    
private:
    inline const std::vector<Variant> breakInList(/* const ValueImpl* val*/) const
    {
        std::vector< Variant > list;
        
        std::istringstream f( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        std::string s;
        
        while (getline(f, s, ' '))
        {
            if ( !s.empty() )
                list.push_back( Variant ( s ) );
        }
        
        return list;
    }
    
    
    /* *** *** *** *** *** *** *** *** *** */
    
    inline static int getIntFromStr( const std::string &str) noexcept
    {
        try
        {
            return std::stoi( str );
                             
        } catch (const std::invalid_argument& err)
        {
            return 0;
        }
    }
    
    /* *** *** *** *** *** *** *** *** *** */
    
    inline static float getFloatFromStr( const std::string &str) noexcept
    {
        try
        {
            return std::stof( str );
            
        } catch (const std::invalid_argument& err)
        {
            return 0;
        }
    }
    
    /* *** *** *** *** *** *** *** *** *** */
    
    inline static double getDoubleFromStr( const std::string &str) noexcept
    {
        try
        {
            return std::stod( str );
            
        } catch (const std::invalid_argument& err)
        {
            return 0;
        }
    }
    
    /* *** *** *** *** *** *** *** *** *** */
    
    inline static long getLongFromStr( const std::string &str) noexcept
    {
        try
        {
            return std::stol( str );
            
        } catch (const std::invalid_argument& err)
        {
            return 0;
        }
    }
    
    /* *** *** *** *** *** *** *** *** *** */
    
    inline static unsigned long getULongFromStr( const std::string &str) noexcept
    {
        try
        {
            return std::stoul( str );
            
        } catch (const std::invalid_argument& err)
        {
            return 0;
        }
    }
    
    
    /* *** *** *** *** *** */
    
    T _data;
};





#endif
