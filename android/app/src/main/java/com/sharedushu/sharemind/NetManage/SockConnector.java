package com.sharedushu.sharemind.NetManage;

import android.text.TextUtils;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.net.Socket;


/**
 * Created by flj on 2016/11/29.
 */
public class SockConnector {
    public static final int SOCK_CONNECT_ERROR_OK=0;
    public static final int SOCK_CONNECT_ERROR_BAD_INPUT=BookDefine.CODE_LOCAL_START+1;
    public static final int SOCK_CONNECT_ERROR_SYSTEM=BookDefine.CODE_LOCAL_START+2;
    public static final int SOCK_CONNECT_ERROR_IO=BookDefine.CODE_LOCAL_START+3;
    public static final int SOCK_CONNECT_ERROR_NOT_READY=BookDefine.CODE_LOCAL_START+4;

    public final static String DEFAULT_ENCODE="utf-8";
    private String m_addr;
    private int m_port;
    private Socket m_sock;
    private int m_last_error=0;
    private String TAG ="BookInetSearcher Error";
    private static final int SOCK_TIMEOUT=10000;

    public SockConnector(String addr,int port) {

        m_addr= addr;
        m_port= port;
        m_sock=null;
    }

    private String sendJsonObject(JSONObject jobj,boolean resend_enalbe)
    {
        String resp=null;
        if(m_sock==null||m_sock.isClosed()||!m_sock.isConnected())
        {
            DebugPrintUtil.e(TAG,"Sockect Not Ready!");
            m_last_error=SOCK_CONNECT_ERROR_NOT_READY;
            return resp;
        }

        try {
            String json_str=jobj.toString();

            byte[] json_bytes=json_str.getBytes(DEFAULT_ENCODE);
            byte[] buff = new byte[BookDefine.CMD_HEADER_SIZE+json_bytes.length];

            buff[0]=(byte)BookDefine.CMD_INFO_HEADER0;
            buff[1]=(byte)BookDefine.CMD_INFO_HEADER1;
            buff[2]=(byte)BookDefine.CMD_INFO_HEADER2;
            buff[3]=(byte)BookDefine.CMD_INFO_HEADER3;
            buff[4]=(byte)(((json_bytes.length)>>8)&0xff);
            buff[5]=(byte)((json_bytes.length)&0xff);

            for(int i=0,j=6;i<json_bytes.length;i++,j++)
            {
                buff[j]=json_bytes[i];
            }

            m_sock.getOutputStream().write(buff);

            BufferedReader reader=new BufferedReader(new InputStreamReader(m_sock.getInputStream()));
            resp=reader.readLine();
            if(resp==null&&resend_enalbe) {//remote close,resend
                close();
                open();
                resp=sendJsonObject(jobj,false);
            }
        }
        catch (IOException e)
        {
            resp=null;
            m_last_error=SOCK_CONNECT_ERROR_IO;
            close();
            e.printStackTrace();
        }
        catch (Exception e){
            resp=null;
            m_last_error=SOCK_CONNECT_ERROR_SYSTEM;
            e.printStackTrace();
        }

        return resp;
    }
    public void sendCommand(Command cmd)
    {
        JSONObject object=cmd.onCreate();
        if(object==null)
        {
            cmd.onError(SOCK_CONNECT_ERROR_BAD_INPUT);
            return;
        }
        String resp=sendJsonObject(object,true);
        if(resp==null)
        {
            cmd.onError(m_last_error);
        }
        else {
            cmd.onResponse(resp);
        }
    }

    public boolean open() {

        if(m_sock==null||!m_sock.isConnected()) {
            close();
            m_sock = new Socket();
            try {
                InetSocketAddress serverAddr = new InetSocketAddress(m_addr, m_port);
                m_sock.connect(serverAddr, SOCK_TIMEOUT);
                m_sock.setSoTimeout(SOCK_TIMEOUT);
            } catch (Exception e) {
                close();
                e.printStackTrace();
            }
        }

        return (m_sock!=null&&m_sock.isConnected());
    }

    public void close() {
        if(m_sock!=null&&!m_sock.isClosed()) {
            DebugPrintUtil.v(TAG,"Closing Sock");
            try {
                m_sock.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        m_sock=null;
    }

    public boolean isOpen()
    {
        return (m_sock!=null&&m_sock.isConnected());
    }
}
