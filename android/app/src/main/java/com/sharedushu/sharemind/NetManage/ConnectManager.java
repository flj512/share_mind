package com.sharedushu.sharemind.NetManage;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

/**
 * Created by flj on 2016/11/29.
 */
public class ConnectManager {
    private static ConnectManager m_ConnectManager=null;
    private List<SockThread> m_threads;
    private final static String TAG="ConnectManager";

    private ConnectManager(){
        m_threads=new LinkedList<>();
    }

    public static synchronized  ConnectManager getSingleton()
    {
        if(m_ConnectManager==null)
        {
            m_ConnectManager=new ConnectManager();
        }

        return m_ConnectManager;
    }

    public void sendCommand(Command cmd)
    {
        Iterator<SockThread> iterator=m_threads.iterator();
        while (iterator.hasNext())
        {
            if(iterator.next().is_stop_run())
            {
                DebugPrintUtil.d(TAG,"Remove Stop Thread");
                iterator.remove();
            }
        }

        DebugPrintUtil.d(TAG,"Current Thread Number:"+m_threads.size());
        for(SockThread thread:m_threads)
        {
            if(thread.isIdle())
            {
                thread.putCommand(cmd);
                return;
            }
        }

        SockThread new_thread=new SockThread(BookDefine.SERVER_ADDR,BookDefine.SERVER_PORT);
        m_threads.add(new_thread);
        new_thread.putCommand(cmd);
        new_thread.start();
    }
}
