package com.sharedushu.sharemind.BookManage;

import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.net.Socket;


/**
 * Created by flj on 2016/11/7.
 */
public class BookInetSearcher extends BookSearcher {
    private final String DEFAULT_ENCODE="utf-8";
    private String m_addr;
    private int m_port;
    private Socket m_sock;
    private String ERR_TAG ="BookInetSearcher Error";
    private static final int SOCK_TIMEOUT=10000;

    public BookInetSearcher(String addr,int port) {
        super();
        m_addr= addr;
        m_port= port;
        m_sock=null;
    }

    private void parseBookListResponse(String resp)
    {
        try {
            JSONArray jsonArray= new JSONArray(resp);
            for (int i=0;i<jsonArray.length();i++)
            {
                BookInfo info=new BookInfo(BookInfo.BOOK_TYPE_INET);
                JSONObject obj=jsonArray.getJSONObject(i);
                try {
                    String tag_str=obj.getString(BookDefine.TAG_BOOK_INFO_ID);
                    info.setBookId( Long.parseLong(tag_str));

                    tag_str=obj.getString(BookDefine.TAG_BOOK_INFO_NAME);
                    info.setBookName(tag_str);

                    tag_str=obj.getString(BookDefine.TAG_BOOK_INFO_AUTHOR);
                    info.setBookAuthor(tag_str);

                    tag_str=obj.getString(BookDefine.TAG_BOOK_INFO_PATH);
                    info.setBookHttpPath(tag_str);

                    notifyResult(info);
                }
                catch (JSONException e)
                {
                    e.printStackTrace();
                }
            }
        }catch (JSONException e)
        {
            e.printStackTrace();
        }

    }

    private String sendJsonObject(JSONObject jobj)
    {
        String resp=null;
        if(m_sock==null)
        {
            DebugPrintUtil.e(ERR_TAG,"Sockect Closed!");
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
        }catch (Exception e){
            e.printStackTrace();
        }

        return resp;
    }
    @Override
    public void search(String key) {
        int ret_code=0;
        notifyStart();

        JSONObject jobj=new JSONObject();
        try {
            jobj.put(BookDefine.TAG_CMD,BookDefine.CMD_GET_BOOK_LIST);
            if (key!=null) {
                jobj.put(BookDefine.TAG_BOOK_LIST_KEY, key);
            }

            String resp=sendJsonObject(jobj);
            if(resp!=null)
            {
                parseBookListResponse(resp);
            }
            else
            {
                ret_code=-1;
            }
        }catch (Exception e){
            ret_code=-1;
            e.printStackTrace();
        }

        notifyEnd(ret_code);
    }

    @Override
    public boolean open() {
        synchronized(this){
            if (m_sock == null) {
                m_sock = new Socket();
            }
        }

        try {
            InetSocketAddress serverAddr=new InetSocketAddress(m_addr,m_port);
            m_sock.connect(serverAddr,SOCK_TIMEOUT);
            m_sock.setSoTimeout(SOCK_TIMEOUT);
        } catch (Exception e) {
            close();
            e.printStackTrace();
        }

        return m_sock!=null;
    }

    @Override
    public void close() {
        Socket socket;

        synchronized (this){
            socket=m_sock;
        }

        if(socket!=null) {
            try {
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
