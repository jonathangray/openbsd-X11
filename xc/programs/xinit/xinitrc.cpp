XCOMM!/bin/sh
XCOMM $XConsortium: xinitrc.cpp,v 1.4 91/08/22 11:41:34 rws Exp $
XCOMM $OpenBSD: xinitrc.cpp,v 1.3 2001/01/30 20:32:28 todd Exp $

userresources=$HOME/.Xresources
usermodmap=$HOME/.Xmodmap
sysresources=XINITDIR/.Xresources
sysmodmap=XINITDIR/.Xmodmap

XCOMM merge in defaults and keymaps

if [ -f $sysresources ]; then
    xrdb -merge $sysresources
fi

if [ -f $sysmodmap ]; then
    xmodmap $sysmodmap
fi

if [ -f $userresources ]; then
    xrdb -merge $userresources
fi

if [ -f $usermodmap ]; then
    xmodmap $usermodmap
fi

XCOMM if we have private ssh key(s), start ssh-agent and add the key(s)
id1=$HOME/.ssh/identity
id2=$HOME/.ssh/id_dsa
if [ -e /usr/bin/ssh-agent ] && [ -f $id1 -o -f $id2 ]; then
	eval `ssh-agent -s`
	[ -f $id1 ] && ssh-add $id1 < /dev/null
	[ -f $id2 ] && ssh-add $id2 < /dev/null
fi

XCOMM start some nice programs

xclock -geometry 50x50-1+1 &
xconsole &
xterm -geometry 80x24 &
fvwm || xterm

if [ "$SSH_AGENT_PID" ]; then
	ssh-add -D < /dev/null
	eval `ssh-agent -s -k`
fi
