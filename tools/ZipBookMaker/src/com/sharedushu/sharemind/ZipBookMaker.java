package com.sharedushu.sharemind;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.zip.CRC32;
import java.util.zip.CheckedOutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import javax.crypto.Cipher;
import javax.crypto.CipherOutputStream;
import javax.crypto.spec.SecretKeySpec;

public class ZipBookMaker {
	private static final String INDEX="index"; 
	private String m_path;
	private String m_input_code;
	private String m_output_code;
	private String m_output_name;
	private ZipBookMakerListener m_Listener;
	//cipher
	private String KEY_MODE="AES";
	private String CIPHER_MODE="AES/ECB/PKCS5Padding";
	private String KEY="BkKy1302?Dhs=271";
	
	
	public ZipBookMaker(String path,String outFileName,String inCode,String outCode)
	{
		m_path=path;
		m_input_code=inCode;
		m_output_code=outCode;
		m_output_name=outFileName;
	}
	
	static public class Catalog{
		public String m_path;
		public String m_name;
		public int m_index;
		public int m_size;
		
		public String toString() {
			return m_name+" "+m_index+" "+m_size;
		}
	}
	private Cipher getCipher(int mode)
	{
		Cipher cipher=null;
		try{
			 SecureRandom secureRandom=new SecureRandom();
	            SecretKeySpec keySpec=new SecretKeySpec(KEY.getBytes(),KEY_MODE);

	            cipher= Cipher.getInstance(CIPHER_MODE);
	            cipher.init(mode, keySpec,secureRandom);
		}catch(Exception e){
			e.printStackTrace();
		}
		return cipher;
	}
	
