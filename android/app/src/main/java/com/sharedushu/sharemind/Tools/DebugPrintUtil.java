package com.sharedushu.sharemind.Tools;

import android.util.Log;

/**
 * Created by flj on 2016/11/7.
 */
public class DebugPrintUtil {
    public static final int VERBOSE=1;
    public static final int DEBUG=2;
    public static final int INFO=3;
    public static final int WARN=4;
    public static final int ERROR=5;
    public static final int DISABLE=6;

    public static final int ENABLE_LEVEL=ERROR;

    public static void v(String tag,String str)
    {
        if(ENABLE_LEVEL <= VERBOSE)
        {
            Log.v(tag,str);
        }
    }

    public static void d(String tag,String str)
    {
        if(ENABLE_LEVEL<=DEBUG)
        {
            Log.d(tag,str);
        }
    }

    public static void i(String tag,String str)
    {
        if(ENABLE_LEVEL<=INFO)
        {
            Log.i(tag,str);
        }
    }

    public static void w(String tag,String str)
    {
        if(ENABLE_LEVEL<=WARN)
        {
            Log.w(tag,str);
        }
    }

    public static void e(String tag,String str)
    {
        if(ENABLE_LEVEL<=ERROR)
        {
            Log.e(tag,str);
        }
    }
}
