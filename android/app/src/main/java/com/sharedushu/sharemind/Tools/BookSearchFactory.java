package com.sharedushu.sharemind.Tools;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.BookManage.BookInetSearcher;
import com.sharedushu.sharemind.BookManage.BookSearcher;

/**
 * Created by flj on 2016/11/8.
 */
public class BookSearchFactory {

    public static final String INET_SEACHE = "inet";

    public static BookSearcher createBookSearcher(String type)
    {
        if(type.equals(INET_SEACHE) )
        {
            return new BookInetSearcher(BookDefine.SERVER_ADDR,BookDefine.SERVER_PORT);
        }

        return null;
    }
}
