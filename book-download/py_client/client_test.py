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

MSG_NUM_PER_SEC=1
SLEEP_SEC_PER_CIRCLE=1
CONNECT_COUNT=100
MSG_COUNT_PER_PRINT=1


class recieveThread (threading.Thread):   
    def __init__(self,sock_list ):
        threading.Thread.__init__(self)
        self.sock_list = sock_list
        self.sock_sz_list=[]
        for item in self.sock_list:
            self.sock_sz_list.append((item,0))
        
    def run(self):   
        
        while True:
            if(len(self.sock_list)==0):
                break
            r_list,w_list,x_list=select.select(self.sock_list,[],[])

            for r_list_item in r_list:
                read_sz=0
                i=0
                data = r_list_item.recv(32*1024)
                
                for i in range(0,len(self.sock_sz_list)):
                    if r_list_item==self.sock_sz_list[i][0]:
                        read_sz=self.sock_sz_list[i][1]
                        break
                            
        
                if len(data) > 0:
                    #print('Recieve:'+str(len(data)));
                    self.sock_sz_list[i]=(r_list_item,read_sz+len(data))
                else:
                    self.sock_list.remove(r_list_item)
                    r_list_item.close()
                    if(read_sz!=7268295+4):
                        print('Fail')
                  
                  
            
    
if __name__ == '__main__':

    SERVER_ADDR = 'localhost'
    PORT = 60001

    header_tag_bytes=bytearray([CMD_INFO_HEADER0,CMD_INFO_HEADER1,CMD_INFO_HEADER2,CMD_INFO_HEADER3])
    header_tag_len=len(header_tag_bytes)
    
    while True:
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
                
                msg='hello.'
                msg_bytes=bytes(msg,encoding = "utf8")
                msg_len=len(msg_bytes)
                format_str='!'+str(header_tag_len)+'sH'+str(msg_len)+'s'
                    
                obj=struct.pack(format_str,header_tag_bytes,msg_len,msg_bytes)
                
                #sock.send(obj)


        print('Total Connected Sock:'+str(len(sock_list)))
    
        rv=recieveThread(sock_list)
        rv.start()
        time.sleep(SLEEP_SEC_PER_CIRCLE)
       
        
    input('')
    
