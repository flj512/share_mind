package com.sharedushu.sharemind.BookManage;

import android.database.sqlite.SQLiteDatabase;

import com.sharedushu.sharemind.Database.BookDbManager;
import com.sharedushu.sharemind.Tools.ApplicationE;

import java.util.List;

/**
 * Created by flj on 2016/11/9.
 */
public class BookManager {
    private static BookManager m_sigleton=null;

    private BookDbManager m_dbManager;
    private SQLiteDatabase m_db;
    private BookManager(){
        m_dbManager=new BookDbManager(ApplicationE.getContext(),"BookShareMind.db",null,1);
        m_db=m_dbManager.getReadableDatabase();
    }

    public synchronized static BookManager getSingleton()
    {
        if(m_sigleton==null)
        {
            m_sigleton=new BookManager();
        }

        return m_sigleton;
    }

    public List<BookInfo> getAllBooklist()
    {
        return m_dbManager.getAllBookList(m_db);
    }

    public long insertBook(BookInfo info)
    {
        return m_dbManager.insertBook(m_db,info);
    }

    public void deleteBook(BookInfo info)
    {
        m_dbManager.deleteBook(m_db,info);
    }

    public void setBookCharCout(BookInfo info)
    {
        m_dbManager.setBookCharSize(m_db,info);
    }

    public void updateBookLastReadTime(long id)
    {
        m_dbManager.updateBookLastReadTime(m_db,id);
    }

    public void insertBookMark(BookMark mark)
    {
        m_dbManager.insertBookMark(m_db,mark);
    }

    public void modifyBookMark(BookMark mark)
    {
        m_dbManager.modifyBookMark(m_db,mark);
    }

    public BookMark getBookMark(int id,String name)
    {
        return m_dbManager.getBookMark(m_db,id,name);
    }
    public BookInfo getBookInfoByLocalId(int id)
    {
        return m_dbManager.getBookInfo(m_db,id);
    }
    public BookInfo getBookInfoByBookId(long id)
    {
        return m_dbManager.getBookInfo(m_db,id);
    }
}
