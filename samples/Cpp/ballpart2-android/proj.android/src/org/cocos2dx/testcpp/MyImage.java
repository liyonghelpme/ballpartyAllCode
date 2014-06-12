package org.cocos2dx.testcpp;

import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;

public class MyImage {
	public static final int PHOTO = 100;
	private static Activity act;
	
	
	public static void setAct(Activity a) {
		act = a;
	}
	
	public static void showImgSelect(){
		
		Cocos2dxHelper.runOnMainThread(new Runnable(){

			@Override
			public void run() {
				//外部sdcard 上面的图像数据
				Intent photo = new Intent(Intent.ACTION_PICK, android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
				//photo.setType("image/*");
			
				act.startActivityForResult(photo, PHOTO);
			}
			
		});
		
	}
	
	public static void storeImageData(Intent imageRet) {
		Uri sim = imageRet.getData();
		
	}
	
	
	
	public static native void nativeSaveImageData(final byte[] bytes);
	public static native void nativeStartGetImage();
}
