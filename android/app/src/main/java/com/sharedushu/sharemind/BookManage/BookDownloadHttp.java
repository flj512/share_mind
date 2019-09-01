package com.sharedushu.sharemind.BookManage;

import android.content.Context;
import android.os.AsyncTask;

import com.sharedushu.sharemind.Tools.ApplicationE;
import com.sharedushu.sharemind.Tools.BookPath;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;


/**
 * Created by flj on 2016/11/7.
 */
public class BookDownloadHttp extends AsyncTask<String,Integer,Boolean> {
    private static final String TAG="BookDownLoad";
    private static final int MAX_FILE_SIZE=8*1024*1024;//8M
    public static final int CONNECT_TIMEOUT=10000;
    public static final int READ_TIMEOUT=10000;
    private HttpURLConnection m_connection=null;
    private boolean m_user_canceled=false;

    public BookDownloadHttp() {
        super();
        m_user_canceled=false;
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
        InputStream response=null;
        FileOutputStream save_file;
        BufferedOutputStream writer=null;
        Boolean ret=Boolean.TRUE;

        if(isUserCanceled())
        {
            return Boolean.FALSE;
        }

        try {
            //make download path
            File donwload_path=new File(BookPath.getSDDownloadPath());
            if(!donwload_path.exists()||!donwload_path.isDirectory())
            {
                if(!donwload_path.mkdirs())
                {
                    DebugPrintUtil.e(TAG,"IO Event:Make Download Path Fail");
                    return Boolean.FALSE;
                }
            }

            URL url = new URL(params[0]);
            HttpURLConnection connection;

            connection=(HttpURLConnection) url.openConnection();
            synchronized (this) {
                m_connection = connection;
            }
            m_connection.setRequestMethod("GET");
            m_connection.setConnectTimeout(CONNECT_TIMEOUT);
            m_connection.setReadTimeout(READ_TIMEOUT);
            m_connection.connect();

            if(isUserCanceled())
            {
                ret=Boolean.FALSE;
            }
            else {
                //save_file = ApplicationE.getContext().openFileOutput(params[1], Context.MODE_PRIVATE);
                save_file=new FileOutputStream(new File(BookPath.mapSDDownloadPath(params[1])));
                writer = new BufferedOutputStream(save_file);
                response = m_connection.getInputStream();
                BufferedInputStream reader = new BufferedInputStream(response);

                int rd_byte,total=0;
                while ((rd_byte = reader.read()) != -1) {
                    total++;
                    if(total>MAX_FILE_SIZE)
                    {
                        ret=Boolean.FALSE;
                        DebugPrintUtil.e(TAG,"Safe Event:File too big");
                        break;
                    }
                    writer.write(rd_byte);
                }
            }
        }
        catch(Exception e){
            ret=Boolean.FALSE;
            e.printStackTrace();
        } finally {
            try {
                if (response != null) {
                    response.close();
                }
                if (writer != null) {
                    writer.close();
                }
            }catch (Exception e) {
                e.printStackTrace();
            }
            if(m_connection!=null)
            {
                m_connection.disconnect();
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
        setUserCanceled();

        HttpURLConnection connection;
        synchronized (this) {
            connection=m_connection;

        }

        if (connection != null) {
            connection.disconnect();
        }
    }
}
