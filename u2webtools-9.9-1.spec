Summary: Unix2Web Tools
Name: u2webtools
Provides: u2webtools
Version: SETVERSION
Release: SETRELEASE
License: GPL
Group: Productivity/Networking/Web/Servers
Source: SETSOURCE
BuildRoot: /dev/shm/u2w_makerpm
#DATAPORT#Packager: Henning Sander <henning.sander@dataport.de>
#NODATAPORT#Packager: Henning Sander <sandix@universal-logging-system.org>
#DATAPORT#Vendor: Henning Sander (Dataport)
#NODATAPORT#Vendor: Universal-Logging-System.org
Conflicts: u2webtools_mysql
#DEB#Section: httpd

%description
Unix2Web-Http/Https-Server
unix2webd without MySQL-Extension or MariaDB-Extension, httpget

%global debug_package %{nil}

%prep
echo

%setup
echo

%build
if [[ -d SRC ]]; then
  if cd SRC; then
    for i in *; do
      (cd $i && make)
    done
  fi
fi

%install
if [[ -n "$RPM_BUILD_ROOT" ]]; then
  mkdir -p $RPM_BUILD_ROOT
  cp -a `ls| grep -v 'SRC'` $RPM_BUILD_ROOT/
else
  rm -rf /dev/shm/u2w_makerpm
  mkdir -p /dev/shm/u2w_makerpm
  cp -a `ls| grep -v 'SRC'` /dev/shm/u2w_makerpm
fi
if [[ -d SRC ]]; then
  if cd SRC; then
    for i in *; do
      (cd $i && PREFIX=${RPM_BUILD_ROOT:-/dev/shm/u2w_makerpm}/usr make install)
    done
  fi
fi

%files

%pre
#BEGIN SYSTEMD#############################################################################
if [[ -x /etc/init.d/unix2web ]]; then
  if [[ -d /usr/lib/systemd/system ]]; then
    SYSTEMDSYSTEM=/usr/lib/systemd/system
  else
    SYSTEMDSYSTEM=/lib/systemd/system
  fi
  if [[ ! -f $SYSTEMDSYSTEM/unix2web@.service ]]; then
    /etc/init.d/unix2web stop
    if which insserv >/dev/null 2>&1; then
      insserv -r -f unix2web
    elif which chkconfig >/dev/null 2>&1; then
      chkconfig unix2web off
#DEB#      elif which update-rc.d >/dev/null 2>&1; then
#DEB#        update-rc.d -f unix2web remove
    else
      echo "unable to disable service unix2web"
    fi
  fi
fi
#END SYSTEMD###############################################################################

%post
#BEGIN SYSTEMD#############################################################################
if [[ -d /usr/lib/systemd/system ]]; then
  SYSTEMDSYSTEM=/usr/lib/systemd/system
else
  SYSTEMDSYSTEM=/lib/systemd/system
fi
if [[ ! -f $SYSTEMDSYSTEM/unix2web@.service ]]; then
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
  if ls /etc/unix2web/*.conf &>/dev/null; then
    for f in /etc/unix2web/*.conf; do
      n=${f##*/}
      systemctl enable unix2web@${n%.conf}
    done
  fi
fi
systemctl restart unix2web@* --all
#END SYSTEMD###############################################################################
#BEGIN NOSYSTEMD###########################################################################
#NODEB#if [[ "$1" = "1" ]]; then
#DEB#if [[ "$1" = "configure" ]]; then
  if which insserv >/dev/null 2>&1; then
    insserv unix2web
  elif which chkconfig >/dev/null 2>&1; then
    chkconfig unix2web on
#DEB#  elif which update-rc.d >/dev/null 2>&1; then
#DEB#    update-rc.d unix2web defaults || update-rc.d -f unix2web defaults
  else
    echo "you have to enable the service unix2web"
  fi
fi
/etc/init.d/unix2web restart
#END NOSYSTEMD#############################################################################
exit 0

%preun
#NODEB#if [[ "$1" = "0" ]]; then
#DEB#if [[ "$1" = "remove" || "$1" = "deconfigure" ]]; then
#BEGIN SYSTEMD#############################################################################
  systemctl stop 'unix2web@*'
  for s in $(systemctl list-units unix2web@* --all | sed -n 's/.*\(unix2web@.*\) *loaded .*/\1/p'); do
    systemctl disable "$s"
  done
#END SYSTEMD###############################################################################
#BEGIN NOSYSTEMD###########################################################################
  /etc/init.d/unix2web stop
  if which insserv >/dev/null 2>&1; then
    insserv -r -f unix2web
  elif which chkconfig >/dev/null 2>&1; then
    chkconfig unix2web off
#DEB#  elif which update-rc.d >/dev/null 2>&1; then
#DEB#    update-rc.d -f unix2web remove
  else
    echo "unable to disable service unix2web"
  fi
#END NOSYSTEMD#############################################################################
fi
exit 0

%postun
#BEGIN SYSTEMD#############################################################################
#NODEB#if [[ "$1" = "0" ]]; then
#DEB#if [[ "$1" = "remove" ]]; then
  if [[ -f /usr/lib/systemd/system/unix2web@.service ]]; then
    rm /usr/lib/systemd/system/unix2web@.service
    systemctl daemon-reload
  elif [[ -f /lib/systemd/system/unix2web@.service ]]; then
    rm /lib/systemd/system/unix2web@.service
    systemctl daemon-reload
  fi
  exit 0
fi
#END SYSTEMD###############################################################################

%clean
if [[ -n "$RPM_BUILD_ROOT" ]]; then
  rm -rf $RPM_BUILD_ROOT
else
  rm -rf /dev/shm/u2w_makerpm
fi

%changelog
