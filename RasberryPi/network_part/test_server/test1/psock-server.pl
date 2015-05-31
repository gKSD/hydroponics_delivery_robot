#!/usr/bin/perl -w
# http://2lx.ru - Блог помешанного программиста
# Статьи, учебники, руководства по программированию на C, C++, C#, PHP, Perl, RegEx, и многое другое...
use strict;
use IO::Socket;

my $port = 8091;

# Создаем сокет
socket(SOCK, # Указатель сокета
       PF_INET, # коммуникационный домен
       SOCK_STREAM, # тип сокета
       getprotobyname('tcp') # протокол
) or die ("Не могу создать сокет!");
setsockopt(SOCK, SOL_SOCKET, SO_REUSEADDR, 1);

# Связываем сокет с портом
my $paddr = sockaddr_in($port, INADDR_ANY);
bind(SOCK, $paddr) or die("Не могу привязать порт!");

# Ждем подключений клиентов
print "Ожидаем подключения...\n";
listen(SOCK, SOMAXCONN);
while (my $client_addr = accept(CLIENT, SOCK)){
    # Получаем адрес клиента
    my ($client_port, $client_ip) = sockaddr_in($client_addr);
    my $client_ipnum = inet_ntoa($client_ip);
    my $client_host = gethostbyaddr($client_ip, AF_INET);
    print "Client connected \n";

    # Принимаем данные от клиента
    my $data;
    while ($data = <CLIENT>)
    {
        #my @data = <CLIENT>;
        #my $count = sysread(CLIENT, $data, 1024);
        #print "bytes => ". socket()->recv($data, 1024)."\n";
        #print "Принято ${count} байт от ${client_host} [${client_ipnum}]\n";
        print $data;

        if ($data =~ /quit/)
        {
            # Закрываем соединение
            print "Closing connection\n";
            close(CLIENT);
            last;
        }   
        # Отправляем данные клиенту
        print CLIENT "Hello, world\r\n";
    }
}
