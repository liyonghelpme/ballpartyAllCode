//
//  MyBase64.h
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#ifndef __TestCpp__MyBase64__
#define __TestCpp__MyBase64__

#include <iostream>
char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length);

#endif /* defined(__TestCpp__MyBase64__) */
