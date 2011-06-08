find .  \! -path  "./bin*" -type f -print | xargs chmod 644
chmod +x permissions.sh
find .  \! -path  "./bin*" -type d -print | xargs chmod 755
