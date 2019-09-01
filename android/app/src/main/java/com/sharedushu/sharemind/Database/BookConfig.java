package com.sharedushu.sharemind.Database;

import android.content.Context;
import android.content.SharedPreferences;

import com.sharedushu.sharemind.BookManage.BookDefine;
import com.sharedushu.sharemind.Tools.BookTextUtils;

import java.security.SecureRandom;

import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;

/**
 * Created by flj on 2016/11/26.
 */
public class BookConfig {
    private SharedPreferences m_pre;
    SharedPreferences.Editor m_editor;

    //cipher
    private String KEY_MODE="AES";
    private String KEY="BkpreKy?f0g2<126";

    public BookConfig(Context context)
    {
        m_pre=context.getSharedPreferences(BookDefine.GLOBAL_CONFIG_FILE,Context.MODE_PRIVATE);
        m_editor=m_pre.edit();
    }
    public String getUser()
    {
        return m_pre.getString(BookDefine.GLOBAL_CONFIG_USER,"");
    }
    public String getPasswd()
    {
        byte[] bytes= BookTextUtils.hexStringToBytes(m_pre.getString(BookDefine.GLOBAL_CONFIG_PASSWD,""));
        if(bytes==null||bytes.length==0)
        {
            return "";
        }

        return code(bytes,Cipher.DECRYPT_MODE);
    }
    public void setUser(String user)
    {
        m_editor.putString(BookDefine.GLOBAL_CONFIG_USER,user);
    }
    public void setPasswd(String passwd)
    {
        m_editor.putString(BookDefine.GLOBAL_CONFIG_PASSWD,code(passwd.getBytes(),Cipher.ENCRYPT_MODE));
    }
    public void save()
    {
        m_editor.commit();
    }
    private String code(byte[] bytes,int mode)
    {
        byte[] decodebyte=null;
        try{
            Cipher cipher;

            SecureRandom secureRandom=new SecureRandom();
            SecretKeySpec keySpec=new SecretKeySpec(KEY.getBytes(),KEY_MODE);

            cipher= Cipher.getInstance(KEY_MODE);
            cipher.init(mode, keySpec,secureRandom);
            decodebyte=cipher.doFinal(bytes);
        }catch(Exception e){
            e.printStackTrace();
        }

        if(decodebyte==null)
        {
            return "";
        }
        else {
            if(mode==Cipher.DECRYPT_MODE)
            {
                return new String(decodebyte);
            }
            else if(mode==Cipher.ENCRYPT_MODE)
            {
                return BookTextUtils.bytesToHexString(decodebyte);
            }
            return "";
        }
    }
}
