#include "megacli.h"
#include <thread>
#include <mutex>
#include <fuse.h>
#include <condition_variable>
struct file_stat
{
    int mode;
    int size;
    int nlink;
    int error;
    bool dir;
    time_t mtime;
    file_stat() : error(0){};
};

struct file_cache_row
{
    enum CacheStatus{WAIT_D_TOPEN,DOWNLOADING,UPLOADING,AVAILABLE};
    std::string localname;

    CacheStatus status;
    int size;
    int available_bytes;
    int n_clients;
    time_t last_modified;
    int td;
    bool modified;
    file_cache_row(): td(-1),status(WAIT_D_TOPEN),modified(false),n_clients(0),available_bytes(0),size(0){};
};

class MegaFuseCallback : public DemoApp
{
    public:


};

class MegaFuse : public DemoApp
{
    std::map <std::string,file_cache_row> file_cache;

    public:
    private:
    static void event_loop(MegaFuse* megaFuse);
    std::thread event_loop_thread;
    std::mutex engine_mutex;
    std::mutex api_mutex;
    byte pwkey[SymmCipher::KEYLENGTH];
    std::pair<std::string,std::string> splitPath(std::string);
    Node* nodeByPath(std::string path);
    Node* childNodeByName(Node *p,std::string name);
    int open(std::string);
    public:
    bool start();
    bool stop();
    bool login(std::string username, std::string password);
    bool download(std::string,std::string dst);
    bool upload(std::string,std::string dst);
    int unlink(std::string);
    std::vector<std::string> ls(std::string path);
    std::map <std::string,file_cache_row>::iterator findCacheByTransfer(int, file_cache_row::CacheStatus);
    void check_cache();
    /*callbacks*/

    error last_error;
    int result;
    void login_result(error e);
    void transfer_failed(int, string&, error);
    void transfer_complete(int, chunkmac_map*, const char*);
    void transfer_failed(int,  error);
    void transfer_complete(int td, handle uploadhandle, const byte* uploadtoken, const byte* filekey, SymmCipher* key);
    void topen_result(int td, error e);
    void topen_result(int td, string* filename, const char* fa, int pfa);
    void unlink_result(handle, error);
    void putnodes_result(error, targettype, NewNode*);
    void transfer_update(int, m_off_t, m_off_t, dstime);


    /*inter thread*/
    std::mutex cvm;
    std::condition_variable cv;
    int login_ret;
    int opend_ret;
    int putnodes_ret;
    int unlink_ret;

    /*fuse*/
    int open(const char *path, struct fuse_file_info *fi);
    int readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi);
    int getAttr(const char *path, struct stat *stbuf);
    int release(const char *path, struct fuse_file_info *fi);
    int mkdir(const char * path, mode_t mode);
    int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
    int create(const char *path, mode_t mode, struct fuse_file_info * fi);
    int write(const char * path, const char *buf, size_t size, off_t offset, struct fuse_file_info * fi);


};
