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

Variant::Variant():
//_variant ( new Value< void* >(  nullptr  ,ValueImpl::T_NULL ) ),
_value(NULL)
{}

Variant::Variant( int val):
//_variant ( new Value< int >( val ,ValueImpl::T_INT ) ),
_value( GBNumberInitWithInt(val))
{}

Variant::Variant( unsigned int val ):
//_variant ( new Value<int >( static_cast<int>( val ) ,ValueImpl::T_INT ) ),
_value( GBNumberInitWithInt(static_cast<int>( val )))
{}

Variant::Variant( int64_t val):
//_variant ( new Value< int64_t >( val ,ValueImpl::T_64 ) ),
_value( GBNumberInitWithLong(static_cast<long>( val )))
{}

Variant::Variant( uint64_t val):
//_variant ( new Value<  uint64_t >( val ,ValueImpl::T_U64 ) ),
_value( GBNumberInitWithLong(static_cast<long>( val )))
{}

Variant::Variant( float val ):
//_variant ( new Value< float >( val , ValueImpl::T_FLOAT ) ),
_value( GBNumberInitWithFloat(val))
{}

Variant::Variant( double val ):
//_variant ( new Value< double >( val ,ValueImpl::T_DOUBLE ) ),
_value( GBNumberInitWithDouble(val))
{}

Variant::Variant( long val ):
//_variant ( new Value< long >( val ,ValueImpl::T_LONG ) ),
_value( GBNumberInitWithLong( val  ))
{}

Variant::Variant( unsigned long val ):
//_variant ( new Value< unsigned long >( val ,ValueImpl::T_ULONG ) ),
_value( GBNumberInitWithLong(static_cast<long>( val )))
{}

Variant::Variant(bool val):
//_variant ( new Value< bool >( val ,ValueImpl::T_BOOL ) ),
_value( GBNumberInitWithInt( val  ))
{}

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

Variant::Variant( GBRef gbObject):
_value( const_cast<GBObject*>( gbObject))
{
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


Variant::Variant(const std::string &val):
//_variant ( new Value< std::string >( val , ValueImpl::T_STRING ) )
_value(GBStringInitWithCStr( val.c_str()))
{
    
}

Variant::Variant( const char* val ) :
//_variant( new Value<std::string> ( std::string( val ) , ValueImpl::T_STRING  ) ),
_value(GBStringInitWithCStr( val ) )
{

}

Variant::Variant( const std::string &name , const Variant &val)
//_variant( new Value<DataPair> ( std::make_pair(name, val) , ValueImpl::T_PAIR  ) )
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::Variant ( const Variant &val ):
_value( val._value)
{
    GBRetain(_value);
}

Variant& Variant::operator=(Variant const& copy)
{
    GBRelease( _value );
    _value = copy._value;
    
    GBRetain(_value);
    
    return *this;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::~Variant()
{
    GBRelease(_value);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Variant::getInt() const
{
    return GBNumberToInt( static_cast<const GBNumber*>( _value ));
}

int64_t Variant::getInt64() const
{
    return static_cast<int64_t>( GBNumberToLong( static_cast<const GBNumber*>( _value )));
}

uint64_t Variant::getUInt64() const
{
    return static_cast<uint64_t>( GBNumberToLong( static_cast<const GBNumber*>( _value )));
}

float Variant::getFloat() const
{
    return GBNumberToFloat( static_cast<const GBNumber*>( _value ));
}

double Variant::getDouble() const
{
    return GBNumberToDouble( static_cast<const GBNumber*>( _value ));
}

long Variant::getLong() const
{
    return GBNumberToLong( static_cast<const GBNumber*>( _value ));
}

unsigned long Variant::getULong() const
{
    return static_cast<unsigned long>( GBNumberToLong( static_cast<const GBNumber*>( _value )) );
}

bool Variant::getBool() const
{
    return static_cast<bool>(getInt());
}
const std::string Variant::getString() const
{
    return std::string( GBStringGetCStr( static_cast<const GBString*>( _value ) ) );
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
    return GBNumberGetType( static_cast<const GBNumber*>( _value)) == GBNumberTypeInt;
}

bool Variant::isInt64() const noexcept
{
    return GBNumberGetType( static_cast<const GBNumber*>( _value)) == GBNumberTypeLong;
}

bool Variant::isUInt64() const noexcept
{
    return GBNumberGetType( static_cast<const GBNumber*>( _value)) == GBNumberTypeLong;
}

bool Variant::isFloat() const noexcept
{
    return GBNumberGetType( static_cast<const GBNumber*>( _value)) == GBNumberTypeFloat;
}

bool Variant::isLong() const noexcept
{
    return GBNumberGetType( static_cast<const GBNumber*>( _value)) == GBNumberTypeLong;
}

bool Variant::isULong() const noexcept
{
    return GBNumberGetType( static_cast<const GBNumber*>( _value)) == GBNumberTypeLong;
}

bool Variant::isDouble() const noexcept
{
    return GBNumberGetType( static_cast<const GBNumber*>( _value)) == GBNumberTypeDouble;
}

bool Variant::isBool() const noexcept
{
    return GBNumberGetType( static_cast<const GBNumber*>( _value)) == GBNumberTypeInt;
}

bool Variant::isString() const noexcept
{
    return IsKindOfClass(_value, GBStringClass);
}

bool Variant::isList() const noexcept
{
    return false;
}

bool Variant::isNull() const noexcept
{
    if (_value == nullptr )
        return true;
    
    return  !GBObjectIsValid(_value);
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
    return GBObjectEquals(_value, rhs._value) == 0;
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



