package com.sharedushu.sharemind.BookManage;

/**
 * Created by flj on 2016/11/7.
 */

public class BookDefine {
    //tags
    static public final String TAG_CMD="CMD";
    static public final String TAG_BOOK_LIST_KEY="BookListKey";
    static public final String TAG_BOOK_INFO_ID="ID";
    static public final String TAG_BOOK_INFO_AUTHOR="Author";
    static public final String TAG_BOOK_INFO_NAME="Name";
    static public final String TAG_BOOK_INFO_PATH="Path";
    static public final String TAG_BOOK_OPINION_CHAPTER="chapter";
    static public final String TAG_BOOK_OPINION_START="start";
    static public final String TAG_BOOK_OPINION_END="end";
    static public final String TAG_BOOK_OPINION="opinion";
    static public final String TAG_BOOK_RESULT="result";
    static public final String TAG_BOOK_USER="user";
    static public final String TAG_BOOK_VERIFY_CODE="verifyCode";
    static public final String TAG_BOOK_SHA="sha";
    static public final String TAG_BOOK_RESULT_CODE="code";

    //error code
    static public final int CODE_SUCCESS=0;
    static public final int CODE_DB=1;
    static public final int CODE_PARAM=2;
    static public final int CODE_NOTEXIST=3;
    static public final int CODE_AUTH=4;
    static public final int CODE_TIMESTAMP=5;
    static public final int CODE_DUPLITE=6;
    static public final int CODE_BAD_DATA=7;
    static public final int CODE_LOCAL_START=200;


    //cmds
    static public final String CMD_GET_BOOK_LIST="GetBookList";
    static public final String CMD_GET_OPINION_LIST="GetOpinionList";
    static public final String CMD_SET_OPINION="SetOpionion";

    //result
    static public final String CMD_RESPONSE_FAIL="Fail";
    static public final String CMD_RESPONSE_SUCCESS="Success";


    //limit
    static public final int CMD_SIZE = 1024;
    static public final int CMD_HEADER_SIZE = 6;
    static public final int CMD_INFO_HEADER0=0x5a;
    static public final int CMD_INFO_HEADER1=0xb7;
    static public final int CMD_INFO_HEADER2=0xd0;
    static public final int CMD_INFO_HEADER3=0x2e;

    //server
    static public final String SERVER_ADDR="192.168.0.105";
    static public final String SERVER_HTTP_PATH="http://192.168.0.105/";
    static public final int SERVER_PORT=60000;
    static public final int FILE_SERVER_PORT=60001;

    //action
    static public final String CONTENT_VIEW_ACTION="com.mindshare_flj.booksharemind.showbook";
    static public final String CONTENT_VIEW_SCROLL_CATALOG="com.mindshare_flj.booksharemind.scroll";
    static public final String CONTENT_VIEW_PAGE_CATALOG="com.mindshare_flj.booksharemind.page";
    static public final String CONTENT_EDIT_ACTION="com.mindshare_flj.booksharemind.editopinion";

    //intent
    static public final String INTENT_BOOK_ID="ID";
    static public final String INTENT_BOOK_LOCAL_ID="local_id";
    static public final String INTENT_BOOK_NAME="Name";
    static public final String INTENT_BOOK_PATH="Path";
    static public final String INTENT_BOOK_INDEX="Index";
    static public final String INTENT_BOOK_CHAPTER="Chapter";
    static public final String INTENT_BOOK="Book";
    static public final String INTENT_BOOK_SELECT_STRING="Selected";

    //global
    static public final int SEARCH_REQUEST_CODE=1;
    static public final int SHOW_CONTENT_REQUEST_CODE=2;
    static public final int INPUT_USER_PASSWD_CODE=3;
    static public final int SD_PERMITION_REQUEST_CODE=4;
    static public final String MASTER_BOOK_MARK_NAME="Master-Bookmark";
    static public final String GLOBAL_CONFIG_FILE="config";
    static public final String GLOBAL_CONFIG_USER="user";
    static public final String GLOBAL_CONFIG_PASSWD="passwd";

}
