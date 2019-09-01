package com.sharedushu.sharemind.NetManage;

import android.os.Handler;
import android.os.Message;

/**
 * Created by flj on 2016/11/29.
 */
public class SequenceCommand {
    private int m_cmd_id;
    private int m_response_id;
    private Handler m_handle;

    public SequenceCommand(int cmd_id,int reponse_id,Handler handler)
    {
        m_cmd_id=cmd_id;
        m_response_id=reponse_id;
        m_handle=handler;
    }

    public void sendMessage(Object obj)
    {
        Message message=new Message();

        message.what=m_response_id;
        message.arg1=m_cmd_id;
        message.arg2=SockConnector.SOCK_CONNECT_ERROR_OK;
        message.obj=obj;

        m_handle.sendMessage(message);
    }
    public void sendError(int error)
    {
        Message message=new Message();

        message.what=m_response_id;
        message.arg1=m_cmd_id;
        message.arg2=error;

        m_handle.sendMessage(message);
    }
}
