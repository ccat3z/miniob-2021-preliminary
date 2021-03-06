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
std::string observer_binary = "./bin/observer";

int init_unix_sock(const char *unix_sock_path) {
  int sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "failed to create unix socket. %s", strerror(errno));
    return -1;
  }

  struct sockaddr_un sockaddr;
  memset(&sockaddr, 0, sizeof(sockaddr));
  sockaddr.sun_family = PF_UNIX;
  snprintf(sockaddr.sun_path, sizeof(sockaddr.sun_path), "%s", unix_sock_path);

  if (connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
    fprintf(stderr,
            "failed to connect to server. unix socket path '%s'. error %s",
            sockaddr.sun_path, strerror(errno));
    close(sockfd);
    return -1;
  }
  return sockfd;
}

Server *init_server(std::string socket_path, std::string storage_dir) {
  common::ProcessParam *process_param = common::the_process_param();
  process_param->set_unix_socket_path(socket_path.c_str());
  process_param->set_conf(conf_path);
  common::get_properties()->put("BaseDir", storage_dir, "DefaultStorageStage");
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
  virtual ~TestServer(){};
  virtual void start(std::string data_dir, std::string socket_path){};
  virtual void stop(){};
};

class ThreadTestServer : public TestServer {
public:
  ~ThreadTestServer(){};
  static void *start_server_func(void *server) {
    ((Server *)server)->serve();
    return nullptr;
  }
  void start(std::string data_dir, std::string socket_path) {
    server = init_server(socket_path, data_dir);
    pthread_create(&pid, nullptr, ThreadTestServer::start_server_func, server);
    std::this_thread::sleep_for(SERVER_START_STOP_TIMEOUT);
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
  ~ForkTestServer(){};
  void start(std::string data_dir, std::string socket_path) {
    pid = fork();
    if (pid == 0) {
      fs::current_path(data_dir);
      Server *server = init_server(socket_path, data_dir);
      server->serve();
      exit(0);
    }
    std::this_thread::sleep_for(SERVER_START_STOP_TIMEOUT);
  }
  void stop() {
    kill(pid, SIGTERM);
    waitpid(pid, nullptr, 0);
  }

private:
  pid_t pid;
};

class ExecTestServer : public TestServer {
public:
  ~ExecTestServer(){};
  void start(std::string data_dir, std::string socket_path) {
    pid = fork();
    if (pid == 0) {
      fs::current_path(data_dir);
      if (execl(observer_binary.c_str(), "observer", "-s", socket_path.c_str(),
                "-f", conf_path.c_str(), NULL) != 0) {
        std::cerr << "Failed to start observer" << std::endl;
      }
      exit(0);
    }
    std::this_thread::sleep_for(SERVER_START_STOP_TIMEOUT);
  }
  void stop() {
    kill(pid, SIGTERM);
    std::this_thread::sleep_for(.5s);
    kill(pid, SIGTERM);
    waitpid(pid, nullptr, 0);
  }

private:
  pid_t pid;
};

class SQLTest : public ::testing::Test {
public:
  SQLTest() {
    const char *test_server_workaround =
        std::getenv("SQL_TEST_SERVER_WORKAROUND");
    if (test_server_workaround == nullptr) {
      server = new ThreadTestServer();
    } else if (strcmp(test_server_workaround, "fork") == 0) {
      server = new ForkTestServer();
    } else if (strcmp(test_server_workaround, "exec") == 0) {
      server = new ExecTestServer();
    } else {
      server = new ThreadTestServer();
    }
  }

  ~SQLTest() { delete server; }

  void SetUp() override {
    data_dir = fs::temp_directory_path() /
               ("miniob-sql-test-" + common::random_string(6));
    fs::path socket_path = data_dir / ".sock";

    fs::create_directory(data_dir);
    fs::current_path(data_dir);
    server->start(data_dir, socket_path);

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

// ########     ###     ######  ####  ######
// ##     ##   ## ##   ##    ##  ##  ##    ##
// ##     ##  ##   ##  ##        ##  ##
// ########  ##     ##  ######   ##  ##
// ##     ## #########       ##  ##  ##
// ##     ## ##     ## ##    ##  ##  ##    ##
// ########  ##     ##  ######  ####  ######

TEST_F(SQLTest, BasicCreateTableShouldWork) {
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
}

TEST_F(SQLTest, BasicInsertShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
}

TEST_F(SQLTest, BasicInsertWithWrongValueShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, \"A\");"), "FAILURE\n");
  ASSERT_EQ(exec_sql("insert into t values (1);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n");
}

TEST_F(SQLTest, BasicSelectShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select a from t;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select t.a from t;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select a, b from t;"), "a | b\n1 | 2\n");
}

// TODO: Fix issues that select after sync not work
TEST_F(SQLTest, DISABLED_BasicSelectAfterSyncShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("sync;"), "SUCCESS");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select a from t;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select t.a from t;"), "a\n1\n");
}

TEST_F(SQLTest, BasicSelectWithConditionShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t where 1 = 1;"), "a | b\n1 | 2\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where a = 1;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select * from t where t.a = 1;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select a from t where a = 1;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select * from t where a < 2 and a > 1;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where 1 = a;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select * from t where 1 = t.a;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select a from t where 1 = a;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select * from t where 2 > a and 1 < a;"), "a | b\n");
}

TEST_F(SQLTest, BasicSelectWithIndex) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where a > 1;"), "a | b\n2 | 2\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where a = 2;"), "a | b\n2 | 2\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where a < 2;"), "a | b\n1 | 2\n");
}

