/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Pedro Maciel
/// @author Tiago Quintino
///
/// @date Apr 2015


#include "mir/caching/legendre/SharedMemoryLoader.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/sem.h>

#include "eckit/eckit.h"
#include "eckit/config/Resource.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/io/StdFile.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/TraceTimer.h"
#include "eckit/memory/Shmget.h"
#include "eckit/os/SemLocker.h"
#include "eckit/os/Stat.h"
#include "eckit/runtime/Main.h"

#include "mir/config/LibMir.h"
#include "mir/param/SimpleParametrisation.h"
#include "mir/util/Pretty.h"


namespace mir {
namespace caching {
namespace legendre {


namespace {

const int MAGIC  =  1234567890;
const size_t INFO_PATH = 1024;
struct info {
    int ready;
    int magic;
    char path[INFO_PATH];
};

}


class Unloader {

    std::vector<eckit::PathName> paths_;

public:

    /// This ensures unloader is destructed in correct order with other static objects (like eckit::Log)
    static Unloader& instance() {
        static Unloader unloader;
        return unloader;
    }

    void add(const eckit::PathName& path) {
        paths_.push_back(path);
    }

    ~Unloader() {
        for (std::vector<eckit::PathName>::const_iterator j = paths_.begin(); j != paths_.end(); ++j) {
            try {
                SharedMemoryLoader::unloadSharedMemory(*j);
            } catch (std::exception& e) {
                eckit::Log::error() << e.what() << std::endl;
            }
        }
    }
};


class GlobalSemaphore {
public:

    GlobalSemaphore(eckit::PathName path) : path_(path) {

        eckit::Log::debug<LibMir>() << "Semaphore for " << path << std::endl;

        key_t key = ::ftok(path.asString().c_str(), 1);
        if (key == key_t(-1)) {
            throw eckit::FailedSystemCall("ftok(" + path.asString() + ")");
        }
        SYSCALL(semaphore_ = ::semget(key, 1, IPC_CREAT | 0600));
    }

