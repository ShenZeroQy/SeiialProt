cdef extern from "Protocl.c":
    int encode_frame(unsigned char* data, int data_len,unsigned char* frame) 
    int decode_frame(unsigned char* frame, unsigned char* data) 
    void local_send()
    int data_frame(unsigned long time, int PosX,double AngX,unsigned char* send_frame_ptr)
    int Read_data_frame(unsigned long* time,int* PosX,double* AngX,unsigned char* rec_frame_ptr)

    unsigned long getMilliseconds_64()


