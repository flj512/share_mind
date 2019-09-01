package com.sharedushu.sharemind.UI;

import android.content.DialogInterface;
import android.content.Intent;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.BookManage.BookManager;
import com.sharedushu.sharemind.R;
import com.sharedushu.sharemind.Tools.BookPath;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import java.util.List;

public class MainActivity extends AppCompatActivity {
    private static final String TAG="MainActivity";
    private  BookItemAdapter m_adapter=null;

    private void initListView()
    {
        ListView bookListView=(ListView)findViewById(R.id.book_list);
        List<BookInfo> bookInfoList=BookManager.getSingleton().getAllBooklist();

        if(bookInfoList!=null) {
            m_adapter = new BookItemAdapter(MainActivity.this, R.layout.book_item, bookInfoList);
            bookListView.setAdapter(m_adapter);
        }
        TextView tip=(TextView)findViewById(R.id.search_tip_text);
        if(bookInfoList==null||bookInfoList.size()==0)
        {
            tip.setVisibility(View.VISIBLE);
        }
        else {
            tip.setVisibility(View.INVISIBLE);
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN );
        setContentView(R.layout.activity_main);

        Button button=(Button)findViewById(R.id.start_search);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivityForResult(new Intent(MainActivity.this,SearchActivity.class),BookDefine.SEARCH_REQUEST_CODE);
            }
        });

        initListView();
        ListView bookListView=(ListView)findViewById(R.id.book_list);

        bookListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                BookInfo bookInfo=(BookInfo) view.getTag();

                BookManager.getSingleton().updateBookLastReadTime(bookInfo.getBookLocalId());

                Intent intent=new Intent(BookDefine.CONTENT_VIEW_ACTION);
                intent.addCategory(BookDefine.CONTENT_VIEW_PAGE_CATALOG);
                intent.putExtra(BookDefine.INTENT_BOOK_PATH,bookInfo.getBookHttpPath());
                intent.putExtra(BookDefine.INTENT_BOOK_NAME,bookInfo.getBookName());
                intent.putExtra(BookDefine.INTENT_BOOK_ID,bookInfo.getBookId());
                intent.putExtra(BookDefine.INTENT_BOOK_LOCAL_ID,bookInfo.getBookLocalId());

                startActivityForResult(intent,BookDefine.SHOW_CONTENT_REQUEST_CODE);
            }
        });

        bookListView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> parent, final View view, int position, long id) {
                AlertDialog.Builder builder=new AlertDialog.Builder(MainActivity.this);
                builder.setTitle(R.string.remove_title);
                builder.setMessage(String.format(getResources().getString( R.string.remove_text),((BookInfo) view.getTag()).getBookName()));
                builder.setPositiveButton(R.string.button_confirm, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        DebugPrintUtil.i(TAG,"confirmed");
                        BookInfo info=(BookInfo) view.getTag();
                        BookManager.getSingleton().deleteBook(info);
                        m_adapter.remove(info);
                        //deleteFile(info.getBookHttpPath());
                        BookPath.delete(BookPath.mapSDDownloadPath(info.getBookHttpPath()));
                        BookPath.delete(BookPath.mapSDPath(info.getBookHttpPath()));
                        m_adapter.notifyDataSetChanged();
                        if(m_adapter.getCount()==0)
                        {
                            TextView tip=(TextView)findViewById(R.id.search_tip_text);
                            tip.setVisibility(View.VISIBLE);
                        }
                    }
                });
                builder.setNegativeButton(R.string.button_cancel, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        DebugPrintUtil.i(TAG,"canceled");
                    }
                });
                builder.show();
                return true;
            }
        });
    }

    private void refreshBookList()
    {
        if(m_adapter!=null) {
            m_adapter.clear();
            m_adapter.addAll(BookManager.getSingleton().getAllBooklist());
            m_adapter.notifyDataSetChanged();
            TextView tip = (TextView) findViewById(R.id.search_tip_text);
            tip.setVisibility(m_adapter.getCount() > 0 ? View.INVISIBLE : View.VISIBLE);
        }
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode)
        {
            case BookDefine.SEARCH_REQUEST_CODE:
                if(resultCode==RESULT_OK)
                {
                    DebugPrintUtil.i(TAG,"Recive Search Result");
                    if(m_adapter!=null) {
                        refreshBookList();
                    }else{
                        initListView();
                    }
                }
                break;
            case BookDefine.SHOW_CONTENT_REQUEST_CODE:
                if(resultCode==RESULT_OK)
                {
                    DebugPrintUtil.i(TAG,"Recive Show Content Result");
                    refreshBookList();
                }
                break;
        }
    }
}
