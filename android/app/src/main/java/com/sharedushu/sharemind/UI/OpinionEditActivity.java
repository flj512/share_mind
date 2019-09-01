package com.sharedushu.sharemind.UI;

import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.BookManage.BookManager;
import com.sharedushu.sharemind.BookManage.BookOpinion;
import com.sharedushu.sharemind.Database.BookConfig;
import com.sharedushu.sharemind.R;
import com.sharedushu.sharemind.Tools.BookPath;
import com.sharedushu.sharemind.Tools.BookSearchUtil;
import com.sharedushu.sharemind.Tools.BookTextUtils;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;
import com.sharedushu.sharemind.Tools.OnSearchComplete;

import java.util.List;

public class OpinionEditActivity extends AppCompatActivity {
    private final String TAG="OpinionEdit";
    private long m_bookid;
    private int m_index;
    private int m_chapter;
    private String m_select_string=null;
    private BookSearchUtil m_search_util;
    private boolean m_destory_flag=false;
    private String m_user;
    private String m_key;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN );

        Intent intent=getIntent();
        m_bookid=intent.getLongExtra(BookDefine.INTENT_BOOK_ID,-1);
        m_index=intent.getIntExtra(BookDefine.INTENT_BOOK_INDEX,-1);
        m_chapter=intent.getIntExtra(BookDefine.INTENT_BOOK_CHAPTER,-1);
        m_select_string=intent.getStringExtra(BookDefine.INTENT_BOOK_SELECT_STRING);

        setContentView(R.layout.activity_opinion_edit);
        Button publishButton=(Button)findViewById(R.id.book_opinion_publish);
        publishButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(m_search_util==null) {
                    m_search_util = new BookSearchUtil();
                    m_search_util.setCompleteHandler(new OnSearchComplete() {
                        @Override
                        public void onSearchComplete(List<BookInfo> booklist) {

                        }

                        @Override
                        public void onGetOpinionListComplete(List<BookOpinion> opinionList) {

                        }

                        @Override
                        public void onPublishOpinionComplete(int ret) {
                            showWaiting(false);
                            if(m_destory_flag)
                            {
                                return;
                            }
                            if (ret == BookDefine.CODE_SUCCESS) {
                                Toast.makeText(OpinionEditActivity.this, R.string.publish_success, Toast.LENGTH_SHORT).show();
                                finish();
                            } else {
                                AlertDialog.Builder builder=new AlertDialog.Builder(OpinionEditActivity.this);
                                builder.setTitle(R.string.publish_fail);
                                builder.setMessage(loadErrorString(ret));
                                if(ret != BookDefine.CODE_AUTH) {
                                    builder.setPositiveButton(R.string.i_kown, null);
                                }else{
                                    builder.setNegativeButton(R.string.input_user_passwd_cancel,null);
                                    builder.setPositiveButton(R.string.input_user_passwd_confrim, new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                            startActivityForResult(new Intent(OpinionEditActivity.this, LoginActivity.class),BookDefine.INPUT_USER_PASSWD_CODE);
                                        }
                                    });
                                }
                                builder.show();
                            }
                        }
                    });
                }
                if(TextUtils.isEmpty(m_user)||TextUtils.isEmpty(m_key)) {
                    BookConfig config=new BookConfig(OpinionEditActivity.this);
                    m_user = config.getUser();
                    m_key = config.getPasswd();
                }

                if(TextUtils.isEmpty(m_user)||TextUtils.isEmpty(m_key))
                {
                    AlertDialog.Builder builder=new AlertDialog.Builder(OpinionEditActivity.this);
                    builder.setTitle(R.string.input_user_passwd_title);
                    builder.setMessage(R.string.input_user_passwd);
                    builder.setPositiveButton(R.string.input_user_passwd_confrim, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            startActivityForResult(new Intent(OpinionEditActivity.this, LoginActivity.class),BookDefine.INPUT_USER_PASSWD_CODE);
                        }
                    });
                    builder.setNegativeButton(R.string.input_user_passwd_cancel, null);
                    builder.show();

                }
                else {
                    sendOpinion();
                }
            }
        });

        final EditText opinionEdit=(EditText)findViewById(R.id.book_opinion_edit);
        opinionEdit.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if(hasFocus)
                {
                    if(opinionEdit.getText().toString().length()==0&&m_select_string!=null)
                    {
                        m_select_string= BookTextUtils.skipStartPunctuation(m_select_string);
                        m_select_string=BookTextUtils.skipEndPunctuation(m_select_string);
                        if(m_select_string.length()>0) {
                            opinionEdit.setText(m_select_string + "：");
                        }
                    }
                }
            }
        });

        showWaiting(false);
    }
    private void sendOpinion()
    {
        if(TextUtils.isEmpty(m_user)||TextUtils.isEmpty(m_key)) {
            BookConfig config=new BookConfig(OpinionEditActivity.this);
            m_user = config.getUser();
            m_key = config.getPasswd();
            if(TextUtils.isEmpty(m_user)||TextUtils.isEmpty(m_key))
            {
                return;
            }
        }

        EditText opinionEdit = (EditText) findViewById(R.id.book_opinion_edit);
        String opinion = opinionEdit.getText().toString();
        if (opinion.length() > 0) {
            m_search_util.publicOpinion(m_bookid, m_chapter, m_index, opinion, m_user, m_key);
            showWaiting(true);
        } else {
            Toast.makeText(OpinionEditActivity.this, R.string.publish_conten_empty, Toast.LENGTH_LONG).show();
            //opinionEdit.setError(getResources().getString(R.string.publish_conten_empty));
        }
    }
    private void showWaiting(boolean waiting)
    {
        Button button=(Button)findViewById(R.id.book_opinion_publish);
        ProgressBar progressBar=(ProgressBar)findViewById(R.id.book_opinion_publish_waiting);

        if (waiting)
        {
            progressBar.setVisibility(View.VISIBLE);
            button.setVisibility(View.INVISIBLE);
        }
        else
        {
            progressBar.setVisibility(View.INVISIBLE);
            button.setVisibility(View.VISIBLE);
        }
    }

    String loadErrorString(int code)
    {
        int id;
        switch (code)
        {
            case BookDefine.CODE_AUTH:
                id=R.string.error_auth;
                break;
            case BookDefine.CODE_TIMESTAMP:
                id=R.string.error_time;
                break;
            case BookDefine.CODE_DUPLITE:
                id=R.string.error_dup;
                break;
            default:
                id=R.string.error_sys;
        }

        return getResources().getString(id);
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode)
        {
            case BookDefine.INPUT_USER_PASSWD_CODE:
                if(resultCode==RESULT_OK)
                {
                    m_user=data.getStringExtra(BookDefine.GLOBAL_CONFIG_USER);
                    m_key=data.getStringExtra(BookDefine.GLOBAL_CONFIG_PASSWD);
                    sendOpinion();
                }
                break;
        }
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        m_destory_flag=true;
        if(m_search_util!=null)
        {
            m_search_util.cancelPublish();
        }
    }
}
