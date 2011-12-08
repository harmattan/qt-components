TEMPLATE = subdirs
SUBDIRS = components

meego {
    SUBDIRS += meego
    extras:SUBDIRS += meego/extras
}

fremantle {
    SUBDIRS += fremantle/ssu fremantle/cssu
    extras:SUBDIRS += fremantle/ssu/extras fremantle/cssu/extras
    i18n:SUBDIRS += fremantle/ssu/i18n fremantle/cssu/i18n
}

symbian3 {
    SUBDIRS += symbian
    extras:SUBDIRS += symbian/extras
}
