package com.sharedushu.sharemind.BookManage;

import android.os.AsyncTask;

import com.sharedushu.sharemind.NetManage.SockConnector;
import com.sharedushu.sharemind.Tools.BookPath;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;


/**
 * Created by flj on 2016/12/20.
 */
public class BookDownloadLite extends AsyncTask<String,Integer,Boolean> {
    private static final String TAG="BookDownLoadLite";
    private static final int MAX_FILE_SIZE=8*1024*1024;//8M
    public static final int CONNECT_TIMEOUT=60000;
    public static final int READ_TIMEOUT=30000;
    private String m_addr;
    private int m_port;
    private Socket m_sock;
    private boolean m_user_canceled=false;

    public BookDownloadLite(String addr,int port) {
        super();
        m_user_canceled=false;
        m_addr=addr;
        m_port=port;
        m_sock=null;
    }

    private synchronized void setUserCanceled()
    {
        m_user_canceled=true;
    }
    private synchronized boolean isUserCanceled()
    {
        return m_user_canceled;
    }
    @Override
    protected void onPreExecute() {
        super.onPreExecute();
    }

    @Override
    protected void onPostExecute(Boolean aBoolean) {
        super.onPostExecute(aBoolean);
    }

    @Override
    protected void onProgressUpdate(Integer... values) {
        super.onProgressUpdate(values);
    }

    @Override
    protected Boolean doInBackground(String... params) {
        Boolean ret=Boolean.FALSE;
        InputStream response=null;
        OutputStream request=null;
        FileOutputStream save_file;
        BufferedOutputStream writer=null;

        try {
            //make download path
            File donwload_path=new File(BookPath.getSDDownloadPath());
            if(!donwload_path.exists()||!donwload_path.isDirectory())
            {
                if(!donwload_path.mkdirs())
                {
                    DebugPrintUtil.e(TAG,"IO Event:Make Download Path Fail");
                    return ret;
                }
            }

            synchronized (this) {
                m_sock = new Socket();
            }
            if(isUserCanceled())
            {
                m_sock.close();
                return ret;
            }
            InetSocketAddress serverAddr=new InetSocketAddress(m_addr,m_port);
            m_sock.connect(serverAddr,CONNECT_TIMEOUT);
            m_sock.setSoTimeout(READ_TIMEOUT);

            byte[] name=params[1].getBytes();
            byte[] buff = new byte[BookDefine.CMD_HEADER_SIZE+name.length];

            buff[0]=(byte)BookDefine.CMD_INFO_HEADER0;
            buff[1]=(byte)BookDefine.CMD_INFO_HEADER1;
            buff[2]=(byte)BookDefine.CMD_INFO_HEADER2;
            buff[3]=(byte)BookDefine.CMD_INFO_HEADER3;
            buff[4]=(byte)(((name.length)>>8)&0xff);
            buff[5]=(byte)((name.length)&0xff);

            for(int i=0,j=6;i<name.length;i++,j++)
            {
                buff[j]=name[i];
            }
            request=m_sock.getOutputStream();
            request.write(buff);


            //save_file = ApplicationE.getContext().openFileOutput(params[1], Context.MODE_PRIVATE);
            save_file=new FileOutputStream(new File(BookPath.mapSDDownloadPath(params[1])));
            writer = new BufferedOutputStream(save_file);
            response = m_sock.getInputStream();
            BufferedInputStream reader = new BufferedInputStream(response);

            int rd_byte,total=0,file_sz=0;

            while ((rd_byte = reader.read()) != -1) {
                total++;
                if(total<=4)
                {
                    file_sz<<=8;
                    file_sz+=rd_byte;
                }
                else {
                    if (total > MAX_FILE_SIZE + 4) {
                        DebugPrintUtil.e(TAG, "Safe Event:File too big");
                        break;
                    }
                    writer.write(rd_byte);
                }
            }

            if(total>=4&&file_sz+4==total)
            {
                ret=Boolean.TRUE;
            }
            else
            {
                DebugPrintUtil.e(TAG,"FileSz="+file_sz+" total="+total);
            }

        }
        catch(Exception e){
            e.printStackTrace();
        } finally {
            try {
                if (response != null) {
                    response.close();
                }
                if (writer != null) {
                    writer.close();
                }
                if(request!=null)
                {
                    request.close();
                }

                if(m_sock!=null)
                {
                    m_sock.close();
                }

            }catch (Exception e) {
                e.printStackTrace();
            }
            finally {
                m_sock=null;
            }

            if(ret==Boolean.FALSE)
            {
                //ApplicationE.getContext().deleteFile(params[1]);
                BookPath.delete(BookPath.mapSDDownloadPath(params[1]));
            }
        }
        return ret;
    }
    public void cancle_download()
    {
        Socket socket;

        setUserCanceled();
        synchronized (this){
            socket=m_sock;
        }

        if(socket!=null)
        {
            try{
                socket.close();
            }catch (Exception e){
                e.printStackTrace();
            }
        }
    }
}
