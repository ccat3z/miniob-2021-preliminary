#include "large_block_pool.h"
#include "common/log/log.h"
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

LargeBlockPool::LargeBlockPool() {}
LargeBlockPool::~LargeBlockPool() { close(); }

RC LargeBlockPool::open_file(std::string file_name) {
  close();

  bool need_init = false;
  if (!fs::exists(file_name)) {
    need_init = true;
  }

  this->file_name = file_name;
  fd = open(file_name.c_str(), O_RDWR | O_CREAT | O_NONBLOCK,
            S_IREAD | S_IWRITE);
  if (fd < 0) {
    LOG_ERROR("Failed to open file: %s", strerror(errno));
    return RC::CANTOPEN;
  }

  if (need_init) {
    return init();
  }

  return RC::SUCCESS;
}

void LargeBlockPool::close() {
  if (fd > 0) {
    ::close(fd);
  }

  fd = -1;
}

RC LargeBlockPool::init() {
  char data[LARGE_BLOCK_POOL_BLOCK_SIZE] = {1};
  return set(0, data, LARGE_BLOCK_POOL_BLOCK_SIZE);
}

RC LargeBlockPool::remove() {
  close();

  if (!file_name.empty() && std::remove(file_name.c_str()) != 0) {
    LOG_ERROR("Failed to remove file: %s", file_name.c_str());
    return RC::IOERR_DELETE;
  }

  return RC::SUCCESS;
}

std::unique_ptr<LargeBlock> LargeBlockPool::get(uint32_t idx) const {
  if (fd < 0) {
    LOG_ERROR("LargeBlockPool is not opened");
    return nullptr;
  }

  auto blk = std::make_unique<LargeBlock>();
  lseek(fd, ((size_t)idx) * LARGE_BLOCK_POOL_BLOCK_SIZE, SEEK_SET);
  if (read(fd, blk->data, LARGE_BLOCK_POOL_BLOCK_SIZE) < 0) {
    LOG_ERROR("Failed to read block %d: %s", idx, strerror(errno));
    return nullptr;
  }

  return blk;
}

RC LargeBlockPool::set(uint32_t idx, const char *data, size_t size) {
  if (fd < 0) {
    LOG_ERROR("LargeBlockPool is not opened");
    return RC::GENERIC_ERROR;
  }

  lseek(fd, ((size_t)idx) * LARGE_BLOCK_POOL_BLOCK_SIZE, SEEK_SET);
  if (size > LARGE_BLOCK_POOL_BLOCK_SIZE) {
    size = LARGE_BLOCK_POOL_BLOCK_SIZE;
  }
  if (write(fd, data, size) < 0) {
    LOG_ERROR("Failed to read set %d: %s", idx, strerror(errno));
    return RC::IOERR;
  }

  if (idx > 0) {
    mark(idx, true);
  }

  return RC::SUCCESS;
}

void LargeBlockPool::mark(uint32_t idx, bool used) {
  size_t section = idx / 8;
  size_t block = idx % 8;

  auto mask = get(0);
  assert(section < LARGE_BLOCK_POOL_BLOCK_SIZE);

  if (used) {
    mask->data[section] |= 1 << block;
  } else {
    mask->data[section] &= ~(1 << block);
  }

  set(0, mask->data, LARGE_BLOCK_POOL_BLOCK_SIZE);
}

uint32_t LargeBlockPool::find_next_free() {
  auto mask = get(0);

  size_t section;
  for (section = 0; section < LARGE_BLOCK_POOL_BLOCK_SIZE; section++) {
    if (mask->data[section] != 255) {
      break;
    }
  }

  char &blocks = mask->data[section];
  size_t block;
  for (block = 0; block < 8; block++) {
    if ((blocks & (1 << block)) != (1 << block)) {
      break;
    }
  }

  return section * 8 + block;
}