package com.sharedushu.sharemind.NetManage;

import android.os.Handler;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.Tools.BookTextUtils;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import org.json.JSONException;
import org.json.JSONObject;

import java.security.MessageDigest;

/**
 * Created by flj on 2016/11/29.
 */
public class PublishOpinionCommand extends SequenceCommand implements Command {
    private long m_bookid;
    private int m_chapter;
    private int m_start;
    private String m_opinion;
    private String m_user;
    private String m_key;
    private static final String TAG="PublishOpinionCommand";

    public PublishOpinionCommand(int response_id,int cmd_id,long book_id,int chapter,int start,String opinion,String user,String key,Handler handler) {
        super(cmd_id,response_id,handler);
        m_bookid=book_id;
        m_chapter=chapter;
        m_start=start;
        m_opinion=opinion;
        m_user=user;
        m_key=key;
    }

    @Override
    public JSONObject onCreate() {

        JSONObject jobj=new JSONObject();
        String verifyCode=String.valueOf(System.currentTimeMillis()/1000);
        String hash;

        try {
            MessageDigest digest=MessageDigest.getInstance("MD5");
            digest.reset();
            digest.update(String.valueOf(m_bookid).getBytes(SockConnector.DEFAULT_ENCODE));
            digest.update(String.valueOf(m_chapter).getBytes(SockConnector.DEFAULT_ENCODE));
            digest.update(String.valueOf(m_start).getBytes(SockConnector.DEFAULT_ENCODE));
            digest.update(m_user.getBytes(SockConnector.DEFAULT_ENCODE));
            digest.update(m_key.getBytes(SockConnector.DEFAULT_ENCODE));
            digest.update(verifyCode.getBytes(SockConnector.DEFAULT_ENCODE));
            digest.update(m_opinion.getBytes(SockConnector.DEFAULT_ENCODE));

            hash= BookTextUtils.bytesToHexString(digest.digest());

            jobj.put(BookDefine.TAG_CMD,BookDefine.CMD_SET_OPINION);

            jobj.put(BookDefine.TAG_BOOK_INFO_ID, m_bookid);
            jobj.put(BookDefine.TAG_BOOK_OPINION_CHAPTER, m_chapter);
            jobj.put(BookDefine.TAG_BOOK_OPINION_START, m_start);
            jobj.put(BookDefine.TAG_BOOK_OPINION, m_opinion);
            jobj.put(BookDefine.TAG_BOOK_USER,m_user);
            jobj.put(BookDefine.TAG_BOOK_VERIFY_CODE,verifyCode);
            jobj.put(BookDefine.TAG_BOOK_SHA,hash);

        }catch (Exception e){
            jobj=null;
            e.printStackTrace();
        }

        return jobj;
    }

    @Override
    public void onResponse(String response) {
        int ret;
        try {
            JSONObject resp = new JSONObject(response);
            String result = resp.getString(BookDefine.TAG_BOOK_RESULT);
            ret = resp.getInt(BookDefine.TAG_BOOK_RESULT_CODE);
            DebugPrintUtil.i(TAG, result + ",code:" + ret);
        }catch (JSONException e)
        {
            ret=BookDefine.CODE_BAD_DATA;
            e.printStackTrace();
        }

        sendError(ret);
    }

    @Override
    public void onError(int error) {
        sendError(error);
    }
}