TEST_F(SQLTest, BasicSelectWithIndexEqualToMinValue) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 2);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where a = -1;"), "a | b\n");
}

TEST_F(SQLTest, BasicExtFloatFormat) {
  ASSERT_EQ(exec_sql("create table t(a float);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1.0);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1.2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1.23);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1.257);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (10.0);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t;"), "a\n1\n1.2\n1.23\n1.26\n10\n");
}

TEST_F(SQLTest, BasicExtConditionBetweenDifferentType) {
  ASSERT_EQ(exec_sql("create table t(a int, b float);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1.0);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1.2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3, 1.23);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where a > b;"), "a | b\n3 | 1.23\n");
  ASSERT_EQ(exec_sql("select * from t where a > 1.2;"), "a | b\n3 | 1.23\n");
  ASSERT_EQ(exec_sql("select * from t where 1.2 < a;"), "a | b\n3 | 1.23\n");
}

//  ######  ######## ##       ########  ######  ########
// ##    ## ##       ##       ##       ##    ##    ##
// ##       ##       ##       ##       ##          ##
//  ######  ######   ##       ######   ##          ##
//       ## ##       ##       ##       ##          ##
// ##    ## ##       ##       ##       ##    ##    ##
//  ######  ######## ######## ########  ######     ##

// ##     ## ######## ########    ###
// ###   ### ##          ##      ## ##
// #### #### ##          ##     ##   ##
// ## ### ## ######      ##    ##     ##
// ##     ## ##          ##    #########
// ##     ## ##          ##    ##     ##
// ##     ## ########    ##    ##     ##

TEST_F(SQLTest, SelectMetaShouldResponseHeadWhenNoData) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a\n");
}

TEST_F(SQLTest, SelectMetaInvalidTableShouldFailure) {
  ASSERT_EQ(exec_sql("select * from t2;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaInvalidTableShouldFailureInMultiTables) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t2, t;"), "FAILURE\n");
}

TEST_F(SQLTest, DISABLED_SelectMetaSameTableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  // TODO: Undefined behavior
  ASSERT_EQ(exec_sql("select * from t, t;"), "t.a\n");
}

TEST_F(SQLTest, SelectMetaSelectInvalidColumnShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select b from t;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select t2.a from t;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectInvalidColumnInMultiTablesShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select t.b from t, t2;"), "FAILURE\n");
}

TEST_F(SQLTest,
       SelectMetaSelectIndeterminableColumnInMultiTablesShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select a from t, t2;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectInvalidConditionShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t where a > 0;"), "a\n");

  ASSERT_EQ(exec_sql("select * from t where b > a;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where b > 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where 1 > b;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where b = 1;"), "FAILURE\n");

  ASSERT_EQ(exec_sql("select * from t where b < c;"), "FAILURE\n");

  ASSERT_EQ(exec_sql("select * from t where t.b > 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where t2.b > 1;"), "FAILURE\n");

  ASSERT_EQ(exec_sql("select * from t where a > '1';"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where a = 'a';"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where 1 = 'a';"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t where 1.1 > '10';"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectValidConditionInMultiTablesShouldSuccess) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int);"), "SUCCESS\n");
  ASSERT_NE(exec_sql("select * from t, t2 where a > b;"), "FAILURE\n");
  ASSERT_NE(exec_sql("select * from t, t2 where t.a > t2.b;"), "FAILURE\n");
  ASSERT_NE(
      exec_sql("select * from t, t2 where a > 1 and b > 1 and t.a > t2.b;"),
      "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectInvalidConditionInMultiTablesShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where a > c;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where t.a > t3.b;"), "FAILURE\n");
  ASSERT_EQ(
      exec_sql("select * from t, t2 where a > 1 and b > 1 and t.a > t3.b;"),
      "FAILURE\n");

  ASSERT_EQ(exec_sql("create table t3(c char);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t, t3 where a > c;"), "FAILURE\n");
}

TEST_F(SQLTest,
       SelectMetaSelectIndeterminableConditionInMultiTablesShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t, t2 where a > 0;"), "FAILURE\n");
}

// ########  ########   #######  ########
// ##     ## ##     ## ##     ## ##     ##
// ##     ## ##     ## ##     ## ##     ##
// ##     ## ########  ##     ## ########
// ##     ## ##   ##   ##     ## ##
// ##     ## ##    ##  ##     ## ##
// ########  ##     ##  #######  ##

// ########    ###    ########  ##       ########
//    ##      ## ##   ##     ## ##       ##
//    ##     ##   ##  ##     ## ##       ##
//    ##    ##     ## ########  ##       ######
//    ##    ######### ##     ## ##       ##
//    ##    ##     ## ##     ## ##       ##
//    ##    ##     ## ########  ######## ########

TEST_F(SQLTest, DropTableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n\n");
  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");
  ASSERT_EQ(exec_sql("select * from t;"), "FAILURE\n");
}

TEST_F(SQLTest, DropTableWithIndexShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a_idx on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n\n");
  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");
}

TEST_F(SQLTest, DropTableWithDataShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a\n1\n");
  ASSERT_EQ(exec_sql("sync;"), "SUCCESS");

  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a\n");
}

TEST_F(SQLTest, DropTableFailureIfNotExist) {
  ASSERT_EQ(exec_sql("drop table t2;"), "FAILURE\n");
}

TEST_F(SQLTest, DropTableCanCreateAgain) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n\n");
  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n\n");
}

