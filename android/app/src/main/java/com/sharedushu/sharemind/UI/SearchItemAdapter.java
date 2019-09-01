package com.sharedushu.sharemind.UI;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.R;

import java.util.List;

/**
 * Created by flj on 2016/11/8.
 */
public class SearchItemAdapter extends ArrayAdapter<BookInfo> {
    private int m_item_resource_id;

    public SearchItemAdapter(Context context, int resource, List<BookInfo> objects) {
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
        TextView name = (TextView) view.findViewById(R.id.search_item_book_name);
        name.setText(bookInfo.getBookName());

        TextView author = (TextView) view.findViewById(R.id.search_item_book_author);
        author.setText(bookInfo.getBookAuthor());

        view.setTag(bookInfo);

        return view;
    }
}
