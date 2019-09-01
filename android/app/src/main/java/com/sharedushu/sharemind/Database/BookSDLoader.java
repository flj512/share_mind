package com.sharedushu.sharemind.Database;

import android.os.Debug;

import com.sharedushu.sharemind.BookManage.BookInfo;
import com.sharedushu.sharemind.BookManage.BookManager;
import com.sharedushu.sharemind.Tools.BookPath;
import com.sharedushu.sharemind.Tools.DebugPrintUtil;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.zip.CRC32;
import java.util.zip.CheckedInputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;

import javax.crypto.Cipher;
import javax.crypto.CipherInputStream;
import javax.crypto.CipherOutputStream;
import javax.crypto.spec.SecretKeySpec;

/**
 * Created by flj on 2016/11/20.
 */
public class BookSDLoader {
    static private final String TAG="BookSDLoader";
    static private final String INDEX="index";
    static private final String META="meta";
    static private final String TEMP_SURFIX=".temp";
    //cipher
    private String KEY_MODE="AES";
    private String CIPHER_MODE="AES/ECB/PKCS5Padding";
    private String KEY="BkKy1302?Dhs=271";

    private String m_sd_path;
    private  String m_zip_path;
    private int m_total_size;
    private ArrayList<BookCatalog> m_catalog_list;

    public BookSDLoader(String httpPath)
    {
        //m_zip_path=BookPath.getFullPathInData(httpPath);
        m_zip_path=BookPath.mapSDDownloadPath(httpPath);
        m_sd_path=BookPath.mapSDPath(httpPath);
        m_catalog_list=null;
        m_total_size=-1;//负数表示未知。
    }

    private Cipher getCipher(int mode)
    {
        Cipher cipher;
        try{
            SecureRandom secureRandom=new SecureRandom();
            SecretKeySpec keySpec=new SecretKeySpec(KEY.getBytes(),KEY_MODE);

            cipher= Cipher.getInstance(CIPHER_MODE);
            cipher.init(mode, keySpec,secureRandom);
        }catch(Exception e){
            e.printStackTrace();
            cipher=null;
        }
        return cipher;
    }

