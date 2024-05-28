cat /etc/passwd | grep "/bin/nologin" | cut -d: -f1 | grep [m-z]$ > so/lab3/max15.txt
who | tr -s ' ' | cut -d" " -f4 | sort -n | head -n 12 > so/lab3/FirstLogins.txt
