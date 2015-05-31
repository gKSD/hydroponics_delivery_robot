#!/usr/bin/perl -w
# http://2lx.ru - Блог помешанного программиста
# Статьи, учебники, руководства по программированию на C, C++, C#, PHP, Perl, RegEx, и многое другое...
use strict;
use IO::Socket;

# Создаем сокет
socket(SOCK, # Указатель сокета
       PF_INET, # коммуникационный домен
       SOCK_STREAM, # тип сокета
       getprotobyname('tcp') # протокол
);

# Задаем адрес сервера
my $host = "127.0.0.1";
my $port = 8080;
my $paddr = sockaddr_in($port,
                        inet_aton($host)
            );

# Соединяемся с сервером
connect(SOCK, $paddr);

# Отправляем запрос
send(SOCK, "Hello, World!", 0);

# Принимаем данные
my @data = <SOCK>;
print join(" ", @data);

# Закрываем сокет
close(SOCK);