TEST_F(SQLTest, DropTableWithIndexCreateAgain) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a_idx on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n\n");

  ASSERT_EQ(exec_sql("drop table t;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");

  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a_idx on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("show tables;"), "t\n\n");
}

// ##     ## ########  ########     ###    ######## ########
// ##     ## ##     ## ##     ##   ## ##      ##    ##
// ##     ## ##     ## ##     ##  ##   ##     ##    ##
// ##     ## ########  ##     ## ##     ##    ##    ######
// ##     ## ##        ##     ## #########    ##    ##
// ##     ## ##        ##     ## ##     ##    ##    ##
//  #######  ##        ########  ##     ##    ##    ########

TEST_F(SQLTest, UpdateShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 100;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "100 | 1\n"
                                          "100 | 10\n"
                                          "100 | 3\n"
                                          "100 | 5\n");
}

TEST_F(SQLTest, UpdateWithConditionsShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 100 where a = 1;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "100 | 1\n"
                                          "100 | 10\n"
                                          "2 | 3\n"
                                          "2 | 5\n");
}

TEST_F(SQLTest, UpdateWithIndexShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where a = 1;"), "a | b\n"
                                                      "1 | 1\n"
                                                      "1 | 10\n");

  ASSERT_EQ(exec_sql("update t set a = 100 where a = 1;"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "100 | 1\n"
                                          "100 | 10\n"
                                          "2 | 3\n"
                                          "2 | 5\n");
  ASSERT_EQ(exec_sql("select * from t where a = 1;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where a = 100;"), "a | b\n"
                                                        "100 | 1\n"
                                                        "100 | 10\n");
}

TEST_F(SQLTest, UpdateWithInvalidColumnShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set c = 100 where a = 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "1 | 1\n"
                                          "1 | 10\n"
                                          "2 | 3\n"
                                          "2 | 5\n");
}

TEST_F(SQLTest, UpdateWithInvalidConditionShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 100 where c = 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "1 | 1\n"
                                          "1 | 10\n"
                                          "2 | 3\n"
                                          "2 | 5\n");
}

TEST_F(SQLTest, UpdateWithInvalidValueShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 5);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 1.0 where a = 1;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "1 | 1\n"
                                          "1 | 10\n"
                                          "2 | 3\n"
                                          "2 | 5\n");
}

// ########     ###    ######## ########
// ##     ##   ## ##      ##    ##
// ##     ##  ##   ##     ##    ##
// ##     ## ##     ##    ##    ######
// ##     ## #########    ##    ##
// ##     ## ##     ##    ##    ##
// ########  ##     ##    ##    ########

TEST_F(SQLTest, DateCanCreateTable) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("desc t;"),
            "t(\n"
            "\tfield name=__trx, type=ints, len=4, visible=no, nullable=no\n"
            "\tfield name=__null, type=ints, len=4, visible=no, nullable=no\n"
            "\tfield name=a, type=ints, len=4, visible=yes, nullable=no\n"
            "\tfield name=d, type=date, len=4, visible=yes, nullable=no\n"
            ")\n");
}

TEST_F(SQLTest, DateInsertShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2100-2-29');"), "FAILURE\n");
}

TEST_F(SQLTest, DateSelectShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '20-1-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '4000-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
            "a | d\n1 | 0020-01-10\n1 | 2020-10-10\n1 | 2020-01-01\n1 | "
            "4000-01-01\n");
}

TEST_F(SQLTest, DateSelectWhereShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select d from t where d > '2020-12-1';"),
            "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2021-1-1';"),
            "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2020-1-21';"), "d\n");
  ASSERT_EQ(exec_sql("select d from t where d < '2020-12-1';"),
            "d\n2020-10-10\n");
}

TEST_F(SQLTest, DateSelectWithIndexShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_d on t(d);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select d from t where d > '2020-12-1';"),
            "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2021-1-1';"),
            "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2020-1-21';"), "d\n");
  ASSERT_EQ(exec_sql("select d from t where d < '2020-12-1';"),
            "d\n2020-10-10\n");
}

TEST_F(SQLTest, DateCharsNotBeAffected) {
  ASSERT_EQ(exec_sql("create table t(a int, b char(20), d date);"),
            "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-1-1', '2020-1-1');"),
            "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b | d\n"
                                          "1 | 2020-1-1 | 2020-01-01\n");
}

TEST_F(SQLTest, DateUpdateShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set d='2022-2-22' where d < '2020-12-31';"),
            "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | d\n"
                                          "1 | 2022-02-22\n"
                                          "1 | 2021-01-01\n");
}

TEST_F(SQLTest, DateUpdateWithIndexShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_d on t(d);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set d='2022-2-22' where d < '2020-12-31';"),
            "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t where d='2022-2-22';"),
            "a | d\n"
            "1 | 2022-02-22\n");
}

TEST_F(SQLTest, DateUpdateInvalidDateShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set d='2022-2-30' where d < '2020-12-31';"),
            "FAILURE\n");
}

//  ######  ######## ##       ########  ######  ########
// ##    ## ##       ##       ##       ##    ##    ##
// ##       ##       ##       ##       ##          ##
//  ######  ######   ##       ######   ##          ##
//       ## ##       ##       ##       ##          ##
// ##    ## ##       ##       ##       ##    ##    ##
//  ######  ######## ######## ########  ######     ##
// ########    ###    ########  ##       ########  ######
//    ##      ## ##   ##     ## ##       ##       ##    ##
//    ##     ##   ##  ##     ## ##       ##       ##
//    ##    ##     ## ########  ##       ######    ######
//    ##    ######### ##     ## ##       ##             ##
//    ##    ##     ## ##     ## ##       ##       ##    ##
//    ##    ##     ## ########  ######## ########  ######

