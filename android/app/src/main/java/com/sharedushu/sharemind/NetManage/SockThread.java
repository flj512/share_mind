package com.sharedushu.sharemind.NetManage;

import java.util.concurrent.BlockingDeque;
import java.util.concurrent.LinkedBlockingDeque;

/**
 * Created by flj on 2016/11/29.
 */
public class SockThread extends Thread {
    private BlockingDeque<Command> m_queue=new LinkedBlockingDeque<>();
    private boolean m_idle=true;
    private SockConnector m_sock;
    private boolean m_stopped =false;

    SockThread(String addr,int port)
    {
        m_sock=new SockConnector(addr,port);
    }
    @Override
    public void run() {
        while(true) {
            try {
                Command cmd = m_queue.take();
                if (is_stop_run()) {
                    m_sock.close();
                    break;
                }
                setIdle(false);
                m_sock.open();
                m_sock.sendCommand(cmd);
                if(!m_sock.isOpen())
                {
                   stop_run();
                    break;
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            setIdle(true);
        }
    }
    public synchronized void stop_run()
    {
        m_stopped =true;
    }
    public synchronized boolean is_stop_run()
    {
        return m_stopped;
    }
    public synchronized boolean isIdle()
    {
        return m_idle;
    }
    private synchronized void setIdle(boolean idle)
    {
        m_idle=idle;
    }

    public void putCommand(Command cmd)
    {
        m_queue.add(cmd);
    }
}
