/*
 * Celesta
 *
 * Copyright (c) 2015 Manuel Deneu
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

#ifndef __Broadway_core__Memory__
#define __Broadway_core__Memory__

namespace Memory
{

    void fillBufferFloat(const float* source, float* dest , unsigned int num);
    
    void fillBufferCustom(const void* source , void* dest , unsigned int size );
    
} // Memory

#endif /* defined(__Broadway_core__Memory__) */
