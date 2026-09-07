// Linux-only, deliberately fixed-scope privileged loopback benchmark helper.
#include <array>
#include <cerrno>
#include <charconv>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <dirent.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <fcntl.h>
#include <grp.h>
#include <linux/loop.h>
#include <linux/magic.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <sys/syscall.h>
#include <sys/sysmacros.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef BENCH_UID
#error "Build with an explicit non-root BENCH_UID and BENCH_GID"
#endif
#ifndef BENCH_GID
#error "Build with an explicit non-root BENCH_UID and BENCH_GID"
#endif
static_assert(BENCH_UID > 0 && BENCH_GID > 0);
namespace {
constexpr uid_t owner_uid = BENCH_UID;
constexpr gid_t owner_gid = BENCH_GID;
constexpr off_t image_size = 10LL * 1024 * 1024 * 1024;
constexpr std::string_view image_name = "image.img";
const std::string base_name = "gwz-fs-bench-" + std::to_string(owner_uid);
const std::string base_path = "/var/lib/" + base_name;
const std::string mount_path = base_path + "/mnt";

[[noreturn]] void refuse(const std::string& message) { throw std::runtime_error(message); }
[[noreturn]] void system_error(const std::string& operation) {
    const int error = errno;
    refuse(operation + ": " + std::strerror(error));
}
class Fd {
    int value_ = -1;
public:
    explicit Fd(int value = -1) : value_(value) {}
    ~Fd() { if (value_ >= 0) ::close(value_); }
    Fd(const Fd&) = delete;
    Fd& operator=(const Fd&) = delete;
    Fd(Fd&& other) noexcept : value_(std::exchange(other.value_, -1)) {}
    Fd& operator=(Fd&& other) noexcept {
        if (this != &other) { Fd old(std::exchange(value_, std::exchange(other.value_, -1))); }
        return *this;
    }
    int get() const { return value_; }
};
struct stat metadata(int fd) {
    struct stat value {};
    if (::fstat(fd, &value) != 0) system_error("fstat");
    return value;
}
void trusted_directory(int fd) {
    const auto value = metadata(fd);
    if (!S_ISDIR(value.st_mode) || value.st_uid != 0 || (value.st_mode & 0022) != 0)
        refuse("directory must be root-owned and not writable by group/others");
}
Fd directory_at(int parent, const char* name, bool create) {
    bool created = false;
    if (create) {
        if (::mkdirat(parent, name, 0755) == 0) created = true;
        else if (errno != EEXIST) system_error("mkdirat");
    }
    Fd fd(::openat(parent, name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC));
    if (fd.get() < 0) system_error("open directory");
    trusted_directory(fd.get());
    if (created && ::fchmod(fd.get(), 0755) != 0) system_error("chmod new directory");
    return fd;
}
void require_empty_directory(int fd) {
    const int copy = ::fcntl(fd, F_DUPFD_CLOEXEC, 3);
    if (copy < 0) system_error("duplicate directory descriptor");
    DIR* stream = ::fdopendir(copy);
    if (!stream) { ::close(copy); system_error("open directory stream"); }
    const auto close_directory = [](DIR* dir) { ::closedir(dir); };
    std::unique_ptr<DIR, decltype(close_directory)> directory(stream, close_directory);
    for (;;) {
        errno = 0;
        const auto* entry = ::readdir(directory.get());
        if (!entry) {
            if (errno != 0) system_error("read mount directory");
            break;
        }
        const std::string_view name(entry->d_name);
        if (name != "." && name != "..") refuse("unmounted mount directory is not empty");
    }
}
void private_file(int fd) {
    const auto value = metadata(fd);
    if (!S_ISREG(value.st_mode) || value.st_uid != 0 || value.st_gid != 0 ||
        (value.st_mode & 07777) != 0600 || value.st_nlink != 1)
        refuse("metadata/image must be a root-owned 0600 regular file with one link");
}
std::uint64_t mount_id(int fd) {
    struct statx value {};
    if (::statx(fd, "", AT_EMPTY_PATH, STATX_MNT_ID, &value) != 0)
        system_error("statx mount ID");
    if ((value.stx_mask & STATX_MNT_ID) == 0) refuse("kernel lacks mount ID reporting");
    return value.stx_mnt_id;
}
void host_namespaces() {
    for (const char* kind : {"mnt", "user", "pid"}) {
        struct stat self {}, init {};
        const std::string suffix = std::string("/ns/") + kind;
        if (::stat(("/proc/self" + suffix).c_str(), &self) != 0 ||
            ::stat(("/proc/1" + suffix).c_str(), &init) != 0)
            system_error("inspect host namespaces");
        if (self.st_dev != init.st_dev || self.st_ino != init.st_ino)
            refuse("run from the host namespaces, not a container or private namespace");
    }
}
void enter_privileged_scope() {
    if (::getuid() != owner_uid && ::getuid() != 0) refuse("caller UID is not authorized");
    if (::geteuid() != 0) refuse("requires sudo or a root-owned setuid installation");
    if (::clearenv() != 0) system_error("clearenv");
    ::umask(0077);
    const struct rlimit no_core {0, 0};
    if (::setrlimit(RLIMIT_CORE, &no_core) != 0) system_error("disable core dumps");
    if (::setgroups(0, nullptr) != 0 || ::setresgid(0, 0, 0) != 0 ||
        ::setresuid(0, 0, 0) != 0) system_error("normalize privileged identity");
    sigset_t unblocked;
    ::sigemptyset(&unblocked);
    if (::sigprocmask(SIG_SETMASK, &unblocked, nullptr) != 0) system_error("signal mask");
    if (::signal(SIGCHLD, SIG_DFL) == SIG_ERR) system_error("SIGCHLD");
    for (const int signal : {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGPIPE})
        if (::signal(signal, SIG_IGN) == SIG_ERR) system_error("signal disposition");
    if (::syscall(SYS_close_range, 3U, UINT_MAX, 0U) != 0)
        system_error("close inherited descriptors (Linux 5.9+ required)");
    for (int fd = 0; fd < 3; ++fd) {
        if (::fcntl(fd, F_GETFD) >= 0) continue;
        if (errno != EBADF) system_error("inspect standard descriptor");
        const int replacement = ::open("/dev/null", O_RDWR | O_NOFOLLOW);
        if (replacement != fd) refuse("cannot establish standard descriptors");
    }
    if (::chdir("/") != 0) system_error("chdir");
    host_namespaces();
}
struct Sandbox {
    Fd dir;
    Fd lock;
    explicit Sandbox(Fd base) : dir(std::move(base)),
        lock(::openat(dir.get(), "lock", O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC, 0600)) {
        if (lock.get() < 0) system_error("open lock");
        private_file(lock.get());
        if (::flock(lock.get(), LOCK_EX | LOCK_NB) != 0) system_error("benchmark helper busy");
    }
    Fd mount_directory() const {
        Fd fd(::openat(dir.get(), "mnt", O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC));
        if (fd.get() < 0) system_error("open mount directory");
        return fd;
    }
    bool mounted(int mount_fd) const { return mount_id(dir.get()) != mount_id(mount_fd); }
    std::optional<Fd> image() const {
        Fd fd(::openat(dir.get(), image_name.data(), O_RDWR | O_NOFOLLOW | O_CLOEXEC));
        if (fd.get() < 0) {
            if (errno == ENOENT) return std::nullopt;
            system_error("open image");
        }
        private_file(fd.get());
        const auto value = metadata(fd.get());
        if (value.st_size < 0 || value.st_size > image_size) refuse("unexpected image size");
        return fd;
    }
};
std::optional<Fd> base_directory(bool create) {
    Fd root(::open("/", O_RDONLY | O_DIRECTORY | O_CLOEXEC));
    if (root.get() < 0) system_error("open root");
    trusted_directory(root.get());
    auto var = directory_at(root.get(), "var", false);
    auto lib = directory_at(var.get(), "lib", false);
    struct stat value {};
    if (!create && ::fstatat(lib.get(), base_name.c_str(), &value, AT_SYMLINK_NOFOLLOW) != 0) {
        if (errno == ENOENT) return std::nullopt;
        system_error("inspect benchmark directory");
    }
    return directory_at(lib.get(), base_name.c_str(), create);
}
struct Loop { int number; Fd fd; };
Fd open_loop(int number) {
    const std::string path = "/dev/loop" + std::to_string(number);
    Fd fd(::open(path.c_str(), O_RDWR | O_NOFOLLOW | O_CLOEXEC));
    if (fd.get() < 0) system_error("open loop device");
    const auto value = metadata(fd.get());
    if (!S_ISBLK(value.st_mode) || major(value.st_rdev) != 7 ||
        minor(value.st_rdev) != static_cast<unsigned int>(number)) refuse("unexpected loop device");
    return fd;
}
Loop allocate_loop(int image_fd) {
    Fd control(::open("/dev/loop-control", O_RDWR | O_NOFOLLOW | O_CLOEXEC));
    if (control.get() < 0) system_error("open loop-control");
    const auto value = metadata(control.get());
    if (!S_ISCHR(value.st_mode) || major(value.st_rdev) != 10 || minor(value.st_rdev) != 237)
        refuse("unexpected loop-control device");
    for (int attempt = 0; attempt < 32; ++attempt) {
        const int number = ::ioctl(control.get(), LOOP_CTL_GET_FREE);
        if (number < 0) system_error("allocate loop device");
        auto fd = open_loop(number);
        struct loop_config config {};
        config.fd = static_cast<std::uint32_t>(image_fd);
        config.info.lo_flags = LO_FLAGS_AUTOCLEAR;
        config.info.lo_sizelimit = static_cast<std::uint64_t>(image_size);
        if (::ioctl(fd.get(), LOOP_CONFIGURE, &config) == 0) return {number, std::move(fd)};
        if (errno != EBUSY) system_error("LOOP_CONFIGURE");
    }
    refuse("no loop device could be reserved; image retained");
}
std::optional<Loop> find_loop(int image_fd) {
    const auto image = metadata(image_fd);
    std::optional<Loop> result;
    for (const auto& entry : std::filesystem::directory_iterator("/sys/block")) {
        const auto name = entry.path().filename().string();
        if (!name.starts_with("loop")) continue;
        int number = -1;
        const auto parsed = std::from_chars(name.data() + 4, name.data() + name.size(), number);
        if (parsed.ec != std::errc{} || parsed.ptr != name.data() + name.size() || number < 0) continue;
        // Unconfigured loop nodes can be absent while udev is catching up.
        if (!std::filesystem::exists(entry.path() / "loop")) continue;
        auto fd = open_loop(number);
        struct loop_info64 info {};
        if (::ioctl(fd.get(), LOOP_GET_STATUS64, &info) != 0) {
            if (errno == ENXIO) continue;
            system_error("inspect loop association");
        }
        if (info.lo_inode != image.st_ino || info.lo_device != image.st_dev) continue;
        if (result) refuse("multiple loop associations; image retained");
        if (info.lo_offset != 0 || info.lo_sizelimit != static_cast<std::uint64_t>(image_size) ||
            info.lo_encrypt_type != 0 || (info.lo_flags & LO_FLAGS_AUTOCLEAR) == 0)
            refuse("unexpected loop geometry/flags; image retained");
        result.emplace(Loop {number, std::move(fd)});
    }
    return result;
}
const char* verify_mount(int mount_fd, const Loop& loop) {
    if (metadata(mount_fd).st_dev != makedev(7, static_cast<unsigned int>(loop.number)))
        refuse("mount does not belong to this benchmark image");
    struct statfs value {};
    if (::fstatfs(mount_fd, &value) != 0) system_error("inspect mounted filesystem");
    if (value.f_type == XFS_SUPER_MAGIC) return "xfs";
    if (value.f_type == EXT4_SUPER_MAGIC) return "ext4";
    refuse("unexpected mounted filesystem");
}
void format_image(const char* filesystem, int loop_fd, int lock_fd) {
    const bool xfs = std::string_view(filesystem) == "xfs";
    const char* executable = xfs ? "/usr/sbin/mkfs.xfs" : "/usr/sbin/mkfs.ext4";
    struct stat value {};
    if (::stat(executable, &value) != 0) system_error("formatter is not installed");
    if (!S_ISREG(value.st_mode) || value.st_uid != 0 || (value.st_mode & 0022) != 0)
        refuse("formatter must be a trusted root-owned system binary");
    const std::string device = "/proc/self/fd/" + std::to_string(loop_fd);
    const pid_t child = ::fork();
    if (child < 0) system_error("fork formatter");
    if (child == 0) {
        // Ignored dispositions survive exec; mkfs must remain terminable by an admin.
        for (const int signal : {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGPIPE})
            if (::signal(signal, SIG_DFL) == SIG_ERR) _exit(126);
        // Pin both the device and lock even if the parent is killed during mkfs.
        if (::fcntl(loop_fd, F_SETFD, 0) != 0 || ::fcntl(lock_fd, F_SETFD, 0) != 0) _exit(126);
        char* const environment[] = {const_cast<char*>("PATH=/usr/sbin:/usr/bin"),
                                     const_cast<char*>("LC_ALL=C"), nullptr};
        const char* xfs_args[] = {executable, "-q", "-f", "-m", "reflink=1", device.c_str(), nullptr};
        const char* ext4_args[] = {executable, "-q", "-F", "-E",
                                  "lazy_itable_init=0,lazy_journal_init=0", device.c_str(), nullptr};
        ::execve(executable, const_cast<char* const*>(xfs ? xfs_args : ext4_args), environment);
        _exit(127);
    }
    int status = 0;
    while (::waitpid(child, &status, 0) < 0) if (errno != EINTR) system_error("wait for formatter");
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) refuse("formatter failed; use destroy to clean up");
}
void status(Sandbox& sandbox) {
    auto mount = sandbox.mount_directory();
    auto image = sandbox.image();
    const bool mounted = sandbox.mounted(mount.get());
    if (!mounted) trusted_directory(mount.get());
    auto loop = image ? find_loop(image->get()) : std::nullopt;
    if (mounted && !loop) refuse("unexpected mount without matching image/loop");
    std::cout << "{\"mount\":\"" << mount_path << "\",\"mounted\":" << (mounted ? "true" : "false");
    if (mounted) {
        const char* filesystem = verify_mount(mount.get(), *loop);
        if (::syncfs(mount.get()) != 0 || ::fsync(image->get()) != 0) system_error("flush benchmark filesystem");
        struct statvfs usage {};
        if (::fstatvfs(mount.get(), &usage) != 0) system_error("filesystem usage");
        std::cout << ",\"filesystem\":\"" << filesystem << "\",\"used_bytes\":"
                  << static_cast<std::uint64_t>(usage.f_blocks - usage.f_bfree) * usage.f_frsize
                  << ",\"available_bytes\":" << static_cast<std::uint64_t>(usage.f_bavail) * usage.f_frsize;
    }
    std::cout << ",\"image_exists\":" << (image ? "true" : "false");
    if (image) {
        const auto value = metadata(image->get());
        std::cout << ",\"image_bytes\":" << value.st_size << ",\"image_allocated_bytes\":"
                  << static_cast<std::uint64_t>(value.st_blocks) * 512;
    }
    std::cout << "}\n";
}
void create(Sandbox& sandbox, const char* filesystem) {
    auto mount_dir = directory_at(sandbox.dir.get(), "mnt", true);
    if (sandbox.mounted(mount_dir.get())) refuse("already mounted; destroy first");
    require_empty_directory(mount_dir.get());
    Fd image(::openat(sandbox.dir.get(), image_name.data(), O_RDWR | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600));
    if (image.get() < 0) system_error("create NEW image (destroy existing image first)");
    private_file(image.get());
    if (::ftruncate(image.get(), image_size) != 0) system_error("size image");
    auto loop = allocate_loop(image.get());
    format_image(filesystem, loop.fd.get(), sandbox.lock.get());
    const std::string device = "/proc/self/fd/" + std::to_string(loop.fd.get());
    const std::string target = "/proc/self/fd/" + std::to_string(mount_dir.get());
    if (::mount(device.c_str(), target.c_str(), filesystem,
                MS_NODEV | MS_NOSUID | MS_NOEXEC | MS_NOATIME, nullptr) != 0) system_error("mount image");
    auto mounted = sandbox.mount_directory();
    verify_mount(mounted.get(), loop);
    if (::fchown(mounted.get(), owner_uid, owner_gid) != 0 || ::fchmod(mounted.get(), 0700) != 0)
        system_error("make mounted filesystem accessible to benchmark user");
    status(sandbox);
}
void destroy(Sandbox& sandbox) {
    auto mount = sandbox.mount_directory();
    auto image = sandbox.image();
    const bool mounted = sandbox.mounted(mount.get());
    if (!mounted) trusted_directory(mount.get());
    auto loop = image ? find_loop(image->get()) : std::nullopt;
    if (mounted) {
        if (!loop) refuse("unexpected mount; refusing to unmount");
        verify_mount(mount.get(), *loop);
        if (::syncfs(mount.get()) != 0) system_error("flush before unmount");
        // Pin the root-owned parent; holding the mounted directory itself would
        // make an ordinary unmount busy. Only root can replace this child entry.
        if (::fchdir(sandbox.dir.get()) != 0) system_error("pin unmount parent");
        mount = Fd();
        if (::umount2("mnt", UMOUNT_NOFOLLOW) != 0) system_error("unmount (busy mounts are retained)");
    }
    if (loop) {
        if (::ioctl(loop->fd.get(), LOOP_CLR_FD) != 0 && errno != ENXIO) system_error("detach loop");
        struct loop_info64 info {};
        if (::ioctl(loop->fd.get(), LOOP_GET_STATUS64, &info) == 0)
            refuse("loop remains busy; image retained (no lazy cleanup)");
        if (errno != ENXIO) system_error("confirm loop detached");
    }
    if (image) {
        struct stat current {};
        if (::fstatat(sandbox.dir.get(), image_name.data(), &current, AT_SYMLINK_NOFOLLOW) != 0)
            system_error("recheck image before unlink");
        const auto original = metadata(image->get());
        if (current.st_dev != original.st_dev || current.st_ino != original.st_ino)
            refuse("image identity changed; retained");
        if (::unlinkat(sandbox.dir.get(), image_name.data(), 0) != 0 || ::fsync(sandbox.dir.get()) != 0)
            system_error("remove image");
    }
    std::cout << "Destroyed benchmark image; no mount or backing image remains.\n";
}
void usage() {
    std::cout << "gwz-fs-bench create xfs|ext4\n"
                 "gwz-fs-bench status\n"
                 "gwz-fs-bench destroy\n"
              << "Fixed 10 GiB disposable image; allowed UID=" << owner_uid << " GID=" << owner_gid
              << "\nMount: " << mount_path << "\nDestroy discards ALL data in this image.\n";
}
} // namespace
int main(int argc, char** argv) {
    try {
        if (argc == 2 && (std::string_view(argv[1]) == "--help" || std::string_view(argv[1]) == "help")) {
            usage(); return 0;
        }
        const bool creating = argc == 3 && std::string_view(argv[1]) == "create" &&
                              (std::string_view(argv[2]) == "xfs" || std::string_view(argv[2]) == "ext4");
        if (!creating && !(argc == 2 && (std::string_view(argv[1]) == "status" ||
                                         std::string_view(argv[1]) == "destroy"))) { usage(); return 2; }
        enter_privileged_scope();
        auto base = base_directory(creating);
        if (!base) { std::cout << "{\"mounted\":false,\"image_exists\":false}\n"; return 0; }
        Sandbox sandbox(std::move(*base));
        if (creating) create(sandbox, argv[2]);
        else if (std::string_view(argv[1]) == "status") status(sandbox);
        else destroy(sandbox);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "gwz-fs-bench: " << error.what() << '\n'; return 1;
    }
}
