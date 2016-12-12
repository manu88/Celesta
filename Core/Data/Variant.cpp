//
//  Value.cpp
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//
#include "../Config.h"
#include "../Log/Log.h"
#include "Variant.h"
#include "ValueImpl.h"


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
_variant ( new Value< void* >(  nullptr  ,ValueImpl::T_NULL ) )
{}

Variant::Variant( int val):
_variant ( new Value< int >( val ,ValueImpl::T_INT ) )
{}

Variant::Variant( unsigned int val ):
_variant ( new Value<int >( static_cast<int>( val ) ,ValueImpl::T_INT ) )
{}

Variant::Variant( int64_t val):
_variant ( new Value< int64_t >( val ,ValueImpl::T_64 ) )
{}

Variant::Variant( uint64_t val):
_variant ( new Value<  uint64_t >( val ,ValueImpl::T_U64 ) )
{}

Variant::Variant( float val ):
_variant ( new Value< float >( val , ValueImpl::T_FLOAT ) )
{}

Variant::Variant( double val ):
_variant ( new Value< double >( val ,ValueImpl::T_DOUBLE ) )
{}

Variant::Variant( long val ):
_variant ( new Value< long >( val ,ValueImpl::T_LONG ) )
{}

Variant::Variant( unsigned long val ):
_variant ( new Value< unsigned long >( val ,ValueImpl::T_ULONG ) )
{}

Variant::Variant(bool val):
_variant ( new Value< bool >( val ,ValueImpl::T_BOOL ) )
{}

Variant::Variant( std::initializer_list< Variant > args) :
_variant ( new Value< VariantList >( args ,ValueImpl::T_LIST ) )
{
    /*
    for (const Variant &val : args )
    {
        
    }
    */
}

Variant::Variant( const VariantList &args ):
_variant ( new Value< VariantList >( args ,ValueImpl::T_LIST ) )
{
    
}

Variant::Variant( const std::vector<std::string> &args):
_variant ( new Value< VariantList >( {} ,ValueImpl::T_LIST ) )
{
    _variant->getList().clear();
    
    for (const auto &s : args)
        _variant->getList().push_back( Variant(s) );
}

Variant::Variant( const BytesList &args):
_variant ( new Value< VariantList >( {} ,ValueImpl::T_BYTES ) )
{
    _variant->getList().clear();
    
    for (const auto &s : args)
        _variant->getList().push_back( Variant( (int)s) );
}

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
_variant ( new Value< std::string >( val , ValueImpl::T_STRING ) )
{
    
}

Variant::Variant( const char* val ) :
_variant( new Value<std::string> ( std::string( val ) , ValueImpl::T_STRING  ) )
{

}

Variant::Variant( const std::string &name , const Variant &val):
_variant( new Value<DataPair> ( std::make_pair(name, val) , ValueImpl::T_PAIR  ) )
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::Variant ( const Variant &val ):
_variant ( val._variant)
{    
    _variant->retain();
}

Variant& Variant::operator=(Variant const& copy)
{
    _variant->release();

    if (_variant->refCount() <=0 )
    {
        delete _variant;
        _variant = nullptr;
    }

    _variant = copy._variant;
    
    _variant->retain();
    
    return *this;
}

/*
const Variant& Variant::operator=(Variant const& copy) const
{
    printf("\n REF = %i" , _variant->release() );
    
    if (_variant->getType() != copy._variant->getType() )
    {
        printf("\n assign type differ! %i" , _variant->getId() );

    }
    
    _variant = copy._variant;
    
    _variant->retain();
    
    return *this;
}
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


Variant::~Variant()
{
    if ( _variant && _variant->release() ==0 )
    {
        delete _variant;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Variant::getInt() const
{
    return _variant->getInt();
}

int64_t Variant::getInt64() const
{
    return _variant->getInt64();
}

uint64_t Variant::getUInt64() const
{
    return _variant->getUInt64();
}

float Variant::getFloat() const
{
    return _variant->getFloat();
}

double Variant::getDouble() const
{
    return _variant->getDouble();
}

long Variant::getLong() const
{
    return _variant->getLong();
}

unsigned long Variant::getULong() const
{
    return _variant->getULong();
}

bool Variant::getBool() const
{
    return _variant->getBool();
}
const std::string Variant::getString() const
{
    return _variant->getString();
}

/* **** **** **** **** **** */

const VariantList Variant::getList() const
{
    if ( isList() )
        return _variant->getList();
    else if (isByteArray() )
    {
        VariantList l;
        
        for (const auto &v : _variant->getByteArray() )
            l.push_back( Variant(v) );

        return l;
    }
    
    return { *this };
}

VariantList Variant::getList()
{
    if ( isList() )
        return _variant->getList();

    
    return { *this };
}

/* **** **** **** **** **** */

const std::vector< uint8_t > Variant::getByteArray() const
{
    return _variant->getByteArray();
}


const DataPair& Variant::getPair() const
{
    return _variant->getPair();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Variant::isInt() const noexcept
{
    return _variant->isInt();
}

bool Variant::isInt64() const noexcept
{
    return _variant->isInt64();
}

bool Variant::isUInt64() const noexcept
{
    return _variant->isUInt64();
}

bool Variant::isFloat() const noexcept
{
    return _variant->isFloat();
}

bool Variant::isLong() const noexcept
{
    return _variant->isLong();
}

bool Variant::isULong() const noexcept
{
    return _variant->isULong();
}

bool Variant::isDouble() const noexcept
{
    return _variant->isDouble();
}

bool Variant::isBool() const noexcept
{
    return _variant->isBool();
}

bool Variant::isString() const noexcept
{
    return _variant->isString();
}

bool Variant::isList() const noexcept
{
    return _variant->isList();
}

bool Variant::isNull() const noexcept
{
    if (_variant == nullptr )
        return false;
    
    return  _variant->isNull();
}

bool Variant::isByteArray() const noexcept
{
    return _variant->isByteArray();
}

bool Variant::isDataPair() const noexcept
{
    return _variant->isDataPair();
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
    return _variant != rhs._variant;
}

bool Variant::operator==( const void* ptr) const
{
    return getValue<void*>() == ptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::ostream& operator<<( std::ostream& os, const Variant& val)
{
    return os << val.getString();
}



