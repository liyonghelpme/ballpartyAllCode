//
//  ImgSelect.h
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#import <Foundation/Foundation.h>
#include "RedisInterface.h"

@interface ImgSelect : NSObject <UINavigationControllerDelegate, UIImagePickerControllerDelegate> {
    void *imgdata;
    int len;
    bool getYet;
    bool inGetImageData;
}
+(id)sharedImgSelect;
-(void) showImgSelect;
-(id)init;
-(void)dealloc;
-(void)preparingImg:(id)obj;
@end
