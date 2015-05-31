#!/usr/bin/perl -w
####################
#Shpatserman Maria 
#01.12.2009
#Simple Telnetd 
####################
use strict;
use POSIX;
use POSIX ":sys_wait_h";
use IO::Socket;
use IO::Handle;
###############################################
#Creating daemon process. 
#Breaking link with the terminal
##############################################
my $pid= fork();
exit() if $pid;
die "Couldn't fork: $! " unless defined($pid);
POSIX::setsid() or die "Can't start a new session $!";

#Signal handlers TERM, HUP  an CHLD
#############################################
my $time_to_die =0;
my $server;
sub signal_handler{
	$time_to_die = 1;
	close($server);
}
$SIG{INT}= $SIG{TERM} = \&signal_handler;

my $conf_name="./simple-telnetd.conf";
#Array of default - allowed commands
my @def_commands;
$SIG{HUP} = \&rereading_config;
sub rereading_config{
	@def_commands=();
	open(FILECONF,$conf_name) or die "Can't open config file \n";
	while(<FILECONF>){
		chomp;
		push(@def_commands, $_);
	}
	close(FILECONF);
}

#Deleting zombies
sub REAPER {        
	while ((my $waitedpid = waitpid(-1,WNOHANG)) > 0) { }
	$SIG{CHLD} = \&REAPER;  
}

#At first time server up array of default commands need to be loaded 
rereading_config();

#Creating socket at 23 port
my $server_port=8090;
$server= new IO::Socket::INET(LocalPort => $server_port,
                                  TYPE => SOCK_STREAM,
                                  Reuse => 1,
                                  Listen => 10)
or die "Couldn't be a tcp server on port $server_port: $@\n";

until($time_to_die){

	my $client;
             #Accepting incoming connections
	while($client = $server->accept()){
		$SIG{CHLD} = \&REAPER;
		#Paralleling process for parent and child
		defined(my $child_pid=fork()) or die "Can't fork new child $!";
		#Parent go next(down) and waiting new accept 
		next if $child_pid;
		#Child doesn't need a copy of the server soket 
		if($child_pid == 0) {
			close($server);
		}
		#Clear bufer
		$client->autoflush(1);	
		my $is_def_command=0;
		print $client "Command :";
		
		#Reading client's commands
		while(<$client>){
			next unless /\S/;
			#Full string from client example: df -h
			my $full_enter_str = $_;
			chomp($full_enter_str);
			#Name of command  - example :df
			my $enter_command="";
			#String of parameters - example : -h 
			my $enter_params="";
			#Split full string into command and parametres 
			#############################################
			if($full_enter_str =~ /(\w+)(\s+)(.*)(\s*)/){
				$enter_command = $1;
				$enter_params = $3;
			}
			elsif($full_enter_str =~ /(\w+)/){
				$enter_command = $1;
				$enter_params = "";
			}
			else {
				$enter_command = "";
				$enter_params = "";
			}
			
			#Verifying entering command with default commands(configuration file)
			####################################################################
			foreach (@def_commands) {
				if($enter_command eq $_) { $is_def_command=1;}
			}		
					
			
			#Executing competent command
			#####################################
			if($is_def_command){
				my @lines = qx($enter_command $enter_params);
				foreach (@lines){
					print $client $_;
				}
			}
		
		}
		continue {
		print $client "Command :";
		$is_def_command=0;
		
		}
	exit;	
	}
	continue {
	close($client);
	}

}

