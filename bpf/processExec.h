#pragma once

#define TASK_COMM_LEN      16
#define MAX_FILENAME_LEN  256

struct exec_event {
    unsigned int   pid;
    unsigned int   ppid;
    unsigned int   uid;

    char comm[TASK_COMM_LEN];        // 프로세스 이름
    char filename[MAX_FILENAME_LEN]; // 실행 파일 전체 경로
    unsigned long long timestamp_ns; // 커널 부팅 이후 경과 시간(ns)
};
