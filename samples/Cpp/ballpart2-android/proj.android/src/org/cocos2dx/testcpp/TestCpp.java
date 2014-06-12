/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.testcpp;

import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxHelper;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.BitmapFactory.Options;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

public class TestCpp extends Cocos2dxActivity{
	
    protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);	
		Cocos2dxHelper.init();
		MyAudio.init(this);
		MyImage.setAct(this);
    }
    

    public Cocos2dxGLSurfaceView onCreateView() {
    	Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
    	// TestCpp should create stencil buffer
    	glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
    	
    	MyAudio.setGLSurfaceView(glSurfaceView);
    	
    	return glSurfaceView;
    }
    
    private Bitmap decodeUri(Uri sim) throws FileNotFoundException{
    	Log.d("Image", "decode Uri");
    	
		BitmapFactory.Options o = new BitmapFactory.Options();
		o.inJustDecodeBounds = true;
		BitmapFactory.decodeStream(getContentResolver().openInputStream(sim), null, o);
		final int REQUIRED_SIZE = 600;
		int width_tmp = o.outWidth, height_tmp = o.outHeight;
		//float scale = Math.min(REQUIRED_SIZE*1.0f/o.outWidth, REQUIRED_SIZE*1.0f/o.outHeight);
		int scale = 1;
		while(true) {
			if(width_tmp/2 < REQUIRED_SIZE && height_tmp < REQUIRED_SIZE) {
				break;
			}
			width_tmp /= 2;
			height_tmp /= 2;
			scale *= 2;
		}
		Log.d("Image", "decode size scale "+o.outWidth+" "+o.outHeight+" "+scale);
		
		BitmapFactory.Options o2 = new BitmapFactory.Options();
		o2.inSampleSize = scale;
		
		//获取采样数据
		return BitmapFactory.decodeStream(getContentResolver().openInputStream(sim), null, o2);
	}
    
    //需要一个 中心来分发 返回的 Activity 请求  注册请求对象
    //打开图片 成功之后 将图片数据存储到 本地 当用户需要的时候 返回给 用户 调用static 静态 java JNI 方法
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent imageRet){
    	switch(requestCode){
    	//调用native 方法 保存图像数据
    	case MyImage.PHOTO:
    		if (resultCode == RESULT_OK) {
    			Uri sim = imageRet.getData();
				Log.d("Image", "select Image "+sim.getPath());
				
    			try {
    				MyImage.nativeStartGetImage();
    				
					Bitmap bm = decodeUri(sim);
					
					int wid = bm.getWidth();
					int hei = bm.getHeight();
					float reqS = 300;
					float scale = Math.min(reqS/wid, reqS/hei);
					if (scale < 1) {
						Bitmap resb = Bitmap.createScaledBitmap(bm, (int)(wid*scale), (int)(hei*scale), false);
						bm = resb;
						//resb.compress(Bitmap.CompressFormat.PNG, 90, stream)
					}
					
					
					//在渲染 线程中 通知 c++ 保存字节数组数据
					final ByteArrayOutputStream bao = new ByteArrayOutputStream();
					bm.compress(Bitmap.CompressFormat.PNG, 90, bao);
					
					Cocos2dxHelper.getsCocos2dxHelperListener().runOnGLThread(new Runnable() {
						
						@Override
						public void run() {
							Log.d("Image", "nativeSaveImageData");
							MyImage.nativeSaveImageData(bao.toByteArray());		
						}
					});
					
					
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
    			
			}else {
				Log.d("Image", "cancel select Image");
				runOnGLThread(new Runnable() {
					@Override
					public void run() {
						Log.d("Image", "finish select cancel");
						//停止图像选择 恢复 之前 pause的 ChatView
						Cocos2dxHelper.nativeSendNotification("stopImageSelect");
					}
				});
			}
    	}
    }
    
    
    static {
        System.loadLibrary("testcpp");
    }     
}
