//
//  GXTransformations.h
//  Broadway_test
//
//  Created by Manuel Deneu on 12/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_test_GXTransformations_h
#define Broadway_test_GXTransformations_h

#include "../Internal/Object.h"

#include "../GXDataType/GXGeometry.h"

class GXElement;

class GXTransformation : public Object
{
    friend class GXElement;
    
    private :
    GXTransformation()
    {}
    
    ~GXTransformation()
    {}
    
    void translate( GXPoint &pt );
    void rotate   ( float rZ );
};


#endif
