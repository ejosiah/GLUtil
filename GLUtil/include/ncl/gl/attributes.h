//
//  attributes.h
//  GLUtil
//
//  Created by Josiah Ebhomenye on 06/01/2017.
//  Copyright © 2017 Josiah Ebhomenye. All rights reserved.
//

#ifndef attributes_h
#define attributes_h

namespace ncl {
    namespace gl{
        struct Attribute{
            unsigned int index;
            int size;
            int stride;
			void* ptr;
            const char* name;
        };
        
        const int NUM_ATTRIBUTES = 6;
        
        Attribute attributes[NUM_ATTRIBUTES] = {
            Attribute{0, 3, 0,  0, "position"},
            Attribute{1, 3, 3, (void*)(3 * sizeof(float)), "normal"},
            Attribute{2, 3, 6, (void*)(6 * sizeof(float)), "tangent"},
            Attribute{3, 3, 9, (void*)(9 * sizeof(float)), "bitangent"},
            Attribute{4, 4, 12, (void*)(12 * sizeof(float)), "color"},
            Attribute{5, 2, 16, (void*)(16 * sizeof(float)),  "textureCoord"}
        };
    }
}


#endif /* attributes_h */