	private boolean createIndexAndFile(List<Catalog> list) {	
		BufferedReader reader=null;
		BufferedWriter writer=null;
		String last_line;
		String line;
		int size;
		boolean has_error;
		
		
		for(Catalog catalog:list)
		{
			size=0;
			has_error=false;
			reader=null;
			writer=null;
			last_line=null;
			
			try{
				FileInputStream inputStream=new FileInputStream(new File(m_path+File.separator+catalog.m_path));
				reader=new BufferedReader(new InputStreamReader(inputStream,m_input_code));
				FileOutputStream outputStream=new FileOutputStream(new File(m_path+File.separator+catalog.m_index));
		
				writer=new BufferedWriter(new OutputStreamWriter(outputStream,m_output_code));
				byte[] gbk_space={(byte)0xa1,(byte)0xa1};
				String chn_indent=new String(gbk_space,"GBK");
				
				while((line=reader.readLine())!=null)
				{
					if(line.length()>0)
					{
						line=line.trim();
						line=line.replaceFirst("^"+chn_indent+"+|\t+", "");
						if(line.length()>0)
						{
							line=chn_indent+chn_indent+line;
						}
					}
					if(!(last_line!=null&&last_line.length()==0&&line.length()==0))
					{
						writer.write(line);
						writer.write("\r\n");
						size+=line.length();
						last_line=line;
					}
				}
				catalog.m_size=size;
			}catch (Exception e) {
				e.printStackTrace();
				has_error=true;
			}finally{
				if(reader!=null)
				{
					try{
					reader.close();
					}catch (Exception e) {
						e.printStackTrace();
					}
				}
				if(writer!=null)
				{
					try{
						writer.close();
					}catch (Exception e) {
						e.printStackTrace();
					}
				}
			}
			if(has_error)
			{
				return false;
			}
		}
		
		
		try{
			has_error=false;
			writer=null;
			File file=new File(m_path+File.separator+INDEX);
			FileOutputStream outputStream=new FileOutputStream(file);
			writer=new BufferedWriter(new OutputStreamWriter(outputStream,m_output_code));
			for(Catalog catalog:list)
			{
				writer.write(catalog.toString()+"\r\n");
			}
		}catch (Exception e) {
			has_error=true;
			e.printStackTrace();
		}finally{
			if(writer!=null)
			{
				try {
					writer.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
		
		return !has_error;
	}
	private boolean zipFiles(List<Catalog> list)
	{
		boolean hasError=false;
		String[] srcFileName=new String[list.size()+1];
		for(int i=0;i<list.size();i++)
		{
			srcFileName[i]=new Integer(list.get(i).m_index).toString();
		}
		srcFileName[srcFileName.length-1]=INDEX;
		
		BufferedInputStream inputStream=null;
		BufferedOutputStream outputStream=null;
		
		//
		Cipher cipher=getCipher(Cipher.ENCRYPT_MODE);
		if(cipher==null)
		{
			return false;
		}
		
		try {
			int read_byte;
			File outFile=new File(m_path+File.separator+m_output_name);
			
			CipherOutputStream cipherOutputStream=new CipherOutputStream(new FileOutputStream(outFile), cipher);
			ZipOutputStream zipOutputStream=new ZipOutputStream(new CheckedOutputStream(cipherOutputStream,new CRC32()));
			outputStream=new BufferedOutputStream(zipOutputStream);
			
			for(String path:srcFileName)
			{
				File inFile=new File(m_path+File.separator+path);
				zipOutputStream.putNextEntry(new ZipEntry(path));
				inputStream=new BufferedInputStream(new FileInputStream(inFile));
				
				while((read_byte=inputStream.read())!=-1)
				{
					outputStream.write(read_byte);
				}
				outputStream.flush();
				inputStream.close();
				inputStream=null;
				inFile.delete();
				
			}
		} catch (Exception e) {
			hasError=true;
			e.printStackTrace();
		}
		finally {
			if(inputStream!=null)
			{
				try {
					inputStream.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
				
			}
			if(outputStream!=null)
			{
				try {
					outputStream.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
		return !hasError;
	}
	private boolean build(List<Catalog> list) {
		if(!createIndexAndFile(list))
		{
			return false;
		}
		
		return zipFiles(list);
	}
	public void makeStart()
	{
		File file=new File(m_path);
		if(!file.exists()||!file.isDirectory())
		{
			m_Listener.onComplete(false,!file.exists()?"Directory Not exist":"Input is not a directory");
			return;
		}
		File output=new File(m_path+File.separator+m_output_name);
		if(output.exists())
		{
			output.delete();
		}
		
		String[] dirs=file.list();
		ArrayList<Catalog> list=new ArrayList<Catalog>();
		if(dirs.length==0)
		{
			m_Listener.onComplete(false,"No Files");
		}
		Catalog catalog;
		for(String string:dirs)
		{
			catalog=new Catalog();
			catalog.m_path=string;
			String[] part=string.split("-+|\\.");
			if(part.length<2)
			{
				m_Listener.onComplete(false, string+":bad file name");
				return;
			}
			catalog.m_name=part[1];
			try{
				catalog.m_index=Integer.parseInt(part[0]);
			}catch (NumberFormatException e) {
				m_Listener.onComplete(false, string+":bad file name");
				return;
			}
			list.add(catalog);
		}
		list.sort(new Comparator<Catalog>() {

			@Override
			public int compare(Catalog o1, Catalog o2) {
				return o1.m_index-o2.m_index;
			}
			
		});
		
		String[] buildList= new String[list.size()];
		for(int i=0;i<list.size();i++)
		{
			buildList[i]=(i+1)+":"+list.get(i).m_name;
		}
		
		if(!m_Listener.onBuildList(buildList))
		{
			m_Listener.onComplete(false,"User Cancel");
			return;
		}
		
		if(!build(list))
		{
			m_Listener.onComplete(false, "IO Error");
		}
		else
		{
			m_Listener.onComplete(true, "Successfull");
		}
		
	}
	
	void setListener(ZipBookMakerListener listener)
	{
		m_Listener=listener;
	}
}
