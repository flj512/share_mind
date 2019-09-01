package com.sharedushu.sharemind.NetManage;

import org.json.JSONObject;

/**
 * Created by flj on 2016/11/29.
 */
public interface Command {
    public JSONObject onCreate();
    public void onResponse(String response);
    public void onError(int error);
}
