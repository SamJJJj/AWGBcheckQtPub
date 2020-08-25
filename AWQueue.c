#include "AWQueue.h"

#define BS_MAJOR_VERSION  1
#define BS_MINOR_VERSION  4
#define BS_PATCH_VERSION  0

#define BS_STATUS_OK             0
#define BS_STATUS_FAIL          -1
#define BS_STATUS_EXPECTED_CRLF -2
#define BS_STATUS_JOB_TOO_BIG   -3
#define BS_STATUS_DRAINING      -4
#define BS_STATUS_TIMED_OUT     -5
#define BS_STATUS_NOT_FOUND     -6
#define BS_STATUS_DEADLINE_SOON -7
#define BS_STATUS_BURIED        -8
#define BS_STATUS_NOT_IGNORED   -9

typedef struct bs_message {
    char *data;
    char *status;
    size_t size;
} BSM;

typedef struct bs_job {
    int64_t id;
    char *data;
    size_t size;
} BSJ;


// optional polling call, returns 1 if the socket is ready of the rw operation specified.
// rw: 1 => read, 2 => write, 3 => read/write
// fd: file descriptor of the socket
typedef int (*bs_poll_function)(int rw, int fd);

/*  Handle DSO symbol visibility - Stolen from zmq.h */
#if defined _WIN32
#   if defined DLL_EXPORT
#       define BSC_EXPORT __declspec(dllexport)
#   else
#       define BSC_EXPORT __declspec(dllimport)
#   endif
#else
#   if defined __SUNPRO_C  || defined __SUNPRO_CC
#       define BSC_EXPORT __global
#   elif (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define BSC_EXPORT __attribute__ ((visibility("default")))
#   else
#       define BSC_EXPORT
#   endif
#endif

// export version
//BSC_EXPORT void bs_version(int *major, int *minor, int *patch);

//// polling setup
//BSC_EXPORT void bs_start_polling(bs_poll_function f);
//BSC_EXPORT void bs_reset_polling(void);

//// returns a descriptive text of the error code.
//BSC_EXPORT const char* bs_status_text(int code);

//BSC_EXPORT void bs_free_message(BSM* m);
//BSC_EXPORT void bs_free_job(BSJ *job);

//// returns socket descriptor or BS_STATUS_FAIL
//BSC_EXPORT int bs_connect(const char *host, int port);
//BSC_EXPORT int bs_connect_with_timeout(const char *host, int port, float secs);

//// returns job id or one of the negative failure codes.
//BSC_EXPORT int64_t bs_put(int fd, uint32_t priority, uint32_t delay, uint32_t ttr, const char *data, size_t bytes);

//// rest return BS_STATUS_OK or one of the failure codes.
//BSC_EXPORT int bs_disconnect(int fd);
//BSC_EXPORT int bs_use(int fd, const char *tube);
//BSC_EXPORT int bs_watch(int fd, const char *tube);
//BSC_EXPORT int bs_ignore(int fd, const char *tube);
//BSC_EXPORT int bs_delete(int fd, int64_t job);
//BSC_EXPORT int bs_reserve(int fd, BSJ **job);
//BSC_EXPORT int bs_reserve_with_timeout(int fd, uint32_t ttl, BSJ **job);
//BSC_EXPORT int bs_release(int fd, int64_t id, uint32_t priority, uint32_t delay);
//BSC_EXPORT int bs_bury(int fd, int64_t id, uint32_t priority);
//BSC_EXPORT int bs_touch(int fd, int64_t id);
//BSC_EXPORT int bs_peek(int fd, int64_t id, BSJ **job);
//BSC_EXPORT int bs_peek_ready(int fd, BSJ **job);
//BSC_EXPORT int bs_peek_delayed(int fd, BSJ **job);
//BSC_EXPORT int bs_peek_buried(int fd, BSJ **job);
//BSC_EXPORT int bs_kick(int fd, int bound);
//BSC_EXPORT int bs_list_tube_used(int fd, char **tube);
//BSC_EXPORT int bs_list_tubes(int fd, char **yaml);
//BSC_EXPORT int bs_list_tubes_watched(int fd, char **yaml);
//BSC_EXPORT int bs_stats(int fd, char **yaml);
//BSC_EXPORT int bs_stats_job(int fd, int64_t id, char **yaml);
//BSC_EXPORT int bs_stats_tube(int fd, const char *tube, char **yaml);

void bs_version(int *major, int *minor, int *patch);

// polling setup
void bs_start_polling(bs_poll_function f);
void bs_reset_polling(void);

// returns a descriptive text of the error code.
const char* bs_status_text(int code);

