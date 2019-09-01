# share_mind
Online ebook reader, support sharing opinions, include server and android client

build
====
在线阅读软件，支持观点分享，适合自建数据库场景。程序分成服务端和客户端，客户端只支持android平台  

1.客户端  
直接采用android studio编译即可,源代码在根目录android下面   
修改BookDefine.java文件如下地方更改服务器指向    
  static public final String SERVER_ADDR="192.168.0.105";  
  static public final String SERVER_HTTP_PATH="http://192.168.0.105/";  
 
2.服务端  
服务端的一键编译脚本和安装脚本是基于ubuntu平台，建议采用ubuntu 16.04 64bit以上系统  

1)编译服务器  
执行./build_server.sh编译安装包，生成share_mind_0.0.1.tar.gz，这个文件直接用于部署  

2）配置数据库访问密码  
根目录cfg文件配置了数据库访问用户和密码，以及后台进程执行时所用的账户名称  
配置文件有3行  
第一行 是加密过后的数据库访问用户名  
第二行 是加密过后的数据库访问密码  
第三行 后台进程执行时所用的账户名称，一般不用改变  

如何得到加密过后的数据库访问账户？  
  执行./server/build/test，按提示输入用户名和密码，生成逗号分开的字符串，前半部分为用户名，后半部分为密码  
将生成的用户名和密码替换cfg文件的第一，第二行就是新的用户名和密码，默认是test 123456  

3）配置数据库访问密码之后重新执行./build_server.sh 生成部署包  

3.服务部署  
部署采用ubuntu + mysql  

1）.安装mysql  
2）.mysql倒入数据库，这个数据库里面有几本书和其注解  
数据库在部署包里面，解压share_mind_0.0.1.tar.gz,得到share_mind_0.0.1  
db/booksharemind1223.sql是数据库文件，数据库名称为BookShareMind  
3）.设置访问账号，并且和写入上一步cfg文件的账号保持一致  
4）.设置访问账号对BookShareMind有读写权限  
5）.执行share_mind_0.0.1 里面的 ./install 安装程序  
6）.ps aux|grep /opt/share_mind/bin 查看是否有两个进程  

部署成功后，可以用android客户端阅读，长按阅读界面任何位置，弹出此处的注解  

4.注意事项  
db/booksharemind1223.sql 包含两个默认账户，可以通过app修改文章注解。  
可以自行更改密码  

运行效果
===
![注解界面](https://github.com/flj512/share_mind/blob/master/1.jpg)


