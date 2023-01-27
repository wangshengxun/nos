/**
 * Copyright (C) 2023-2023 胡启航<Nick Hu>
 *
 * Author: 胡启航<Nick Hu>
 *
 * Email: huqihan@live.com
 */

#include <kernel/kernel.h>
#include <kernel/printk.h>
#include <kernel/task.h>
#include <kernel/init.h>
#include <kernel/sleep.h>
#include <kernel/sem.h>
#include <kernel/mutex.h>
#include <kernel/msg_queue.h>

sem_t g_sem;
struct mutex g_lock;
struct msg_queue msg_q;

static void test1_task_entry(void* parameter)
{
    u32 usage;
    u32 all_usage;

    while (1) {
        msleep(500);
        mutex_lock(&g_lock);
        usage = task_get_cpu_usage(current) / 100;
        all_usage = get_cpu_usage() / 100;
        pr_info("%s: usage:%u.%02u%%, total:%u.%02u%%\r\n", current->name,
                usage / 100, usage % 100, all_usage / 100, all_usage % 100);
        mutex_unlock(&g_lock);
    }
}

static int test1_task_init(void)
{
    struct task_struct *test1;

    test1 = task_create("test1", test1_task_entry, NULL, 5, 10, NULL);
    if (test1 == NULL) {
        pr_fatal("creat test1 task err\r\n");
        BUG_ON(true);
        return -EINVAL;
    }
    task_ready(test1);

    return 0;
}

static void test2_task_entry(void* parameter)
{
    u32 usage;
    u32 all_usage;
    char buf[128];
    int size;

    while (1) {
        usage = task_get_cpu_usage(current) / 100;
        all_usage = get_cpu_usage() / 100;
        pr_info("%s: usage:%u.%02u%%, total:%u.%02u%%\r\n", current->name,
                usage / 100, usage % 100, all_usage / 100, all_usage % 100);
        size = msg_q_recv(&msg_q, buf, sizeof(buf));
        if (size > 0) {
            buf[size] = 0;
            pr_info("%s: recv %s\r\n", current->name, buf);
        }
    }
}

static int test2_task_init(void)
{
    struct task_struct *test2;

    test2 = task_create("test2", test2_task_entry, NULL, 5, 10, NULL);
    if (test2 == NULL) {
        pr_fatal("creat test2 task err\r\n");
        BUG_ON(true);
        return -EINVAL;
    }
    task_ready(test2);

    return 0;
}

static void test3_task_entry(void* parameter)
{
    u32 usage;
    u32 all_usage;

    while (1) {
        sem_get(&g_sem);
        usage = task_get_cpu_usage(current) / 100;
        all_usage = get_cpu_usage() / 100;
        pr_info("%s: usage:%u.%02u%%, total:%u.%02u%%\r\n", current->name,
                usage / 100, usage % 100, all_usage / 100, all_usage % 100);
    }
}

static int test3_task_init(void)
{
    struct task_struct *test3;

    test3 = task_create("test3", test3_task_entry, NULL, 15, 10, NULL);
    if (test3 == NULL) {
        pr_fatal("creat test3 task err\r\n");
        BUG_ON(true);
        return -EINVAL;
    }
    task_ready(test3);

    return 0;
}

static void test4_task_entry(void* parameter)
{
    u32 usage;
    u32 all_usage;

    while (1) {
        sleep(3);
        usage = task_get_cpu_usage(current) / 100;
        all_usage = get_cpu_usage() / 100;
        pr_info("%s: usage:%u.%02u%%, total:%u.%02u%%\r\n", current->name,
                usage / 100, usage % 100, all_usage / 100, all_usage % 100);
    }
}

static int test4_task_init(void)
{
    struct task_struct *test4;

    test4 = task_create("test4", test4_task_entry, NULL, 25, 10, NULL);
    if (test4 == NULL) {
        pr_fatal("creat test4 task err\r\n");
        BUG_ON(true);
        return -EINVAL;
    }
    task_ready(test4);

    return 0;
}

static void test5_task_entry(void* parameter)
{
    u32 usage;
    u32 all_usage;
    char buf[] = "lalala";

    mutex_init(&g_lock);
    sem_init(&g_sem, 0);
    msg_q_init(&msg_q);

    test4_task_init();
    test3_task_init();
    test2_task_init();
    test1_task_init();

    while (1) {
        sleep(1);
        mutex_lock(&g_lock);
        usage = task_get_cpu_usage(current) / 100;
        all_usage = get_cpu_usage() / 100;
        pr_info("%s: usage:%u.%02u%%, total:%u.%02u%%\r\n", current->name,
                usage / 100, usage % 100, all_usage / 100, all_usage % 100);
        msg_q_send(&msg_q, buf, sizeof(buf));
        sleep(1);
        sem_send_one(&g_sem);
        mutex_unlock(&g_lock);
    }
}

static int test5_task_init(void)
{
    struct task_struct *test5;

    test5 = task_create("test5", test5_task_entry, NULL, 68, 10, NULL);
    if (test5 == NULL) {
        pr_fatal("creat test5 task err\r\n");
        BUG_ON(true);
        return -EINVAL;
    }
    task_ready(test5);

    return 0;
}
task_init(test5_task_init);