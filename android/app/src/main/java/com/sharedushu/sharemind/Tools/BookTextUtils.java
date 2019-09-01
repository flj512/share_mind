package com.sharedushu.sharemind.Tools;

/**
 * Created by flj on 2016/11/18.
 */
public class BookTextUtils {
    public static final byte[] gbk_space={(byte)0xa1,(byte)0xa1};
    public static String space;
    static {
        try {
            space = new String(new String(gbk_space, "GBK").getBytes());
        } catch (Exception e) {
        }
    }

    public static final String[] Punctuation={  "!",  ";",  ":",  "\'",      "\"",       ",",  ".",  "?"," ","\t",//英文标点,空格
                                                    "！",  "；", "：", "‘","’","“", "”", "，", "。", "？",space};                                   //中文标点,空格
    public static String skipStartPunctuation(String input)//去除开头的标点符号
    {
        boolean match;
        while(input.length()>0)
        {
            match=false;
            for(String str:Punctuation)
            {
                if(input.startsWith(str))
                {
                    input=input.substring(str.length());
                    match=true;
                    break;
                }
            }
            if(!match)
            {
                break;
            }
        }

        return input;
    }
    public static String skipEndPunctuation(String input)//去除尾部的标点符号
    {
        boolean match;
        while(input.length()>0)
        {
            match=false;
            for(String str:Punctuation)
            {
                if(input.endsWith(str))
                {
                    input=input.substring(0,input.length()-str.length());
                    match=true;
                    break;
                }
            }
            if(!match)
            {
                break;
            }
        }

        return input;
    }

    static public String bytesToHexString(byte[] bytes)
    {
        String map="0123456789abcdefg";
        StringBuilder builder=new StringBuilder();
        for(byte bt:bytes)
        {
            builder.append(map.charAt((bt>>4)&0x0f));
            builder.append(map.charAt(bt&0x0f));
        }
        return builder.toString();
    }
    private static int charToInt(char c)
    {
        if(c>='0'&&c<='9')
        {
            return (c-'0');
        }
        else if(c>='a'&&c<='f')
        {
            return (c-'a'+10);
        }
        else if(c>='A'&&c<='F')
        {
            return (c-'A'+10);
        }
        return -1;
    }
    static public byte[] hexStringToBytes(String string)
    {
        byte[] bytes=new byte[string.length()/2];
        int v;
        for (int i=0;i<bytes.length;i++) {
            v=charToInt(string.charAt(2*i));
            if(v<0)
            {
                return null;
            }
            bytes[i]=(byte) ((v<<4)&0xf0);
            v=charToInt(string.charAt(2*i+1));
            if(v<0)
            {
                return null;
            }
            bytes[i]+=(byte) (v&0x0f);
        }
        return bytes;
    }
}
