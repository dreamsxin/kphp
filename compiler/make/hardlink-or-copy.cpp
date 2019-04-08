#include "compiler/make/hardlink-or-copy.h"

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

#include "common/algorithms/find.h"

#include "compiler/stage.h"
#include "compiler/threading/format.h"

static void hard_link_or_copy_impl(const std::string &from, const std::string &to, bool replace, bool allow_copy) noexcept {
  if (!link(from.c_str(), to.c_str())) {
    return;
  }

  if (errno == EEXIST) {
    if (replace) {
      kphp_error_return(!unlink(to.c_str()),
                        format("Can't remove file '%s': %s", to.c_str(), strerror(errno)));
      hard_link_or_copy_impl(from, to, false, allow_copy);
    }
    return;
  }

  // the file is placed on other device, or we have a permission problems, try to copy it
  if (vk::any_of_equal(errno, EXDEV, EPERM) && allow_copy) {
    struct stat file_stat;
    kphp_error_return (!stat(from.c_str(), &file_stat),
                       format("Can't get file stat '%s': %s", from.c_str(), strerror(errno)));

    std::string tmp_file = to + ".XXXXXX";
    int tmp_fd = mkstemp(&tmp_file[0]);
    kphp_error_return(tmp_fd != -1,
                      format("Can't create tmp file '%s': %s", tmp_file.c_str(), strerror(errno)));
    kphp_error_return(fchmod(tmp_fd, file_stat.st_mode) != -1,
                      format("Can't change permissions of tmp file '%s': %s", tmp_file.c_str(), strerror(errno)));
    int from_fd = open(from.c_str(), O_RDONLY);
    const ssize_t s = sendfile(tmp_fd, from_fd, nullptr, file_stat.st_size);
    close(from_fd);
    close(tmp_fd);
    kphp_error_return(s != -1, format("Can't copy file from '%s' to '%s': %s", from.c_str(), tmp_file.c_str(), strerror(errno)));
    kphp_assert(s == file_stat.st_size);
    hard_link_or_copy_impl(tmp_file, to, replace, false);
    unlink(tmp_file.c_str());
    return;
  }

  kphp_error(0, format("Can't copy file from '%s' to '%s': %s", from.c_str(), to.c_str(), strerror(errno)));
}

void hard_link_or_copy(const std::string &from, const std::string &to, bool replace) {
  hard_link_or_copy_impl(from, to, replace, true);
  stage::die_if_global_errors();
}