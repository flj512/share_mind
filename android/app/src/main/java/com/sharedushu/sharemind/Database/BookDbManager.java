package com.sharedushu.sharemind.Database;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.BookManage.BookMark;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by flj on 2016/11/9.
 */
public class BookDbManager extends SQLiteOpenHelper {
    //table bookinfo
    private static final String BOOKINFO_TABLE="bookinfo";
    private static final String BOOKINFO_TABLE_LOCAL_ID="local_id";
    private static final String BOOKINFO_TABLE_ID="id";
    private static final String BOOKINFO_TABLE_NAME="name";
    private static final String BOOKINFO_TABLE_AUTHOR="author";
    private static final String BOOKINFO_TABLE_LOCAL_PATH="local_path";
    private static final String BOOKINFO_TABLE_PATH="http_path";
    private static final String BOOKINFO_TABLE_CHAR_COUNT ="char_count";
    private static final String BOOKINFO_TABLE_LAST_READ_TIME ="last_read_time";

    private static final String CREATE_BOOKINFO_TABLE="create table "+BOOKINFO_TABLE+"(" +
            BOOKINFO_TABLE_LOCAL_ID+" integer primary key autoincrement," +
            BOOKINFO_TABLE_ID+" integer," +
            BOOKINFO_TABLE_NAME+" text," +
            BOOKINFO_TABLE_AUTHOR+" text," +
            BOOKINFO_TABLE_LOCAL_PATH+" text unique," +
            BOOKINFO_TABLE_PATH+" text unique," +
            BOOKINFO_TABLE_CHAR_COUNT +" integer," +
            BOOKINFO_TABLE_LAST_READ_TIME+" datetime default current_timestamp)";

    private static final String QUERY_BOOKINFO_TABLE_ALL="select * from "+BOOKINFO_TABLE+" order by "+BOOKINFO_TABLE_LAST_READ_TIME+" desc";

    //table bookmark
    private static final String BOOKMARK_TABLE="bookmark";
    private static final String BOOKMARK_TABLE_LOCAL_ID="local_id";
    private static final String BOOKMARK_TABLE_CHAPTER_ID="chapter_id";
    private static final String BOOKMARK_TABLE_INDEX="position_index";
    private static final String BOOKMARK_TABLE_NAME="name";
    private static final String BOOKMARK_TABLE_PERCENT="percent";

    private static final String CREATE_BOOKMART_TABLE="create table "+BOOKMARK_TABLE+"("+
            BOOKMARK_TABLE_LOCAL_ID+" interger,"+
            BOOKMARK_TABLE_CHAPTER_ID+" interger,"+
            BOOKMARK_TABLE_INDEX+" interger,"+
            BOOKMARK_TABLE_NAME+" text not null,"+
            BOOKMARK_TABLE_PERCENT+" real,"+
            //"primary key("+BOOKMARK_TABLE_LOCAL_ID+","+BOOKMARK_TABLE_CHAPTER_ID+","+BOOKMARK_TABLE_INDEX+"),"+
            "foreign key("+BOOKMARK_TABLE_LOCAL_ID+") references "+BOOKINFO_TABLE+"("+BOOKINFO_TABLE_LOCAL_ID+")"+"on delete cascade,"+
            "primary key("+BOOKMARK_TABLE_LOCAL_ID+","+BOOKMARK_TABLE_NAME+")"+
            ")";

    public BookDbManager(Context context, String name, SQLiteDatabase.CursorFactory factory, int version) {
        super(context, name, factory, version);
    }

    @Override
    public void onOpen(SQLiteDatabase db) {
        super.onOpen(db);
        db.execSQL("PRAGMA foreign_keys = ON");//enable foreign key
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL(CREATE_BOOKINFO_TABLE);
        db.execSQL(CREATE_BOOKMART_TABLE);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }

    public long insertBook(SQLiteDatabase db, BookInfo info)
    {
        if(db==null)
        {
            return -1;
        }
        ContentValues value=new ContentValues();

        value.put(BOOKINFO_TABLE_ID,info.getBookId());
        value.put(BOOKINFO_TABLE_NAME,info.getBookName());
        value.put(BOOKINFO_TABLE_AUTHOR,info.getBookAuthor());
        value.put(BOOKINFO_TABLE_LOCAL_PATH,info.getBookLocalPath());
        value.put(BOOKINFO_TABLE_PATH,info.getBookHttpPath());
        value.put(BOOKINFO_TABLE_CHAR_COUNT,info.getBookCharCount());

        return db.insert(BOOKINFO_TABLE,null,value);
    }

    public void deleteBook(SQLiteDatabase db, BookInfo info)
    {
        if(db==null)
        {
            return ;
        }

        String SQL_Instrution="delete from "+BOOKINFO_TABLE+
                " where "+ BOOKINFO_TABLE_LOCAL_ID+"="+info.getBookLocalId();

        db.execSQL(SQL_Instrution);
    }

    public void setBookCharSize(SQLiteDatabase db, BookInfo info)
    {
        if(db==null)
        {
            return ;
        }
        String SQL_Instrution="update "+BOOKINFO_TABLE+
                " set "+ BOOKINFO_TABLE_CHAR_COUNT +"="+info.getBookCharCount()+
                " where "+ BOOKINFO_TABLE_LOCAL_ID+"="+info.getBookLocalId();

        db.execSQL(SQL_Instrution);
    }

