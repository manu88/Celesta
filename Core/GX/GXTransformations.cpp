

//
//  GXTransformations.c
//  Broadway_test
//
//  Created by Manuel Deneu on 12/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <math.h>
#include "GXTransformations.h"
#include "GXDefs.h"



void GXTransformation::translate( GXPoint &pt )
{
    vgTranslate( pt.x, pt.y );
}

void GXTransformation::rotate( float rZ )
{
    vgRotate( rZ );
}