    public boolean unpack()
    {
        boolean ret=false,has_error=false,zip_error=false;
        if(m_sd_path==null)
        {
            return false;
        }
        File zipFile=new File(m_zip_path);
        if(!zipFile.exists())
        {
            return false;
        }
        File sd_file=new File(m_sd_path);
        if(!sd_file.exists())
        {
            if (!sd_file.mkdirs())
            {
                return false;
            }
        }
        Cipher cipherD=getCipher(Cipher.DECRYPT_MODE);
        if(cipherD==null)
        {
            return false;
        }
        Cipher cipherE=getCipher(Cipher.ENCRYPT_MODE);
        if(cipherE==null)
        {
            return false;
        }


        BufferedOutputStream writer=null;
        ZipInputStream zipInputStream=null;
        byte[] buffer;//
        int read_byte;

        try {
            //unpack
            CipherInputStream cipherInputStream=new CipherInputStream(new FileInputStream(zipFile),cipherD);
            zipInputStream=new ZipInputStream(new CheckedInputStream(cipherInputStream,new CRC32()));
            buffer=new byte[4096];//此处不用BufferInputStream(zipInputStream)，不用的版本表现不一样，android5.0以上有未知bug

            ZipEntry entry;
            String entryPath,entryDir;
            File  entryFile,entryDirFile;
            CipherOutputStream cipherOutputStream;
            int index;
            while ((entry=zipInputStream.getNextEntry())!=null)
            {
                entryPath=m_sd_path+File.separator+entry.getName();
                index=entryPath.lastIndexOf(File.separator);
                if(index!=-1)
                {
                    entryDir=entryPath.substring(0,index);
                    entryDirFile=new File(entryDir);
                    if(!entryDirFile.exists())
                    {
                        if (!entryDirFile.mkdirs())
                        {
                            has_error=true;
                            break;
                        }
                    }
                }
                entryFile=new File(entryPath);
                cipherOutputStream=new CipherOutputStream(new FileOutputStream(entryFile),cipherE);
                writer=new BufferedOutputStream(cipherOutputStream);

                while ((read_byte=zipInputStream.read(buffer,0,buffer.length))!=-1)
                {
                    writer.write(buffer,0,read_byte);
                }
                writer.close();
                writer=null;
            }

            ret=!has_error;

        }
        catch (ZipException zipe)
        {
            zip_error=true;
            zipe.printStackTrace();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        } finally {
            try {
                if(writer!=null)
                {
                    writer.close();
                }
                if(zipInputStream!=null)
                {
                    zipInputStream.close();
                }
            }catch (IOException e) {
                e.printStackTrace();
            }

            if(zip_error)
            {
                zipFile.delete();//may be zip file is invalidate
            }
        }

        if(ret==false)
        {
            BookPath.delete(m_sd_path);
        }

        return ret;
    }
    private synchronized void getChapterSize()
    {
        if(m_catalog_list==null||m_catalog_list.size()==0)
        {
            return;
        }
        File file=new File(m_sd_path+File.separator+META);
        if(file.exists()) {
            int index=0;
            int start=0;
            BufferedReader reader=null;
            try {
                reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
                int size;
                String line;
                while ((line = reader.readLine())!=null&&index<m_catalog_list.size())
                {
                    size=Integer.parseInt(line);
                    m_catalog_list.get(index).setCharStart(start);
                    m_catalog_list.get(index).setCharCount(size);
                    index++;
                    start+=size;
                }
            }catch (IOException e)
            {
                e.printStackTrace();
            }finally {
                if(reader!=null)
                {
                    try {
                        reader.close();
                    }catch (Exception e)
                    {
                        e.printStackTrace();
                    }

                }
            }

            if(index!=m_catalog_list.size())
            {
                file.delete();
            }
            else {
                m_total_size=start;
            }
        }
    }
    private void buildCatalogList()
    {
        BufferedReader reader=null;
        boolean no_error=true;
        int size=0;

        ArrayList<BookCatalog> catalogArrayList=null;
        Cipher cipherD=getCipher(Cipher.DECRYPT_MODE);
        if(cipherD==null)
        {
            return ;
        }
        try {
            FileInputStream inputStream=new FileInputStream(new File(m_sd_path+File.separator+INDEX));
            CipherInputStream cipherInputStream=new CipherInputStream(inputStream,cipherD);
            reader=new BufferedReader(new InputStreamReader(cipherInputStream));

            catalogArrayList=new ArrayList<BookCatalog>();
            String line;
            String[] element;
            BookCatalog catalog;
            int index=0;
            while ((line=reader.readLine())!=null)//format "name path size ",size  is not mandatory
            {
                element=line.split("\\s+|\t+");
                if(element.length<2)
                {
                    no_error=false;
                    break;
                }

                catalog=new BookCatalog(element[0],element[1],index++);

                if(element.length>2)//size
                {
                    if(size!=-1)//解析大小时没有发生任何异常
                    {
                        catalog.setCharStart(size);
                        try {
                            catalog.setCharCount(Integer.parseInt(element[2]));
                            size +=catalog.getCharCount() ;
                        } catch (NumberFormatException e) {
                            size = -1;
                            e.printStackTrace();
                        }
                    }
                }
                catalogArrayList.add(catalog);
            }
        }catch (Exception e)
        {
            no_error=false;
            e.printStackTrace();
        }

        if (reader!=null)
        {
            try {
                reader.close();
            }catch (IOException e)
            {
                e.printStackTrace();
            }
        }
        if(no_error) {
            m_catalog_list = catalogArrayList;
            getChapterSize();//以本地计算的大小为准
            if(m_total_size>0&&size>0&&m_total_size!=size)
            {
                BookPath.delete(m_sd_path+File.separator+META);
                DebugPrintUtil.e(TAG,"Local Size is inconsistent with remote size,"+"local="+m_total_size+",remote="+size);
            }
            if(size>0&&m_total_size<0)
            {
                m_total_size=size;
            }
        }
    }
    public ArrayList<BookCatalog> getCatalogList()
    {
        if( m_catalog_list==null)
        {
            buildCatalogList();
        }
        return m_catalog_list;
    }

