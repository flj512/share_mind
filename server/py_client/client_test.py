import socket
import select
import threading
import time
import struct
import datetime
import json

CMD_INFO_HEADER0=0x5a
CMD_INFO_HEADER1=0xb7
CMD_INFO_HEADER2=0xd0
CMD_INFO_HEADER3=0x2e

MSG_NUM_PER_SEC=2
SLEEP_SEC_PER_CIRCLE=5
CONNECT_COUNT=512
MSG_COUNT_PER_PRINT=1000


class recieveThread (threading.Thread):   
    def __init__(self,sock_list ):
        threading.Thread.__init__(self)
        self.sock_list = sock_list
        
    def run(self):   
        count=0
        
        while True:
            if(len(self.sock_list)==0):
                break
            r_list,w_list,x_list=select.select(self.sock_list,[],[])

            for r_list_item in r_list:
                data = r_list_item.recv(32*1024)
        
                if len(data) > 0:
                    
                    if(count%MSG_COUNT_PER_PRINT==0):
                        print('[%d] Recieved From %d:%s'%(count,r_list_item.fileno(),str(data,encoding = "utf8")))
                        #print(str(data,encoding = "utf8"))
                    
                    count=count+1
                    
                else:
                    self.sock_list.remove(r_list_item)
                    print('remote close')
                    break
            
           

class sendThread (threading.Thread):   
    def __init__(self,sock_list ):
        threading.Thread.__init__(self)
        self.sock_list = sock_list
        
    def run(self):   
        count =0
        header_tag_bytes=bytearray([CMD_INFO_HEADER0,CMD_INFO_HEADER1,CMD_INFO_HEADER2,CMD_INFO_HEADER3])
        header_tag_len=len(header_tag_bytes)
       
        while True:

            r_list,w_list,x_list=select.select([],self.sock_list,[])
            
            for sock_item in w_list:

                try:
                    for count in range(0,MSG_NUM_PER_SEC):
                    
                        msg={'CMD':'GetBookList','BookListKey':""}
                        #msg={'CMD':'GetOpinionList','ID':1,'start':1,'end':10}
                        #msg={'CMD':'SetOpionion','ID':1,'start':55,'opinion':'徼（jiao）：边际、边界。'}
                        
                        msg_json_str=json.dumps(msg)
                        msg_bytes=bytes(msg_json_str,encoding = "utf8")
                        msg_len=len(msg_bytes)
                        format_str='!'+str(header_tag_len)+'sH'+str(msg_len)+'s'
                    
                        obj=struct.pack(format_str,header_tag_bytes,msg_len,msg_bytes)
            
                        sock_item.send(obj)
                        
                except Exception as e:
                    print('Error:'+str(e))
                    

            time.sleep(SLEEP_SEC_PER_CIRCLE)
        
      
    
if __name__ == '__main__':

    SERVER_ADDR = '192.168.56.101'
    PORT = 60000

    sock_list=[]
    

    for index in range(0,CONNECT_COUNT):
        sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        try:
            sock.connect((SERVER_ADDR,PORT))
        except Exception as a:
            print('Error:'+str(a))
        else:
            sock_list.append(sock)
            sock.setblocking(0)


    print('Total Connected Sock:'+str(len(sock_list)))
    
    rv=recieveThread(sock_list)
    sd=sendThread(sock_list)
    rv.start()
    sd.start()

    rv.join()
    sd.join()
        
    input('')
    
