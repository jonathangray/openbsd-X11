# $XFree86: xc/programs/Xserver/hw/xfree86/XF86Setup/texts/ja/messages.tcl,v 1.1.2.4 1998/02/26 20:11:23 hohndel Exp $
#
# messages in done.tcl :
set messages(done.1)	"\n\
	����򤹤٤ƴ�λ���Ƥ�������Ǥ�����\n\n\
	�褱��� <��λ> �ܥ���򲡤����Ȥǡ���������� X �����С���\n\
	Ω�������뤳�Ȥ��Ǥ��ޤ���\n\
	�ޤ������³���������ϲ��̾�ˤ���ܥ���򲡤��������³����\n\
	���꤬���ä���Ǹ�ˤ⤦���� <���꽪λ> �ܥ���򲡤��Ʋ�������"
set messages(done.2) "��λ"

# messages in srvflags :
set messages(srvflags.1) "�����С�����Υ��ץ����\n\n\
	�����Υ��ץ����Ϥ��餫���ᡢ�̾�����꤬���򤵤�Ƥ��ޤ���\n\
	�����餯���ѹ���ɬ�פϤʤ��Ǥ��礦��"
set messages(srvflags.2) "Ctrl-Alt-Backspace �λ��ĤΥ����򲡤����Ȥ�\n\
	�����С�������λ�Ǥ���褦�ˤ��롣"
set messages(srvflags.3) "�����С���Ω���夬�ä����֤Ǥβ������ѹ���\n\
	�Ǥ���褦�ˤ��롣 "
set messages(srvflags.4) "�����ʥ��̵�뤹�� �� \n\
	���������� prevents the server from exitting cleanly"
set messages(srvflags.5) "¾�Υۥ��Ȥˤ�äƲ����٤��ѹ��Ǥ���褦�ˤ���"
set messages(srvflags.6) "¾�Υۥ��Ȥˤ�äƥ����ܡ��ɤȥޥ�����\n\
	������ѹ��Ǥ���褦�ˤ���"

# messages in phase2 :
set messages(phase2.1) "Ω�������Ƥ��ޤ������Ф餯���Ԥ���������\n\n\n"
set messages(phase2.2) \
	"�����С����饭���ܡ��ɾ�����ɤ߹��ळ�Ȥ��Ǥ��ޤ���Ǥ�����\n\n\
	XKEYBOARD extension ���Ȥ߹���Ǥ��ʤ�����̵���� \
	���Ƥ��륵���С�����Ѥ��Ƥ��ޤ��󤫡�\n\n\
	XKEYBOARD extension ���Բķ�ǤϤ���ޤ��󤬡��ʤ����� \
	�����ܡ������꤬��ʬ�ˤǤ��ʤ������줬����ޤ���\n\n\
	³�Ԥ��ޤ���"
