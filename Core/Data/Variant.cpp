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

#include "../Config.h"
#include "../Log/Log.h"
#include "Variant.h"



#ifdef USE_JAVA_INTERPRETER
#include "../JSMachine/JSMachine.h"
#endif

#ifdef USE_NETWORK
#include <oscpack/osc/OscReceivedElements.h>
#endif

///*static*/ Variant Variant::_null = Variant();
/*static*/ Variant& Variant::null() noexcept
{
    static Variant n = Variant();
    return n;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::Variant()
{
    setGBObject(nullptr);
}

Variant::Variant( int val)
{
    setGBObject( GBNumberInitWithInt(val) );
}

Variant::Variant( unsigned int val )
{
    setGBObject( GBNumberInitWithInt(static_cast<int>( val ) ) );
}

Variant::Variant( int64_t val)
{
    setGBObject( GBNumberInitWithLong(static_cast<long>( val ) ) );
}

Variant::Variant( uint64_t val)
{
    setGBObject( GBNumberInitWithLong(static_cast<long>( val )) );
}

Variant::Variant( float val )
{
    setGBObject( GBNumberInitWithFloat(val) );
}

Variant::Variant( double val )
{
    setGBObject( GBNumberInitWithDouble(val) );
}

Variant::Variant( long val )
{
    setGBObject( GBNumberInitWithLong( val  ) );

}

Variant::Variant( unsigned long val )
{
    setGBObject( GBNumberInitWithLong(static_cast<long>( val ) ) );
}

Variant::Variant(bool val)
{
    setGBObject( GBNumberInitWithInt( val  ));
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::Variant( std::initializer_list< Variant > args)
//_variant ( new Value< VariantList >( args ,ValueImpl::T_LIST ) )
{
    
}

Variant::Variant( const VariantList &args )
//_variant ( new Value< VariantList >( args ,ValueImpl::T_LIST ) )
{
    
}

Variant::Variant( const std::vector<std::string> &args)
//_variant ( new Value< VariantList >( {} ,ValueImpl::T_LIST ) )
{
    /*
    _variant->getList().clear();
    
    for (const auto &s : args)
        _variant->getList().push_back( Variant(s) );
     */
}

Variant::Variant( const BytesList &args)
//_variant ( new Value< VariantList >( {} ,ValueImpl::T_BYTES ) )
{
    /*
    _variant->getList().clear();
    
    for (const auto &s : args)
        _variant->getList().push_back( Variant( (int)s) );
     */
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


Variant::Variant( const GXRect & rect):
::Variant( { Variant(rect.origin.x ),
             Variant(rect.origin.y ),
             Variant(rect.size.width ),
             Variant(rect.size.height )
         })
{}

Variant::Variant( const GXPoint & point):
::Variant({ Variant(  point.x), Variant( point.y) })
{}

Variant::Variant( const GXSize & size):
::Variant({ Variant(  size.width), Variant( size.height) })
{}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::Variant( GBRef gbObject)
{
    setGBObject( const_cast<GBObject*>( gbObject) );
    GBRetain(gbObject);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef USE_JAVA_INTERPRETER

/*static*/ Variant Variant::create( CScriptVar *var)
{

    if ( var->isInt() )
        return Variant ( var->getInt() );
    
    else if ( var->isDouble() )
        return Variant ( var->getDouble() );
    
    else if ( var->isString() )
        return Variant ( var->getString() );

    return Variant();
}
/*static*/ VariantList Variant::createList( CScriptVar *var)
{

    if ( var->isInt() )
        return {Variant ( var->getInt() )};
    
    else if ( var->isDouble() )
        return {Variant ( var->getDouble() )};

    else if ( var->isString() )
        return {Variant ( var->getString() )};
    
    else if ( var->isArray() )
    {
        VariantList ret;
        
        const int size = var->getArrayLength();
        
        for( int i = 0; i< size ; i++)
        {
            CScriptVar *v = var->getArrayIndex(i);

            if ( v->isInt() )
                ret.push_back( Variant( v->getInt() ) );
            
            else if ( v->isDouble() )
                ret.push_back( Variant( v->getDouble() ) );
            
            else if ( v->isString() )
                ret.push_back( Variant( v->getString() ) );
            
            else if( v->isArray() )
            {
                ret.push_back( createList( v ) );
            }
            else if (v->isUndefined())
            {
                
            }
            else //if( v->isFunction() )
            {
                printf("Not defined type '%s' \n", v->getParsableString().c_str() );
                assert( false);
            }
            
        }
        
        return ret;
    }
    else if (var->isUndefined())
    {
        
    }
    else
    {
        assert(false);
    }
    
    

    return VariantList();
}

#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


Variant::Variant(const std::string &val)
{
    setGBObject( GBStringInitWithCStr( val.c_str()) );
}

Variant::Variant( const char* val )
{
    setGBObject( GBStringInitWithCStr( val ) );
}

Variant::Variant( const std::string &name , const Variant &val)
//_variant( new Value<DataPair> ( std::make_pair(name, val) , ValueImpl::T_PAIR  ) )
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::Variant ( const Variant &val )
{
    setGBObject( val.getGBObject<GBObject*>());
    GBRetain( val.getGBRef());
}

Variant& Variant::operator=(Variant const& copy)
{
    GBRelease( getGBRef() );
    setGBObject( copy.getGBObject<GBObject*>());
    //_value = copy._value;
    
    GBRetain( getGBRef() );
    
    return *this;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::~Variant()
{

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Variant::getInt() const
{
    return GBNumberToInt( getGBObject<const GBNumber*>() );
}

int64_t Variant::getInt64() const
{
    return static_cast<int64_t>( GBNumberToLong( getGBObject<const GBNumber*>() ) );
}

uint64_t Variant::getUInt64() const
{
    return static_cast<uint64_t>( GBNumberToLong( getGBObject<const GBNumber*>() ) );
}

float Variant::getFloat() const
{
    return GBNumberToFloat( getGBObject<const GBNumber*>() );
}

double Variant::getDouble() const
{
    return GBNumberToDouble( getGBObject<const GBNumber*>() );
}

long Variant::getLong() const
{
    return GBNumberToLong( getGBObject<const GBNumber*>() );
}

unsigned long Variant::getULong() const
{
    return static_cast<unsigned long>( GBNumberToLong( getGBObject<const GBNumber*>() ) );
}

bool Variant::getBool() const
{
    return static_cast<bool>(getInt());
}
const std::string Variant::getString() const
{
    return std::string( GBStringGetCStr( getGBObject<const GBString*>() ) );
}

/* **** **** **** **** **** */

const VariantList Variant::getList() const
{
    /*
    if ( isList() )
        return _variant->getList();
    else if (isByteArray() )
    {
        VariantList l;
        
        for (const auto &v : _variant->getByteArray() )
            l.push_back( Variant(v) );

        return l;
    }
    */
    return { *this };
}

VariantList Variant::getList()
{
    /*
    if ( isList() )
        return _variant->getList();

    */
    return { *this };
}

/* **** **** **** **** **** */

const std::vector< uint8_t > Variant::getByteArray() const
{
    DEBUG_ASSERT(0);
    return std::vector<uint8_t>();
}


DataPair Variant::getPair() const
{
    DEBUG_ASSERT(0);
    return std::make_pair<std::string , Variant>("lol" , Variant("lol"));
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
template <typename T>
T Variant::getValue() const
{
    return (reinterpret_cast<const Value< T >*>( _variant) )->getValue() ;
    
}

template <typename T>
void Variant::setValue(const T &val) const
{
    (reinterpret_cast<Value< T >*>( _variant) )->setValue( val ) ;
    
}



template <typename T>  bool Variant::isType() const
{
    return typeid( this ) == typeid(T);
}

template unsigned long Variant::getValue< unsigned long >() const;
template bool Variant::isType<unsigned long> () const;
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Variant::isInt() const noexcept
{
    return GBNumberGetType( getGBObject<const GBNumber*>() ) == GBNumberTypeInt;
}

bool Variant::isInt64() const noexcept
{
    return GBNumberGetType( getGBObject<const GBNumber*>() ) == GBNumberTypeLong;
}

bool Variant::isUInt64() const noexcept
{
    return GBNumberGetType( getGBObject<const GBNumber*>() ) == GBNumberTypeLong;
}

bool Variant::isFloat() const noexcept
{
    return GBNumberGetType( getGBObject<const GBNumber*>() ) == GBNumberTypeFloat;
}

bool Variant::isLong() const noexcept
{
    return GBNumberGetType( getGBObject<const GBNumber*>() ) == GBNumberTypeLong;
}

bool Variant::isULong() const noexcept
{
    return GBNumberGetType( getGBObject<const GBNumber*>() ) == GBNumberTypeLong;
}

bool Variant::isDouble() const noexcept
{
    return GBNumberGetType( getGBObject<const GBNumber*>() ) == GBNumberTypeDouble;
}

bool Variant::isBool() const noexcept
{
    return GBNumberGetType( getGBObject<const GBNumber*>() ) == GBNumberTypeInt;
}

bool Variant::isString() const noexcept
{
    return IsKindOfClass(getGBRef(), GBStringClass);
}

bool Variant::isList() const noexcept
{
    return false;
}

bool Variant::isNull() const noexcept
{
    if ( getGBRef() == nullptr )
        return true;
    
    return  !GBObjectIsValid(getGBRef());
}

bool Variant::isByteArray() const noexcept
{
    return false;
}

bool Variant::isDataPair() const noexcept
{
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
bool Variant::operator==( const Variant& rhs) const
{
    if ( isNumeric() && rhs.isNumeric() )
        return getDouble() == rhs.getDouble();
    
    return getString() == rhs.getString();
}

bool Variant::operator!=( const Variant& rhs) const
{
    return GBObjectEquals( getGBRef(), rhs.getGBRef() ) == 0;
}

bool Variant::operator==( const void* ptr) const
{
    DEBUG_ASSERT(0);// ???
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::ostream& operator<<( std::ostream& os, const Variant& val)
{
    return os << val.getString();
}



