package com.sharedushu.sharemind.Tools;

import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.BookManage.BookOpinion;

import java.util.List;

/**
 * Created by flj on 2016/11/8.
 */

public interface OnSearchComplete{
    void onSearchComplete(List<BookInfo> booklist);
    void onGetOpinionListComplete(List<BookOpinion> opinionList);
    void onPublishOpinionComplete(int ret);
}