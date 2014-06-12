package org.cocos2dx.testcpp;

import java.io.IOException;

import org.cocos2dx.lib.Cocos2dxHelper;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaRecorder;
import android.media.MediaRecorder.OnErrorListener;
import android.media.MediaRecorder.OnInfoListener;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.util.Log;

public class MyAudio {
	public static MediaRecorder recorder;
	public static MediaPlayer mediaPlayer;
	public static Handler mainThreadHandler = null;
	public static Context con;
	public static GLSurfaceView surfaceView = null;
	private static native void nativeNotification(final String pText);
	
	
	public static void init(Context context){
		con = context;
		if (null == mainThreadHandler) {
			mainThreadHandler = new Handler();
		}
	}
	public static void setGLSurfaceView(GLSurfaceView g){
		surfaceView = g;
	}
	
	public static void runOnMainThread(Runnable r){
		if (mainThreadHandler != null) {
			mainThreadHandler.post(r);
		}
	}
	public static void runOnGLThread(Runnable r){
		if (surfaceView != null) {
			surfaceView.queueEvent(r);
		}
	}
	
	
	
	//MediaRecorder 必须在 主线程调用么?
	public static void recordAudio(final String fn){
		Log.d("Audio", "start record "+fn);
		runOnMainThread(new Runnable() {
			
			@Override
			public void run() {
				Log.d("Audio", "run on main thread recorder");
				
				// TODO Auto-generated method stub
				recorder = new MediaRecorder();
				recorder.setAudioSource(MediaRecorder.AudioSource.MIC);
				recorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
				recorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
				
				
				//recorder.setOutputFile("/sdcard/sample.3gp");
				recorder.setOutputFile(fn);
				try {
					recorder.prepare();
				} catch (IllegalStateException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				recorder.start();
			}
		});
		
		
		/*
		
		
		recorder.setOutputFile("/sdcard/sample.m4a");
		//recorder.setOutputFile(fn);
		
		OnErrorListener errorListener = new OnErrorListener(){

			@Override
			public void onError(MediaRecorder arg0, int arg1, int arg2) {
				// TODO Auto-generated method stub
				Log.e("Audio", "error "+arg1+" "+arg2);
			}
			
		};
		
		recorder.setOnErrorListener(errorListener );
		OnInfoListener infoListener = new OnInfoListener(){

			@Override
			public void onInfo(MediaRecorder arg0, int arg1, int arg2) {
				// TODO Auto-generated method stub
				Log.e("Audio", "info "+arg1+" "+arg2);
			}
			
		};
		recorder.setOnInfoListener(infoListener );
		
		
		try{
			recorder.prepare();
			recorder.start();
		}catch(IllegalStateException e){
			e.printStackTrace();
		}catch(IOException e){
			e.printStackTrace();
		}
		Log.d("Audio", "recording!");
		
		*/
	}
	
	public static void stopRecord(){
		Log.d("Audio", "stop recording");
		runOnMainThread(new Runnable() {
			
			@Override
			public void run() {
				// TODO Auto-generated method stub
				if (recorder != null) {
					recorder.stop();
					recorder.reset();
					recorder.release();
					recorder = null;	
				}
			}
		});
	}
	
	
	public static int getVoiceLength(final String fn){
		Log.e("Audio", "getVoiceLength "+fn);
		
		//设定未获取时间
		SharedPreferences setting = ((Activity)con).getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
		SharedPreferences.Editor editor = setting.edit();
		editor.putBoolean("getAudioTime", false);
		editor.putInt("audioTime", 0);
		editor.commit();
		
		
		
		runOnMainThread(new Runnable() {
			
			@Override
			public void run() {
				
				if (mediaPlayer != null) {
					Log.d("Audio", "media player start yet so not start again");
					return;
				}
				
				// TODO Auto-generated method stub
				mediaPlayer = new MediaPlayer();
				mediaPlayer.setOnBufferingUpdateListener(new OnBufferingUpdateListener(){

					@Override
					public void onBufferingUpdate(MediaPlayer arg0, int arg1) {
						
					}
					
				});
				mediaPlayer.setOnPreparedListener(new OnPreparedListener() {
					
					@Override
					public void onPrepared(MediaPlayer mp) {
						// TODO Auto-generated method stub
						Log.d("Audio", "real duration "+mp.getDuration());
						mediaPlayer.release();
						mediaPlayer = null;
					}
				});
				
				mediaPlayer.reset();
				
				try {
					mediaPlayer.setDataSource(fn);
				} catch (IllegalArgumentException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (SecurityException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IllegalStateException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				try {
					mediaPlayer.prepare();
				} catch (IllegalStateException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				Integer td = mediaPlayer.getDuration();
				Log.e("Audio", "audio length "+td);
				
				
				//SharedPreference 线程安全的
				SharedPreferences setting = ((Activity)con).getSharedPreferences(Cocos2dxHelper.PREFS_NAME, 0);
				SharedPreferences.Editor editor = setting.edit();
				editor.putBoolean("getAudioTime", true);
				editor.putInt("audioTime", td);
				editor.commit();
				
				//通知 render thread 获取成功了
				
				runOnGLThread(new Runnable() {
					
					@Override
					public void run() {
						nativeNotification("VOICE");		
					}
				});
				
				
				//return td;
			}
		});
		
		
		return 0;
		
	}
}
