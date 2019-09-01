package com.sharedushu.sharemind.UI;

import android.app.Activity;
import android.content.Intent;;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.BookManage.BookDownloadHttp;
import com.sharedushu.sharemind.BookManage.BookDownloadLite;
import com.sharedushu.sharemind.R;
import com.sharedushu.sharemind.Tools.BookPath;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class BookSrcollViewActivity extends Activity {
    private boolean m_destroy_flag=false;
    private BookDownloadLite m_bookdown;

    private void showBookContent(String path,String title)
    {

        BufferedReader reader=null;
        try {
            reader = new BufferedReader(new InputStreamReader(BookSrcollViewActivity.this.openFileInput(path)));
            StringBuilder builder=new StringBuilder();
            String line;
            while ((line=reader.readLine())!=null){
                builder.append(line+"\n");
            }

            View downloadWait=findViewById(R.id.download_waiting);
            downloadWait.setVisibility(View.GONE);

            TextView bookContent=(TextView)findViewById(R.id.book_view);
            bookContent.setText(builder.toString());
            bookContent.setMovementMethod(ScrollingMovementMethod.getInstance());
            bookContent.setVisibility(View.VISIBLE);

            TextView bookTitleView=(TextView)findViewById(R.id.book_title);
            bookTitleView.setText(title);
            bookTitleView.setVisibility(View.VISIBLE);

        }catch (Exception e) {
            e.printStackTrace();
        }finally {
            if(reader!=null)
            {
                try {
                    reader.close();
                }catch (Exception e){
                    e.printStackTrace();
                }
            }
        }

    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_book_scroll_view);
        getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN );
        Intent intent=getIntent();
        final String path=intent.getStringExtra(BookDefine.INTENT_BOOK_PATH);
        final String name=intent.getStringExtra(BookDefine.INTENT_BOOK_NAME);
        final String http_path= BookDefine.SERVER_HTTP_PATH+path;
        String[] parm={http_path,path};

        m_bookdown=new BookDownloadLite(BookDefine.SERVER_ADDR,BookDefine.FILE_SERVER_PORT){
            @Override
            protected void onPreExecute() {
                TextView bookContent=(TextView)findViewById(R.id.book_view);
                bookContent.setVisibility(View.GONE);
                TextView bookTitleView=(TextView)findViewById(R.id.book_title);
                bookTitleView.setVisibility(View.GONE);
                View downloadWait=findViewById(R.id.download_waiting);
                downloadWait.setVisibility(View.VISIBLE);
            }

            @Override
            protected void onPostExecute(Boolean aBoolean) {
                if(m_destroy_flag)
                {
                    return;
                }
                if(aBoolean)
                {
                    showBookContent(path,name);
                }
                else
                {
                    Toast.makeText(BookSrcollViewActivity.this,R.string.network_fail,Toast.LENGTH_SHORT).show();
                }
            }
        };

        if(!BookPath.bookFileExistInData(path)) {
            m_bookdown.execute(parm);
        }else
        {
            showBookContent(path,name);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        m_destroy_flag=true;
        if(m_bookdown!=null)
        {
            m_bookdown.cancle_download();
        }
    }
}
