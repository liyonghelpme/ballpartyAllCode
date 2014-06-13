//
//  TestVoice.h
//  HelloCpp
//
//  Created by dozengame on 14-5-13.
//
//

#import <Foundation/Foundation.h>
#include "RedisInterface.h"
#import <AVFoundation/AVFoundation.h>
@interface TestVoice : NSObject <AVAudioRecorderDelegate, AVAudioPlayerDelegate> {
    AVAudioRecorder *recorder;
    AVAudioPlayer *player;
    int myVid;
    //NSString *recordPath;
    
    int playVid;
}

+(id)sharedRecord;
-(void)test;
-(void)pause;
-(void)stop;
-(void)play;
-(void)playTempVoice:(int)vid;
-(int)getLength:(const char*)fn;
-(void)testUtf8:(const char *)s;

@end