TEST_F(SQLTest, SelectTablesShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t, t2;"), "t.a | t.b | t2.b | t2.d\n"
                                              "1 | 1 | 100 | 200\n"
                                              "1 | 1 | 300 | 500\n"
                                              "2 | 3 | 100 | 200\n"
                                              "2 | 3 | 300 | 500\n");

  ASSERT_EQ(exec_sql("create table t3(o int, a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (999, 888);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 666);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t, t2, t3;"),
            "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
            "1 | 1 | 100 | 200 | 999 | 888\n"
            "1 | 1 | 100 | 200 | 777 | 666\n"
            "1 | 1 | 300 | 500 | 999 | 888\n"
            "1 | 1 | 300 | 500 | 777 | 666\n"
            "2 | 3 | 100 | 200 | 999 | 888\n"
            "2 | 3 | 100 | 200 | 777 | 666\n"
            "2 | 3 | 300 | 500 | 999 | 888\n"
            "2 | 3 | 300 | 500 | 777 | 666\n");
}

TEST_F(SQLTest, SelectTablesColumnsOrderShouldCorrect) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t3(o int, a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (999, 888);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 666);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select o, t.a, t2.b from t, t2, t3;"),
            "t3.o | t.a | t2.b\n"
            "999 | 1 | 100\n"
            "777 | 1 | 100\n"
            "999 | 1 | 300\n"
            "777 | 1 | 300\n"
            "999 | 2 | 100\n"
            "777 | 2 | 100\n"
            "999 | 2 | 300\n"
            "777 | 2 | 300\n");

  ASSERT_EQ(exec_sql("select * from t2, t;"), "t2.b | t2.d | t.a | t.b\n"
                                              "100 | 200 | 1 | 1\n"
                                              "100 | 200 | 2 | 3\n"
                                              "300 | 500 | 1 | 1\n"
                                              "300 | 500 | 2 | 3\n");
}

TEST_F(SQLTest, SelectTablesSingleColumnShouldShowTableName) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select t.a from t, t2;"), "t.a\n"
                                                "1\n"
                                                "1\n"
                                                "2\n"
                                                "2\n");
}

TEST_F(SQLTest, SelectTablesBothStarAndColumnsShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select t.*, t2.b from t, t2;"), "t.a | t.b | t2.b\n"
                                                      "1 | 1 | 100\n"
                                                      "1 | 1 | 300\n"
                                                      "2 | 3 | 100\n"
                                                      "2 | 3 | 300\n");
}

TEST_F(SQLTest, SelectTablesWithConditionsShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t3(o int, a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (999, 888);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 666);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 0);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t, t2, t3 where o <= 777 and t.a >= 2;"),
            "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
            "2 | 3 | 100 | 200 | 777 | 666\n"
            "2 | 3 | 100 | 200 | 777 | 0\n"
            "2 | 3 | 300 | 500 | 777 | 666\n"
            "2 | 3 | 300 | 500 | 777 | 0\n");

  ASSERT_EQ(exec_sql("select * from t, t2, t3 where o <= 777 and t.a >= 2 and "
                     "t.a < t3.a;"),
            "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
            "2 | 3 | 100 | 200 | 777 | 666\n"
            "2 | 3 | 300 | 500 | 777 | 666\n");

  ASSERT_EQ(
      exec_sql("select * from t, t2, t3 where t.a < t2.b and t.a > t3.a;"),
      "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
      "1 | 1 | 100 | 200 | 777 | 0\n"
      "1 | 1 | 300 | 500 | 777 | 0\n"
      "2 | 3 | 100 | 200 | 777 | 0\n"
      "2 | 3 | 300 | 500 | 777 | 0\n");
}

TEST_F(SQLTest, SelectTablesWithConditionsNotInProjectionShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (1, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (3, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select t.a, t.b from t, t2 where t.a < t2.b;"),
            "t.a | t.b\n"
            "1 | 1\n"
            "2 | 3\n");
}

//    ###     ######    ######      ######## ##     ## ##    ##  ######
//   ## ##   ##    ##  ##    ##     ##       ##     ## ###   ## ##    ##
//  ##   ##  ##        ##           ##       ##     ## ####  ## ##
// ##     ## ##   #### ##   ####    ######   ##     ## ## ## ## ##
// ######### ##    ##  ##    ##     ##       ##     ## ##  #### ##
// ##     ## ##    ##  ##    ##     ##       ##     ## ##   ### ##    ##
// ##     ##  ######    ######      ##        #######  ##    ##  ######

TEST_F(SQLTest, AggFuncCountShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select count(1) from t;"), "count(1)\n2\n");
  ASSERT_EQ(exec_sql("select count(*) from t;"), "count(*)\n2\n");
  ASSERT_EQ(exec_sql("select count(*), count(*) from t;"),
            "count(*) | count(*)\n2 | 2\n");
  ASSERT_EQ(exec_sql("select count(a), count(*) from t;"),
            "count(a) | count(*)\n2 | 2\n");
}

TEST_F(SQLTest, AggFuncCountEmptyTableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select count(a) from t;"), "count(a)\n0\n");
  ASSERT_EQ(exec_sql("select count(*) from t;"), "count(*)\n0\n");
}

TEST_F(SQLTest, AggFuncMaxShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select max(1) from t;"), "max(1)\n1\n");
  ASSERT_EQ(exec_sql("select max(a) from t;"), "max(a)\n2\n");
  ASSERT_EQ(exec_sql("select max(t.b) from t;"), "max(t.b)\n3\n");
}

TEST_F(SQLTest, AggFuncMaxEmptyTableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select max(a) from t;"), "max(a)\nNULL\n");
}

TEST_F(SQLTest, AggFuncMinShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select min(1) from t;"), "min(1)\n1\n");
  ASSERT_EQ(exec_sql("select min(a) from t;"), "min(a)\n1\n");
  ASSERT_EQ(exec_sql("select min(t.b) from t;"), "min(t.b)\n1\n");
}

TEST_F(SQLTest, AggFuncMinEmptyTableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select min(a) from t;"), "min(a)\nNULL\n");
}

TEST_F(SQLTest, AggFuncAvgShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b float, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1.1, '2021-10-31');"),
            "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3.0, '2021-11-1');"),
            "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3, 3.0, '2021-11-2');"),
            "SUCCESS\n");

  ASSERT_EQ(exec_sql("select avg(1) from t;"), "avg(1)\n1\n");
  ASSERT_EQ(exec_sql("select avg(a) from t;"), "avg(a)\n2\n");
  ASSERT_EQ(exec_sql("select avg(t.b) from t;"), "avg(t.b)\n2.37\n");
  ASSERT_EQ(exec_sql("select avg(d) from t;"), "avg(d)\n2021-11-01\n");
}

