Summary: Unix2Web Tools
Name: u2webtools_mariadb
Provides: u2webtools
Version: SETVERSION
Release: SETRELEASE
License: GPL
Group: Productivity/Networking/Web/Servers
Source: SETSOURCE
BuildRoot: /dev/shm/u2w_my_makerpm
#DATAPORT#Packager: Henning Sander <henning.sander@dataport.de>
#NODATAPORT#Packager: Henning Sander <sandix@universal-logging-system.org>
#DATAPORT#Vendor: Henning Sander (Dataport)
#NODATAPORT#Vendor: Universal-Logging-System.org
Conflicts: u2webtools_nomysql, u2webtools_mysql
#DEB#Section: httpd

%description
Unix2Web-Http/Https-Server
unix2webd with MariaDB-Extension, httpget, code_http

%prep
echo

%setup
echo

%build
#MAKEDEFINE#MYSQL="_mariadb"
if [[ -d SRC ]]
 then
  if cd SRC
   then
    for i in *
     do
      (cd $i && MYSQL="_mariadb" make)
    done
  fi
fi

%install
if [[ -n "$RPM_BUILD_ROOT" ]]
 then
  mkdir -p $RPM_BUILD_ROOT
  cp -a `ls| grep -v 'SRC'` $RPM_BUILD_ROOT/
else
  rm -rf /dev/shm/u2w_my_makerpm
  mkdir -p /dev/shm/u2w_my_makerpm
  cp -a `ls| grep -v 'SRC'` /dev/shm/u2w_my_makerpm
fi
if [[ -d SRC ]]
 then
  if cd SRC
   then
    for i in *
     do
      (cd $i && MYSQL="_mariadb" PREFIX=${RPM_BUILD_ROOT:-/dev/shm/u2w_my_makerpm}/usr make install)
    done
  fi
fi

%files

%pre
#BEGIN SYSTEMD#############################################################################
if [[ -x /etc/init.d/unix2web ]]
 then
  if [[ -d /usr/lib/systemd/system ]]
   then
    SYSTEMDSYSTEM=/usr/lib/systemd/system
  else
    SYSTEMDSYSTEM=/lib/systemd/system
  fi
  if [[ ! -f $SYSTEMDSYSTEM/unix2web@.service ]]
   then
    /etc/init.d/unix2web stop
    if which insserv >/dev/null 2>&1
     then
      insserv -r -f unix2web
    elif which chkconfig >/dev/null 2>&1
     then
      chkconfig unix2web off
#DEB#      elif which update-rc.d >/dev/null 2>&1
#DEB#       then
#DEB#        update-rc.d -f unix2web remove
    else
      echo "unable to disable service unix2web"
    fi
  fi
fi
#END SYSTEMD###############################################################################

%post
#BEGIN SYSTEMD#############################################################################
if [[ -d /usr/lib/systemd/system ]]
 then
  SYSTEMDSYSTEM=/usr/lib/systemd/system
else
  SYSTEMDSYSTEM=/lib/systemd/system
fi
if [[ ! -f $SYSTEMDSYSTEM/unix2web@.service ]]
 then
  cat >$SYSTEMDSYSTEM/unix2web@.service <<EOF
[Unit]
Description=Unix2Web Web Server
#SLES#Requires=var-run.mount network-online.target local-fs.target
#SLES#After=var-run.mount network-online.target local-fs.target
#NOSLES#Requires=network-online.target local-fs.target
#NOSLES#After=network-online.target local-fs.target

[Service]
Type=oneshot
ExecStart=/etc/init.d/unix2web -s start %i
ExecReload=/etc/init.d/unix2web -s reload %i
ExecStop=/etc/init.d/unix2web -s stop %i
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
EOF
  systemctl daemon-reload
  if ls /etc/unix2web/*.conf &>/dev/null
   then
    for f in /etc/unix2web/*.conf
     do
      n=${f##*/}
      systemctl enable unix2web@${n%.conf}
    done
  fi
fi
systemctl restart unix2web@* --all
#END SYSTEMD###############################################################################
#BEGIN NOSYSTEMD###########################################################################
#NODEB#if [[ "$1" = "1" ]]
#DEB#if [[ "$1" = "configure" ]]
 then
  if which insserv >/dev/null 2>&1
   then
    insserv unix2web
  elif which chkconfig >/dev/null 2>&1
   then
    chkconfig unix2web on
#DEB#  elif which update-rc.d >/dev/null 2>&1
#DEB#   then
#DEB#    update-rc.d unix2web defaults || update-rc.d -f unix2web defaults
  else
    echo "you have to enable the service unix2web"
  fi
fi
/etc/init.d/unix2web restart
#END NOSYSTEMD#############################################################################
exit 0

