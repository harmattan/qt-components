TEMPLATE = subdirs
SUBDIRS = components

meego {
    SUBDIRS += meego
    extras:SUBDIRS += meego/extras
}

fremantle {
    SUBDIRS += fremantle/ssu fremantle/cssu
    extras:SUBDIRS += fremantle/ssu/extras fremantle/cssu/extras
}

symbian3 {
    SUBDIRS += symbian
    extras:SUBDIRS += symbian/extras
}
