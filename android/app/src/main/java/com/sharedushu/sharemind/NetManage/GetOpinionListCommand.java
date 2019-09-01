package com.sharedushu.sharemind.NetManage;

import android.os.Handler;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.BookManage.BookOpinion;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by flj on 2016/11/29.
 */
public class GetOpinionListCommand extends SequenceCommand implements Command {
    private long m_bookid;
    private int m_chapter_id;
    private int m_start;
    private int m_end;
    private static final String TAG="GetOpinionListCommand";

    public GetOpinionListCommand(int response_id,int cmd_id, long book_id, int chapter_id, int start, int end, Handler handler)
    {
        super(cmd_id,response_id,handler);
        m_bookid=book_id;
        m_chapter_id=chapter_id;
        m_start=start;
        m_end=end;
    }
    @Override
    public JSONObject onCreate() {
        JSONObject jobj=new JSONObject();
        try {
            jobj.put(BookDefine.TAG_CMD,BookDefine.CMD_GET_OPINION_LIST);

            jobj.put(BookDefine.TAG_BOOK_INFO_ID, m_bookid);
            jobj.put(BookDefine.TAG_BOOK_OPINION_CHAPTER,m_chapter_id);
            jobj.put(BookDefine.TAG_BOOK_OPINION_START, m_start);
            jobj.put(BookDefine.TAG_BOOK_OPINION_END, m_end);

        }catch (JSONException e){
            jobj=null;
            e.printStackTrace();
        }
        return jobj;
    }

    private List<BookOpinion> parseOpinionResponse(String resp)
    {
        List<BookOpinion> bookOpininList=new ArrayList<BookOpinion>();
        try {
            JSONArray jsonArray= new JSONArray(resp);
            for (int i=0;i<jsonArray.length();i++)
            {
                BookOpinion opinion=new BookOpinion();
                JSONObject obj=jsonArray.getJSONObject(i);
                try {
                    String tag_str=obj.getString(BookDefine.TAG_BOOK_OPINION);
                    opinion.setText(tag_str);

                    bookOpininList.add(opinion);
                }
                catch (JSONException e)
                {
                    e.printStackTrace();
                }
            }
        }catch (JSONException e)
        {
            e.printStackTrace();
        }

        return bookOpininList;
    }
    @Override
    public void onResponse(String response) {
        DebugPrintUtil.i(TAG,"Recieve:="+response);
         sendMessage(parseOpinionResponse(response));
    }

    @Override
    public void onError(int error) {
        DebugPrintUtil.i(TAG,"ErrorCode="+error);
        sendError(error);
    }
}
