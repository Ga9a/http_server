# 简单的HTTP服务器

## 环境配置

- Ubuntu 22.04.4
- gcc version 11.4.0 (Ubuntu 11.4.0-1ubuntu1~22.04）
- cmake version 3.22.1

## 系统实现

## 快速运行

如下图配置数据库的登陆密码，用户名，新建立数据库，并将名字修改至文件/root/cgi/CgiMySQL.cpp文件中：

```bash
agni@agni-virtual-machine:~/Desktop/httpserver_edit$ mysql -u root -p
Enter password: 
Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 10
Server version: 8.0.37-0ubuntu0.22.04.3 (Ubuntu)

Copyright (c) 2000, 2024, Oracle and/or its affiliates.

Oracle is a registered trademark of Oracle Corporation and/or its
affiliates. Other names may be trademarks of their respective
owners.

Type 'help;' or '\h' for help. Type '\c' to clear the current input statement.

mysql> SHOW DATABASES;
+--------------------+
| Database           |
+--------------------+
| information_schema |
| mysql              |
| performance_schema |
| sys                |
| yourdb             |
+--------------------+
5 rows in set (0.11 sec)

mysql> use yourdb;
Reading table information for completion of table and column names
You can turn off this feature to get a quicker startup with -A

Database changed
mysql> show tables;
+------------------+
| Tables_in_yourdb |
+------------------+
| user             |
+------------------+
1 row in set (0.00 sec)

mysql> show columns from user
    -> ;
+----------+----------+------+-----+---------+-------+
| Field    | Type     | Null | Key | Default | Extra |
+----------+----------+------+-----+---------+-------+
| username | char(50) | YES  |     | NULL    |       |
| passwd   | char(50) | YES  |     | NULL    |       |
+----------+----------+------+-----+---------+-------+
2 rows in set (0.02 sec)
```

![image-20240718162443469](.\md_picture\image-20240718162443469.png)

在build目录下执行命令

```bash
sh ./build.sh
```

进入out目录执行

```bash
./Httpserver 9090
```

其中9090为端口号，然后打开浏览器输入

```
ip:端口号
```

即可打开登录界面：

![image-20240718163620505](.\md_picture\image-20240718163620505.png)

错误登录名/密码提示：

![image-20240718163713616](.\md_picture\image-20240718163713616.png)

使用存入MySQL数据库的用户名登录,进入应用界面:

![image-20240718163853992](.\md_picture\image-20240718163853992.png)

目前仅实现Nmap简单ip扫描，向ip地址中输入127.0.0.1，任意端口号查看cgi调用结果：