TEST_F(SQLTest, AggFuncAvgEmptyTableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b float, d date);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select avg(a) from t;"), "avg(a)\nNULL\n");
}

TEST_F(SQLTest, AggFuncWithValueShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select count(1) from t;"), "count(1)\n2\n");
  ASSERT_EQ(exec_sql("select count(1.1) from t;"), "count(1.1)\n2\n");
  ASSERT_EQ(exec_sql("select count('a') from t;"), "count('a')\n2\n");
}

TEST_F(SQLTest, AggFuncWithConditionShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select count(1) from t where a > 2;"), "count(1)\n0\n");
  ASSERT_EQ(exec_sql("select count(1) from t where a > 1;"), "count(1)\n1\n");
}

TEST_F(SQLTest, AggFuncUnsupportFuncShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select magic(1) from t;"), "FAILURE\n");
}

TEST_F(SQLTest, DISABLED_AggFuncInvalidCondiionShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  // TODO: Test case
}

TEST_F(SQLTest, AggFuncInvalidArgumentShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select count(c) from t;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select max(*) from t;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select min(*) from t;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select min() from t;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select min(a,*) from t;"), "FAILURE\n");
}

//       ##  #######  #### ##    ##
//       ## ##     ##  ##  ###   ##
//       ## ##     ##  ##  ####  ##
//       ## ##     ##  ##  ## ## ##
// ##    ## ##     ##  ##  ##  ####
// ##    ## ##     ##  ##  ##   ###
//  ######   #######  #### ##    ##
// ########    ###    ########  ##       ########  ######
//    ##      ## ##   ##     ## ##       ##       ##    ##
//    ##     ##   ##  ##     ## ##       ##       ##
//    ##    ##     ## ########  ##       ######    ######
//    ##    ######### ##     ## ##       ##             ##
//    ##    ##     ## ##     ## ##       ##       ##    ##
//    ##    ##     ## ########  ######## ########  ######

TEST_F(SQLTest, JoinTablesShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (100, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (300, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("create table t3(o int, a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (999, 888);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 666);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t3 values (777, 0);"), "SUCCESS\n");

  ASSERT_EQ(
      exec_sql("select * from t, t2 inner join t3 on o <= 777 and t.a >= 2;"),
      "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
      "2 | 3 | 100 | 200 | 777 | 666\n"
      "2 | 3 | 100 | 200 | 777 | 0\n"
      "2 | 3 | 300 | 500 | 777 | 666\n"
      "2 | 3 | 300 | 500 | 777 | 0\n");

  ASSERT_EQ(exec_sql("select * from t "
                     "inner join t2 on 1 = 1 "
                     "inner join t3 on 1 = 1 "
                     "where o <= 777 and t.a >= 2 and t.a < t3.a;"),
            "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
            "2 | 3 | 100 | 200 | 777 | 666\n"
            "2 | 3 | 300 | 500 | 777 | 666\n");

  ASSERT_EQ(exec_sql("select * from t "
                     "inner join t2 on t.a < t2.b "
                     "inner join t3 on t.a > t3.a;"),
            "t.a | t.b | t2.b | t2.d | t3.o | t3.a\n"
            "1 | 1 | 100 | 200 | 777 | 0\n"
            "1 | 1 | 300 | 500 | 777 | 0\n"
            "2 | 3 | 100 | 200 | 777 | 0\n"
            "2 | 3 | 300 | 500 | 777 | 0\n");
}

// #### ##    ##  ######  ######## ########  ########
//  ##  ###   ## ##    ## ##       ##     ##    ##
//  ##  ####  ## ##       ##       ##     ##    ##
//  ##  ## ## ##  ######  ######   ########     ##
//  ##  ##  ####       ## ##       ##   ##      ##
//  ##  ##   ### ##    ## ##       ##    ##     ##
// #### ##    ##  ######  ######## ##     ##    ##

TEST_F(SQLTest, InsertMultiTupleShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1), (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "1 | 1\n"
                                          "2 | 3\n");
}

