// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "common/conf/ini.h"
#include "common/lang/string.h"
#include "common/os/process_param.h"
#include "init.h"
#include "net/server.h"
#include <chrono>
#include <cstdlib>
#include <event.h>
#include <ghc/filesystem.hpp>
#include <gtest/gtest.h>
#include <pthread.h>
#include <sys/un.h>
#include <thread>

#define MAX_MEM_BUFFER_SIZE 8192
#define SERVER_START_STOP_TIMEOUT 1s

namespace fs = ghc::filesystem;
using namespace std::chrono_literals;
std::string conf_path = "../etc/observer.ini";


int init_unix_sock(const char *unix_sock_path) {
  int sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "failed to create unix socket. %s", strerror(errno));
    return -1;
  }

  struct sockaddr_un sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sun_family = PF_UNIX;
  snprintf(sockaddr.sun_path, sizeof(sockaddr.sun_path), "%s",
           unix_sock_path);

  if (connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
    fprintf(stderr,
            "failed to connect to server. unix socket path '%s'. error %s",
            sockaddr.sun_path, strerror(errno));
    close(sockfd);
    return -1;
  }
  return sockfd;
}

Server *init_server(std::string socket_path) {
  common::ProcessParam *process_param = common::the_process_param();
  process_param->set_unix_socket_path(socket_path.c_str());
  process_param->set_conf(conf_path);
  init(process_param, true);

  std::map<std::string, std::string> net_section =
      common::get_properties()->get("NET");

  long listen_addr = INADDR_ANY;
  long max_connection_num = MAX_CONNECTION_NUM_DEFAULT;
  int port = PORT_DEFAULT;

  std::map<std::string, std::string>::iterator it =
      net_section.find(CLIENT_ADDRESS);
  if (it != net_section.end()) {
    std::string str = it->second;
    common::str_to_val(str, listen_addr);
  }

  it = net_section.find(MAX_CONNECTION_NUM);
  if (it != net_section.end()) {
    std::string str = it->second;
    common::str_to_val(str, max_connection_num);
  }

  if (process_param->get_server_port() > 0) {
    port = process_param->get_server_port();
    LOG_INFO("Use port config in command line: %d", port);
  } else {
    it = net_section.find(PORT);
    if (it != net_section.end()) {
      std::string str = it->second;
      common::str_to_val(str, port);
    }
  }

  ServerParam server_param;
  server_param.listen_addr = listen_addr;
  server_param.max_connection_num = max_connection_num;
  server_param.port = port;

  if (process_param->get_unix_socket_path().size() > 0) {
    server_param.use_unix_socket = true;
    server_param.unix_socket_path = process_param->get_unix_socket_path();
  }

  Server *server = new Server(server_param);
  Server::init();
  return server;
}

// TestServer is observer for test
class TestServer {
  public:
    virtual ~TestServer() { };
    virtual void start(std::string data_dir, std::string socket_path) { };
    virtual void stop() { };
};

class ThreadTestServer : public TestServer {
  public:
    ~ThreadTestServer() { };
    static void *start_server_func(void *server) {
      ((Server *)server)->serve();
      return nullptr;
    }
    void start(std::string data_dir, std::string socket_path) {
      server = init_server(socket_path);
      pthread_create(&pid, nullptr, ThreadTestServer::start_server_func, server);
    }
    void stop() {
      server->shutdown();
      delete server;
      std::this_thread::sleep_for(SERVER_START_STOP_TIMEOUT);
    }
  private:
    Server *server;
    pthread_t pid;
};

class ForkTestServer : public TestServer {
  public:
    ~ForkTestServer() { };
    void start(std::string data_dir, std::string socket_path) {
      pid = fork();
      if (pid == 0) {
        fs::current_path(data_dir);
        Server *server = init_server(socket_path);
        server->serve();
        exit(0);
      }
    }
    void stop() {
      kill(pid, SIGTERM);
      waitpid(pid, nullptr, 0);
    }
  private:
    pid_t pid;
};

class SQLTest : public ::testing::Test {
public:
  SQLTest() {
    server = new ThreadTestServer();
  }

  ~SQLTest() {
    delete server;
  }

  void SetUp() override {
    data_dir = fs::temp_directory_path() / ("miniob-sql-test-" + common::random_string(6));
    fs::path socket_path = data_dir / ".sock";

    fs::create_directory(data_dir);
    fs::current_path(data_dir);
    server->start(data_dir, socket_path);

    std::this_thread::sleep_for(SERVER_START_STOP_TIMEOUT);
    sockfd = init_unix_sock(socket_path.c_str());
  }

  void TearDown() override {
    server->stop();
    fs::remove_all(data_dir);
  }

  std::string exec_sql(std::string sql) {
    char send_bytes;
    if ((send_bytes = write(sockfd, sql.c_str(), sql.length() + 1)) == -1) {
      fprintf(stderr, "send error: %d:%s \n", errno, strerror(errno));
      return "FAILURE";
    }

    memset(recv_buf, 0, MAX_MEM_BUFFER_SIZE);
    int len = 0;
    std::string resp;
    while ((len = recv(sockfd, recv_buf, MAX_MEM_BUFFER_SIZE, 0)) > 0) {
      bool msg_end = false;
      int last_char;
      for (last_char = 0; last_char < len; last_char++) {
        if (0 == recv_buf[last_char]) {
          msg_end = true;
          break;
        }
      }

      resp.append(recv_buf, recv_buf + last_char);
      if (msg_end) {
        break;
      }
      memset(recv_buf, 0, MAX_MEM_BUFFER_SIZE);
    }

    return resp;
  }

private:
  fs::path data_dir;
  TestServer *server;

  int sockfd;
  char recv_buf[MAX_MEM_BUFFER_SIZE];
};

TEST_F(SQLTest, Basic) {
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
}

TEST_F(SQLTest, UpdateMeta) {
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  conf_path = fs::absolute(conf_path);
  return RUN_ALL_TESTS();
}