set messages(phase2.3) �ޥ���(M)
set messages(phase2.4) �����ܡ���(K)
set messages(phase2.5) ������(C)
set messages(phase2.6) �ǥ����ץ쥤(D)
set messages(phase2.7) ������(M)
set messages(phase2.8) ����¾(O)
set messages(phase2.9) ���(A)
set messages(phase2.10) ���꽪λ(D)
set messages(phase2.11) �إ��(H)
if !$pc98 {
    set messages(phase2.12) "\n\
�ޤ������Υ�å�������إ�פβ��������ɤ�ʤ���硢PageUp/Down�ʤɤΥ�����\n\
�������Ȥǥ��������뤹�뤳�Ȥ��Ǥ��ޤ���\n\n\
�����С�������ϡ�6�Ĥ���ʬ�ˤ狼��Ƥ��ޤ���\n\
\t�ޥ���\t\t�� �ޥ����Υץ��ȥ��롢�̿�®�٤ʤɤ����ꤷ�ޤ���\n\
\t�����ܡ���\t\t�� �����ܡ��ɤθ���ȥ쥤�����Ȥ����ꤷ�ޤ���\n\
\t������\t\t�� ���Ѥ��륰��ե��å�����������졼��(SVGA ������) \n\
\t\t\t �������Ԥʤ��ޤ���\n\
\t�ǥ����ץ쥤\t�� �ǥ����ץ쥤��ɽ���Ǥ����ʿ����ľ���ȿ���\n\
\t\t\t ���ꤷ�ޤ���\n\
\t������\t\t�� ���̤β����٤ȿ��������ꤷ�ޤ���\n\
\t����¾\t\t�� ����¾�������Ԥʤ��ޤ���\n\n\
�ޤ����ޥ��������꤫��Ϥ�뤳�Ȥ򤪴��ᤷ�ޤ���(Enter �����򲡤���\n\
���꤬�Ϥޤ�ޤ���)\n\
���٤Ƥ����꤬���ä��顢���ˤ���<���꽪λ> �Υܥ���򲡤��Ʋ�������\n\n\
�ܥ�������ֻ��ϡ�CTRL �ޤ��� ALT �����Ȳ����������Ƥ���ʸ���ʤ��Ȥ��С�\n\
�ޥ����ʤ� CTRL+'M' �����ˤ򲡤����������륭����TAB �����Ǥ���������\n\
����������碌�� ENTER �򲡤����ޥ����ǥܥ���򺸥���å����롢�ʤɤ�\n\
���Ʋ��������ޤ���'?'�����򲡤�����HELP �ܥ���򥯥�å�����г������\n\
�Ĥ��ơ��إ�פ򸫤뤳�Ȥ��Ǥ��ޤ���"
} else {
    set messages(phase2.12) "\n\
�ޤ������Υ�å�������إ�פβ��������ɤ�ʤ���硢ROLLUP/DOWN�ʤɤΥ�����\n\
�������Ȥǥ��������뤹�뤳�Ȥ��Ǥ��ޤ���\n\n\
�����С�������ϡ�5�Ĥ���ʬ�ˤ狼��Ƥ��ޤ���\n\
\t�ޥ���\t\t�� �ޥ����Υץ��ȥ��롢�̿�®�٤ʤɤ����ꤷ�ޤ���\n\
\t������\t\t�� ���Ѥ��륰��ե��å�����������졼��(SVGA ������) \n\
\t\t\t �������Ԥʤ��ޤ���\n\
\t�ǥ����ץ쥤\t�� �ǥ����ץ쥤��ɽ���Ǥ����ʿ����ľ���ȿ���\n\
\t\t\t ���ꤷ�ޤ���\n\
\t������\t\t�� ���̤β����٤ȿ��������ꤷ�ޤ���\n\
\t����¾\t\t�� ����¾�������Ԥʤ��ޤ���\n\n\
�ޤ����ޥ��������꤫��Ϥ�뤳�Ȥ򤪴��ᤷ�ޤ���\n\
\t�ʥ꥿���󥭡��򲡤������꤬�Ϥޤ�ޤ�����\n\
���٤Ƥ����꤬���ä��顢���ˤ���<���꽪λ> �Υܥ���򲡤��Ʋ�������\n\n\
�ܥ�������ֻ��ϡ�CTRL �ޤ��� GRPH �����Ȳ����������Ƥ���ʸ��(���Ȥ��С�\n\
�ޥ����ʤ� CTRL+'M' �����ˤ򲡤����������륭���� TAB �����Ǥ���������\n\
����������碌�� �꥿���󥭡��򲡤����ޥ����ǥܥ���򺸥���å����롢\n\
�ʤɤ򤷤Ʋ��������ޤ���'?'�����򲡤�����HELP �ܥ���򥯥�å������\n\
������ˤĤ��ơ��إ�פ򸫤뤳�Ȥ��Ǥ��ޤ���"
}
set messages(phase2.13) "�إ�׽�λ"
# messages in phase4 :
set messages(phase4.1) "Ω�������Ƥ��ޤ������Ф餯���Ԥ���������"
# phase.2-5 is generated by proc 'make_message_phase4'.
set messages(phase4.6) "Okey"
set messages(phase4.7) \
	"���̤��礭����ǥ����ץ쥤��Ǥβ��̤ΰ��֤ʤɡ����̤������\n\
	�ѹ����뤿��� xvidtune ����Ѥ��뤳�Ȥ��Ǥ��ޤ���\n\
	�ѹ���ɬ�פ��ʤ��ʤ齪λ���Ʋ�������\n\n\n\
	�������¸����Ȥ������Ǥ�¾�����꤬¸�ߤ��Ƥ���ХХå����å�\n\
	�Ȥ�����¸����ޤ���"
