/*!
 *  \brief     GXElement
 *  \details   Base class for all Graphic Elements
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       
 *  \warning   .
 *  \copyright GNU Public License.
 */


#ifndef ____GXElement__
#define ____GXElement__

#include "../Internal/Element.h"

#include "../GXDataType/GXGeometry.h"
#include "../GXDataType/GXColors.h"
#include "../Env_Variables.h"
#include "GXTransformations.h"

#ifdef USE_JSON_PARSER
#include "../Parsers/JSONParser.h"
#endif

class GXAnimation; // forward
class DisplayController;

class Event;

class GXElement : public virtual Element
{
public:
    

    /**/
    
    friend class DisplayController;
    friend class GXScene;
    friend class GXAnimation;
    
    // default is set To true
    void performChangedSignalOnGUIThread( bool callOnGUIThread );

    void prepare();
    void cleanUp();
    
    //! signal that this GXElement has to be redrawn once
    void setNeedsDisplay();
    
    //! signal that this GXElement has to be redrawn for numFrames
    void setNeedsDisplayFor( int numFrames);
    
    //! signal that this GXElement has to be redrawn once in a specified region.
    void setNeedsDisplayInRect( const GXRect &rect);
    
    // base function called by every methods above
    void setNeedsDisplayInRectForFrames( const GXRect &rect , int numFrames);
    
    void setUpdated();
    
    bool startContinuousRendering();
    bool stopContinuousRendering();
    
    bool attachAnimation( GXAnimation *anim , Event *completionEvent = nullptr ) const;

    void removeFromParent() noexcept
    {
        changeCycleTo( WillDiseapper );
        _shouldBeRemoved = true;
        _added  = false;
        
    }
    
    bool shouldBeRemoved() const noexcept
    {
        return _shouldBeRemoved;
    }
    
    bool isPrepared() const noexcept
    {
        return _prepared;
    }
    
    void setUnprepared();
    
    virtual bool needsDisplay() const;

    /* **** **** **** **** **** **** **** **** **** */
    
    /* These setters will call changed() later*/
    
    // 1 atop 0
    virtual void setLayer(int layer);
    int getLayer() const;
    
    virtual void setAlpha( uint8_t alpha)
    {
        _alpha = alpha;
    }
    
    uint8_t getAlpha() const noexcept
    {
        return _alpha;
    }

    virtual void setBounds( const GXRect &bounds );
    
    void moveTo( const GXPoint &point );
    void moveOf( int dX , int dY);
    void moveTo( int x  , int y);
    
    void setSize( int width , int height);
    void setSize( const GXSize &size);

    const GXRect &getBounds() const noexcept
    {
        return _bounds;
    }
    
    /* **** **** **** **** **** **** **** **** **** */
    
    void setBackgroundColor( const GXColor &color);
    
    const GXColor &getBackgroundColor() const noexcept
    {
        return _backgroundColor;
    }

    /* **** **** **** **** **** **** **** **** **** */

    // will call changed()
    void setTransparency( bool transparent );
    void setOpacity( bool opaque );
    
    bool isTransparent() const noexcept
    {
        return _isTransparent;
    }

    bool isOpaque() const noexcept
    {
        return !isTransparent();
    }
    
    // will call changed()
    void setHidden(  bool hidden);
    void setVisible( bool visible);
    bool flipVisibility();
    
    bool isVisible() const noexcept
    {
        return !_hidden;
    }

    /* **** **** **** **** **** **** **** **** **** */
    
    void setParentElement( GXElement *parentElement)
    {
        _parentElement = parentElement;
        
        Element::setParentElement( parentElement );
        _added = true;
    }
    
    const GXElement* getParentElement() const
    {
        return _parentElement;
    }
    
    /* **** **** **** **** **** **** **** **** **** */
    /*         transformations                      */
    
    void beginTransformation();
    void endTransformation();
    
    inline void translate( GXPoint &pt )
    {
        _transformation->translate( pt );
    }
    
    inline void rotate( float rZ )
    {
        _transformation->rotate( rZ);
    }
    
    inline GXPoint getAbsoluteCoords() const
    {
        GXPoint p =makePointNULL();// getBounds().origin;
        
        const GXElement *e = this;
        
        while ( e != nullptr)
        {
            printf("Get absolute from %s %i %i \n" , e->getElementName().c_str() ,e->getBounds().origin.x , e->getBounds().origin.y );
            p.x += e->getBounds().origin.x;
            p.y += e->getBounds().origin.y;
            e = e->getParentElement();
        }
        return p;
    }
    
    /* **** **** **** **** **** **** **** **** **** */
    
#ifdef USE_JSON_PARSER
    // create a stack of GXElements from a JSON file
    const std::string stringify() const;
    
#endif
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
    
    static const Variant staticSelector(const std::string& selector , const VariantList& arguments, bool *validCall);
    
    static void init()
    {
        addStaticCallbackForClassName( staticSelector, ClassNames::GXElement );
    }
#endif
    
    
    
    
protected:
    
    // derived only.
    GXElement();
    virtual ~GXElement();
    
    void update( const GXRect &rect, GXAnimation* anim );
    
    // called _ONLY_ from GUI thread
    virtual void paint( const GXRect &rect, GXAnimation* anim ) = 0;
    virtual void prepareRessources() = 0;
    virtual void deleteRessources() = 0;
    
    
    // view life cycle. base impl does nothing.
    virtual void elementWillAppear();
    virtual void elementDidAppear();
    virtual void elementWillDisappear();
    virtual void elementDidDisappear();
    
    // can be called synchronusly on caller's thread,
    // or on GUI thread if performChangedSignalOnGUIThread( true ) is called.
    virtual void changed() = 0;

    virtual void childRemoved(GXElement *element);
    
    void setStrokeColor( GXColor color );
    void setFillColor  ( GXColor color );
    
#ifdef USE_JSON_PARSER
    JSON::Node initNode() const;
    virtual bool addJSONinfos( JSON::Node &node) const;
#endif

private:
    
    void elementChanged();

    int             _layer;
    
    uint8_t         _alpha;
    
    bool            _prepared;
    bool            _shouldBeRemoved;
    bool            _added;
    
    int             _needsDisplay;
    GXRect          _updateRect;
    
    bool            _hidden;
    bool            _layerChanged;
    
    bool            _changed_flag;
    bool            m_callChangedOnGUIThread;
    
    GXElement       *_parentElement;
    
    GXRect          _bounds;
    GXColor         _backgroundColor;

    // default to false             
    bool            _isTransparent;
    bool            _autoRendering;
    
//    bool            _moved;
    GXRect          _lastBounds;
    
    mutable GXAnimation    *_anim;
    mutable Event          *_animCompletionEvent;
    
    GXTransformation *_transformation;
    
    
    typedef enum
    {
        NotVisible    = 0,
        WillAppear    = 1,
        Visible       = 2,
        WillDiseapper = 3
    } CycleState;
    
    void changeCycleTo( CycleState state);
    /*
        0 // not added to any superview
                // TODO : send 'added notif'
        1 // is added, will appear soon
                // send willAppear()
        2 // view is Visible
                // send didAppear()
        3 // will desappear
                // send willDisappear()
        -> return to 0
                // send didDisappear()
     */
    CycleState _elementCycleState;

};




#endif /* defined(____GXElement__) */
