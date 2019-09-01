package com.sharedushu.sharemind.UI;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.drawable.ColorDrawable;
import android.os.Build;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.BookManage.BookDownloadHttp;
import com.sharedushu.sharemind.BookManage.BookDownloadLite;
import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.BookManage.BookManager;
import com.sharedushu.sharemind.BookManage.BookMark;
import com.sharedushu.sharemind.BookManage.BookOpinion;
import com.sharedushu.sharemind.Database.BookCatalog;
import com.sharedushu.sharemind.Database.BookSDLoader;
import com.sharedushu.sharemind.R;
import com.sharedushu.sharemind.Tools.BookPath;
import com.sharedushu.sharemind.Tools.BookSearchUtil;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;
import com.sharedushu.sharemind.Tools.OnSearchComplete;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class BookPageViewActivity extends AppCompatActivity {
    private String m_path;
    private String m_name;
    private long m_id;
    private int m_local_id;
    private String m_http_path;
    private PopupWindow m_opinionListWindow;
    private PopupWindow m_CatalogWindow;
    private BookSearchUtil m_search_util;
    private BookDownloadLite m_download;
    private boolean m_destroy_falg=false;
    private String m_select_string=null;
    private BookSDLoader m_sd_loader=null;
    private int m_current_chaptor=0;
    private BufferedReader m_current_reader=null;

    static final private String TAG="BookPageView";

    private void updateBookChaptor(String title,int start)
    {
        if(m_current_reader!=null)
        {
            try {
                m_current_reader.close();
            }catch (IOException e)
            {
                e.printStackTrace();
            }
        }
        m_current_reader=m_sd_loader.getChapterReader(m_current_chaptor);

        TextView titleView=(TextView)findViewById(R.id.book_page_view_title);
        titleView.setText(title+"."+m_sd_loader.getChapterName(m_current_chaptor));

        BookPageView bookContent=(BookPageView) findViewById(R.id.book_pager_content_view);
        bookContent.reset();
        bookContent.setReader(m_current_reader);
        bookContent.setStart(start);
        bookContent.invalidate();
    }
    private void showBookContent(String path,String title,long id,int local_id)
    {
        m_sd_loader = new BookSDLoader(path);
        if(!BookPath.bookFileExistInFileSystem(BookPath.mapSDPath(path)))
        {
            if(!m_sd_loader.unpack())
            {
                return;
            }
        }

        int start=0;
        if(local_id!=-1)//get/set default bookmark
        {
            BookMark mark= BookManager.getSingleton().getBookMark(local_id,BookDefine.MASTER_BOOK_MARK_NAME);
            if(mark==null)
            {
                mark=new BookMark();
                mark.setIndex(0);
                mark.setChapterId(0);
                mark.setName(BookDefine.MASTER_BOOK_MARK_NAME);
                mark.setLocalId(local_id);
                BookManager.getSingleton().insertBookMark(mark);
            }
            m_current_chaptor=mark.getChapterId();
            start=mark.getIndex();
        }

        updateBookChaptor(title,start);

        if(local_id!=-1) {
            //set result to update read progerss
            setResult(RESULT_OK);
            m_sd_loader.calculateSize(local_id);
        }
    }
    private void showWaiting(boolean waiting)
    {
        TextView bookContent=(TextView)findViewById(R.id.book_pager_content_view);
        TextView bookTitleView=(TextView)findViewById(R.id.book_page_view_title);
        TextView bookReadProgressView=(TextView)findViewById(R.id.book_page_view_read_progerss);
        View downloadWait=findViewById(R.id.book_page_view_download_waiting);

        if(waiting) {
            downloadWait.setVisibility(View.VISIBLE);
            bookContent.setVisibility(View.GONE);
            bookTitleView.setVisibility(View.GONE);
            bookReadProgressView.setVisibility(View.GONE);
        }
        else
        {
            downloadWait.setVisibility(View.GONE);
            bookContent.setVisibility(View.VISIBLE);
            bookTitleView.setVisibility(View.VISIBLE);
            bookReadProgressView.setVisibility(View.VISIBLE);
        }
    }
    private void fetchOpinionList(long book_id,int start,int end)
    {
        if(m_search_util ==null) {
            m_search_util = new BookSearchUtil();
            m_search_util.setCompleteHandler(new OnSearchComplete() {
                @Override
                public void onSearchComplete(List<BookInfo> booklist) {
                }

                @Override
                public void onGetOpinionListComplete(List<BookOpinion> opinionList) {
                    DebugPrintUtil.i(TAG, "GetOpinion Complete");
                    if (opinionList != null) {
                        for (BookOpinion opinion : opinionList) {
                            DebugPrintUtil.i(TAG, "Opinon:" + opinion.getText());
                        }
                    }
                    updateOpinionList(opinionList);
                }

                @Override
                public void onPublishOpinionComplete(int ret) {

                }
            });
        }
        m_search_util.getOpinionList(book_id,m_current_chaptor,start,end);
    }
    private void initListView(ListView listView,List<BookOpinion> list)
    {
        if(list==null)
        {
            return;
        }

        String[] strings=new String[list.size()];
        for(int i=0;i<strings.length;i++)
        {
            strings[i]=list.get(i).getText();
        }

        ArrayAdapter<String> arrayAdapter=new ArrayAdapter<String>(this,R.layout.book_opinion_simple,strings);
        listView.setAdapter(arrayAdapter);
    }
    private void updateOpinionList(List<BookOpinion> list)
    {
        if(m_opinionListWindow ==null)
        {
            return;
        }

        View view= m_opinionListWindow.getContentView();
        ListView opinionListView=(ListView)view.findViewById(R.id.book_opinion_list_view);
        ProgressBar waiting=(ProgressBar)view.findViewById(R.id.book_opinion_list_wating);
        TextView tips=(TextView)view.findViewById(R.id.book_opinion_tip);
        initListView(opinionListView,list);

        if(list!=null&&list.size()>0) {
            opinionListView.setVisibility(View.VISIBLE);
        }else
        {
            if(list==null)
            {
                tips.setText(R.string.opinion_load_fail);
            }
            else {
                tips.setText(R.string.opinion_list_empty);
            }
            tips.setVisibility(View.VISIBLE);
        }
        waiting.setVisibility(View.INVISIBLE);
    }
    private void showOpinionList(View refView, float x, float y, List<BookOpinion> list, final int pos)
    {
        if(list==null/*||list.size()==0*/)
        {
            return;
        }

        View opinion_list= LayoutInflater.from(this).inflate(R.layout.book_opinion_list,null);
        ListView opinionListView=(ListView)opinion_list.findViewById(R.id.book_opinion_list_view);
        initListView(opinionListView,list);

        ProgressBar waiting=(ProgressBar)opinion_list.findViewById(R.id.book_opinion_list_wating);
        TextView tips=(TextView)opinion_list.findViewById(R.id.book_opinion_tip);
        tips.setVisibility(View.INVISIBLE);

        if(list.size()==0)
        {
            opinionListView.setVisibility(View.INVISIBLE);
        }
        else
        {
            waiting.setVisibility(View.INVISIBLE);
        }

        Button addOpinion=(Button)opinion_list.findViewById(R.id.book_add_opinion);
        addOpinion.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent=new Intent(BookDefine.CONTENT_EDIT_ACTION);
                intent.putExtra(BookDefine.INTENT_BOOK_ID,m_id);
                intent.putExtra(BookDefine.INTENT_BOOK_INDEX,pos);
                intent.putExtra(BookDefine.INTENT_BOOK_CHAPTER,m_current_chaptor);
                intent.putExtra(BookDefine.INTENT_BOOK_SELECT_STRING,m_select_string);
                m_select_string=null;
                if(m_opinionListWindow !=null) {
                    m_opinionListWindow.dismiss();
                }
                startActivity(intent);
            }
        });

        float pop_width=getResources().getDimension(R.dimen.opinion_list_popup_width);
        float pop_height=getResources().getDimension(R.dimen.opinion_list_popup_height);

        m_opinionListWindow =new PopupWindow(opinion_list,(int)pop_width,(int)pop_height,true);
        m_opinionListWindow.setTouchable(true);
        m_opinionListWindow.setTouchInterceptor(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return false;
            }
        });
        int screenWidth=getResources().getDisplayMetrics().widthPixels;
        int screenHeight=getResources().getDisplayMetrics().heightPixels;
        if(screenWidth-x<pop_width)
        {
            x=x-pop_width;
        }
        if(screenHeight-y<pop_height)
        {
            y=y-pop_height;
        }
        m_opinionListWindow.setBackgroundDrawable(new ColorDrawable(0xbd000000));
        m_opinionListWindow.showAtLocation(refView,Gravity.NO_GRAVITY,(int)x,(int)y);
        m_opinionListWindow.setOnDismissListener(new PopupWindow.OnDismissListener() {
            @Override
            public void onDismiss() {
                if(m_search_util!=null)
                {
                    m_search_util.cancelGetOpinionList();
                }
            }
        });

        if(list.size()==0)
        {
            waiting.setVisibility(View.VISIBLE);
            fetchOpinionList(m_id,pos-3,pos+3);
        }
    }
    private void showCatalogList(View refView)
    {
        if(m_sd_loader==null)
        {
            return;
        }
        View catalog_list= LayoutInflater.from(this).inflate(R.layout.book_catalog_list,null);
        ListView catalogListView=(ListView)catalog_list.findViewById(R.id.book_catalog_list_view);
        ArrayList<BookCatalog> arrayList=m_sd_loader.getCatalogList();

        if(arrayList!=null&&arrayList.size()>0) {
            BookCatalogAdaptor adaptor=new BookCatalogAdaptor(BookPageViewActivity.this,R.layout.book_catalog,arrayList);
            catalogListView.setAdapter(adaptor);
            catalogListView.setSelection(m_current_chaptor);
        }
        catalogListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                BookCatalog catalog=(BookCatalog) view.getTag();
                m_current_chaptor=catalog.getIndex();
                m_CatalogWindow.dismiss();
                updateBookChaptor(m_name ,0);
            }
        } );

        float pop_width=getResources().getDimension(R.dimen.catalog_list_popup_width);
        float pop_height=getResources().getDisplayMetrics().heightPixels;

        m_CatalogWindow =new PopupWindow(catalog_list,(int)pop_width,(int)pop_height,true);
        m_CatalogWindow.setTouchable(true);
        m_CatalogWindow.setTouchInterceptor(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return false;
            }
        });

        m_CatalogWindow.setBackgroundDrawable(new ColorDrawable(0xfff0f0f0));
        m_CatalogWindow.showAtLocation(refView,Gravity.NO_GRAVITY,0,0);
    }
    private float getCurrentBookPercent()
    {
        float per=0;

        if(m_sd_loader!=null&&m_sd_loader.getSize()>0) {
            BookPageView bookContent = (BookPageView) findViewById(R.id.book_pager_content_view);
            per= ((float) (bookContent.getCurrentIndex() + m_sd_loader.getChapterStartIndex(m_current_chaptor)))/m_sd_loader.getSize()*100;
            per = Math.min(100, per);
        }

        return per;
    }
    private void requestPermition()
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            String[] permissions = {"android.permission.READ_EXTERNAL_STORAGE", "android.permission.WRITE_EXTERNAL_STORAGE"};
            ArrayList<String> permit_not_grant=new ArrayList<>();

            for (String permit:permissions)
            {
                if(ActivityCompat.checkSelfPermission(this,permit)!=PackageManager.PERMISSION_GRANTED)
                {
                    permit_not_grant.add(permit);
                }
            }
            if(permit_not_grant.size()>0) {
                String[] req=new String[permit_not_grant.size()];
                permit_not_grant.toArray(req);
                ActivityCompat.requestPermissions(this, req, BookDefine.SD_PERMITION_REQUEST_CODE);
            }
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_book_page_view);
        getWindow().setFlags( WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN );

        requestPermition();

        Intent intent=getIntent();
        m_path =intent.getStringExtra(BookDefine.INTENT_BOOK_PATH);
        m_name =intent.getStringExtra(BookDefine.INTENT_BOOK_NAME);
        m_id =intent.getLongExtra(BookDefine.INTENT_BOOK_ID,-1);
        m_local_id=intent.getIntExtra(BookDefine.INTENT_BOOK_LOCAL_ID,-1);
        m_http_path = BookDefine.SERVER_HTTP_PATH+ m_path;

        String[] parm={m_http_path, m_path};

        final BookPageView bookContent=(BookPageView) findViewById(R.id.book_pager_content_view);
        bookContent.setListner(new OnBookPageViewEvent() {
            @Override
            public void onSelect(int pos, float x, float y,String selected) {
                final float xPos=x,yPos=y;
                final int touch_index=pos;
                m_select_string=selected;
                DebugPrintUtil.i(TAG,"Touch Text Index="+pos+",x="+x+",y="+y);
                List<BookOpinion> opinionlist=new ArrayList<BookOpinion>();
                showOpinionList(bookContent,x,y,opinionlist,touch_index);
            }

            @Override
            public void onPress() {
                showCatalogList(bookContent);
            }

            @Override
            public void onRefresh() {
                TextView bookReadProgressView = (TextView) findViewById(R.id.book_page_view_read_progerss);
                bookReadProgressView.setText(String.format("%.1f%%",getCurrentBookPercent() ));
            }

            @Override
            public void onJumpPrevious() {


            }

            @Override
            public void onJumpNext() {

            }

            @Override
            public boolean onJumpBeforeFirst() {
                if(m_current_chaptor>0)
                {
                    m_current_chaptor--;
                    updateBookChaptor(m_name ,-1);
                    return true;
                }
                return false;
            }

            @Override
            public boolean onJumpAfterLast() {
                ArrayList<BookCatalog> arrayList=m_sd_loader.getCatalogList();
                if(arrayList!=null&&m_current_chaptor<m_sd_loader.getCatalogList().size()-1)
                {
                    m_current_chaptor++;
                    updateBookChaptor(m_name ,0);
                    return true;
                }
                return false;
            }
        });

        m_download=new BookDownloadLite(BookDefine.SERVER_ADDR,BookDefine.FILE_SERVER_PORT){
            @Override
            protected void onPreExecute() {
                super.onPreExecute();
               showWaiting(true);
            }

            @Override
            protected void onPostExecute(Boolean aBoolean) {
                super.onPostExecute(aBoolean);
                if(m_destroy_falg)
                {
                    return;
                }
                showWaiting(false);
                if(aBoolean)
                {
                    showBookContent(m_path, m_name,m_id,m_local_id);
                }
                else
                {
                    Toast.makeText(BookPageViewActivity.this,R.string.network_fail,Toast.LENGTH_LONG).show();
                }
            }
        };

        //if(!BookPath.bookFileExistInData(m_path)) {
        if(!BookPath.bookFileExistInFileSystem(BookPath.mapSDDownloadPath(m_path))) {
            m_download.execute(parm);
        }else
        {
            showWaiting(false);
            showBookContent(m_path, m_name,m_id,m_local_id);
        }
    }
    public int getChaptor()
    {
        return m_current_chaptor;
    }
    private void saveBookMark()
    {
        if(m_local_id!=-1)
        {
            BookMark mark= BookManager.getSingleton().getBookMark(m_local_id,BookDefine.MASTER_BOOK_MARK_NAME);
            if(mark==null)
            {
               DebugPrintUtil.e(TAG,"Master Bookmark Lost OR Not Set");
            }
            else
            {
                BookPageView bookContent=(BookPageView) findViewById(R.id.book_pager_content_view);
                int currentIndex=bookContent.getCurrentIndex();
                float per=getCurrentBookPercent();

                if(currentIndex!=mark.getIndex()||m_current_chaptor!=mark.getChapterId()||per!=mark.getReadPercent()) {
                    mark.setIndex(currentIndex);
                    mark.setChapterId(m_current_chaptor);
                    mark.setReadPercent(per);
                    BookManager.getSingleton().modifyBookMark(mark);
                }
            }
        }
    }

    @Override
    protected void onPause() {
        saveBookMark();
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        m_destroy_falg=true;
        if(m_download!=null)
        {
            m_download.cancle_download();
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == BookDefine.SD_PERMITION_REQUEST_CODE) {
            if (grantResults[0] != PackageManager.PERMISSION_GRANTED || grantResults[1] != PackageManager.PERMISSION_GRANTED) {
                AlertDialog.Builder builder=new AlertDialog.Builder(this);
                builder.setTitle(R.string.permition_title);
                builder.setMessage(R.string.sd_permition_deny);
                builder.setPositiveButton(R.string.i_kown, null);
                builder.show();
            } else {

            }

        }
    }
}
