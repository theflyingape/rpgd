while [ 1 ]; do

echo -n "Account: "
read ACCOUNT

echo -n "Handle: "
read HANDLE

echo -n "Password: "
read PASSWORD

echo -n "Email: "
read EMAIL

cat > mail.txt <<EOD
Subject: Your RPGd account information

This e-mail is being sent to you because it was used in registering to play
our online game.  You may sign in with the following account information:

Your account ID is: $ACCOUNT - $HANDLE
Your account Password is: $PASSWORD

If this e-mail was sent to you by mistake, please take no action, 
as this game account will automatically delete in a few days.

For the latest developments of RPGd, visit us at:

	http://robert.hurst-ri.us/games/rpgd

There, you can download the free Windows client software, or play
online with your Java-enabled web browser.
EOD

echo "mailing ... "
sendmail -f webmaster@hurst-ri.us "${EMAIL}" <mail.txt
echo

done

