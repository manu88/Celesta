//
//  GXTextRenderer.h
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__GXTextRenderer__
#define __Broadway_test__GXTextRenderer__

#include "GXElement.h"


class GXFont;
class GXGlyph;
class GXPath;

class GXTextRenderer : public GXElement 
{
public:
    
    typedef enum
    {
        HorizontalLeft     = 0,
        HorizontalCentered = 1,
        HorizontalRight    = 2
    } HorizontalAlignement;

    typedef enum
    {
        VerticalTop       = 0,
        VerticalCentered  = 1,
        VerticalBottom    = 2
    } VerticalAlignement;
    
    GXTextRenderer();
    ~GXTextRenderer();
    
    
    enum { FUCKING_SCALE_CONV_ = 8};
    /* *** */    
    
    void setFont( const GXFont* const font);
    
    const GXFont* getFont() const
    {
        return _font;
    }
    
    /* *** */
    
    void setSizeInPoints( float size);
    
    float getSizeInPoints() const
    {
        return _size;
    }
    
    /* *** */
    
    void setTextColor( const GXColor &color);
    
    GXColor getTextColor() const
    {
        return _textColor;
    }
    
    /* *** */    
    
    void setDisplayedText( const std::string &text);
    
    const std::string &getDisplayedText() const
    {
        return _text;
    }
    
    
    // will return -1 if invalid
    int getTextLengthInPixels() const noexcept
    {
        return _textLengthInPix;
    }
    

#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
protected:
    
#ifdef USE_JSON_PARSER
    bool addJSONinfos( JSON::Node &node) const;
#endif
    
    
    float _size;
    float _sizeCoef;
    
    void paint( const GXRect &rect , GXAnimation* anim );
    void prepareRessources();
    void deleteRessources();
    void changed();
    
    void drawCharAtPositionWithSize( const GXGlyph *glyph , const GXPoint &pos , float size );
    
    std::string    _text;
    const GXFont*  _font;

    GXColor        _textColor;
    
    GXPath         *_textPath;
    
    int _textLengthInPix;
    
};
#endif /* defined(__Broadway_test__GXTextRenderer__) */
