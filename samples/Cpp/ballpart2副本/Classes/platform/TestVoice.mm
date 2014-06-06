//
//  TestVoice.m
//  HelloCpp
//
//  Created by dozengame on 14-5-13.
//
//

#import "TestVoice.h"
#include "Logic.h"

@implementation TestVoice
+(id)sharedRecord{
    static TestVoice *sh = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sh = [[self alloc] init];
        
    });
    return sh;
}

string storeFile(const unsigned char*con, int len, int vid){
    //NSArray *savePath = [NSArray arrayWithObjects:[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject], [NSString stringWithFormat:@"tempAudio%d.m4a", vid], nil] ;
    //NSURL *outputFileURL = [NSURL fileURLWithPathComponents:savePath];
    
    
    NSLog(@"store File");
    //const char*fn = [[outputFileURL path] UTF8String];
    //NSLog([NSString stringWithFormat:@"filename %s", fn ]);
    
    
    NSString *tfile = [NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"tempAudio%d.m4a", vid]];
    const char *fn = [tfile UTF8String];
    
    FILE *f = fopen(fn, "wb");
    fwrite(con, 1, len, f);
    fclose(f);
    NSLog([NSString stringWithFormat:@"store File %s", fn]);
    
    //返回文件名称
    return fn;
}

int getMyRecordVid(){
    TestVoice *tv = [TestVoice sharedRecord];
    return tv->myVid;
}
int getVoiceLength(const char*fn) {
    return [[TestVoice sharedRecord] getLength:fn];
}

-(int)getLength:(const char *)fn{
    NSLog(@"get voice length");
    
    NSURL *fileURL = [NSURL fileURLWithPath:[NSString stringWithCString:fn ]];
    
    AVURLAsset *audioAsset = [AVURLAsset URLAssetWithURL:fileURL options:nil];
    NSLog([NSString stringWithFormat:@"%@", audioAsset]);
    CMTime audioDuration = audioAsset.duration;
    float audioT = CMTimeGetSeconds(audioDuration);
    NSLog(@"audio time %f", audioT);
    return audioT;
}