set messages(phase4.8) "�������¸����ե�����̾"
set messages(phase4.9) "xvidtune ����Ѥ���"
set messages(phase4.10) "�����ե��������¸���ơ���λ����"
set messages(phase4.11) "���� �� �������¸���ʤ��ǽ�λ����"
set messages(phase4.12) "��������Ǥ��ޤ�����"
set messages(phase4.13) "����ǤȤ��������ޤ���\
	�����С������꤬��λ���ޤ���\n\n"

# messages in card.tcl :
set messages(card.1) "���򤵤줿����ե��å������� :"
set messages(card.2) "����ե��å������ɤ����򤵤�Ƥ��ޤ���"
set messages(card.3) "README �ե�������ɤ�"
set messages(card.4) "�ܺ�����"
set messages(card.5) �����С�:
set messages(card.7) ���åץ��å�
set messages(card.8) RamDac
set messages(card.9) �����å����å�
set messages(card.10) "RAMDAC Max Speed"
set messages(card.11) "Probed"
set messages(card.12) "Video RAM"
set messages(card.13) "Probed"
set messages(card.14) "256 ����"
set messages(card.15) "512 ����"
set messages(card.16) "1 �ᥬ"
set messages(card.17) "2 �ᥬ"
set messages(card.18) "3 �ᥬ"
set messages(card.19) "4 �ᥬ"
set messages(card.20) "6 �ᥬ"
set messages(card.21) "8 �ᥬ"
set messages(card.22) "���򤵤줿\n���ץ����:"
set messages(card.23) "XF86Config �� Device �ι���դ��ä�������:"
#set messages(card.24) "Probed: Yes"
#set messages(card.25) "Probed: No"
set messages(card.26) "�����ɤ�����"
set messages(card.27) "�ܺ�����"
set messages(card.28) "���򤵤줿����ե��å�������: "
set messages(card.29) "��λ"
set messages(card.30) \
	" �ޤ��������С������������ɤ�����������Ƥ��顢\n\
	ɬ�פʤ�������ѹ����Ʋ����������åץ��åȡ�RamDac ��\n\
	�����å����åפ�����ξ�硢��ưǧ������ޤ���"
set messages(card.31) \
	" �ꥹ�Ȥ���ȤäƤ��륰��ե��å������ɤ�����ǲ�������\n\
	�⤷���ȤäƤ��륫���ɤ��ꥹ�Ȥˤʤ���硢�ܺ������ \n\
	�ܥ���򲡤��Ʋ�������"
set messages(card.32) \
	"�̾������Ǥ褱��С������ɤ�����Ϥ���ǽ�ʬ�Ǥ���\n\
	�̾�Ȱ㤦����򤷤�����硢�ܺ�����Υܥ���򲡤��Ʋ�������"
set messages(card.33) \
	"�̾������Ǥ褱��С������ɤ�����Ϥ���ǽ�ʬ�Ǥ���\n\
	����������ǧ�Τ���� README ���ɤ�褦�ˤ��ޤ��礦��\n\
	�⤷���ʤ�餫���ѹ���ɬ�פǤ���оܺ�����Υܥ����\n\
	�����Ʋ�������"
