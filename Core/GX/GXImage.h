/*!
 *  \brief     GXImage
 *  \details   GXElement specialised in jpeg display.
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       Only jpeg files for now
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef __Broadway_test__GXImage__
#define __Broadway_test__GXImage__


#include "GXDefs.h"
/* moved in ^
#ifdef TARGET_RASPBERRY_PI
#include <EGL/egl.h>
#include <GLES/gl.h>
#include "VG/openvg.h"
#include "VG/vgu.h"

#else
#include "../Plateforms/MacOS/MacDummies/vg_mac.h"
#endif
*/

#include "../GXDataType/GXGeometry.h"

#include "GXElement.h"

class GXImage : public GXElement
{
public:
    GXImage(const std::string &filename);

    GXImage():
    ::GXImage( "" )
    {}
    
    ~GXImage();
    
    bool setFileSource( const std::string &filename );
    
    const std::string &getFileSource() const
    {
        return _filename;
    }
    
    void setScale( const GXSize &size)
    {
        if( size != _imageSize)
            _imageSize = size;
        
    }
    
    const GXSize &getScale() const
    {
        return _imageSize;
    }

#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif

protected:
    void paint( const GXRect &rect , GXAnimation* anim );
    void prepareRessources();
    void deleteRessources();
    void changed();
    
#ifdef USE_JSON_PARSER
    bool addJSONinfos( JSON::Node &node) const;
#endif
    
private:
    
    std::string _filename;
    
    GXSize      _imageSize;

    VGImage _image;
    
    static VGImage createImageFromJpeg(const char *filename , int &w , int &h);
    static VGImage createImageFromPng(const char *filename , int &w , int &h);


    
};



#endif /* defined(__Broadway_test__GXImage__) */
