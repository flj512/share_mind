package com.sharedushu.sharemind;

import java.io.File;

public class ZipMake {

	public static void main(String[] args) {
		if(args.length==0)
		{
			System.out.println("Input a path !");
			return;
		}
		String root_path=args[0];
		File file=new File(root_path);
		if(!file.isDirectory())
		{
			System.out.println(root_path+" is not a path");
			return;
		}
		
		String[] subs=file.list();
		for(String sub:subs)
		{
			ZipBookMaker bookMaker=new ZipBookMaker(root_path+File.separator+sub, sub, "GBK", "UTF-8");
			bookMaker.setListener(new ZipBookMakerListener() {
				
				@Override
				public void onComplete(boolean result, String reason) {
					// TODO Auto-generated method stub
					System.out.println("Complete,"+result+":"+reason);
				}
				
				@Override
				public boolean onBuildList(String[] catalog_list) {
					for(String string:catalog_list)
					{
						System.out.println(string);
					}
					return true;
				}
			});
			
			bookMaker.makeStart();
		}
	}

}
