package com.sharedushu.sharemind;


public interface ZipBookMakerListener {
	boolean onBuildList(String[] catalog_list);
	void onComplete(boolean result,String reason);
}
