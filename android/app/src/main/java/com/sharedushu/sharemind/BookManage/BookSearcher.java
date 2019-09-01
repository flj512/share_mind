package com.sharedushu.sharemind.BookManage;

import java.util.LinkedList;
import java.util.List;

/**
 * Created by flj on 2016/11/7.
 */
abstract public class BookSearcher {
    private List<BookSearchListener> m_listenr;

    public abstract boolean open();
    public abstract void search(String key);
    public abstract void close();

    public BookSearcher() {
        m_listenr=new LinkedList<BookSearchListener>();
    }


    public void addListener(BookSearchListener listener)
    {
        m_listenr.add(listener);
    }

    public void removeListener(BookSearchListener listener)
    {
        m_listenr.remove(listener);
    }

    public void notifyResult(BookInfo info)
    {
        for(BookSearchListener item:m_listenr)
        {
            item.onFind(info);
        }
    }

    public void notifyStart()
    {
        for(BookSearchListener item:m_listenr)
        {
            item.onStart();
        }
    }

    public void notifyEnd(int code)
    {
        for(BookSearchListener item:m_listenr)
        {
            item.onEnd(code);
        }
    }

}