TEST_F(SQLTest, InsertInvalidValuesShouldInsertNothing) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1), (2, 3.3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n");
}

TEST_F(SQLTest, InsertInvalidTupleShouldInsertNothing) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1), (1), (2, 3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n");
  ASSERT_EQ(exec_sql("insert into t values (1), (1), (2, 3), (4);"),
            "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n");
}

//  #######  ########  ########  ######## ########
// ##     ## ##     ## ##     ## ##       ##     ##
// ##     ## ##     ## ##     ## ##       ##     ##
// ##     ## ########  ##     ## ######   ########
// ##     ## ##   ##   ##     ## ##       ##   ##
// ##     ## ##    ##  ##     ## ##       ##    ##
//  #######  ##     ## ########  ######## ##     ##

TEST_F(SQLTest, OrderBySingleAttrShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (100, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t order by a asc;"), "a | b\n"
                                                         "1 | 1\n"
                                                         "2 | 3\n"
                                                         "100 | 3\n");
  ASSERT_EQ(exec_sql("select * from t order by a;"), "a | b\n"
                                                     "1 | 1\n"
                                                     "2 | 3\n"
                                                     "100 | 3\n");
  ASSERT_EQ(exec_sql("select * from t order by a desc;"), "a | b\n"
                                                          "100 | 3\n"
                                                          "2 | 3\n"
                                                          "1 | 1\n");
}

TEST_F(SQLTest, OrderByMultiAttrShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 10, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 10, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 1, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t order by a desc, b asc;"),
            "a | b | c\n"
            "2 | 1 | 3\n"
            "2 | 10 | 1\n"
            "1 | 10 | 2\n");
  ASSERT_EQ(exec_sql("select * from t order by a, b;"), "a | b | c\n"
                                                        "1 | 10 | 2\n"
                                                        "2 | 1 | 3\n"
                                                        "2 | 10 | 1\n");
  ASSERT_EQ(exec_sql("select * from t order by b, a;"), "a | b | c\n"
                                                        "2 | 1 | 3\n"
                                                        "1 | 10 | 2\n"
                                                        "2 | 10 | 1\n");
}

TEST_F(SQLTest, OrderByInvalidAttrShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (100, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t order by c asc;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t order by a, c;"), "FAILURE\n");
}

//  ######  ##     ## ########
// ##    ## ##     ## ##     ##
// ##       ##     ## ##     ##
//  ######  ##     ## ########
//       ## ##     ## ##     ##
// ##    ## ##     ## ##     ##
//  ######   #######  ########
//  #######  ##     ## ######## ########  ##    ##
// ##     ## ##     ## ##       ##     ##  ##  ##
// ##     ## ##     ## ##       ##     ##   ####
// ##     ## ##     ## ######   ########     ##
// ##  ## ## ##     ## ##       ##   ##      ##
// ##    ##  ##     ## ##       ##    ##     ##
//  ##### ##  #######  ######## ##     ##    ##

TEST_F(SQLTest, SubQueryShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a float, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1.0, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2.0, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3.0, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (1, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (3, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t "
                     "where a in "
                     "(select t2.b from t2 where d in (select a from t));"),
            "a | b\n");

  ASSERT_EQ(exec_sql("select * from t where a > (select avg(t2.b) from t2);"),
            "a | b\n"
            "3 | 3\n");

  ASSERT_EQ(exec_sql("select * from t where b > (select avg(t2.b) from t2);"),
            "a | b\n"
            "2 | 3\n"
            "3 | 3\n");

  ASSERT_EQ(
      exec_sql("select * from t where a > (select avg(t2.b) from t2 where d "
               ">= (select min(t.b) from t));"),
      "a | b\n"
      "3 | 3\n");
}

TEST_F(SQLTest, SubQueryWithOfficialTestCaseShouldWork) {
  ASSERT_EQ(exec_sql("create table CSQ_1(ID int, COL1 int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table CSQ_2(ID int, COL2 int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table CSQ_3(ID int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("SELECT * FROM CSQ_1 WHERE COL1 NOT IN (SELECT CSQ_2.COL2 "
                     "FROM CSQ_2 WHERE CSQ_2.ID IN (SELECT CSQ_3.ID FROM CSQ_3 "
                     "WHERE CSQ_1.ID = CSQ_3.ID));"),
            "ID | COL1\n");
}

TEST_F(SQLTest, SubQueryWithInvalidReferenceShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a float, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  // Ambiguous column: b
  ASSERT_EQ(exec_sql("select * from t where a > (select avg(b) from t2);"),
            "FAILURE\n");
}

TEST_F(SQLTest, SubQueryCompareMultiQueryResultsShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a float, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1.0, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2.0, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3.0, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (1, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (3, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where a = (select t2.b from t2);"),
            "FAILURE\n");
}

TEST_F(SQLTest, SubQueryInShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a float, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1.0, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2.0, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3.0, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (1, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (3, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where b in (select t2.b from t2);"),
            "a | b\n1 | 1\n3 | 3\n");
}

TEST_F(SQLTest, SubQueryNotInShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a float, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int, d int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1.0, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2.0, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3.0, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (1, 200);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (3, 500);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where b not in (select t2.b from t2);"),
            "a | b\n2 | 2\n");
}

TEST_F(SQLTest, SubQueryCanReferenceTableInParentQuery) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2(b int);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (3);"), "SUCCESS\n");

  ASSERT_EQ(
      exec_sql(
          "select * from t where a <> (select avg(b) from t2 where b >= a);"),
      "a\n1\n2\n");
}

// ##    ## ##     ## ##       ##
// ###   ## ##     ## ##       ##
// ####  ## ##     ## ##       ##
// ## ## ## ##     ## ##       ##
// ##  #### ##     ## ##       ##
// ##   ### ##     ## ##       ##
// ##    ##  #######  ######## ########

TEST_F(SQLTest, NullCreateTableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a float not null, b int nullable);"),
            "SUCCESS\n");
  ASSERT_EQ(exec_sql("desc t;"),
            "t(\n"
            "\tfield name=__trx, type=ints, len=4, visible=no, nullable=no\n"
            "\tfield name=__null, type=ints, len=4, visible=no, nullable=no\n"
            "\tfield name=a, type=floats, len=4, visible=yes, nullable=no\n"
            "\tfield name=b, type=ints, len=4, visible=yes, nullable=yes\n"
            ")\n");
}

