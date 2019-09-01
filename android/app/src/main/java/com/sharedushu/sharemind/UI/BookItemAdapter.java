package com.sharedushu.sharemind.UI;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.BookManage.BookManager;
import com.sharedushu.sharemind.BookManage.BookMark;
import com.sharedushu.sharemind.R;

import java.util.List;

/**
 * Created by flj on 2016/11/9.
 */
public class BookItemAdapter extends ArrayAdapter<BookInfo> {
    private int m_item_resource_id;

    public BookItemAdapter(Context context, int resource, List<BookInfo> objects) {
        super(context, resource, objects);
        m_item_resource_id=resource;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View view;
        BookInfo bookInfo=getItem(position);

        if(convertView==null) {
            view = LayoutInflater.from(getContext()).inflate(m_item_resource_id, null);
        }
        else {
            view= convertView;
        }
        TextView name = (TextView) view.findViewById(R.id.item_book_name);
        name.setText(bookInfo.getBookName());

        TextView author = (TextView) view.findViewById(R.id.item_book_author);
        author.setText(bookInfo.getBookAuthor());

        TextView percent = (TextView) view.findViewById(R.id.read_progress_percent);//view 可能会复用，需要清除原来的显示。
        percent.setText("0.0%");
        BookMark bookMark= BookManager.getSingleton().getBookMark(bookInfo.getBookLocalId(), BookDefine.MASTER_BOOK_MARK_NAME);
        if(bookMark!=null) {
            percent.setText(String.format("%.1f%%", bookMark.getReadPercent()));
        }

        view.setTag(bookInfo);

        return view;
    }
}
