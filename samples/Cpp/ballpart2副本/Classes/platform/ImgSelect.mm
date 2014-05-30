//
//  ImgSelect.m
//  TestCpp
//
//  Created by dozengame on 14-5-19.
//
//

#import "ImgSelect.h"
#import "EAGLView.h"
#include "Logic.h"

@implementation ImgSelect
+(id)sharedImgSelect{
    static ImgSelect *sh = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sh = [[self alloc] init];
        
    });
    return sh;
}
-(void)dealloc{
    if (imgdata != NULL) {
        free(imgdata);
        imgdata = NULL;
    }
    [super dealloc];
    
}
-(id)init{
    self = [super init];
    if (self) {
        self->imgdata = NULL;
        self->len = 0;
        self->getYet = false;
        inGetImageData = false;
        
        NSLog(@"init imgdata");
    }
    
    return self;
}
void openImageSelect(){
    ImgSelect *ig = [ImgSelect sharedImgSelect];
    [ig showImgSelect];
}
-(void) showImgSelect {
    //正在获取图片数据则 不要 再次打开窗口了
    if (inGetImageData) {
        return;
    }
    
    getYet = false;
    UIImagePickerController *ipc = [[UIImagePickerController alloc] init];
    ipc.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
    ipc.delegate = self;
    
    [[EAGLView sharedEGLView] addSubview:ipc.view];
    [ipc viewWillAppear:YES];
    [ipc viewDidAppear:YES];
    
    
}

-(void)imagePickerControllerDidCancel:(UIImagePickerController *)picker{
    NSLog(@"cancel");
    [picker.view removeFromSuperview];
}
-(void)preparingImg:(id) obj{
    NSDictionary *info = obj;
    
    struct timeval start;
    gettimeofday(&start, NULL);
    NSLog(@"select image info %@", info);
    UIImage *img = [info objectForKey:@"UIImagePickerControllerOriginalImage"];
    CGFloat newWidth = IMG_WIDTH;
    CGFloat newHeight  = IMG_HEIGHT;
    CGFloat minSca = MIN(newWidth/img.size.width, newHeight/img.size.height);
    UIImage *newImage = img;
    //缩放限制 图片的比例大小
    if (minSca < 1) {
        CGSize newSize  = CGSizeMake(minSca*img.size.width, minSca*img.size.height);
        UIGraphicsBeginImageContextWithOptions(newSize, NO, 0.0);
        [img drawInRect:CGRectMake(0, 0, newSize.width, newSize.height)];
        newImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
    }
    NSLog(@"scale image finish");
    //time_t endt;
    //time(&endt);
    struct timeval endt;
    gettimeofday(&endt, NULL);
    double ett = endt.tv_sec+(double)endt.tv_usec/1000000.0;
    double stt = start.tv_sec+(double)start.tv_usec/1000000.0;
    
    float passTime = ett-stt;
    NSLog(@"scale image time");
    NSLog([NSString stringWithFormat:@"%f", passTime]);
    
    
    
    NSData *data = UIImagePNGRepresentation(newImage);
    //NSString *b64 = data.base64Encoding;
    //[b64 UTF8String]
    NSLog(@"getImageData use data to initial Cocos2dx CCSprite then sendData to network");
    if (imgdata != NULL) {
        free(imgdata);
        imgdata = NULL;
    }
    imgdata = malloc(data.length*sizeof(char));
    memcpy(imgdata, data.bytes, data.length);
    len = data.length;
    NSLog([NSString stringWithFormat:@"image size %d", len]);
    
    //外部检测这歌状态即可
    inGetImageData = false;
    getYet = true;
    
    
    //time_t overt;
    //time(&overt);
    NSLog(@"getYet set image data time");
    struct timeval overt;
    gettimeofday(&overt, NULL);
    double ott = overt.tv_sec+(double)overt.tv_usec/1000000.0;
    float ot = ott-ett;
    NSLog([NSString stringWithFormat:@"%f", ot]);

}

-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info{
    [picker.view removeFromSuperview];
    //time_t start;
    //time(&start);
    
    inGetImageData = true;
    [NSThread detachNewThreadSelector:@selector(preparingImg:) toTarget:self withObject:info];
    
    /*
    struct timeval start;
    gettimeofday(&start, NULL);
    NSLog(@"select image info %@", info);
    UIImage *img = [info objectForKey:@"UIImagePickerControllerOriginalImage"];
    CGFloat newWidth = IMG_WIDTH;
    CGFloat newHeight  = IMG_HEIGHT;
    CGFloat minSca = MIN(newWidth/img.size.width, newHeight/img.size.height);
    UIImage *newImage = img;
    //缩放限制 图片的比例大小
    if (minSca < 1) {
        CGSize newSize  = CGSizeMake(minSca*img.size.width, minSca*img.size.height);
        UIGraphicsBeginImageContextWithOptions(newSize, NO, 0.0);
        [img drawInRect:CGRectMake(0, 0, newSize.width, newSize.height)];
        newImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
    }
    NSLog(@"scale image finish");
    //time_t endt;
    //time(&endt);
    struct timeval endt;
    gettimeofday(&endt, NULL);
    double ett = endt.tv_sec+(double)endt.tv_usec/1000000.0;
    double stt = start.tv_sec+(double)start.tv_usec/1000000.0;
    
    float passTime = ett-stt;
    NSLog(@"scale image time");
    NSLog([NSString stringWithFormat:@"%f", passTime]);
    
    
    
    NSData *data = UIImagePNGRepresentation(newImage);
    //NSString *b64 = data.base64Encoding;
    //[b64 UTF8String]
    NSLog(@"getImageData use data to initial Cocos2dx CCSprite then sendData to network");
    if (imgdata != NULL) {
        free(imgdata);
        imgdata = NULL;
    }
    imgdata = malloc(data.length*sizeof(char));
    memcpy(imgdata, data.bytes, data.length);
    len = data.length;
    NSLog([NSString stringWithFormat:@"image size %d", len]);
    getYet = true;
    
    
    //time_t overt;
    //time(&overt);
    NSLog(@"getYet set image data time");
    struct timeval overt;
    gettimeofday(&overt, NULL);
    double ott = overt.tv_sec+(double)overt.tv_usec/1000000.0;
    float ot = ott-ett;
    NSLog([NSString stringWithFormat:@"%f", ot]);
    */
}
void *getImage(int *length){
    ImgSelect *ig = [ImgSelect sharedImgSelect];
    NSLog([NSString stringWithFormat:@"getImage %x %d", ig->imgdata, ig->len]);
    if (ig->imgdata != NULL) {
        *length = ig->len;
        return ig->imgdata;
    }
    *length = 0;
    return NULL;
}

bool checkGetYet(){
    ImgSelect *ig = [ImgSelect sharedImgSelect];
    return ig->getYet;
}
@end
