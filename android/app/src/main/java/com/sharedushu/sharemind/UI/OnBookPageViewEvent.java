package com.sharedushu.sharemind.UI;

/**
 * Created by flj on 2016/11/11.
 */
public interface OnBookPageViewEvent {
    void onSelect(int pos,float x,float y,String selected);
    void onJumpPrevious();
    void onJumpNext();
    boolean onJumpBeforeFirst();
    boolean onJumpAfterLast();
    void onPress();
    void onRefresh();
}