TEST_F(SQLTest, NullInsertShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "1 | NULL\n"
                                          "1 | 1\n");
}

TEST_F(SQLTest, NullInsertNullOnNotNullableShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null, null);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("insert into t values (null, 1);"), "FAILURE\n");
}

TEST_F(SQLTest, NullInsertWithIndexShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_b on t(b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "1 | NULL\n"
                                          "1 | 1\n");
}

TEST_F(SQLTest, NullCompareWithNullShouldAlwaysFalse) {
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t where a = null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where b = null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where b < null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where b > null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where null = null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where 1 = null;"), "a | b\n");
}

TEST_F(SQLTest, NullIsNullShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 1);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select * from t where b is null;"), "a | b\n1 | NULL\n");
  ASSERT_EQ(exec_sql("select * from t where b is not null;"), "a | b\n1 | 1\n");
  ASSERT_EQ(exec_sql("select * from t where 1 is null;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where 1 is not null;"), "a | b\n"
                                                              "1 | NULL\n"
                                                              "1 | 1\n");
}

TEST_F(SQLTest, NullAggCountNullableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select count(*) from t;"), "count(*)\n2\n");
  ASSERT_EQ(exec_sql("select count(a) from t;"), "count(a)\n1\n");
}

TEST_F(SQLTest, NullAggMaxOfNullableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select max(a) from t;"), "max(a)\nNULL\n");

  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select max(a) from t;"), "max(a)\n2\n");
}

TEST_F(SQLTest, NullAggMinOfNullableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select min(a) from t;"), "min(a)\nNULL\n");

  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select min(a) from t;"), "min(a)\n1\n");
}

TEST_F(SQLTest, NullAggAvgOfNullableShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int nullable);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (null);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select avg(a) from t;"), "avg(a)\nNULL\n");

  ASSERT_EQ(exec_sql("insert into t values (1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select avg(a) from t;"), "avg(a)\n1.5\n");
}

// ######## ##     ## ########  ########
// ##        ##   ##  ##     ## ##     ##
// ##         ## ##   ##     ## ##     ##
// ######      ###    ########  ########
// ##         ## ##   ##        ##   ##
// ##        ##   ##  ##        ##    ##
// ######## ##     ## ##        ##     ##

TEST_F(SQLTest, ExpressionInConditionShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t where a + b = 5;"), "a | b\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where a - b = -1;"), "a | b\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where a - b = a + b - a * b;"),
            "a | b\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where b / a = 1.5;"), "a | b\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where 2 = a + b - a * (b - b / a);"),
            "a | b\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where 1 + 1 > 2;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where a-2 > 0;"), "a | b\n");
  ASSERT_EQ(exec_sql("select * from t where a - -2 = 4;"), "a | b\n2 | 3\n");
  ASSERT_EQ(exec_sql("select * from t where -a = -2;"), "a | b\n2 | 3\n");
}

TEST_F(SQLTest, ExpressionInSelectShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select a + b from t;"), "a+b\n5\n");
  ASSERT_EQ(exec_sql("select a + b, a - b from t;"), "a+b | a-b\n5 | -1\n");
  ASSERT_EQ(exec_sql("select 1 + 1, a - b from t;"), "1+1 | a-b\n2 | -1\n");
  ASSERT_EQ(exec_sql("select a + b - a * (b - b / a) from t;"),
            "a+b-a*(b-b/a)\n2\n");
  ASSERT_EQ(exec_sql("select -a + b from t;"), "-a+b\n1\n");
  ASSERT_EQ(exec_sql("select a,3*a from t;"), "a | 3*a\n2 | 6\n");
}

TEST_F(SQLTest, ExpressionInSelectTablesShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create table t2 (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t2 values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select t.a + 1, t.a + t.b from t, t2;"),
            "t.a+1 | t.a+t.b\n"
            "3 | 5\n");
}

//  ######   ########   #######  ##     ## ########
// ##    ##  ##     ## ##     ## ##     ## ##     ##
// ##        ##     ## ##     ## ##     ## ##     ##
// ##   #### ########  ##     ## ##     ## ########
// ##    ##  ##   ##   ##     ## ##     ## ##
// ##    ##  ##    ##  ##     ## ##     ## ##
//  ######   ##     ##  #######   #######  ##

TEST_F(SQLTest, GroupByShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select a, count(b) from t group by a;"), "a | count(b)\n"
                                                               "1 | 2\n"
                                                               "2 | 1\n");
}