    private void fillBookInfo(BookInfo info,Cursor cursor)
    {
        info.setBookId(cursor.getInt(cursor.getColumnIndex(BOOKINFO_TABLE_ID)));
        info.setBookName(cursor.getString(cursor.getColumnIndex(BOOKINFO_TABLE_NAME)));
        info.setBookAuthor(cursor.getString(cursor.getColumnIndex(BOOKINFO_TABLE_AUTHOR)));
        info.setBookLocalPath(cursor.getString(cursor.getColumnIndex(BOOKINFO_TABLE_LOCAL_PATH)));
        info.setBookHttpPath(cursor.getString(cursor.getColumnIndex(BOOKINFO_TABLE_PATH)));
        info.setBookLocalId(cursor.getInt(cursor.getColumnIndex(BOOKINFO_TABLE_LOCAL_ID)));
        info.setBookCharCount(cursor.getInt(cursor.getColumnIndex(BOOKINFO_TABLE_CHAR_COUNT)));

        if(info.getBookHttpPath()!=null)
        {
            info.setBookType(BookInfo.BOOK_TYPE_INET);
        }
        else
        {
            info.setBookType(BookInfo.BOOK_TYPE_LOCAL);
        }
    }
    public BookInfo getBookInfo(SQLiteDatabase db,int local_id)
    {
        BookInfo info=null;

        if(db==null)
        {
            return null;
        }
        String SQL_Instruct="select * from "+BOOKINFO_TABLE+
                " where "+ BOOKINFO_TABLE_LOCAL_ID+"="+local_id;

        Cursor cursor= db.rawQuery(SQL_Instruct,null);

        if(cursor.moveToFirst())
        {
            info=new BookInfo();

            fillBookInfo(info,cursor);
        }

        return info;
    }
    public BookInfo getBookInfo(SQLiteDatabase db,long id)
    {
        BookInfo info=null;

        if(db==null)
        {
            return null;
        }
        String SQL_Instruct="select * from "+BOOKINFO_TABLE+
                " where "+ BOOKINFO_TABLE_ID+"="+id;

        Cursor cursor= db.rawQuery(SQL_Instruct,null);

        if(cursor.moveToFirst())
        {
            info=new BookInfo();

            fillBookInfo(info,cursor);
        }

        return info;
    }
    public void updateBookLastReadTime(SQLiteDatabase db,long id)
    {
        if(db==null)
        {
            return ;
        }
        String SQL_Instruct="update "+BOOKINFO_TABLE+
                " set "+BOOKINFO_TABLE_LAST_READ_TIME+"= CURRENT_TIMESTAMP"+
                " where "+ BOOKINFO_TABLE_LOCAL_ID+"="+id;

        db.execSQL(SQL_Instruct);
    }
    public List<BookInfo> getAllBookList(SQLiteDatabase db)
    {
        if(db==null)
        {
            return null;
        }

        Cursor cursor= db.rawQuery(QUERY_BOOKINFO_TABLE_ALL,null);
        List<BookInfo> bookInfoList=new ArrayList<>();
        if(cursor.moveToFirst())
        {

            do{
                BookInfo info=new BookInfo();

                fillBookInfo(info,cursor);

                bookInfoList.add(info);

            }while (cursor.moveToNext());
        }

        return bookInfoList;
    }

    public long insertBookMark(SQLiteDatabase db, BookMark mark)
    {
        if(db==null)
        {
            return -1;
        }
        ContentValues value=new ContentValues();

        value.put(BOOKMARK_TABLE_LOCAL_ID,mark.getLocalId());
        value.put(BOOKMARK_TABLE_CHAPTER_ID,mark.getChapterId());
        value.put(BOOKMARK_TABLE_NAME,mark.getName());
        value.put(BOOKMARK_TABLE_INDEX,mark.getIndex());
        value.put(BOOKMARK_TABLE_PERCENT,mark.getReadPercent());

        return db.insert(BOOKMARK_TABLE,null,value);
    }

    public void modifyBookMark(SQLiteDatabase db, BookMark mark)
    {
        if(db==null)
        {
            return;
        }
        String SQL_Instruct="update "+BOOKMARK_TABLE+
                " set "+BOOKMARK_TABLE_INDEX+"="+mark.getIndex()+","+
                BOOKMARK_TABLE_CHAPTER_ID+"="+mark.getChapterId()+","+
                BOOKMARK_TABLE_PERCENT+"="+mark.getReadPercent()+
                " where "+ BOOKMARK_TABLE_LOCAL_ID+"="+mark.getLocalId()+
                " and "+BOOKMARK_TABLE_NAME+"="+"\'"+mark.getName()+"\'";

        db.execSQL(SQL_Instruct);
    }

    public BookMark getBookMark(SQLiteDatabase db, int local_id,String name)
    {
        BookMark mark=null;

        if(db==null)
        {
            return null;
        }
        String SQL_Instruct="select * from "+BOOKMARK_TABLE+
                " where "+ BOOKMARK_TABLE_LOCAL_ID+"="+local_id+
                " and "+BOOKMARK_TABLE_NAME+"="+"\'"+name+"\'";

        Cursor cursor= db.rawQuery(SQL_Instruct,null);

        if(cursor.moveToFirst())
        {
            mark=new BookMark();

            mark.setLocalId(cursor.getInt(cursor.getColumnIndex(BOOKMARK_TABLE_LOCAL_ID)));
            mark.setIndex(cursor.getInt(cursor.getColumnIndex(BOOKMARK_TABLE_INDEX)));
            mark.setName(cursor.getString(cursor.getColumnIndex(BOOKMARK_TABLE_NAME)));
            mark.setChapterId(cursor.getInt(cursor.getColumnIndex(BOOKMARK_TABLE_CHAPTER_ID)));
            mark.setReadPercent(cursor.getFloat(cursor.getColumnIndex(BOOKMARK_TABLE_PERCENT)));
        }

        return mark;
    }
}
