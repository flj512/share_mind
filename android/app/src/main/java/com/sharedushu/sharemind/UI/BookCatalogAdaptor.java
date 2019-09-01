package com.sharedushu.sharemind.UI;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.Database.BookCatalog;
import com.sharedushu.sharemind.R;

import java.util.List;

/**
 * Created by flj on 2016/11/20.
 */
public class BookCatalogAdaptor extends ArrayAdapter<BookCatalog> {
    private int m_item_resource_id;
    private BookPageViewActivity m_page_view_activity;
    private final int  FOCUS_BG=0xff3F51B5;
    private final int FOCUS_FONT=0xffffffff;
    private final int NORMAL_FONT=0xff808080;

    public BookCatalogAdaptor(Context context, int resource, List<BookCatalog> objects) {
        super(context, resource, objects);
        m_item_resource_id=resource;
        m_page_view_activity=(BookPageViewActivity)context;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View view;
        BookCatalog catalog= getItem(position);

        if(convertView==null) {
            view = LayoutInflater.from(getContext()).inflate(m_item_resource_id, null);
        }
        else {
            view= convertView;
        }
        TextView name = (TextView) view.findViewById(R.id.book_catalog_name);
        name.setText(catalog.getName());
        if(m_page_view_activity.getChaptor()==position)
        {
            name.setTextColor(FOCUS_FONT);
            name.setBackgroundColor(FOCUS_BG);
        }
        else {
            name.setTextColor(NORMAL_FONT);
            name.setBackgroundColor(0);
        }

        view.setTag(catalog);

        return view;
    }
}