%preun
#NODEB#if [[ "$1" = "0" ]]
#DEB#if [[ "$1" = "remove" || "$1" = "deconfigure" ]]
 then
#BEGIN SYSTEMD#############################################################################
  systemctl stop 'unix2web@*'
  systemctl disable 'unix2web@*'
#END SYSTEMD###############################################################################
#BEGIN NOSYSTEMD###########################################################################
  /etc/init.d/unix2web stop
  if which insserv >/dev/null 2>&1
   then
    insserv -r -f unix2web
  elif which chkconfig >/dev/null 2>&1
   then
    chkconfig unix2web off
#DEB#  elif which update-rc.d >/dev/null 2>&1
#DEB#   then
#DEB#    update-rc.d -f unix2web remove
  else
    echo "unable to disable service unix2web"
  fi
#END NOSYSTEMD#############################################################################
fi
exit 0

%postun
#BEGIN SYSTEMD#############################################################################
#NODEB#if [[ "$1" = "0" ]]
#DEB#if [[ "$1" = "remove" ]]
 then
  if [[ -f /usr/lib/systemd/system/unix2web@.service ]]
   then
    rm /usr/lib/systemd/system/unix2web@.service
    systemctl daemon-reload
  elif [[ -f /lib/systemd/system/unix2web@.service ]]
   then
    rm /lib/systemd/system/unix2web@.service
    systemctl daemon-reload
  fi
  exit 0
fi
#END SYSTEMD###############################################################################

%clean
if [[ -n "$RPM_BUILD_ROOT" ]]
 then
  rm -rf $RPM_BUILD_ROOT
else
  rm -rf /dev/shm/u2w_makerpm
fi

