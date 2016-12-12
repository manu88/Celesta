/*!
 *  \brief     GXScene
 *  \details   holds many GXElements derived objects.
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef ____GXScene__
#define ____GXScene__

#include <unordered_set>
#include <set>
#include <vector>

#include "GXElement.h"
#include "../GXDataType/GXColors.h"




//! Displays a set of graphical elements and manage their life-cycle.
/********************************************//**
*
* A GXScene instance is the higest level, just
* before the DisplayController, in Broadway's graphic hierarchy.
*   
*
*
*
***********************************************/
class GXScene : public GXElement
{
public:
    GXScene();
    
    // you should call removeElements before calling the destructor,
    // and give it a little time, since some instances of GXElements will be destroyed asynchronously.
    ~GXScene();
    
    //! Adds (and take ownership) a new element to the scene.
    /*!
        \param element a graphical element to add.
        \return true if the element is added, false in case of any error, or if this element already exists.
     */
    bool addElement   ( GXElement* element );
    
    //! Removes and deletes an element from the scene.
    /*!
     \param element a graphical element to remove.
     \return true if the element is removed, false in case of any error, or if this element does not belongs to this scene.
     */
    bool removeElement( GXElement* element );
    
    
    //! Removes and deletes all elements from the scene.
    void removeAllElements();
    
    void forceUpdate();
    
    //! returns the number of GXElements added to the scene.
    int getElementsCount() const
    {
        return ( int ) _elements.size();
    }
    
    const GXElement* childAtPoint( const GXPoint &pt) const;
    
    //! Found and return a GXElement by its ID, if added to the scene. nullptr if not.
    GXElement* getElementByID( element_id _id) const ;
    
    const GXElement* getElementByName( const std::string &name ) const ;
    

    inline const std::vector<GXElement* > &getChildren() const
    {
        return _elements;
    }
    inline const std::vector<GXElement* > &getChildrenAscending() const
    {
        return _childrenAscending;
    }
    
    bool needsDisplay() const;
    
    /* **** **** **** **** **** **** **** */
    
    //! Returns the deepest z-order in the scene.
    int findDeepestLayer() const;
    int findTopLayer() const;
    void setOwnsElements( bool owns)
    {
        _ownsElements = owns;
    }
    
    
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
protected:
    void paint( const GXRect &rect , GXAnimation* anim );
    void prepareRessources();
    void deleteRessources();
    void changed();
    

    
    void childRemoved( GXElement *element );
    
#ifdef USE_JSON_PARSER
    bool addJSONinfos( JSON::Node &node) const;
#endif
private:

    void reorderElements();    
    void updateElementInRect( GXElement *element , const GXRect &rect);
    
    bool isAlreadyInScene( GXElement* elementTofind )  const ;
    
    void deleteElementFromGUIThread(GXElement *element);
    
    struct layer_comparor
    {
        bool operator() (const GXElement* lhs, const GXElement* rhs) const
        {
            return lhs->getLayer() <= rhs->getLayer();
        }
    };
    
    struct layer_comparorAscending
    {
        bool operator() (const GXElement* lhs, const GXElement* rhs) const
        {
            return lhs->getLayer() >= rhs->getLayer();
        }
    };
    
    
    std::vector<GXElement* > _elements;
    std::vector<GXElement* > _childrenAscending;
    

    bool _ownsElements;

   
};

#endif /* defined(____GXScene__) */
