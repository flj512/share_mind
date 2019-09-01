package com.sharedushu.sharemind.Database;

/**
 * Created by flj on 2016/11/20.
 */
public class BookCatalog {
    private String m_name;
    private String m_path;
    private int m_index;
    private int m_char_count=0;
    private int m_char_start=0;

    public BookCatalog(String name,String path,int index)
    {
        m_name=name;
        m_path=path;
        m_char_count=-1;//负数代表未知
        m_char_start=-1;
        m_index=index;
    }

    public String getName()
    {
        return m_name;
    }

    public void setCharCount(int charCount)
    {
        m_char_count=charCount;
    }

    public int getCharCount()
    {
        return m_char_count;
    }

    public void setCharStart(int charStart)
    {
        m_char_start=charStart;
    }

    public int getCharStart()
    {
        return m_char_start;
    }

    public String getPath()
    {
        return m_path;
    }
    public int getIndex()
    {
        return m_index;
    }
}
