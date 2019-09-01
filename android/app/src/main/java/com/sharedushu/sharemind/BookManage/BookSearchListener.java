package com.sharedushu.sharemind.BookManage;

/**
 * Created by flj on 2016/11/7.
 */
public interface BookSearchListener {
    void onStart();
    void onFind(BookInfo info);
    void onEnd(int code);
}
