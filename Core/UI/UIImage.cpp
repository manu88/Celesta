//
//  UIImage.cpp
//  JSCenter
//
//  Created by Manuel Deneu on 27/11/2015.
//  Copyright © 2015 Manuel Deneu. All rights reserved.
//

#include "UIImage.h"

#include "../Env_Variables.h"
#include "UIJSON.h"

UIImage::UIImage():
_image( new GXImage() )
{
    setElementName( ClassNames::UIImage );
    className = ClassNames::UIImage;

    
    addElement( _image);
}

UIImage::~UIImage()
{
    
}

void UIImage::setPosition( const GXPoint &pt)
{
    setBounds(makeRect(pt, getBounds().size ) );
    _image->setBounds( makeRect( pt, _image->getBounds().size ) );
    
//    _image->moveTo( pt);
    _image->setNeedsDisplay();
    setNeedsDisplay();
}

bool UIImage::setImageSource( const std::string &filename )
{
    return _image->setFileSource( filename );
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant UIImage::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall )
{
    if( validCall )
        *validCall = true;
    
    
    if( selector == "setImageSource")
    {
        return  Variant( setImageSource( arguments.at(0).getString() ));
    }
    
    else if( selector == "setScale")
    {
        _image->setScale( makeSize( arguments.at(0).getInt(), arguments.at(1).getInt() ));
        return Variant::null();
    }
    else if( selector == "getScale")
    {
        return Variant({ Variant( _image->getScale().width ) , Variant( _image->getScale().height ) });
    }
    
    if( validCall )
        *validCall = false;
    
    return UIView::performSelectorWithArguments(selector, arguments, validCall);
}
#endif

#ifdef USE_JSON_PARSER
UIImage* UIImage::createFromJSON( const JSON::Node &node )
{
    if( node.isInvalid() )
        return nullptr;
    
    UIImage *img = new UIImage();
    
    const std::string &file = node.getObjectItem( UIAttribute::Ressource ).getString();
    img->setImageSource( file );
    
    return img;
}

bool UIImage::saveJSONSpec( JSON::Node &node) const
{
    node.addItemToObject( Variant( _image->getFileSource() ), UIAttribute::Ressource );
    return true;
}
#endif
