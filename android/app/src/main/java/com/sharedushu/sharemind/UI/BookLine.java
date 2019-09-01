package com.sharedushu.sharemind.UI;

/**
 * Created by flj on 2016/11/11.
 */
public class BookLine {
    private int m_startIndex;
    private String m_data;

    void setStartLineIndex(int index)
    {
        m_startIndex=index;
    }
    int getStartLineIndex()
    {
        return m_startIndex;
    }
    void setLineString(String line)
    {
        m_data=line;
    }
    String getLineString()
    {
        return m_data;
    }
}
