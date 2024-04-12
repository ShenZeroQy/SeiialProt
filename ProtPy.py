import ctypes
import os
import binascii
# 加载共享库

# current_dir = os.getcwd()
# print("Current working directory:", current_dir)


# 调用C函数




class CPySerialProtocl():
    def __init__(self) -> None:
        self.send_frame_ptr = ctypes.create_string_buffer(256) 
        self.sendlen=0
        self.mylib = ctypes.CDLL('./CpyProt/CpyProt/Prot.cpython-36m-x86_64-linux-gnu.so')
        pass
    def Serial_FrameData(self,t,P,A):
        # 调用 data_frame 函数
        time = ctypes.c_ulong(t)
        PosX = ctypes.c_int(P)
        AngX = ctypes.c_double(A)
        # ctypes.c_wchar_p

        self.sendlen = self.mylib.data_frame(time, PosX, AngX, self.send_frame_ptr)
        # 访问元素
        return self.send_frame_ptr, self.sendlen

    def Serial_Read(self,Rec_cache):
        # 调用 Read_data_frame 函数
        time = ctypes.c_ulong()
        PosX = ctypes.c_int()
        AngX = ctypes.c_double()

        result = self.mylib.Read_data_frame(ctypes.byref(time), ctypes.byref(PosX), ctypes.byref(AngX),Rec_cache)

        # 打印结果
        print(f"Result : {result}")
        print(f"Ftime: {time.value}")
        print(f"PosX: {PosX.value}")
        print(f"AngX: {AngX.value}")

    def Get_timestamp(self):
        # 声明getMilliseconds_64函数的返回类型
        self.mylib.getMilliseconds_64.restype = ctypes.c_ulong

        # 调用getMilliseconds_64函数
        milliseconds = self.mylib.getMilliseconds_64()
        return milliseconds
    def hexFrame(self):       
        hex_string = binascii.hexlify(self.send_frame_ptr[0:self.sendlen])
        return(hex_string)
# if __name__ == '__main__':
def Test_Prot():
    print('Testing Port module using:12349,-42,3.141590006')
    
    P=CPySerialProtocl()
    F,F_len=P.Serial_FrameData(12349,-42,3.141590006)

    hex_string = binascii.hexlify(F[0:F_len])
    print('Encoded to:',end='')
    print(hex_string)

    P.Serial_Read(F)

    MS=P.Get_timestamp()
    print('Ms timer:'+str(MS))

    #     # 调用 data_frame 函数
    #     time = ctypes.c_ulong(12349)
    #     PosX = ctypes.c_int(-42)
    #     AngX = ctypes.c_double(3.14159)
    #     send_frame_ptr = ctypes.create_string_buffer(256) 
    #     # ctypes.c_wchar_p

    #     result = self.mylib.data_frame(time, PosX, AngX, send_frame_ptr)
    #     hex_string = binascii.hexlify(send_frame_ptr[0:result])
    #     # 访问元素

    #     print(hex_string)



    # # for i in range(result):
    # #     print(send_frame_ptr[i])

    # # 调用 local_send 函数
    # # self.mylib.local_send()



    # # 调用 Read_data_frame 函数
    # time = ctypes.c_ulong()
    # PosX = ctypes.c_int()
    # AngX = ctypes.c_double()

    # result = self.mylib.Read_data_frame(ctypes.byref(time), ctypes.byref(PosX), ctypes.byref(AngX),send_frame_ptr)

    # # 打印结果
    # print(f"Result from Read_data_frame: {result}")
    # print(f"time: {time.value}")
    # print(f"PosX: {PosX.value}")
    # print(f"AngX: {AngX.value}")


    # print('calling saerial read')
    # Serial_Read(F)
    # return

# # 调用 encode_frame 函数
# data = b"Hello, World!"
# data_len = len(data)
# frame = ctypes.create_string_buffer(256)  # 假设缓冲区大小为256

# result = self.mylib.encode_frame(data, data_len, frame)



# # 调用 decode_frame 函数
# frame_data = ctypes.create_string_buffer(256)  # 假设缓冲区大小为256

# result = self.mylib.decode_frame(frame, frame_data)



# # 调用 local_send 函数
# self.mylib.local_send()



# # 调用 data_frame 函数
# time = ctypes.c_ulong(123456789)
# PosX = ctypes.c_int(42)
# AngX = ctypes.c_double(3.14159)
# send_frame_ptr = ctypes.create_string_buffer(256)  # 假设缓冲区大小为256

# result = self.mylib.data_frame(time, PosX, AngX, send_frame_ptr)

# # 打印结果
# print(f"Result from data_frame: {result}")
# print(f"time: {time.value}")
# print(f"PosX: {PosX.value}")
# print(f"AngX: {AngX.value}")
# print(f"send_frame_ptr: {send_frame_ptr.value.decode('utf-8','ignore')}")

# # 调用 Read_data_frame 函数
# time = ctypes.c_ulong()
# PosX = ctypes.c_int()
# AngX = ctypes.c_double()


# # result = self.mylib.Read_data_frame(ctypes.byref(time), ctypes.byref(PosX), ctypes.byref(AngX))
# result = self.mylib.Read_data_frame(ctypes.byref(time), ctypes.byref(PosX), ctypes.byref(AngX))


# # 打印结果
# print(f"Result from Read_data_frame: {result}")
# print(f"time: {time.value}")
# print(f"PosX: {PosX.value}")
# print(f"AngX: {AngX.value}")





################################block


# # 调用 read_chunk 函数

# class DataPoint(ctypes.Structure):
#     _fields_ = [("TimeStart", ctypes.POINTER(ctypes.c_ulong)), 
#                 ("X", ctypes.c_double),
#                 ("Y", ctypes.c_double)]

# lib = ctypes.CDLL("/path/to/lib") 

# lib.read_chunk.argtypes = [ctypes.c_char_p, 
#                            ctypes.POINTER(ctypes.c_ulong),
#                            ctypes.POINTER(DataPoint),
#                            ctypes.c_int]

# data = (DataPoint * size)() # 创建结构体数组
# t = ctypes.c_ulong()

# lib.read_chunk(filename.encode(), 
#               ctypes.byref(t), 
#               data, 
#               size)

# print(t.value)
# print(data[0].X)