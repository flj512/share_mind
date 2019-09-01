package com.sharedushu.sharemind.Tools;

import android.app.Application;
import android.content.Context;

/**
 * Created by flj on 2016/11/7.
 */
public class ApplicationE extends Application {
    private static Context context;
    @Override
    public void onCreate() {
        super.onCreate();
        context=getApplicationContext();
    }

    public static Context getContext()
    {
        return context;
    }
}
