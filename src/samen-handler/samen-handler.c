/**
Samen is a ultra small wpa_supplicant action handler.
Copyright (C) 2011  Kai Uwe Jesussek

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SCRIPT_PATH
  #define SCRIPT_PATH "/etc/samen/"
#endif

int dhcp_client_stop(char* interface);
 
char executable_exists(char* name) {
	char cmd[64 + strlen(name)];
	sprintf(cmd, "which \"%s\" > /dev/null 2>&1", name);
	return WEXITSTATUS(system(cmd)) == 0 ? 1 : 0;
}

int dhcp_client_start(char* interface) {
	dhcp_client_stop(interface);
	char command[32 + strlen(interface)];
	if(executable_exists("dhcpcd"))
		strcpy(command, "dhcpcd");
	else if(executable_exists("dhclient"))
		strcpy(command, "dhclient");
	else if(executable_exists("dhclient3"))
		strcpy(command, "dhclient3");
	else if(executable_exists("udhcpcd"))
		strcpy(command, "udhcpcd");
	else {
		puts("error: no dhcp client executable found!");
		return -1;
	}
	strcat(command, " ");
	strcat(command, interface);
	return WEXITSTATUS(system(command));
}

int dhcp_client_stop(char* interface) {
	char command[32 + strlen(interface)];
	if(executable_exists("dhcpcd"))
		strcpy(command, "dhcpcd");
	else if(executable_exists("dhclient"))
		strcpy(command, "dhclient");
	else if(executable_exists("dhclient3"))
		strcpy(command, "dhclient3");
	else if(executable_exists("udhcpcd"))
		strcpy(command, "udhcpcd");
	else {
		puts("error: no dhcp client executable found!");
		return -1;
	}
	strcat(command, " -x ");
	strcat(command, interface);
	return WEXITSTATUS(system(command));
}

int usage() {
	puts("this is the event handler for wpa_cli");
	puts("usage:");
	puts("samen-handler <interface> (CONNECTED|DISCONNECTED)");
	return -1;
}

char* is_specific() {
	char* id = getenv("WPA_ID_STR");
	if (id && strlen(id) > 0)
		return id;
	return NULL;
}

int handle_connect_specific(char* device, char* id) {
	unsigned int addition = strlen(id) + 16 + strlen(device);
	char cmd[strlen(SCRIPT_PATH) + addition];
	sprintf(cmd, "%sup-%s.sh %s", SCRIPT_PATH, id, device);
	return WEXITSTATUS(system(cmd));
}

int handle_disconnect_specific(char* device, char* id) {
	unsigned int addition = strlen(id) + 16 + strlen(device);
	char cmd[strlen(SCRIPT_PATH) + addition];
	sprintf(cmd, "%sdown-%s.sh %s", SCRIPT_PATH, id, device);
	return WEXITSTATUS(system(cmd));
}

int handle_connect(char* device) {
	char* id = is_specific();
	if(!id)
		return dhcp_client_start(device);
	return handle_connect_specific(device, id);
}

int handle_disconnect(char* device) {
	char* id = is_specific();
	if(!id)
		return dhcp_client_stop(device);
	return handle_disconnect_specific(device, id);
}

int main(int argc, char* argv[]) {
	if(argc != 3) {
		return usage();
	}
	char* device = argv[1];
	char* event = argv[2];
	if(strcmp(event, "CONNECTED") == 0)
		return handle_connect(device);
	else if(strcmp(event, "DISCONNECTED") == 0)
		return handle_disconnect(device);
	return usage();
}

