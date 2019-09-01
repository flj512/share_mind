package com.sharedushu.sharemind.BookManage;

/**
 * Created by flj on 2016/11/16.
 */
public class BookMark {
    private String m_name;
    private int m_local_id;
    private int m_chapter_id;
    private int m_index;
    private float m_read_percent=0;

    public void setName(String name)
    {
        m_name=name;
    }
    public String getName()
    {
        return m_name;
    }
    public void setLocalId(int id)
    {
        m_local_id=id;
    }
    public int getLocalId()
    {
        return m_local_id;
    }
    public void setIndex(int index)
    {
        m_index=index;
    }
    public int getIndex()
    {
        return m_index;
    }
    public void setChapterId(int id)
    {
        m_chapter_id=id;
    }
    public int getChapterId()
    {
        return m_chapter_id;
    }

    public float getReadPercent()
    {
        return m_read_percent;
    }
    public void setReadPercent(float percent)
    {
        m_read_percent=percent;
    }
}
