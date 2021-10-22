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
    ~ForkTestServer() { };
    void start(std::string data_dir, std::string socket_path) {
      pid = fork();
      if (pid == 0) {
        fs::current_path(data_dir);
        Server *server = init_server(socket_path);
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
    ~ExecTestServer() { };
    void start(std::string data_dir, std::string socket_path) {
      pid = fork();
      if (pid == 0) {
        fs::current_path(data_dir);
        if (execl(observer_binary.c_str(), "observer", "-s", socket_path.c_str(), "-f", conf_path.c_str(), NULL) != 0) {
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
    const char *test_server_workaround = std::getenv("SQL_TEST_SERVER_WORKAROUND");
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

  ~SQLTest() {
    delete server;
  }

  void SetUp() override {
    data_dir = fs::temp_directory_path() / ("miniob-sql-test-" + common::random_string(6));
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

TEST_F(SQLTest, BasicCreateTableShouldWork) {
  ASSERT_EQ(exec_sql("show tables;"), "No table\n");
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
}

TEST_F(SQLTest, BasicInsertShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, \"A\");"), "FAILURE\n");
}

TEST_F(SQLTest, BasicSelectShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, b int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values (1, 2);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a | b\n1 | 2\n");
  ASSERT_EQ(exec_sql("select a from t;"), "a\n1\n");
  ASSERT_EQ(exec_sql("select t.a from t;"), "a\n1\n");
}

TEST_F(SQLTest, SelectMetaShouldResponseHeadWhenNoData) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"), "a\n");
}

TEST_F(SQLTest, SelectMetaInvalidTableShouldFailure) {
  ASSERT_EQ(exec_sql("select * from t2;"), "FAILURE\n");
}

TEST_F(SQLTest, SelectMetaSelectInvalidColumnShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select b from t;"), "FAILURE\n");
  ASSERT_EQ(exec_sql("select t2.a from t;"), "FAILURE\n");
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
}

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

TEST_F(SQLTest, DateCanCreateTable) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("desc t;"),
    "t(\n"
    "\tfield name=__trx, type=ints, len=4, visible=no\n"
    "\tfield name=a, type=ints, len=4, visible=yes\n"
    "\tfield name=d, type=date, len=4, visible=yes\n"
    ")\n"
  );
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
  ASSERT_EQ(exec_sql("select * from t;"), "a | d\n1 | 0020-01-10\n1 | 2020-10-10\n1 | 2020-01-01\n1 | 4000-01-01\n");
}

TEST_F(SQLTest, DateSelectWhereShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");

  ASSERT_EQ(exec_sql("select d from t where d > '2020-12-1';"), "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2021-1-1';"), "d\n2021-01-01\n");
  ASSERT_EQ(exec_sql("select d from t where d = '2020-1-21';"), "d\n");
  ASSERT_EQ(exec_sql("select d from t where d < '2020-12-1';"), "d\n2020-10-10\n");
}

TEST_F(SQLTest, DateCharsNotBeAffected) {
  ASSERT_EQ(exec_sql("create table t(a int, b char(20), d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-1-1', '2020-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
    "a | b | d\n"
    "1 | 2020-1-1 | 2020-01-01\n"
  );
}

TEST_F(SQLTest, DateUpdateShouldWork) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set d='2022-2-22' where d < '2020-12-31';"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("select * from t;"),
    "a | d\n"
    "1 | 2022-02-22\n"
    "1 | 2021-01-01\n"
  );
}

TEST_F(SQLTest, DateUpdateInvalidDateShouldFailure) {
  ASSERT_EQ(exec_sql("create table t(a int, d date);"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2020-10-10');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("insert into t values(1, '2021-1-1');"), "SUCCESS\n");
  ASSERT_EQ(exec_sql("update t set d='2022-2-30' where d < '2020-12-31';"), "FAILURE\n");
}

int main(int argc, char **argv) {
  srand((unsigned)time(NULL));
  testing::InitGoogleTest(&argc, argv);
  conf_path = fs::absolute(conf_path);
  observer_binary = fs::absolute(observer_binary);
  return RUN_ALL_TESTS();
}