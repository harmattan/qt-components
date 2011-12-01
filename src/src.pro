TEMPLATE = subdirs
SUBDIRS = components

meego {
    SUBDIRS += meego
    extras:SUBDIRS += meego/extras
}

fremantle {
    SUBDIRS += fremantle/PR1.3 fremantle/PR1.4
    extras:SUBDIRS += fremantle/PR1.3/extras fremantle/PR1.4/extras
}

symbian3 {
    SUBDIRS += symbian
    extras:SUBDIRS += symbian/extras
}