    eckit::PathName path_;
    int semaphore_;
};


SharedMemoryLoader::SharedMemoryLoader(const param::MIRParametrisation &parametrisation, const eckit::PathName& path) :
    LegendreLoader(parametrisation, path),
    address_(0),
    size_(path.size()),
    unload_(false) {

    log() << "Loading shared memory legendre coefficients from " << path << std::endl;

    std::string name;

    if (parametrisation.get("legendre-loader", name)) {
        unload_ = name.substr(0, 4) == "tmp-";
    }

    eckit::TraceTimer<LibMir> timer("Loading Legendre coefficients from shared memory");
    eckit::PathName real = path.realName();

    log() << "Loading Legendre coefficients from " << real << std::endl;

    if (real.asString().size() >= INFO_PATH - 1) {
        std::ostringstream os;
        os << "SharedMemoryLoader: path name to long " << real << ", maximum " << INFO_PATH;
        throw eckit::SeriousBug(os.str());
    }

    // Try to get an exclusing lock, we may be waiting for another process
    // to create the memory segment and load it with the file content
//    GlobalSemaphore gsem(real.dirName());
//    static const int max_wait_lock = eckit::Resource<int>("$MIR_SEMLOCK_RETRIES", 60);
//    eckit::SemLocker locker(gsem.semaphore_, gsem.path_, max_wait_lock);

    key_t key = ::ftok(real.asString().c_str(), 1);
    if (key == key_t(-1)) {
        throw eckit::FailedSystemCall("ftok(" + real.asString() + ")");
    }

    int page_size = ::getpagesize();
    ASSERT(page_size > 0);
    size_t shmsize = ((size_ + page_size - 1) / page_size) * page_size + sizeof(struct info) ;

#ifdef IPC_INFO
    // Only on Linux?
    struct shminfo shm_info;
    SYSCALL(shmctl(0, IPC_INFO, reinterpret_cast<shmid_ds*>(&shm_info)));
    log() << "Maximum shared memory segment size: " << eckit::Bytes((shm_info.shmmax >> 10) * 1024) << std::endl;
#endif
    // This may return EINVAL is the segment is too large 256MB
    // To find the maximum:
    // Linux:ipcs -l, Mac/bsd: ipcs -M

    log() << "SharedMemoryLoader: size is " << shmsize
          << " (" << eckit::Bytes(shmsize) << "), key=0x" << std::hex << key << std::dec
          << ", page size: " << eckit::Bytes(page_size)
          << ", pages: " << util::Pretty(shmsize / page_size)
          << std::endl;

    int shmid;
    if ((shmid = eckit::Shmget::shmget(key, shmsize , IPC_CREAT | 0600)) < 0) {
        std::ostringstream oss;
        oss << "Failed to aquire shared memory for " << eckit::Bytes(shmsize) << ", check the maximum authorised on this system (Linux ipcs -l, Mac/BSD ipcs -M)";
        throw eckit::FailedSystemCall(oss.str());
    }

    log() << "SHM LOAD " << eckit::Main::hostname() << " path " << real << " key " << key << " shmid " << shmid << std::endl;

    // Make sure memory is unloaded on exit
    if (unload_) {
        Unloader::instance().add(path);
    }

#ifdef SHM_PAGESIZE
    {

        // log() << "SharedMemoryLoader: attempting to use 64K pages"  << std::endl;

        /* Use 64K pages to back the shared memory region */
        size_t shm_size;
        struct shmid_ds shm_buf = { 0 };
        psize_t psize_64k;
        psize_64k = 64 * 1024;

        shm_buf.shm_pagesize = psize_64k;
        if (shmctl(shmid, SHM_PAGESIZE, &shm_buf)) {
            /*perror("shmctl(SHM_PAGESIZE) failed");*/
        }
    }

#endif

    /* attach shared memory */

    address_ = eckit::Shmget::shmat( shmid, NULL, 0 );
    if (address_ == (void*) - 1) {
        std::ostringstream oss;
        oss << "shmat(" << real << "), id=" << shmid << ", size=" << shmsize;
        throw eckit::FailedSystemCall(oss.str());
    }


    try {

        char *addr = reinterpret_cast<char*>(address_);

        info* nfo  = reinterpret_cast<info*>(addr + (((size_ + page_size - 1) / page_size) * page_size));

        // Check if the file has been loaded in memory
        bool loadfile = true;
        if (nfo->ready) {
            loadfile = false;
            if (nfo->magic != MAGIC) {
                std::ostringstream os;
                os << "SharedMemoryLoader: bad magic found " << nfo->magic;

                throw eckit::SeriousBug(os.str());
            }

            if (real.asString() != nfo->path) {
                std::ostringstream os;
                os << "SharedMemoryLoader: path mismatch " << real << " and " << nfo->path;

                throw eckit::SeriousBug(os.str());
            }
        }


        if (loadfile) {

            log() << "SharedMemoryLoader: loading " << path_ << std::endl;
            eckit::Timer("Loading " + path_ + " into shared memory ");

            eckit::AutoStdFile file(real);
            ASSERT(::fread(address_, 1, size_, file) == size_);

            // Set info record for checkes

            nfo->magic = MAGIC;
            strcpy(nfo->path, real.asString().c_str());
            nfo->ready = 1;

        } else {
            log() << "SharedMemoryLoader: " << path_ << " already loaded" << std::endl;
        }

        if (unload_) {
            Unloader::instance().add(path);
        }

    } catch (...) {
        eckit::Shmget::shmdt(address_);
        throw;
    }

    // eckit::StdPipe f("ipcs", "r");
    // char line[1024];

    // while(fgets(line, sizeof(line), f)) {
    //     log() << "LOAD IPCS " << line << std::endl;
    // }
}


SharedMemoryLoader::~SharedMemoryLoader() {
    if (address_) {
        SYSCALL(eckit::Shmget::shmdt(address_));
    }
    if (unload_) {
        unloadSharedMemory(path_);
    }
}

void SharedMemoryLoader::loadSharedMemory(const eckit::PathName& path) {
    param::SimpleParametrisation param;
    SharedMemoryLoader loader(param, path);
}

void SharedMemoryLoader::unloadSharedMemory(const eckit::PathName& path) {

    log() << "Unloading SharedMemory from " << path << std::endl;

    eckit::PathName real = path.realName();
    int shmid = 0;
    key_t key;

    key = ftok(real.asString().c_str(), 1);
    if (key == key_t(-1)) {
        throw eckit::FailedSystemCall("ftok(" + real.asString() + ")");
    }

    shmid = eckit::Shmget::shmget(key, 0, 0600);
    if (shmid < 0 && errno != ENOENT) {
        // throw eckit::FailedSystemCall("Cannot get shared memory for " + path);
        eckit::Log::warning() << "Cannot get shared memory for " << path << eckit::Log::syserr << std::endl;
        return;
    }

    if (shmid < 0 && errno == ENOENT) {
        log() << "SharedMemory from " << path  << " already unloaded" <<std::endl;
    } else {
        if (shmctl(shmid, IPC_RMID, 0) < 0) {
            eckit::Log::warning() << "Cannot delete memory for " << path << eckit::Log::syserr << std::endl;
        }

        log() << "Succefully unloaded SharedMemory from " << path  << std::endl;
    }

    // eckit::StdPipe f("ipcs", "r");
    // char line[1024];s
    // while(fgets(line, sizeof(line), f)) {
    //     log() << "UNLOAD IPCS " << line << std::endl;
    // }
}

void SharedMemoryLoader::print(std::ostream &out) const {
    out << "SharedMemoryLoader[path=" << path_ << ",size=" << eckit::Bytes(size_) << ",unload=" << unload_ << "]";
}

const void *SharedMemoryLoader::address() const {
    return address_;
}

size_t SharedMemoryLoader::size() const {
    return size_;
}

bool SharedMemoryLoader::inSharedMemory() const {
    return true;
}


bool SharedMemoryLoader::shared() {
    return true;
}


namespace {

static LegendreLoaderBuilder<SharedMemoryLoader> loader1("shared-memory");
static LegendreLoaderBuilder<SharedMemoryLoader> loader2("shmem");
static LegendreLoaderBuilder<SharedMemoryLoader> loader3("tmp-shmem");
static LegendreLoaderBuilder<SharedMemoryLoader> loader5("tmp-shared-memory");

}


}  // namespace legendre
}  // namespace caching
}  // namespace mir

