package com.sharedushu.sharemind.UI;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;

import com.sharedushu.sharemind.R;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.Vector;

/**
 * Created by flj on 2016/11/11.
 */
public class BookPageView extends TextView {
    private String BOOK_PAGE_VIEW_TAG="BookPageView";

    private Vector<BookLine> m_line_vector;
    private BufferedReader m_reader;
    private String m_current_section;
    private int m_current_page;
    private int m_cached_page;
    private int m_max_page;
    private int m_lines_per_page=0;
    private int m_startIndex;//for bookmark
    //selection
    //default attrubute
    private static final int SELECT_BK_DEF=0xa0000000;
    private static final int SELECT_FR_DEF=0xffffffff;
    private static final int SELECT_STRING_LEN_DEF=6;
    private static final int SELECT_RECT_MARGIN_DEF=10;
    private static final int SELECT_RECT_YOFFSET_DEF=30;

    private boolean m_select_mode=false;
    private int m_select_row=0;
    private int m_select_column=0;
    private String m_select_text=null;
    private int m_select_bk_color=SELECT_BK_DEF;
    private int m_select_fr_color=SELECT_FR_DEF;
    private int m_hight_length=SELECT_STRING_LEN_DEF;
    private int m_hight_rect_margin=SELECT_RECT_MARGIN_DEF;//显示高亮文字的浮动矩形留白
    private int m_hight_offset_height=SELECT_RECT_YOFFSET_DEF;//相对于选中字体的真实位置在Y轴上的偏移

    private OnBookPageViewEvent m_listenr;

    void setListner(OnBookPageViewEvent listner)
    {
        m_listenr =listner;
    }

