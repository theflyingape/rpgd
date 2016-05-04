Summary:	The Role Playing Game Daemon
Name:		rpgd
Version:	1.1c
Release:	3
Copyright:	free for non-commercial use
Group:		Daemons/MUD
URL:		http://www.microwavesoft.com/%{name}/
Vendor:		Robert Hurst <r_hurst@microwavesoft.com>
Source:		http://www.microwavesoft.com/files/%{name}-%{version}.src.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-root
Prefix:		%{_prefix}
Packager:	Chris Ausbrooks <weed@bucket.pp.ualr.edu>

%description
RPGD is The Role-Playing Game Daemon which allows telnet users to explore a
medieval fantasy world of monsters, spells, fortune and glory. It is a Hack &
Slash style of play governed by BBS-style access.

%prep
%setup -q -n %{name}

%build
cd src
for i in daemon client watch sysop/src ; do
	cd $i
	make all
	cd ..
done

%install
rm -rf %{buildroot}
mv -f doc/* .
mkdir -p %{buildroot}/%{prefix}/games/%{name}
mkdir -p %{buildroot}/etc/rc.d/init.d
cp -a bin etc help log mail menu msg pix snd tmp \
	%{buildroot}/%{prefix}/games/%{name}/
cd src
for i in daemon client watch ; do
	cd $i
	make INSTALLDIR=%{buildroot}/%{prefix}/games/%{name} install
	cd ..
done
cd sysop/src
install -D sysop %{buildroot}/%{prefix}/games/%{name}/bin
cd ../../..
cat etc/%{name}.rc \
	| sed -e 's,chkconfig: - 91 35,chkconfig: 345 91 35,g' \
	-e 's,/usr/local/,%{prefix}/games/,g' \
	> %{buildroot}/etc/rc.d/init.d/%{name}
chmod 755 %{buildroot}/etc/rc.d/init.d/%{name}
echo HOME=%{prefix}/games/%{name} > %{buildroot}/etc/%{name}.conf
echo GROUP=games >> %{buildroot}/etc/%{name}.conf
echo USER=games >> %{buildroot}/etc/%{name}.conf
chmod 664 %{buildroot}/etc/%{name}.conf
chown -R games.games %{buildroot}/%{prefix}/games/%{name}
ln -s %{prefix}/games/%{name}/bin/rpgclient %{buildroot}/%{prefix}/games/

%clean
rm -rf %{buildroot}

%post
echo "%{name}	7001/tcp" \
	>> /etc/services
echo "%{name}	stream	tcp	nowait	root	/usr/sbin/tcpd	" \
	%{prefix}/games/%{name}/bin/rpgclient \
	>> /etc/inetd.conf
chkconfig %{name} reset
/etc/rc.d/init.d/inet restart
/etc/rc.d/init.d/%{name} restart
echo To play, either run %{prefix}/games/rpgclient or telnet to port 7001 of
echo your machine "(i.e. telnet localhost 7001)".
echo IMPORTANT NOTE:
echo The first user to logon as NEW will default to being the King or SYSOP.
echo The King can then use the invisible '@' option at the Main Menu to enter
echo Sysop menu functions.

%preun
for i in services inetd.conf ; do
	cat /etc/$i | grep -v %{name} > $i.tmp
	chmod --reference=/etc/$i $i.tmp
	chown --reference=/etc/$i $i.tmp
	mv -f $i.tmp /etc/$i
done
/etc/rc.d/init.d/inet restart
/etc/rc.d/init.d/%{name} stop

%files
%doc CHANGES.txt README.txt
%config /etc/%{name}.conf
/etc/rc.d/init.d/*
%dir %{prefix}/games/%{name}
%dir %{prefix}/games/%{name}/bin
%{prefix}/games/%{name}/bin/rpg*
%{prefix}/games/%{name}/etc
%{prefix}/games/%{name}/help
%{prefix}/games/%{name}/log
%{prefix}/games/%{name}/mail
%{prefix}/games/%{name}/menu
%{prefix}/games/%{name}/msg
%{prefix}/games/%{name}/pix
%{prefix}/games/%{name}/snd
%{prefix}/games/%{name}/tmp
%{prefix}/games/rpgclient

%package gtk-sysop
Summary:	gtk sysop program for rpgd
Group:		Daemons/MUD
Requires:	rpgd

%description gtk-sysop
RPGD is The Role-Playing Game Daemon which allows telnet users to explore a
medieval fantasy world of monsters, spells, fortune and glory. It is a Hack &
Slash style of play governed by BBS-style access.  This is a gtk sysop
program RPGD.

%files gtk-sysop
%{prefix}/games/%{name}/bin/sysop

%changelog
* Mon Jan 17 2000 Chris Ausbrooks <weed@bucket.pp.ualr.edu>
- original rpm