    public BufferedReader getChapterReader(int index)
    {
        ArrayList<BookCatalog> list=getCatalogList();
        if (list!=null&&index>=0&&index<list.size())
        {
            BufferedReader reader=null;
            //cipher
            Cipher cipher=getCipher(Cipher.DECRYPT_MODE);
            if(cipher==null)
            {
                return null;
            }
            try {
                FileInputStream inputStream=new FileInputStream(new File(m_sd_path+File.separator+list.get(index).getPath()));
                CipherInputStream cipherInputStream=new CipherInputStream(inputStream,cipher);
                reader=new BufferedReader(new InputStreamReader(cipherInputStream));
            }catch (IOException e)
            {
                e.printStackTrace();
                if (reader!=null)
                {
                    try {
                        reader.close();
                    }catch (IOException error)
                    {
                        error.printStackTrace();
                    }
                }
                reader=null;
            }

            return reader;
        }
        return null;
    }

    public int getChapterStartIndex(int index)
    {
        ArrayList<BookCatalog> list=getCatalogList();
        if (list!=null&&index>=0&&index<list.size())
        {
            return list.get(index).getCharStart();
        }
        return 0;
    }

    public String getChapterName(int index)
    {
        ArrayList<BookCatalog> list=getCatalogList();
        if (list!=null&&index>=0&&index<list.size())
        {
            return list.get(index).getName();
        }
        return null;
    }

    public void calculateSize(int local_id)
    {
        if(m_catalog_list==null||m_catalog_list.size()==0)
        {
            return;
        }
        //final BookInfo info = BookManager.getSingleton().getBookInfoByLocalId(local_id);
        //if(info!=null&&info.getBookCharCount()==0)
        //if(m_total_size < 0)
        File meta_file=new File(m_sd_path+File.separator+META);
        if(!meta_file.exists())
        {
            new Thread(){
                //private BookInfo m_book_info;
                @Override
                public void run() {
                    //m_book_info=info;
                    int char_size=0,total_size=0;

                    //DebugPrintUtil.i(TAG,"Start Scan:"+m_book_info.getBookHttpPath());
                    BufferedReader bufferedReader=null;
                    int index=0;
                    ArrayList<Integer> sizeList = new ArrayList<Integer>();

                    //cipher
                    Cipher cipher=getCipher(Cipher.DECRYPT_MODE);
                    if(cipher==null)
                    {
                        return ;
                    }
                    try {
                        FileInputStream inputStream;
                        for(BookCatalog catalog:m_catalog_list) {
                            inputStream = new FileInputStream(m_sd_path + File.separator + catalog.getPath());
                            CipherInputStream cipherInputStream=new CipherInputStream(inputStream,cipher);
                            bufferedReader = new BufferedReader(new InputStreamReader(cipherInputStream));
                            String line;
                            char_size=0;
                            while (true) {
                                line = bufferedReader.readLine();
                                if (line == null) {
                                    break;
                                }
                                char_size += line.length();
                            }
                            DebugPrintUtil.i(TAG,m_sd_path + File.separator + catalog.getName()+":"+char_size);
                            sizeList.add(new Integer(char_size));
                            index++;
                            total_size+=char_size;
                            bufferedReader.close();
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    } finally {
                        if (bufferedReader != null) {
                            try {
                                bufferedReader.close();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }

                        }
                    }

                    if(index==m_catalog_list.size())
                    {
                        BufferedWriter writer=null;
                        try {
                            File file = new File(m_sd_path + File.separator + META);

                            writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(file)));
                            for(Integer integer:sizeList)
                            {
                                writer.write(integer.toString()+"\r\n");
                            }
                            writer.close();

                            //m_book_info.setBookCharCount(total_size);
                            //DebugPrintUtil.i(TAG, m_book_info.getBookHttpPath() + " Size=" + total_size);
                            //BookManager.getSingleton().setBookCharCout(m_book_info);

                            getChapterSize();
                        }catch (IOException e)
                        {
                            e.printStackTrace();
                            if(writer!=null)
                            {
                                try{
                                    writer.close();
                                }catch (IOException err)
                                {
                                    err.printStackTrace();
                                }
                            }
                        }
                    }
                }
            }.start();
        }
    }
    public int getSize()
    {
        return m_total_size;
    }
}