    void setStart(int start)
    {
        m_startIndex=start;
    }
    public void reset()
    {
        m_line_vector=new Vector<BookLine>();
        m_reader=null;
        m_current_section=null;
        m_current_page=-1;
        m_cached_page=-1;
        m_max_page=-1;
        m_startIndex=0;
        m_select_mode=false;
        m_select_row=-1;
        m_select_column=-1;
        m_select_text=null;
    }
    private void init()
    {
        reset();
        setOnTouchListener(new OnTouchListener() {
            private float m_down_x,m_down_y;
            private static final int MOVE_LEFT=-1;
            private static final int MOVE_RIGHT=1;
            private static final int MOVE_IGNORE=0;

            private int actionType(float x, float y)
            {
                float delta_x=x-m_down_x;
                float delta_y=y-m_down_y;
                int ret=MOVE_IGNORE;

                if(delta_x <-50)
                {
                    ret=MOVE_RIGHT;
                }
                else if(delta_x >50)
                {
                    ret=MOVE_LEFT;
                }

                return ret;
            }
            @Override
            public boolean onTouch(View v, MotionEvent event) {

                //DebugPrintUtil.i(BOOK_PAGE_VIEW_TAG,"Motion Type="+event.getAction()+",x="+event.getX()+",y="+event.getY());
                if(m_current_page==-1)
                {
                    return false;
                }
                if(event.getAction()==MotionEvent.ACTION_DOWN)
                {
                    m_down_x=event.getX();
                    m_down_y=event.getY();
                    getCharIndex(m_down_x, m_down_y);
                }
                else if(event.getAction()==MotionEvent.ACTION_UP)
                {
                    if(!m_select_mode) {
                        int type= actionType(event.getX(),event.getY());
                        if (type == MOVE_LEFT) {
                            if(m_current_page > 0) {
                                previousPage();
                                m_listenr.onJumpPrevious();
                                invalidate();
                            }else{
                                if(m_listenr.onJumpBeforeFirst())
                                {
                                    m_listenr.onJumpPrevious();
                                    invalidate();
                                }
                            }
                        } else if (type == MOVE_RIGHT) {
                            if(!reachLastPage()) {
                                nextPage();
                                m_listenr.onJumpNext();
                                invalidate();
                            }else {
                                if(m_listenr.onJumpAfterLast()) {
                                    m_listenr.onJumpNext();
                                    invalidate();
                                }
                            }
                            invalidate();
                        }
                        else {
                            m_listenr.onPress();
                        }
                    }
                    else {
                        m_select_mode=false;
                        if (m_select_row != -1&&m_select_column!=-1) {
                            int index=m_line_vector.get(m_select_row).getStartLineIndex()+m_select_column;
                            m_listenr.onSelect(index, event.getRawX(), event.getRawY(),m_select_text);
                        }
                        invalidate();
                    }
                }
                else if(event.getAction()==MotionEvent.ACTION_MOVE)
                {
                    if(m_select_mode) {
                        getCharIndex(event.getX(), event.getY());
                        DebugPrintUtil.i(BOOK_PAGE_VIEW_TAG, "row=" + m_select_row + ",column=" + m_select_column);

                        invalidate();
                    }
                }

                return false;
            }
        });

        setOnLongClickListener(new OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                DebugPrintUtil.i(BOOK_PAGE_VIEW_TAG,"Long Touch");
                if(m_current_page==-1)
                {
                    return false;
                }
                m_select_mode=true;
                invalidate();
                return false;
            }
        });
    }
    private void highLightSelection(Canvas canvas)
    {
        //DebugPrintUtil.i(BOOK_PAGE_VIEW_TAG,"HightLightSelection");

        if(m_select_column!=-1&&m_select_row!=-1&&m_select_mode)
        {
            //计算浮动文字的范围
            String lineString=m_line_vector.get(m_select_row).getLineString();
            int left_start=Math.max(m_select_column-m_hight_length/2,0);
            int right_start=left_start+m_hight_length;
            if(right_start>lineString.length())
            {
                int shift=right_start-lineString.length();
                right_start-=shift;
                left_start=Math.max(left_start-shift,0);
            }
            m_select_text=lineString.substring(left_start,right_start);

            //计算浮动文字显示框大小
            int lineIndex=m_select_row-m_current_page*m_lines_per_page;
            int lineHeight=getLineHeight();
            int selectLineYpos=getPaddingTop()+lineIndex*lineHeight;
            Rect rect=new Rect();
            Paint paint=getPaint();

            int highLightHeight=lineHeight+2*m_hight_rect_margin;//高亮矩形的高度
            if(selectLineYpos>=highLightHeight+m_hight_offset_height)
            {
                rect.top=selectLineYpos-(highLightHeight+m_hight_offset_height);
            }
            else
            {
                rect.top=selectLineYpos+m_hight_offset_height+lineHeight;
            }
            rect.bottom=rect.top+highLightHeight;
            rect.left=getPaddingLeft()+(int)(paint.measureText(lineString,0,left_start));
            rect.right=rect.left+(int)(paint.measureText(lineString,left_start,right_start))+2*m_hight_rect_margin;
            if(rect.right>getWidth())
            {
                int shiftWidth=rect.right-getWidth();
                rect.left-=shiftWidth;
                rect.right-=shiftWidth;
            }

            //高亮绘制
            int old=paint.getColor();
            paint.setColor(m_select_bk_color);
            canvas.drawRect(rect,paint);//绘制浮动窗口的背景
            canvas.drawRect(getLeft(),selectLineYpos,getRight(),selectLineYpos+lineHeight, paint);//绘制选中行的背景

            int x=rect.left+m_hight_rect_margin;
            /*float ascent=paint.getFontMetrics().ascent;
            float descent=paint.getFontMetrics().descent;
            float bottom=paint.getFontMetrics().bottom;
            float top=paint.getFontMetrics().top;*/

            int fontHeight=(int)(0-paint.getFontMetrics().ascent);
            int y=rect.bottom-(rect.bottom-rect.top-fontHeight)/2;
            paint.setColor(m_select_fr_color);
            canvas.drawText(m_select_text,x,y,paint);//绘制浮动字体
            canvas.drawText(lineString,getPaddingLeft(),selectLineYpos+lineHeight-(lineHeight-fontHeight)/2,paint);//绘制选中行字体
            paint.setColor(old);
        }
    }
    private void getCharIndex(float x ,float y)
    {
        int lineIndex=(int)((y-getPaddingTop())/getLineHeight());
        if(lineIndex>=m_lines_per_page||lineIndex<0)
        {
            m_select_row=-1;//outbound
            return ;
        }
        lineIndex+=m_current_page*m_lines_per_page;
        if(lineIndex>=m_line_vector.size())
        {
            m_select_row=-1;//outbound
            return ;
        }

        m_select_row=lineIndex;
        m_select_column=getPaint().breakText(m_line_vector.get(lineIndex).getLineString(),true,x-getPaddingLeft(),null);
        if(m_select_column>=m_line_vector.get(lineIndex).getLineString().length())
        {
            m_select_column=-1;
        }
    }
    public BookPageView(Context context) {
        super(context);
        init();
    }

    public BookPageView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
        TypedArray typedArray=context.obtainStyledAttributes(attrs, R.styleable.BookPageView);
        m_select_bk_color= typedArray.getColor(R.styleable.BookPageView_rectBackground,SELECT_BK_DEF);
        m_select_fr_color=typedArray.getColor(R.styleable.BookPageView_rectForeground,SELECT_FR_DEF);
        m_hight_rect_margin=typedArray.getDimensionPixelSize(R.styleable.BookPageView_highLightRectMargin,SELECT_RECT_MARGIN_DEF);
        m_hight_offset_height=typedArray.getDimensionPixelSize(R.styleable.BookPageView_highLightRectYOffset,SELECT_RECT_YOFFSET_DEF);
        m_hight_length=typedArray.getInteger(R.styleable.BookPageView_highLightStringLen,SELECT_STRING_LEN_DEF);
        typedArray.recycle();

    }

    public void setReader(BufferedReader reader)
    {
        m_reader=reader;
    }

    private String readSection()
    {
        String section;
        if(m_reader==null)
        {
            return null;
        }
        try {
            section=m_reader.readLine();
        }catch (IOException e) {
            e.printStackTrace();
            section=null;
        }
        return section;
    }
    private boolean reachLastPage()
    {
        return m_max_page!=-1&&m_current_page==m_max_page;
    }
    private void previousPage()
    {
        if(m_current_page>0)
        {
            m_current_page--;
        }
    }
    private void nextPage()
    {
        int maxTextPerLine=0;
        int start_line=0;
        int line_start_index=0;

        if(reachLastPage())
        {
            return;
        }

        if(++m_current_page<=m_cached_page)
        {
            return ;
        }

        if(m_current_page!=0)
        {
            start_line=m_current_page*m_lines_per_page;
            line_start_index=m_line_vector.get(start_line-1).getStartLineIndex()+m_line_vector.get(start_line-1).getLineString().length();
        }
        for(int read_line=0;read_line<m_lines_per_page;read_line++)
        {
            BookLine line=new BookLine();
            line.setStartLineIndex(line_start_index);

            if(m_current_section==null||m_current_section.length()==0)
            {
                m_current_section=readSection();
                if(m_current_section==null)
                {
                    if(m_current_page>0&&m_line_vector.size()-start_line==0) {//empty page
                        m_current_page--;
                    }
                    m_max_page=m_current_page;
                    break;
                }

                if (m_current_section.length()==0)
                {
                    line.setLineString(m_current_section);
                    m_line_vector.add(line);
                    continue;
                }
            }
            maxTextPerLine=getPaint().breakText(m_current_section,true,getWidth()-getPaddingLeft()-getPaddingRight(),null);
            line.setLineString(m_current_section.substring(0,maxTextPerLine));
            m_line_vector.add(line);
            m_current_section=m_current_section.substring(maxTextPerLine);
            line_start_index+=maxTextPerLine;
        }
        if(m_cached_page<m_current_page)
        {
            m_cached_page=m_current_page;
        }
    }
    private void readAll()
    {
        if(!reachLastPage())
        {
            nextPage();
        }
    }
    private void jumpTo(int index)
    {
        if(m_current_page==-1)
        {
            nextPage();
        }
        m_current_page=0;

        int last_line=Math.min((m_current_page+1)*m_lines_per_page,m_line_vector.size());
        int last_char_index=-1;
        if(last_line>0) {
            last_char_index=m_line_vector.get(last_line - 1).getStartLineIndex() + m_line_vector.get(last_line - 1).getLineString().length();
        }

        while(last_char_index!=-1&&(last_char_index<=index||index==-1)&&!reachLastPage())
        {
            nextPage();
            last_line=Math.min((m_current_page+1)*m_lines_per_page,m_line_vector.size());
            last_char_index=m_line_vector.get(last_line - 1).getStartLineIndex() + m_line_vector.get(last_line - 1).getLineString().length();
        }
    }
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        setMeasuredDimension(MeasureSpec.getSize(widthMeasureSpec), MeasureSpec.getSize(heightMeasureSpec));
    }

    @Override
    protected void onDraw(Canvas canvas) {
        int lineHeight=getLineHeight();
        if(m_lines_per_page==0)
        {
            m_lines_per_page=(getHeight()-getPaddingBottom()-getPaddingTop())/lineHeight;
        }
        if( m_current_page==-1)//first page
        {
            jumpTo(m_startIndex);
        }

        //pre draw
        m_listenr.onRefresh();

        int line_start = m_current_page * m_lines_per_page;
        int line_end = Math.min(m_line_vector.size(), line_start + m_lines_per_page);
        Paint paint = getPaint();
        int fontHeight=(int)(0-paint.getFontMetrics().ascent);
        int yPos = getPaddingTop() + lineHeight-(lineHeight-fontHeight)/2;

        for (int i = line_start; i < line_end; i++) {
            canvas.drawText(m_line_vector.get(i).getLineString(), getPaddingLeft(), yPos, paint);
            yPos += lineHeight;
        }
        //draw page index
        /*Paint.Align oldTextAlign = paint.getTextAlign();
        paint.setTextAlign(Paint.Align.CENTER);
        canvas.drawText(String.format("- %d -", m_current_page), getWidth() / 2, getPaddingTop() + lineHeight * (m_lines_per_page + 1), paint);
        paint.setTextAlign(oldTextAlign);*/
        //select mode ,show selected text
        highLightSelection(canvas);
    }

    public int getCurrentIndex()
    {
        if(m_current_page==-1)
        {
            return 0;
        }

        int start_line=m_current_page*m_lines_per_page;

        if(start_line<m_line_vector.size()) {
            return m_line_vector.get(start_line).getStartLineIndex();
        }
        else
        {
            return 0;
        }
    }
}
