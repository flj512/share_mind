package com.sharedushu.sharemind.Tools;

import android.os.Handler;
import android.os.Message;

import com.sharedushu.sharemind.BookManage.BookInetSearcher;
import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.BookManage.BookOpinion;
import com.sharedushu.sharemind.BookManage.BookSearchListener;
import com.sharedushu.sharemind.NetManage.ConnectManager;
import com.sharedushu.sharemind.NetManage.GetOpinionListCommand;
import com.sharedushu.sharemind.NetManage.PublishOpinionCommand;
import com.sharedushu.sharemind.NetManage.SockConnector;

import java.util.ArrayList;
import java.util.List;
import java.lang.Thread;


/**
 * Created by flj on 2016/11/8.
 */
public class BookSearchUtil {
    private static final String SEARCH_UTIL_TAG="SearchUtil";
    private OnSearchComplete m_comple_handler;
    private Handler m_handler=null;
    private static int CMD_ID=1;

    private synchronized int nextCommandID()
    {
        int ret=CMD_ID;

        CMD_ID++;
        if(CMD_ID==0)
        {
            CMD_ID=1;
        }

        return ret;
    }
    public BookSearchUtil()
    {
        m_handler=new Handler(){
            @Override
            public void handleMessage(Message msg) {
                if(m_comple_handler!=null) {
                    if (msg.what == BookSearchUtilListen.GET_OPINION_LIST_COMPLETE) {
                        if(msg.arg1==m_get_opinion_list_current_id) {
                            if (msg.arg2 == SockConnector.SOCK_CONNECT_ERROR_OK) {
                                m_comple_handler.onGetOpinionListComplete((List<BookOpinion>) msg.obj);
                            } else {
                                m_comple_handler.onGetOpinionListComplete(null);
                            }
                        }else{
                            DebugPrintUtil.v(SEARCH_UTIL_TAG,"Cmd="+msg.arg1+" is expired");
                        }

                    } else if (msg.what == BookSearchUtilListen.PUBLISH_OPINION_COMPLETE) {
                        if(msg.arg1==m_publish_opinion_current_id) {
                            m_comple_handler.onPublishOpinionComplete(msg.arg2);
                        }
                    } else if (msg.what == BookSearchUtilListen.FIND_COMPETE) {
                        m_search_thread=null;
                        if(m_search_waiting==0) {
                            m_comple_handler.onSearchComplete((List<BookInfo>) msg.obj);
                        }
                        else {
                            m_search_waiting=0;
                            startSearchThread();
                        }
                    }
                }

            }
        };
    }
    private static class BookSearchUtilListen implements BookSearchListener{
        public List<BookInfo> m_book_list;
        private Handler m_handler;
        public static final int FIND_COMPETE=1;
        public static final int GET_OPINION_LIST_COMPLETE=2;
        public static final int PUBLISH_OPINION_COMPLETE=3;

        public BookSearchUtilListen(List<BookInfo> booklist,Handler handler) {
            m_book_list = booklist;
            m_handler=handler;
        }

        @Override
        public void onStart() {
            DebugPrintUtil.i(SEARCH_UTIL_TAG,"Search Start");
        }

        @Override
        public void onEnd(int code) {
            Message message=new Message();
            message.what=BookSearchUtilListen.FIND_COMPETE;
            if (code==0) {
                message.obj = m_book_list;
            }else{
                message.obj = null;
            }
            m_handler.sendMessage(message);
        }

        @Override
        public void onFind(BookInfo info) {
            m_book_list.add(info);
        }
    }
    private static class SearchUtilThread extends Thread {
        protected BookInetSearcher m_searcher;
        private boolean m_user_cancled;
        SearchUtilThread(String type)
        {
            m_searcher=(BookInetSearcher)BookSearchFactory.createBookSearcher(type);
            m_user_cancled =false;
        }
        public synchronized void setUserCancled()
        {
            m_user_cancled =true;
        }
        public synchronized boolean isUserCancled()
        {
            return m_user_cancled;
        }
        public void cancle_run()
        {
            setUserCancled();
            if (m_searcher!=null)
            {
                m_searcher.close();
            }
        }
    }

    private static class SearchThread  extends SearchUtilThread{
        private String m_key;
        private BookSearchListener m_listen;


        public SearchThread(String type,String key,BookSearchListener listener) {
            super(type);
            m_key = key;
            m_listen=listener;
        }

        @Override
        public void run() {
            if(m_searcher!=null&&!isUserCancled())
            {
                if(m_searcher.open())
                {
                    m_searcher.addListener(m_listen);
                    m_searcher.search(m_key);
                    m_searcher.close();
                    m_searcher.removeListener(m_listen);
                }
                else
                {
                    m_listen.onEnd(-1);
                }
            }
            else {
                m_listen.onEnd(-1);
            }
        }
    }

    private SearchThread m_search_thread=null;
    private int m_search_waiting=0;
    private String m_type;
    private String m_key;
    private void startSearchThread()
    {
        List<BookInfo> booklist=new ArrayList<BookInfo>();

        BookSearchUtilListen bookSearchUtilListen = new BookSearchUtilListen(booklist, m_handler);
        m_search_thread = new SearchThread(m_type, m_key, bookSearchUtilListen);
        m_search_thread.start();
    }
    public  void SearchBook(String type,String key)
    {
        m_type=type;
        m_key=key;

        if (m_search_thread!=null)
        {
            DebugPrintUtil.i("ThreadInterupt","Interupt Thread");
            m_search_thread.cancle_run();
            m_search_waiting++;
        }
        else {
            startSearchThread();
        }
    }
    public void cancelSearch()
    {
        if(m_search_thread!=null)
        {
            m_search_thread.cancle_run();
        }
    }
    //get opinion list
    private int m_get_opinion_list_current_id;
    public void cancelGetOpinionList()
    {
        m_get_opinion_list_current_id=0;//0 is invalidate
    }
    public  void getOpinionList(long book_id,int chapter_id,int start,int end)
    {
        m_get_opinion_list_current_id=nextCommandID();
        DebugPrintUtil.v(SEARCH_UTIL_TAG,"GetOpinionList Command ID="+m_get_opinion_list_current_id);
        GetOpinionListCommand command=new GetOpinionListCommand(BookSearchUtilListen.GET_OPINION_LIST_COMPLETE,m_get_opinion_list_current_id,
                                                                book_id,chapter_id,start,end,m_handler);
        ConnectManager.getSingleton().sendCommand(command);
    }
    //public opinion
    private int m_publish_opinion_current_id;
    public  void publicOpinion(long book_id,int chapter,int start,String opinion,String user,String key)
    {
        m_publish_opinion_current_id=nextCommandID();
        DebugPrintUtil.v(SEARCH_UTIL_TAG,"PublishOpinion Command ID="+m_publish_opinion_current_id);
        PublishOpinionCommand command=new PublishOpinionCommand(BookSearchUtilListen.PUBLISH_OPINION_COMPLETE,m_publish_opinion_current_id,
                book_id,chapter,start,opinion,user,key,m_handler);
        ConnectManager.getSingleton().sendCommand(command);
    }
    public void cancelPublish() {
        m_publish_opinion_current_id=0;
    }
    public void setCompleteHandler(OnSearchComplete listner)
    {
        m_comple_handler=listner;
    }
}
