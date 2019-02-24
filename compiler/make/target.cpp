#include "compiler/make/target.h"

#include <cassert>
#include <cstdio>

void Target::set_mtime(long long new_mtime) {
  assert(mtime == 0);
  mtime = new_mtime;
}

bool Target::upd_mtime(long long new_mtime) {
  if (new_mtime < mtime) {
    printf("Trying to decrease mtime\n");
    return false;
  }
  mtime = new_mtime;
  return true;
}

Target::Target() :
  mtime(0),
  pending_deps(0),
  is_required(false),
  is_waiting(false),
  is_ready(false) {
}

void Target::compute_priority() {
  priority = 0;
}

bool Target::after_run_success() {
  return true;
}

bool Target::require() {
  if (is_required) {
    return false;
  }
  is_required = true;
  on_require();
  return true;
}

void Target::force_changed(long long new_mtime) {
  if (mtime < new_mtime) {
    mtime = new_mtime;
  }
}

std::string Target::target() {
  return get_name();
}

std::string Target::dep_list() {
  std::string ss;
  for (auto const dep : deps) {
    ss += dep->get_name();
    ss += " ";
  }
  return ss;
}

KphpTarget::KphpTarget() :
  Target(),
  file(nullptr),
  env(nullptr) {
}

string KphpTarget::get_name() {
  return file->path;
}

void KphpTarget::on_require() {
  file->needed = true;
}

bool KphpTarget::after_run_success() {
  long long mtime = file->upd_mtime();
  if (mtime < 0) {
    return false;
  } else if (mtime == 0) {
    fprintf(stdout, "Failed to generate target [%s]\n", file->path.c_str());
    return false;
  }
  return upd_mtime(file->mtime);
}

void KphpTarget::after_run_fail() {
  file->unlink();
}

void KphpTarget::set_file(File *new_file) {
  assert (file == nullptr);
  file = new_file;
  set_mtime(file->mtime);
}

File *KphpTarget::get_file() const {
  return file;
}

void KphpTarget::set_env(KphpMakeEnv *new_env) {
  assert (env == nullptr);
  env = new_env;
}

