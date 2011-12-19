TEMPLATE = subdirs
SUBDIRS = components

meego {
    SUBDIRS += meego
    extras:SUBDIRS += meego/extras
}

fremantle {
    SUBDIRS += fremantle/ssu
    extras:SUBDIRS += fremantle/ssu/extras
    i18n:SUBDIRS += fremantle/ssu/i18n

    cssu {
        SUBDIRS +=fremantle/cssu
        extras:SUBDIRS += fremantle/cssu/extras
        i18n:SUBDIRS += fremantle/cssu/i18n
    }
}

symbian3 {
    SUBDIRS += symbian
    extras:SUBDIRS += symbian/extras
}