TEST_F(SQLTest, GroupByMultiKeysShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a int, b int, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3, 1);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select a, count(*) from t group by a, b;"),
            "a | count(*)\n"
            "1 | 2\n"
            "2 | 1\n");
}

TEST_F(SQLTest, GroupByNullShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a int nullable, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (NULL, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select a, count(b) from t group by a;"), "a | count(b)\n"
                                                               "1 | 2\n"
                                                               "NULL | 1\n");
}

// ##     ## ##    ## ####  #######  ##     ## ########
// ##     ## ###   ##  ##  ##     ## ##     ## ##
// ##     ## ####  ##  ##  ##     ## ##     ## ##
// ##     ## ## ## ##  ##  ##     ## ##     ## ######
// ##     ## ##  ####  ##  ##  ## ## ##     ## ##
// ##     ## ##   ###  ##  ##    ##  ##     ## ##
//  #######  ##    ## ####  ##### ##  #######  ########

TEST_F(SQLTest, UniqueIndexMetaShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t_b on t(b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("desc t;"),
            "t(\n"
            "\tfield name=__trx, type=ints, len=4, visible=no, nullable=no\n"
            "\tfield name=__null, type=ints, len=4, visible=no, nullable=no\n"
            "\tfield name=a, type=ints, len=4, visible=yes, nullable=no\n"
            "\tfield name=b, type=ints, len=4, visible=yes, nullable=no\n"
            "\tindex name=t_a, fields=a, unique=yes\n"
            "\tindex name=t_b, fields=b, unique=no\n"
            ")\n");
}

TEST_F(SQLTest, UniqueIndexInsertConflictRecordShouldFailure) {
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 3);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "1 | 2\n");
}

TEST_F(SQLTest, UniqueIndexUpdateConflictRecordShouldFailure) {
  ASSERT_EQ(exec_sql("create table t (a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create unique index t_a on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (2, 3);"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("update t set a = 1 where b = 3;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n"
                                          "1 | 2\n"
                                          "2 | 3\n");
  // Trigger index scanner
  ASSERT_EQ(exec_sql("select * from t where a > 0;"), "a | b\n"
                                                      "1 | 2\n"
                                                      "2 | 3\n");
}

// ######## ######## ##     ## ########
//    ##    ##        ##   ##     ##
//    ##    ##         ## ##      ##
//    ##    ######      ###       ##
//    ##    ##         ## ##      ##
//    ##    ##        ##   ##     ##
//    ##    ######## ##     ##    ##

TEST_F(SQLTest, TextCreateTableShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a text);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("desc t;"),
            "t(\n"
            "\tfield name=__trx, type=ints, len=4, visible=no, nullable=no\n"
            "\tfield name=__null, type=ints, len=4, visible=no, nullable=no\n"
            "\tfield name=a, type=text, len=4, visible=yes, nullable=no\n"
            ")\n");
}

TEST_F(SQLTest, TextInsertSelectShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a text);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values ('aa');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a\n"
                                          "aa\n");
}

TEST_F(SQLTest, TextInsertSelectVeryLongTextShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a text);"), "SUCCESS\n");
  std::string long_text = common::random_string(4096);
  ASSERT_EQ(
      exec_sql(std::string() + "insert into t values ('" + long_text + "');"),
      "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
            std::string() + "a\n" + long_text + "\n");
}

TEST_F(SQLTest, TextInsertSelectOverTextShouldWork) {
  ASSERT_EQ(exec_sql("create table t (a text);"), "SUCCESS\n");
  std::string long_text = common::random_string(4097);
  ASSERT_EQ(
      exec_sql(std::string() + "insert into t values ('" + long_text + "');"),
      "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
            std::string() + "a\n" + long_text.substr(0, 4096) + "\n");
}

// ##     ## ##     ## ##       ######## ####
// ###   ### ##     ## ##          ##     ##
// #### #### ##     ## ##          ##     ##
// ## ### ## ##     ## ##          ##     ##
// ##     ## ##     ## ##          ##     ##
// ##     ## ##     ## ##          ##     ##
// ##     ##  #######  ########    ##    ####
// #### ##    ## ########  ######## ##     ##
//  ##  ###   ## ##     ## ##        ##   ##
//  ##  ####  ## ##     ## ##         ## ##
//  ##  ## ## ## ##     ## ######      ###
//  ##  ##  #### ##     ## ##         ## ##
//  ##  ##   ### ##     ## ##        ##   ##
// #### ##    ## ########  ######## ##     ##

TEST_F(SQLTest, MultiIndexCanCreate) {
  ASSERT_EQ(exec_sql("create table t (a int, b float, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t1 on t(a, b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t2 on t(b, c);"), "SUCCESS\n");
}

TEST_F(SQLTest, MultiIndexInvalidAttributeShouldFailure) {
  ASSERT_EQ(exec_sql("create table t (a int, b float, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t1 on t(d);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("create index t1 on t(a, d);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("create index t1 on t(d, a);"), "FAILURE\n");
}

TEST_F(SQLTest, MultiIndexDuplicateShouldFailure) {
  ASSERT_EQ(exec_sql("create table t (a int, b float, c int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t0 on t(a);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t1 on t(a);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("create index t2 on t(a, b);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("create index t3 on t(a, b);"), "FAILURE\n");
  ASSERT_EQ(exec_sql("create index t4 on t(b, a);"), "SUCCESS\n");
}

int main(int argc, char **argv) {
  srand((unsigned)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  conf_path = fs::absolute(conf_path);
  observer_binary = fs::absolute(observer_binary);
  return RUN_ALL_TESTS();
}