package com.sharedushu.sharemind.BookManage;

import java.io.Serializable;

/**
 * Created by flj on 2016/11/7.
 */
public class BookInfo implements Serializable{
    public static final int BOOK_TYPE_LOCAL=0;
    public static final int BOOK_TYPE_INET=1;
    public static final int BOOK_TYPE_UNSET=-1;

    private long m_bookId=0;
    private int m_local_id=0;
    private String m_bookName=null;
    private String m_bookAuthor=null;
    private String m_bookLocalPath=null;
    private String m_bookHttpPath=null;
    private int m_bookType=BOOK_TYPE_UNSET;
    private int m_char_count =0;

    public BookInfo(int type)
    {
        m_bookType=type;
    }
    public BookInfo()
    {
        m_bookType=BOOK_TYPE_UNSET;
    }

    public void setBookType(int type)
    {
        m_bookType=type;
    }
    public int getBookType()
    {
        return m_bookType;
    }
    public long getBookId()
    {
        return m_bookId;
    }
    public void setBookId(long id)
    {
        m_bookId=id;
    }
    public String getBookName()
    {
        return m_bookName;
    }

    public void setBookName(String name)
    {
        m_bookName=name;
    }

    public String getBookAuthor()
    {
        return m_bookAuthor;
    }

    public void setBookAuthor(String author)
    {
        m_bookAuthor=author;
    }

    public String getBookLocalPath()
    {
        return m_bookLocalPath;
    }

    public void setBookLocalPath(String path)
    {
        m_bookLocalPath=path;
    }

    public  String getBookHttpPath()
    {
        return m_bookHttpPath;
    }

    public void setBookHttpPath(String path)
    {
        m_bookHttpPath=path;
    }

    public void setBookCharCount(int count)
    {
        m_char_count =count;
    }

    public int getBookCharCount()
    {
        return m_char_count;
    }

    public int getBookLocalId()
    {
        return m_local_id;
    }

    public void setBookLocalId(int id)
    {
        m_local_id=id;
    }


    public String toString(){
        StringBuilder builder=new StringBuilder();
        builder.append("Id:"+m_bookId+"\n");
        builder.append("Name:"+m_bookName+"\n");
        builder.append("Author:"+m_bookAuthor+"\n");
        builder.append("HttpPath:"+m_bookHttpPath+"\n");
        builder.append("LocalPath:"+m_bookLocalPath+"\n");
        builder.append("Type:"+m_bookType);

        return builder.toString();
    }
}