%changelog
* Sun Apr 10 2022 sandix@universal-logging-system.org
- MariaDB / MySQL added code to accept multiple statements
* Sat May  1 2021 sandix@universal-logging-system.org
- Bugfix: %text(...) in Tables
* Sun Mar 14 2021 sandix@universal-logging-system.org
- new: %iframe
* Fri Feb 26 2021 sandix@universal-logging-system.org
- httpget: added support for SIGTERM
* Sat Dec 19 2020 sandix@universal-logging-system.org
- Bugfix: style u2wborder only for first level
* Sun Dec  6 2020 sandix@universal-logging-system.org
- Bugfix: segfault on %mysqlvalue without %mysqlgeline
* Sun Oct 25 2020 sandix@universal-logging-system.org
- New tags: %kbd, %code, %samp, %pcode, %psamp
* Thu Aug 27 2020 sandix@universal-logging-system.org
- <input type="hidden">: not in tables
- %table:<colformat>: changed to css
- %textarea: wrap="soft"
* Wed May 13 2020 sandix@universal-logging-system.org
- added support for systemd
* Sun Feb 23 2020 sandix@universal-logging-system.org
- Bugfix: %option - missing parameter
* Wed Dec 25 2019 sandix@universal-logging-system.org
- Bugfix: %menu with submenus, not working "back" button
* Sun Dec 15 2019 sandix@universal-logging-system.org
- Bugfix: GET with '?' in parameters
* Fri Aug 16 2019 sandix@universal-logging-system.org
- httpget: set cipher suites
* Wed Jul 10 2019 sandix@universal-logging-system.org
- Bugfix: Access rights
* Tue May 14 2019 sandix@universal-logging-system.org
- Bugfix: segfault on non existing directory with old libraries
* Sun Apr 21 2019 sandix@universal-logging-system.org
- enable UNIX2WEBBIN in /etc/unix2web/*.conf
* Tue Mar 12 2019 sandix@universal-logging-system.org
- Bugfix u2w_multi.c: abort on write errors
* Sun Dec  9 2018 sandix@universal-logging-system.org
- Bugfix: Garbage after sending normal files
* Wed Oct 10 2018 sandix@universal-logging-system.org
- ignore return code of insserv
* Sun Sep 23 2018 sandix@universal-logging-system.org
- bugfix: HTML-Code in second parameter of %hlink
* Fri Sep  7 2018 sandix@universal-logging-system.org
- bugfix: not working -tc in httpget
* Sun Jul  1 2018 sandix@universal-logging-system.org
- fixed buffer overflow bug in calling shell
* Sat May  5 2018 sandix@universal-logging-system.org
- new: reload of inifile
* Sun Apr  8 2018 sandix@universal-logging-system.org
- new: %s2w, %s3w, %s4w, %s5w, %u2w, %u3w, %u4w, %u5w
* Mon Apr  2 2018 sandix@universal-logging-system.org
- close connection on mysql_query error
* Sun Jan 14 2018 sandix@universal-logging-system.org
- fixed new html5 input tags, added %datalist
* Sat Dec  9 2017 sandix@universal-logging-system.org
- continue reading on buffer overflow
* Thu Sep 21 2017 sandix@universal-logging-system.org
- -Xi <inifile> to define system variables
* Sat Aug 12 2017 sandix@universal-logging-system.org
- detect too large POST data and answer with HTTP-413
* Sun Jun  4 2017 sandix@universal-logging-system.org
- added u2w-file to errorlogging
* Thu May  4 2017 sandix@universal-logging-system.org
- /etc/init.d/unix2web: write to /var/log/unix2web.log
* Sun Mar 19 2017 sandix@universal-logging-system.org
- bugfix: wrong parametercount in %checkid(...)
* Sat Feb 18 2017 sandix@universal-logging-system.org
- bugfix: disabled buggy code for "range" requests
- changed behaviour auf statements in "..." to interprete
-   after %if, %elif and %while
* Tue Feb 14 2017 sandix@universal-logging-system.org
- bugfix: %queryvars and %hidenvars - variables with multiple values
- new switch: -fS <maxfilesize> set maximum size of post files in bytes
* Thu Jul 14 2016 sandix@universal-logging-system.org
- bugfix: Html Pars in %mysqlread
* Tue Mar 29 2016 sandix@universal-logging-system.org
- new functions: %useragent, %uatype
* Sun Mar 13 2016 sandix@universal-logging-system.org
- new function: %queryvars
* Sun Feb  7 2016 sandix@universal-logging-system.org
- new %heads: %head with u2w-scripts
* Sat Oct  3 2015 sandix@universal-logging-system.org
- new option -Q: enable statuspage
* Sat Sep 19 2015 sandix@universal-logging-system.org
- new option -G <numbacklog>, set backlog in listen()
* Mon Sep  7 2015 sandix@universal-logging-system.org
- new option -SF
* Wed Sep  2 2015 sandix@universal-logging-system.org
- enabled non-blocking in SSL-handshake
* Sun Jul  5 2015 sandix@universal-logging-system.org
- enabled title and value for %pbutton
* Mon May  4 2015 sandix@universal-logging-system.org
- new function "checkid"
- bugfix in u2w_menu
* Tue Mar 17 2015 sandix@universal-logging-system.org
- new function "delonexit"
* Fri Feb  6 2015 sandix@universal-logging-system.org
- some fixes in read from browser routines
* Thu Sep 25 2014 sandix@universal-logging-system.org
- fixed bug in read multipart content blocks
* Thu Sep 18 2014 sandix@universal-logging-system.org
- fixed bug in nonblocking ssl reading on timout
* Tue Aug 26 2014 sandix@universal-logging-system.org
- fixed bug in nonblocking ssl reading
- fixed bug in reading partial multipart-content header
* Tue Jun 24 2014 sandix@universal-logging-system.org
- fixed bug in converting conf-files from version 8 to 9
* Fri Apr 25 2014 sandix@universal-logging-system.org
- fixed delete post files after SIGPIPE
* Wed Feb 19 2014 sandix@universal-logging-system.org
- fixed %colspan and %rowspan
* Sun Feb 16 2014 sandix@universal-logging-system.org
- some new css defs
* Tue Sep 10 2013 sandix@universal-logging-system.org
- fixed httpget
* Tue Jul  9 2013 sandix@universal-logging-system.org
- fixed some bugs
* Tue Apr  9 2013 sandix@universal-logging-system.org
- added mysqlconnect with characterset
* Sat Jan  5 2013 sandix@universal-logging-system.org
- first V 9.0.0 release
* Sat Dec 29 2012 sandix@universal-logging-system.org
- added -Dp for Pidfile and moved binaries to /usr/bin
* Wed Feb 8 2012 sandix@universal-logging-system.org
- patches in unix2webd
* Thu Sep 15 2011 sandix@universal-logging-system.org
- patch httpget to V 2.2.3
* Mon Feb 7 2011 sandix@universal-logging-system.org
- upgrade httpget to v 2.2.2
* Tue Feb 1 2011 sandix@universal-logging-system.org
- upgrade httpget to v 2.2.1
* Mon Sep 6 2010 sandix@universal-logging-system.org
- upgrade httpget to v 2.1.4
* Mon May 24 2010 sandix@universal-logging-system.org
- upgrade httpget to v 2.1.3
* Mon Nov 16 2009 sandix@universal-logging-system.org
- u2w with language support
* Thu Feb 14 2008 sandix@universal-logging-system.org
- /etc/init.d/unix2web fixed
* Tue Jan 22 2008 sandix@universal-logging-system.org
- new config-files in /etc/u2webtools/*.conf
* Tue Sep 19 2006 sandix@universal-logging-system.org
- first RPM package
