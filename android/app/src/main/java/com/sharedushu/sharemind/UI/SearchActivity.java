package com.sharedushu.sharemind.UI;


import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.BookManage.BookManager;
import com.sharedushu.sharemind.BookManage.BookOpinion;
import com.sharedushu.sharemind.R;
import com.sharedushu.sharemind.Tools.BookSearchFactory;
import com.sharedushu.sharemind.Tools.BookSearchUtil;
import com.sharedushu.sharemind.Tools.OnSearchComplete;

import java.util.List;

public class SearchActivity extends AppCompatActivity {

    private BookSearchUtil m_searchUtil =null;
    private boolean m_destory_flag=false;

    private void search(String key)
    {
        if(m_searchUtil==null)
        {
            m_searchUtil=new BookSearchUtil();
            m_searchUtil.setCompleteHandler(new OnSearchComplete() {
                @Override
                public void onSearchComplete(List<BookInfo> booklist) {
                    if(booklist!=null) {
                        ListView bookListView = (ListView) findViewById(R.id.search_book_list);
                        SearchItemAdapter adapter = new SearchItemAdapter(SearchActivity.this, R.layout.search_book_item, booklist);
                        bookListView.setAdapter(adapter);
                        if(booklist.size()==0&&!m_destory_flag)
                        {
                            Toast.makeText(SearchActivity.this,R.string.search_none,Toast.LENGTH_LONG).show();
                        }
                    }else {
                       if(!m_destory_flag) {
                           Toast.makeText(SearchActivity.this, R.string.search_fail, Toast.LENGTH_LONG).show();
                       }
                    }
                    showWaiting(false);
                }

                @Override
                public void onGetOpinionListComplete(List<BookOpinion> opinionList) {

                }

                @Override
                public void onPublishOpinionComplete(int ret) {

                }
            });
        }

        m_searchUtil.SearchBook(BookSearchFactory.INET_SEACHE, key);
    }

    private void showWaiting(boolean show)
    {
        View view=findViewById(R.id.search_waiting);
        ListView bookListView=(ListView)findViewById(R.id.search_book_list);

        if(show) {
            bookListView.setVisibility(View.GONE);
            view.setVisibility(View.VISIBLE);
        }
        else
        {
            bookListView.setVisibility(View.VISIBLE);
            view.setVisibility(View.GONE);
        }
    }
    private void onOpenBook(BookInfo info)
    {
        //Add automatic
        BookManager.getSingleton().insertBook(info);

        BookInfo newBookinfo=BookManager.getSingleton().getBookInfoByBookId(info.getBookId());
        info.setBookLocalId(newBookinfo.getBookLocalId());
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN );
        setContentView(R.layout.activity_search);

        Button search_confirm=(Button)findViewById(R.id.search_confirm);
        search_confirm.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TextView searchText=(TextView)findViewById(R.id.search_edit);
                String text=searchText.getText().toString();

                if(text.length()>0) {
                    showWaiting(true);
                    search(text);
                }
                else
                {
                    Toast.makeText(SearchActivity.this,R.string.search_enter_key,Toast.LENGTH_LONG).show();
                }
            }
        });

        ListView bookListView=(ListView)findViewById(R.id.search_book_list);
        bookListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                BookInfo bookInfo=(BookInfo) view.getTag();
                onOpenBook(bookInfo);

                Intent intent=new Intent(BookDefine.CONTENT_VIEW_ACTION);
                intent.addCategory(BookDefine.CONTENT_VIEW_PAGE_CATALOG);

                intent.putExtra(BookDefine.INTENT_BOOK_PATH,bookInfo.getBookHttpPath());
                intent.putExtra(BookDefine.INTENT_BOOK_NAME,bookInfo.getBookName());
                intent.putExtra(BookDefine.INTENT_BOOK_ID,bookInfo.getBookId());
                intent.putExtra(BookDefine.INTENT_BOOK_LOCAL_ID,bookInfo.getBookLocalId());

                startActivity(intent);
            }
        });

        showWaiting(true);
        search(null);
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if(keyCode==KeyEvent.KEYCODE_BACK)
        {
            setResult(RESULT_OK,null);
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        m_destory_flag=true;
        if(m_searchUtil!=null)
        {
            m_searchUtil.cancelSearch();
        }
    }
}
