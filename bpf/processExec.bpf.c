#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_core_read.h>
#include "processExec.h"

char LICENSE[] SEC("license") = "GPL";

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);   // 256KB — 이벤트 폭주 시 여유. 부족하면 이벤트 드롭
} events SEC(".maps");


SEC("tp/sched/sched_process_exec")

int handle_exec(struct trace_event_raw_sched_process_exec *ctx) {
    struct exec_event *e;
    struct task_struct *task;
    unsigned int fname_off;

    e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
    if (!e) {
        return 0;
    }

    task = (struct task_struct *)bpf_get_current_task();
    e->pid  = bpf_get_current_pid_tgid() >> 32;       // 상위 32비트 = tgid
    e->ppid = BPF_CORE_READ(task, real_parent, tgid);
    e->uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;  // 하위 32비트 = uid
    e->timestamp_ns = bpf_ktime_get_ns();

    bpf_get_current_comm(&e->comm, sizeof(e->comm));

    fname_off = ctx->__data_loc_filename & 0xFFFF;
    bpf_probe_read_str(&e->filename, sizeof(e->filename), (void *)ctx + fname_off);

    bpf_ringbuf_submit(e, 0);   // reserve한 슬롯을 커밋 → 유저 공간에서 즉시 읽을 수 있게 됨
    
    return 0;
}