-(void) test{
    //保存自己的语音信息到本地数据存储中
    myVid = Logic::getInstance()->getVid();
    
    NSString *myVoiceName = [NSString stringWithFormat:@"tempAudio%d.m4a", myVid];
    NSString *mypath = [NSTemporaryDirectory() stringByAppendingPathComponent:myVoiceName];
   
    /*
    NSArray *pathComponents = [NSArray arrayWithObjects:
                               [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject],
                               myVoiceName,
                               nil];
    */
    
    //NSURL *outputFileURL = [NSURL fileURLWithPathComponents:pathComponents];
    NSLog(@"mypath");
    NSLog(mypath);
    NSURL *outputFileURL = [NSURL fileURLWithPath:mypath];
    AVAudioSession *session = [AVAudioSession sharedInstance];
    NSError *error;
    [session setCategory:AVAudioSessionCategoryPlayAndRecord error:&error];
    NSMutableDictionary *recordSetting = [[NSMutableDictionary alloc] init];
    [recordSetting setValue:[NSNumber numberWithInt:kAudioFormatMPEG4AAC] forKey:AVFormatIDKey];
    [recordSetting setValue:[NSNumber numberWithFloat:44100.0] forKey:AVSampleRateKey];
    [recordSetting setValue:[NSNumber numberWithInt: 2] forKey:AVNumberOfChannelsKey];
    
    if (recorder != nil) {
        [recorder release];
        recorder = nil;
    }
    
    recorder = [[AVAudioRecorder alloc] initWithURL:outputFileURL settings:recordSetting error:NULL];
    recorder.delegate = self;
    recorder.meteringEnabled = YES;
    [recorder prepareToRecord];
    
    
    if(!recorder.recording){
        AVAudioSession *session = [AVAudioSession sharedInstance];
        [session setActive:YES error:nil];
        [recorder record];
        
    }else {
        
    }
}
void testUtf8C(const char *s) {
    [[TestVoice sharedRecord] testUtf8:s];
}
-(void)testUtf8:(const char *)s {
    NSString *us = [NSString stringWithUTF8String:s];
    NSLog(us);

    NSUInteger rl = [us lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
    NSRange range= [us rangeOfComposedCharacterSequenceAtIndex:0];
    unichar ch = [us characterAtIndex:0];
    NSRange fullRange = NSMakeRange(0, [us length]);
    [us enumerateSubstringsInRange:fullRange options:NSStringEnumerationByComposedCharacterSequences usingBlock:^(NSString *substring, NSRange substringRange, NSRange enclosingRange, BOOL *stop){
        NSLog(@"%@ %@", substring, NSStringFromRange(substringRange));
    }];
    
    NSLog([NSString stringWithFormat:@"string length %lu %d %d %@", rl, range.location, range.length, ch]);
}


-(void)pause{
    [recorder pause];
}

-(void)stop {
    [recorder stop];
    AVAudioSession *av = [AVAudioSession sharedInstance];
    [av setActive:NO error:nil];
    //[recorder release];
}



TestVoice *tv;
void *startRecord(){
    tv = [TestVoice sharedRecord];
    [tv test];
    return tv;
}
void *stopRecord(){
    //[tv pause];
    [tv stop];
    return  tv;
}
void startPlay(){
    [tv play];
}

void playVoice(int vid){
    NSLog([NSString stringWithFormat:@"playVoice %d", vid]);
    
    TestVoice *tvv = [TestVoice sharedRecord];
    [tvv playTempVoice: vid];
}
-(void)playTempVoice:(int)vid{
    if (player != nil) {
        //停止当前语音播放
        [player stop];
        
        [player release];
        CCNotificationCenter::sharedNotificationCenter()->postNotification("finishPlay");
        player = nil;
    }
    
    //停止播放语音
    if (vid == -1) {
        return;
    }
    
    //当前正在播放的 声音id
    playVid = vid;
    //NSArray *savePath = [NSArray arrayWithObjects:[NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSUserDomainMask, YES) lastObject], [NSString stringWithFormat:@"tempAudio%d.m4a", vid], nil] ;
    NSString *saveP = [NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"tempAudio%d.m4a", vid]];
    
    //NSURL *outputFileURL = [NSURL fileURLWithPathComponents:savePath];
    NSLog(@"save path");
    NSLog(saveP);
    
    NSURL *outputFileURL = [NSURL fileURLWithPath:saveP];
    NSLog([NSString stringWithFormat:@"out url %@", outputFileURL.path]);
    
    UInt32 audioOverride = kAudioSessionOverrideAudioRoute_Speaker;
    AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(audioOverride), &audioOverride);
    
    player = [[AVAudioPlayer alloc] initWithContentsOfURL:outputFileURL error:nil];
    
    [player setDelegate:self];
    BOOL res = [player play];
    NSLog([NSString stringWithFormat:@"play ok %d", res]);
}


-(void)audioRecorderDidFinishRecording:(AVAudioRecorder *)recorder successfully:(BOOL)flag{
    NSLog(@"finish record");
}

-(void)play{
    NSLog(@"play audio");
    NSLog([NSString stringWithFormat:@"%@", recorder.url]);
    player = [[AVAudioPlayer alloc] initWithContentsOfURL:recorder.url error:nil];
    [player setDelegate:self];
    [player play];
}
-(void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag{
    /*
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle: @"Done"
                                                        message: @"Finish playing the recording!"
                                                       delegate: nil
                                              cancelButtonTitle:@"OK"
                                              otherButtonTitles:nil];
        [alert show];
    */
    //特定播放的声音 如果当前正在播放则停止一切播放即可
    CCLog("finish play");
    //char buf[256];
    //sprintf(buf, "finishPlay_%d", playVid);
    CCNotificationCenter::sharedNotificationCenter()->postNotification("finishPlay");
}



string getFileName(){
    NSLog(@"get File Name");
    NSURL *url = tv->recorder.url;
    NSLog([NSString stringWithFormat:@"%@", tv->recorder.url ]);
    //NSString * s = [url absoluteString];
    NSString *p = [url path];
    //NSLog([url absoluteURL]);
    //const char *fn = [url fileSystemRepresentation];
    return  string([p UTF8String]);
}
@end