set messages(card.34) \
	"���Υ����ɤϡ�XFree86 �ǤϤޤ������ˤϥ��ݡ��Ȥ����\n\
	���ޤ��󡣤��������̾��VGA �⡼�ɤ�ư���褦��\n\
	����ϺѤ�Ǥ��ޤ���"

# messages in keyboard.tcl :
set messages(keyboard.1) "��ǥ�:"
set messages(keyboard.2) "�쥤������ (����):"
set messages(keyboard.3) "����¹�"
set messages(keyboard.4) \
		"��������ǥ�ȥ쥤�����Ȥ�����ǲ�������"
set messages(keyboard.5) "�Ȥ����ȤΤǤ��륪�ץ���� :"
set messages(keyboard.6) \
		"Variant (non U.S. Keyboards only):"
set messages(keyboard.7) "�����ͤ���Ѥ���"
set messages(keyboard.8) "���Ԥ��ޤ��������ľ���Ʋ�������"
set messages(keyboard.9) "������Ǥ���"
set messages(keyboard.10) "���Ԥ���������"

# messages in modeselect.tcl :

set messages(modeselect.1) "���Ѥ����������٤����򤷤Ʋ�����"
set messages(modeselect.2) 640x480
set messages(modeselect.3) 800x600
set messages(modeselect.4) 1024x768
set messages(modeselect.5) 1152x864
set messages(modeselect.6) 1280x1024
set messages(modeselect.7) 1600x1200
set messages(modeselect.8) 640x400
set messages(modeselect.9) 320x200
set messages(modeselect.10) 320x240
set messages(modeselect.11) 400x300
set messages(modeselect.12) 480x300
set messages(modeselect.13) 512x384
set messages(modeselect.14) "���Ѥ��뿧�������򤷤Ʋ�����"
set messages(modeselect.15) " 256�� "
set messages(modeselect.16) " 65536�� "
set messages(modeselect.17) " 1600����(Packed) "
set messages(modeselect.18) " 1600���� "

# messages in monitor.tcl :

set messages(monitor.1) "�ǥ����ץ쥤�μ��ȿ�"
set messages(monitor.2) "���򤵤줿�ǥ����ץ쥤:"
set messages(monitor.3) "��ʿ���ȿ�"
set messages(monitor.4) "��ľ���ȿ�"
set messages(monitor.5) \
	"�ǥ����ץ쥤��ɽ����ǽ�ʿ�ʿ����ľ���ȿ������Ϥ��Ʋ�������\n\
	�狼��ʤ���С��ꥹ�Ȥ����Ŭ�Ȼפ����Τ����򤷤Ʋ�������"

# messages in mouse.tcl :

set messages(mouse.1) "Lines/inch"
set messages(mouse.2) "����ץ�\n�졼��"
set messages(mouse.3) "�ޥ����μ��������ǲ�����"
set messages(mouse.4) "�������"
set messages(mouse.5) "�ޥ����Τ���ǥХ���"
set messages(mouse.6) "Emulate3Buttons"
set messages(mouse.7) ChordMiddle
set messages(mouse.8) "�̿�®��"
set messages(mouse.9) "? �ޤ��� CTRL-H �򲡤��С������ܡ��ɤλȤ�����ɽ������ޤ���"
set messages(mouse.10) �ե饰
set messages(mouse.11) "DTR��\n���ꥢ����"
set messages(mouse.12) "RTS��\n���ꥢ����"
set messages(mouse.13) "����ץ�\n�졼��"
set messages(mouse.14) "3�ܥ��󥨥ߥ�\n�졼�Ȥ�����"
set messages(mouse.15) "����¹�"
set messages(mouse.16) "? �ޤ��� CTRL-H �򲡤��С������ܡ��ɤλȤ�����ɽ������ޤ���"
set messages(mouse.17) "��λ(X)"
set messages(mouse.18) 3
set messages(mouse.19) "����"
set messages(mouse.20) "��"
set messages(mouse.21) "��"
set messages(mouse.22) "��"
set messages(mouse.23) "�ܥ����"