void bs_free_message(BSM* m);
void bs_free_job(BSJ *job);

// returns socket descriptor or BS_STATUS_FAIL
int bs_connect(const char *host, int port);
int bs_connect_with_timeout(const char *host, int port, float secs);

// returns job id or one of the negative failure codes.
int64_t bs_put(int fd, uint32_t priority, uint32_t delay, uint32_t ttr, const char *data, size_t bytes);

// rest return BS_STATUS_OK or one of the failure codes.
int bs_disconnect(int fd);
int bs_use(int fd, const char *tube);
int bs_watch(int fd, const char *tube);
int bs_ignore(int fd, const char *tube);
int bs_delete(int fd, int64_t job);
int bs_reserve(int fd, BSJ **job);
int bs_reserve_with_timeout(int fd, uint32_t ttl, BSJ **job);
int bs_release(int fd, int64_t id, uint32_t priority, uint32_t delay);
int bs_bury(int fd, int64_t id, uint32_t priority);
int bs_touch(int fd, int64_t id);
int bs_peek(int fd, int64_t id, BSJ **job);
int bs_peek_ready(int fd, BSJ **job);
int bs_peek_delayed(int fd, BSJ **job);
int bs_peek_buried(int fd, BSJ **job);
int bs_kick(int fd, int bound);
int bs_list_tube_used(int fd, char **tube);
int bs_list_tubes(int fd, char **yaml);
int bs_list_tubes_watched(int fd, char **yaml);
int bs_stats(int fd, char **yaml);
int bs_stats_job(int fd, int64_t id, char **yaml);
int bs_stats_tube(int fd, const char *tube, char **yaml);

int AW_BSQueue_Init(int *QHandle,unsigned char *ConnectHost,unsigned int ConnectPort, unsigned char *WriteTube, unsigned char *ReadTube)
{
    unsigned char *Host = (unsigned char *)malloc(sizeof(unsigned char) * 15);
    unsigned int Port = 11300;

    if(ConnectHost == NULL)
    {
        memcpy(Host, "127.0.0.1", strlen("127.0.0.1"));
        Host[strlen("127.0.0.1")] = '\0';
    }
    else
    {
        memcpy(Host, ConnectHost, strlen(ConnectHost));
        Host[strlen(ConnectHost)] = '\0';
    }

    if(ConnectPort != 0) Port = ConnectPort;

    *QHandle = bs_connect(Host, Port);

    free(Host);

    if(*QHandle == BS_STATUS_FAIL)
    {
        bs_disconnect(*QHandle);
        return BS_STATUS_FAIL;
    }

	if (WriteTube != NULL)
	{
		if (bs_use(*QHandle, WriteTube) != BS_STATUS_OK)
		{
			bs_disconnect(*QHandle);
			return BS_STATUS_FAIL;
		}
	}

    if (ReadTube != NULL)
    {
        if (bs_watch(*QHandle, ReadTube) != BS_STATUS_OK)
        {
            bs_disconnect(*QHandle);
            return BS_STATUS_FAIL;
        }
    }

    bs_ignore(*QHandle, "default");
    
    return BS_STATUS_OK;
}

void AW_BSQueue_Destroy(int QHandle)
{
    bs_disconnect(QHandle);
}

int AW_BSQueue_PutBuffer(int QHandle, unsigned char *Buf, unsigned int Buf_len)
{
    printf("QHandle = %d\n", QHandle);
    printf("Buf = %s\n", Buf);
    printf("Buf_len = %d\n", Buf_len);
    if(bs_put(QHandle, 10, 0, 60, Buf, Buf_len) <= 0) return BS_STATUS_FAIL;
    
    return BS_STATUS_OK;
}

int AW_BSQueue_GetBuffer(int QHandle, unsigned char *Buf, unsigned int *Buf_len)
{
    BSJ *job = NULL;

   if(bs_reserve_with_timeout(QHandle, 0, &job) != BS_STATUS_OK) return BS_STATUS_FAIL;
    
    if(Buf == NULL)
    {
        *Buf_len = job->size;
        bs_release(QHandle, job->id, 0, 0);
        bs_free_job(job);
        return BS_STATUS_OK;
    }

    if(*Buf_len < job->size)
    {
        bs_release(QHandle, job->id, 0, 0);
        bs_free_job(job);
        return BS_STATUS_FAIL;
    }

    memcpy(Buf, job->data, job->size);
    *Buf_len = job->size;
//    sleep(1);
    if(bs_delete(QHandle, job->id) != BS_STATUS_OK)
    {
        bs_free_job(job);
        return BS_STATUS_FAIL;
    }
    bs_free_job(job);

    return BS_STATUS_OK;
}
