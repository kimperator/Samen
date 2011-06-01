Samen stands for Small Action ManagEr for wireless Networks

It is a ultra small wpa_supplicant (wpa_cli) action handler.

If the entry in wpa_supplicant.conf has an id_str entry,
it will look execute scripts in /etc/samen/:
For connect signal: up-<id_str>.sh
For disconnect signal: down-<id_str>.sh
Commandline Argument for scripts is the name of the Interface
Default action if no id_str is presend is to execute dhcpcd, dhclient or udhcpcd
