package com.sharedushu.sharemind.Tools;

import android.os.Environment;

import com.sharedushu.sharemind.BookManage.BookInfo;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

/**
 * Created by flj on 2016/11/9.
 */
public class BookPath {
    static private final String ROOT_DIR="sharedushu"+File.separator+"sharemind"+File.separator+"bookinfo";
    static private final String DOWNLOAD_DIR="sharedushu"+File.separator+"sharemind"+File.separator+"download";

    public static boolean bookFileExist(BookInfo info)
    {
        boolean ret;


            if (info.getBookType()==BookInfo.BOOK_TYPE_INET) {
                //ret=bookFileExistInData(info.getBookHttpPath());
                ret=bookFileExistInFileSystem(mapSDDownloadPath(info.getBookHttpPath()));
            }else{
                ret = bookFileExistInFileSystem(info.getBookLocalPath());
            }
        return ret;
    }
    public static boolean bookFileExistInData(String path)
    {
        boolean ret=false;
        FileInputStream in=null;

        try
        {
            in = ApplicationE.getContext().openFileInput(path);
                if (in != null) {
                    ret = true;
                }
        }
        catch (FileNotFoundException e)
        {
            in=null;
        }
        finally {
            if(in!=null)
            {
                try {
                    in.close();
                }catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        return ret;
    }
    public static boolean bookFileExistInFileSystem(String path)
    {
        return new File(path).exists();
    }

    static public String mapSDPath(String sd_path)
    {
        File root_path = Environment.getExternalStorageDirectory();

        if(root_path!=null)
        {
            return root_path.getAbsolutePath()+File.separator+ROOT_DIR+File.separator+sd_path;
        }
        return null;
    }
    static public String getSDDownloadPath()
    {
        File root_path = Environment.getExternalStorageDirectory();

        if(root_path!=null)
        {
            return root_path.getAbsolutePath()+File.separator+DOWNLOAD_DIR;
        }
        return null;
    }
    static public String mapSDDownloadPath(String sd_path)
    {
        File root_path = Environment.getExternalStorageDirectory();

        if(root_path!=null)
        {
            return root_path.getAbsolutePath()+File.separator+DOWNLOAD_DIR+File.separator+sd_path;
        }
        return null;
    }

    static public String getFullPathInData(String path)
    {
        return ApplicationE.getContext().getFilesDir()+File.separator+path;
    }

    static public void delete(String path)
    {
        File file=new File(path);
        if(!file.exists())
        {
            return;
        }

        if(file.isDirectory())
        {
            String[] subs=file.list();
            if(subs.length!=0)
            {
                for (String subDir :subs)
                {
                    delete(path+File.separator+subDir);
                }

            }

        }

        file.delete();
    }
}
