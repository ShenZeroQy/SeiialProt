#include <stdio.h>
#include <stdlib.h>

// 定义每个块的大小
#define CHUNK_SIZE 100

// 定义数据结构来存储时间戳和相关数据
typedef struct {
    unsigned long* TimeStart;
    float X;
    float Y;
} DataPoint;

// 函数：将数据块写入文件
void write_chunk(const char* filename, unsigned long* TimeStart,DataPoint* data, int size) {
    FILE* file = fopen(filename, "w");
    fwrite(TimeStart, sizeof(unsigned long), 1, file);
    fwrite(data, sizeof(DataPoint), size, file);
    fclose(file);
    return;
}

// 函数：从文件读取数据块
int read_chunk(const char* filename, unsigned long* TimeStart,DataPoint* data, int size) {
    FILE* file = fopen(filename, "r");
    int ret=0;
    if(0!=fread(TimeStart,sizeof(unsigned long),1,file))
        ret=fread(data, sizeof(DataPoint), size, file);
    if(0!=ret)
        for (int i=0;i<size;i++)
        {
            data[i].TimeStart=TimeStart;
        }
    fclose(file);
    return ret;

}

int main() {
    FILE* file = fopen("data_file.bin", "wb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // 生成示例数据
    int num_data_points = 5000;
    unsigned long L=0;
    DataPoint* data_buffer = (DataPoint*)malloc(num_data_points * sizeof(DataPoint));
    for (int i = 0; i < num_data_points; i++) {
        data_buffer[i].TimeStart = &L;
        data_buffer[i].X = i * 3;
    }
    for (int i = 0; i < CHUNK_SIZE; i++) {
            printf("InitTimestamp: %lu, Data: %f\n", data_buffer[i].TimeStart[0]+i, data_buffer[i].X);
    }

    // 将数据分块写入文件
    int remaining_data = num_data_points;
    int offset = 0;
    while (remaining_data > 0) {
        int chunk_size = (remaining_data > CHUNK_SIZE) ? CHUNK_SIZE : remaining_data;
        write_chunk(file,data_buffer->TimeStart, data_buffer + offset, chunk_size);
        offset += chunk_size;
        remaining_data -= chunk_size;
    }

    fclose(file);

    // 从文件中读取数据块
    file = fopen("data_file.bin", "rb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // 读取数据块并处理
    DataPoint* read_buffer = (DataPoint*)malloc(CHUNK_SIZE * sizeof(DataPoint));
    unsigned long TS;
    while (1) {
        int read_size = read_chunk(file, &TS,read_buffer, CHUNK_SIZE);
        if (read_size == 0) {
            break; // 已经读取完文件
        }

        // 处理读取的数据块，例如打印或其他操作
        for (int i = 0; i < read_size; i++) {
            printf("Timestamp: %lu, Data: %f\n", read_buffer[i].TimeStart[0]+i, read_buffer[i].X);
        }
    }

    fclose(file);
    free(data_buffer);
    free(read_buffer);

    return 0;
}